import { muiXTelemetrySettings } from '@mui/x-license';
import { LineChart } from '@mui/x-charts/LineChart';
import { useEffect, useState } from 'react';
import { useImmer } from 'use-immer';
import './App.css';

muiXTelemetrySettings.disableTelemetry();

function App() {
  // const [horizon, setHorizon] = useState(0);
  const [measurements, udpateMeasurements] = useImmer([]);
  const [isRecording, setRecording] = useState(false);
  const [operation, setOperation] = useState("idle");  // idle, tare, calib, stream
  const [error, setError] = useState("");
  const [scaleState, setScaleState] = useState("");
  const [knownMass, setKnownMass] = useState(1.0);

  function handleStartRecording() {
    udpateMeasurements((draft) => { draft.splice(0) });
    setRecording(true);
  };

  function handleStopRecording() {
    setRecording(false);
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
        const timeStamp = event.timeStamp;
        if (isRecording) {
          console.log("Weight: ", value, "TimeStamp: ", timeStamp);
          udpateMeasurements((draft) => { draft.push([value, timeStamp / 1000]); });
        }
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
  }, [isRecording]);

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

  async function api_call_start_calib() {
    await fetch('/api/v1/scale/startCalib?known_mass=' + String(parseFloat(knownMass)))
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

  async function api_call_start_stream() {
    await fetch('/api/v1/scale/startStream')
      .then(response => {
        if (response.status != 202) {
          let json = response.json();
          setError("Failed to start streaming: " + json["status"]);
        }
      })
      .catch((err) => {
        console.log(err.message);
        setError(err.message);
      });
  };

  async function api_call_stop_stream() {
    await fetch('/api/v1/scale/stopStream')
      .then(response => {
        if (response.status != 202) {
          let json = response.json();
          setError("Failed to stop streaming: " + json["status"]);
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
    setOperation('tare');
    api_call_start_tare();
  }

  function handleStartCalib() {
    setOperation('calib');
    api_call_start_calib(knownMass);
  }
  function handleStartStream() {
    setOperation('stream');
    api_call_start_stream();
  }
  function handleStopStream() {
    setOperation('idle');
    api_call_stop_stream();
  }

  return (
    <div className="App">
      <LineChart
        xAxis={[{
          label: 'Time [s]',
          // data: measurements.slice(0, horizon).map((v) => v[0]),
          data: measurements.map((v) => v[1] - measurements[0][1]),
          scaleType: 'linear',
        }]}
        series={[
          {
            // data: measurements.slice(0, horizon).map((v) => v[1]),
            data: measurements.map((v) => v[0]),
            area: false,
            showMark: true,
            curve: 'linear',
            // label: (location) => location === 'tooltip' ? "Weight [kg]" : "abcs",
            valueFormatter: (v, context) => v + " kg (at " + measurements[context.dataIndex][0] + " s)",
          },
        ]}
        yAxis={[
          {
            label: 'Weight [kg]',
            width: 60,
          },
        ]}
        slotProps={{ tooltip: { trigger: 'item' } }}
        height={300}
      />
      <div>
        <button onClick={handleStartRecording} disabled={isRecording}>Start</button>
        <button onClick={handleStopRecording} disabled={!isRecording}>Stop</button>
      </div>
      <div>
        <button onClick={handleStartTare}>Tare</button>
        <label>Known Mass (kg)
          <input name="known_mass" value={knownMass} onChange={e => setKnownMass(e.target.value)}></input>
        </label>
        <button onClick={handleStartCalib}>Calibrate</button>
        <button onClick={handleStartStream}>Start Streaming</button>
        <button onClick={handleStopStream}>Stop Streaming</button>
      </div>
      <div>
        <p>Error: {error}</p>
        <p>Scale Status: {scaleState}</p>
        <p>Operation: {operation}</p>
      </div>
    </div>
  )
}

export default App
