import { muiXTelemetrySettings } from '@mui/x-license';
import { styled } from '@mui/material/styles';
import Container from '@mui/material/Container';
import Typography from '@mui/material/Typography';
import Box from '@mui/material/Box';
import { Button, ButtonGroup } from '@mui/material';

import AppBar from '@mui/material/AppBar';
import Toolbar from '@mui/material/Toolbar';
import IconButton from '@mui/material/IconButton';
import MenuIcon from '@mui/icons-material/Menu';

import TextField from '@mui/material/TextField';
import Dialog from '@mui/material/Dialog';
import DialogActions from '@mui/material/DialogActions';
import DialogContent from '@mui/material/DialogContent';
import DialogContentText from '@mui/material/DialogContentText';
import DialogTitle from '@mui/material/DialogTitle';

import InputAdornment from '@mui/material/InputAdornment';

import Stack from '@mui/material/Stack';

import Paper from '@mui/material/Paper';
import Grid from '@mui/material/Grid';

import FormGroup from '@mui/material/FormGroup';
import FormControlLabel from '@mui/material/FormControlLabel';
import Switch from '@mui/material/Switch';

import { LineChart } from '@mui/x-charts/LineChart';
import { useEffect, useState } from 'react';
import { useImmer } from 'use-immer';
import './App.css';

muiXTelemetrySettings.disableTelemetry();

const Item = styled(Paper)(({ theme }) => ({
  backgroundColor: '#fff',
  ...theme.typography.body2,
  padding: theme.spacing(1),
  textAlign: 'center',
  color: (theme.vars ?? theme).palette.text.secondary,
  ...theme.applyStyles('dark', {
    backgroundColor: '#1A2027',
  }),
}));

