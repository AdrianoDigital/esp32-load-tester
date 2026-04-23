import { useState } from "react";

import Dialog from '@mui/material/Dialog';
import DialogActions from '@mui/material/DialogActions';
import DialogContent from '@mui/material/DialogContent';
import DialogContentText from '@mui/material/DialogContentText';
import DialogTitle from '@mui/material/DialogTitle';
import InputAdornment from '@mui/material/InputAdornment';
import Stack from '@mui/material/Stack';
import TextField from '@mui/material/TextField';
import Button from '@mui/material/Button';

export default function CalibrationButton({
    calibration,
}) {
    const [valid, setValid] = useState(false);
    const [mass, setMass] = useState("");
    const [isOpen, setOpen] = useState(false);

    function handleMass(e) {
        let value = e.target.value;
        setMass(value);
        setValid(value.trim() !== "" && !Number.isNaN(Number(value)));
    }

    function handleCalibSubmit(event) {
        event.preventDefault();
        const formData = new FormData(event.currentTarget);
        const formJson = Object.fromEntries(formData.entries());
        const known_mass = formJson.known_mass;
        const known_mass_num = Number(known_mass);
        calibration(known_mass);
        setOpen(false);
    }

    return (
        <>
        <Button variant="contained" onClick={() => {setOpen(true)}}>Calibrate</Button>
        <Dialog open={isOpen} onClose={() => { setOpen(false) }}>
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
                            error={!valid}
                            value={mass}
                            onChange={handleMass}
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
                <Button onClick={() => { setOpen(false) }} >Cancel</Button>
                <Button type="submit" form="calib-form" disabled={!valid}>
                    Start
                </Button>
            </DialogActions>
        </Dialog>
        </>
    )
}
