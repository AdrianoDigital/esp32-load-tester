import { useEffect, useState } from 'react';
import { useImmer } from 'use-immer';

import { muiXTelemetrySettings } from '@mui/x-license';
import Box from '@mui/material/Box';
import Button from '@mui/material/Button';
import ButtonGroup from '@mui/material/ButtonGroup';
import Container from '@mui/material/Container';
import Grid from '@mui/material/Grid';
import Stack from '@mui/material/Stack';
import Typography from '@mui/material/Typography';

import CalibrationButton from './CalibrationButton';
import BusyMessageModal from './BusyMessageModal';
import TopBar from './TopBar';
import Plot from './Plot';
import Item from "./Item";

muiXTelemetrySettings.disableTelemetry();


function App() {
  const [measurements, udpateMeasurements] = useImmer([]);
  const [isPlotting, setPlotting] = useState(false);
  const [plotCursor, setPlotCursor] = useState(0);
  const [plotTimeOffset, setPlotTimeOffset] = useState(null);
  const [error, setError] = useState("");
  const [scaleState, setScaleState] = useState("");
  const [weight, setWeight] = useState(0);
  const [temperature, setTemperature] = useState(null);

  const max_points = 500;

  function weightUpdate(value, timeStamp) {
    let offset;
    setWeight(value);
    if (isPlotting) {
      if (plotTimeOffset == null) {
        offset = -timeStamp + plotCursor;
        setPlotTimeOffset(offset);
      } else {
        offset = plotTimeOffset;
      }
      udpateMeasurements((draft) => {
        let plotTimeStamp = timeStamp + offset;
        setPlotCursor(plotTimeStamp + 1);
        draft.push([value, plotTimeStamp]);
        draft.splice(0, draft.length - max_points);
      });
    }
  }

  // UseEffect, because the SSE stream is received *after" page rendering
  useEffect(() => {
    const eventSource = new EventSource("/stream");
    function addListener(channel, callback) {
      eventSource.addEventListener(
        channel,
        (event) => {
          const data = JSON.parse(event.data);
          const value = data["value"];
          const timeStamp = event.timeStamp / 1000;
          callback(value, timeStamp);
        }, {capture: false},
      );
    }
    eventSource.onopen = () => { console.log("SSE /stream connected"); };
    eventSource.onerror = (error) => { console.error("SSE error: ", error); };
    addListener("weight", weightUpdate);
    addListener("temperature", (v, t) => (setTemperature(v)));
    addListener("state", (v, t) => (setScaleState(v)));
    addListener("fsm_error", (v, t) => (setError(v)));
    return () => {
      eventSource.close();
      console.log("SSE /stream disconnected");
    };
  }, [isPlotting, plotTimeOffset]);

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

  function getBusyOperation() {
    switch (scaleState) {
      case "CALIB": return "Calibrating…"; break;
      case "TARE": return "Taring…"; break;
      default: return ""; break;
    }
  }

  return (
    <Container maxWidth="md">
      <BusyMessageModal busy={getBusyOperation()} />
      <TopBar />


      <Box sx={{ flexGrow: 1 }}>
        <Grid container spacing={2}>
          <Grid size={12}>
            <Item>
              <Typography variant="h7" gutterBottom>
                Weight:
              </Typography>
              <Typography variant="h1" gutterBottom={true}>
                {weight == null ? "--" : weight < 100 ? weight.toFixed(1) :  weight.toFixed(0)} kg
              </Typography>
            </Item>
          </Grid>

          <Grid size={12}>
            <Plot
              isPlotting={isPlotting}
              setPlotting={setPlotting}
              setPlotCursor={setPlotCursor}
              measurements={measurements}
              udpateMeasurements={udpateMeasurements}
              setPlotTimeOffset={setPlotTimeOffset}
            />
          </Grid>
          <Grid size={12}>
            <Stack spacing={2} direction="row">
              <Button variant="contained" onClick={api_call_start_tare}>Tare</Button>
              <CalibrationButton calibration={api_call_start_calib} />
            </Stack>
          </Grid>
          <Grid size={12}>
            <Item>
              <p>System Status: {scaleState}</p>
              {error != "" ? <p>System Message: {error}</p> : ""}
              <p>{temperature == null ? "" : temperature.toFixed(2) + " °C"}</p>
            </Item>
          </Grid>
        </Grid>
      </Box>

    </Container>
  )
}

export default App