function App() {
  // const [horizon, setHorizon] = useState(0);
  const [measurements, udpateMeasurements] = useImmer([]);
  const [isPlotting, setPlotting] = useState(false);
  const [plotStartTime, setPlotStartTime] = useState(-1);
  const [error, setError] = useState("");
  const [scaleState, setScaleState] = useState("");
  const [calibFormOpem, setCalibFormOpem] = useState(false);

  const [weight, setWeight] = useState(0);
  const [temperature, setTemperature] = useState(0);

  const plot_horizon = 120;

  function handleStartPlotting() {
    udpateMeasurements((draft) => { draft.splice(0) });
    setPlotStartTime(-1);
    setPlotting(true);
  };

  function handleStopPlotting() {
    setPlotting(false);
  };

  function switchPlotting(event) {
    setPlotting(event.target.checked);
    if (event.target.checked) {
      // Start plotting with empty data set
      udpateMeasurements((draft) => { draft.splice(0) });
      setPlotStartTime(-1);
    }
  };


  useEffect(() => {
    const eventSource = new EventSource("/stream");

    eventSource.onopen = () => {
      console.log("SSE /stream connected");
    };

    eventSource.onerror = (error) => {
      console.error("SSE error: ", error);
    };

    eventSource.addEventListener(
      "weight",
      (event) => {
        const data = JSON.parse(event.data);
        const value = data["value"];
        const timeStamp = event.timeStamp / 1000;
        setWeight(value);
        if (isPlotting) {
          // console.log("Weight: ", value, "TimeStamp: ", timeStamp, "Measurements: ", measurements.length, "start time: ", plotStartTime);
          if (plotStartTime == -1) {
            setPlotStartTime(timeStamp)
          }
          udpateMeasurements((draft) => {
            draft.push([value, timeStamp]);
            let i = 0;
            while ((draft[i][1] < (timeStamp - plot_horizon)) && (i < draft.length - 1)) {
              i++;
            }
            draft.splice(0, i);
          });
        }
      }, false,
    );

    eventSource.addEventListener(
      "temperature",
      (event) => {
        const data = JSON.parse(event.data);
        const value = data["value"];
        const timeStamp = event.timeStamp;
        setTemperature(value);
      }, false,
    );

    eventSource.addEventListener(
      "state",
      (event) => {
        const data = JSON.parse(event.data);
        setScaleState(data["value"]);
      }, false,
    );

    eventSource.addEventListener(
      "fsm_error",
      (event) => {
        const data = JSON.parse(event.data);
        setError(data["value"]);
      }, false,
    );

    return () => {
      eventSource.close();
      console.log("SSE /stream disconnected");
    };
  }, [isPlotting, plotStartTime]);

  async function api_call_start_tare() {
    await fetch('/api/v1/scale/startTare')
      .then(response => {
        if (response.status != 202) {
          let json = response.json();
          setError("Failed to start tare: " + json["status"]);
        }
      })
      .catch((err) => {
        console.log(err.message);
        setError(err.message);
      });
  };

  async function api_call_start_calib(known_mass) {
    await fetch('/api/v1/scale/startCalib?known_mass=' + String(parseFloat(known_mass)))
      .then(response => {
        if (response.status != 202) {
          let json = response.json();
          setError("Failed to start calib: " + json["status"]);
        }
      })
      .catch((err) => {
        console.log(err.message);
        setError(err.message);
      });
  };

  async function api_update_scale_state() {
    await fetch('/api/v1/scale/getState')
      .then(response => response.json())
      .then(json => {
        setScaleState(json["state"])
      })
      .catch(error => console.error(error));
  }

  useEffect(() => {
    api_update_scale_state();
  }, []);


  function handleStartTare() {
    api_call_start_tare();
  }

  function handleClickCalib() {
    setCalibFormOpem(true);
  }
  function handleCalibcClose() {
    setCalibFormOpem(false);
  }
  function handleCalibSubmit(event) {
    event.preventDefault();
    const formData = new FormData(event.currentTarget);
    const formJson = Object.fromEntries(formData.entries());
    const known_mass = formJson.known_mass;
    if (isNaN(known_mass)) {
      // TODO handle error
    } else {
      api_call_start_calib(known_mass);
      handleCalibcClose();
    }
  }

  return (
    <Container maxWidth="sm">


      <AppBar position="static">
        <Toolbar>
          <IconButton
            size="large"
            edge="start"
            color="inherit"
            aria-label="menu"
            sx={{ mr: 2 }}
          >
            <MenuIcon />

          </IconButton>
          <Typography variant="h6" component="div" sx={{ flexGrow: 1 }}>
            RopeSnap
          </Typography>
        </Toolbar>
      </AppBar>

      <Box sx={{ flexGrow: 1 }} height="10px">
        <p></p>
      </Box>
      <Box sx={{ flexGrow: 1 }}>
        <Grid container spacing={2}>
          <Grid size={12}>
            <Item>
              <Typography variant="h7" gutterBottom>
                Weight:
              </Typography>
              <Typography variant="h1" gutterBottom={true}>
                {weight.toFixed()} kg
              </Typography>

            </Item>
          </Grid>

          <Grid size={12}>
            <FormControlLabel control={<Switch onChange={switchPlotting} />} label="Plot measurement" />
            <Item>
              <LineChart
                xAxis={[{
                  label: 'Time [s]',
                  data: measurements.map((v) => v[1] - plotStartTime),
                  scaleType: 'linear',
                  valueFormatter: (value, context) => (context.location == 'tick' ? value.toFixed(0) : value.toFixed(1) + " s"),
                  domainLimit: (minValue, maxValue) => ({ "min": minValue, "max": Math.max(plot_horizon, maxValue) })
                }]}
                series={[
                  {
                    data: measurements.map((v) => v[0]),
                    curve: 'linear',
                    valueFormatter: (v, context) => v.toFixed(1) + " kg",
                  },
                ]}
                yAxis={[
                  {
                    label: 'Weight [kg]',
                    width: 60,
                    domainLimit: (minValue, maxValue) => ({ "min": Math.min(0, minValue), "max": Math.max(10, maxValue) })
                  },
                ]}
                slotProps={{ tooltip: { trigger: 'axis' } }}
                height={300}
              />
            </Item>
          </Grid>
          <Grid size={12}>
            <Stack spacing={2} direction="row">
              <Button variant="contained" onClick={handleStartTare}>Tare</Button>
              <Button variant="contained" onClick={handleClickCalib}>Calibrate</Button>

              <Dialog open={calibFormOpem} onClose={handleCalibcClose}>
                <DialogTitle>Calibration</DialogTitle>
                <DialogContent>
                  <Stack spacing={2} direction="column">
                    <DialogContentText>
                      For calibration, attach a known weight to the load cell and enter it here:
                    </DialogContentText>

                    <form onSubmit={handleCalibSubmit} id="calib-form">
                      <TextField
                        label="Known weight"
                        name="known_mass"
                        sx={{ m: 1, width: '25ch' }}
                        slotProps={{
                          input: {
                            endAdornment: <InputAdornment position="end">kg</InputAdornment>,
                          },
                        }}
                      />

                    </form>
                  </Stack>

                </DialogContent>
                <DialogActions>
                  <Button onClick={handleCalibcClose}>Cancel</Button>
                  <Button type="submit" form="calib-form">
                    Start
                  </Button>
                </DialogActions>
              </Dialog>
            </Stack>
          </Grid>
          <Grid size={12}>
            <Item>
              <p>Scale Status: {scaleState}</p>
              <p>Error: {error}</p>
              <p>{temperature.toFixed(2)} °C</p>
            </Item>
          </Grid>
        </Grid>
      </Box>

    </Container>
  )
}

export default App
