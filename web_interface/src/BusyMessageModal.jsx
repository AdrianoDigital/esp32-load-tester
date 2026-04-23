import * as React from 'react';
import Box from '@mui/material/Box';
import Button from '@mui/material/Button';
import Typography from '@mui/material/Typography';
import Modal from '@mui/material/Modal';
import CircularProgress from '@mui/material/CircularProgress';

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
        <Modal
            open={busy !== ""}
            aria-labelledby="modal-modal-title"
            aria-describedby="modal-modal-description"
        >
            <Box sx={style} >
                <CircularProgress/>
                <Typography variant='h4' style={{ marginLeft: '1em' }}>
                    {busy}
                </Typography>

            </Box>
        </Modal>
    );
}
