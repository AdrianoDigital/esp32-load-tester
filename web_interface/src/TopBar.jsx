import React from "react";

import AppBar from '@mui/material/AppBar';
import IconButton from '@mui/material/IconButton';
import MenuIcon from '@mui/icons-material/Menu';
import Toolbar from '@mui/material/Toolbar';
import Typography from '@mui/material/Typography';

export default function TopBar({

}) {
    return (
        <AppBar position="static" sx={{ marginBottom: 2 }}>
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
    )
}