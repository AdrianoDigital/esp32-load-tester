import Box from '@mui/material/Box';

export default function Led({ isOnline }) {
  return (
    <Box key='online' sx={{
      width: 15,
      height: 15,
      borderRadius: "50%",
      bgcolor: isOnline ? 'success.main' : 'error.main',
      animation: "blink 1s infinite",
      "@keyframes blink": {
        "0%, 100%": { opacity: 1 },
        "50%": { opacity: 0.3 },
      },
    }}
    />
  )
}
