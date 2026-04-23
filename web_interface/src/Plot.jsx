import { Button } from '@mui/material';
import { LineChart } from '@mui/x-charts/LineChart';
import FormControlLabel from '@mui/material/FormControlLabel';
import Switch from '@mui/material/Switch';

import Item from "./Item";


export default function Plot({
  isPlotting,
  setPlotting,
  setPlotCursor,
  measurements,
  udpateMeasurements,
  setPlotTimeOffset,
  plot_horizon = 60,
  min_weight_range = 50,
}) {


  function switchPlotting(event) {
    setPlotting(event.target.checked);
    if (event.target.checked) {
      setPlotTimeOffset(null);
    }
  };

  function handleClear() {
    udpateMeasurements((draft) => {
      draft.splice(0);
      setPlotCursor(0);
      setPlotTimeOffset(null);
    })
  }

  return (
    <>
      <div style={{ display: "flex", alignItems: "center" }}>
        <FormControlLabel control={<Switch value={isPlotting} onChange={switchPlotting} />} label="Plot measurement" />
        <Button onClick={handleClear} sx={{ marginLeft: "auto" }}>Clear</Button>
      </div>
      <Item>
        <LineChart
          xAxis={[{
            label: 'Time [s]',
            data: measurements.map((v) => v[1]),
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
              domainLimit: (minValue, maxValue) => ({ "min": minValue - 1, "max": Math.max(min_weight_range, maxValue) })
            },
          ]}
          slotProps={{ tooltip: { trigger: 'axis' } }}
          height={300}
        />
      </Item>
    </>
  )
}
