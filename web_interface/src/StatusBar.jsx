import Box from '@mui/material/Box';
import Grid from '@mui/material/Grid';
import Typography from '@mui/material/Typography';

import Item from "./Item";
import Led from "./Led";


export default function StatusBar({
  isOnline,
  scaleState,
  temperature,
  error,
}) {

  return (
    <>
      <Grid size={4}>
        <Item>
          <Box sx={{ display: 'flex', alignItems: 'baseline' }}>
            <Led isOnline={isOnline} />
            <Typography sx={{ ml: 1 }}>
              {isOnline === true ? "Online" : "Offline"}
            </Typography>
          </Box>
        </Item>
      </Grid>
      <Grid size={4}>
        <Item><Typography sx={{ ml: 1 }}>
          Status: {scaleState}
        </Typography></Item>
      </Grid>
      <Grid size={4}>
        <Item><Typography sx={{ ml: 1 }}>
          {temperature == null ? "" : temperature.toFixed(2) + " °C"}
        </Typography></Item>
      </Grid>
    </>

  )
}
