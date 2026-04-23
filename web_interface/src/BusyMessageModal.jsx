import Box from '@mui/material/Box';
import CircularProgress from '@mui/material/CircularProgress';
import Modal from '@mui/material/Modal';
import Typography from '@mui/material/Typography';

const style = {
    display: 'flex',
    alignItems: 'center',
    position: 'absolute',
    top: '50%',
    left: '50%',
    transform: 'translate(-50%, -50%)',
    width: 400,
    bgcolor: 'background.paper',
    border: '2px solid #000',
    boxShadow: 24,
    p: 4,
};

export default function BusyMessageModal({
    busy
}) {

    return (
        <Modal open={busy !== ""} >
            <Box sx={style} >
                <CircularProgress />
                <Typography variant='h4' style={{ marginLeft: '1em' }}>
                    {busy}
                </Typography>

            </Box>
        </Modal>
    );
}
