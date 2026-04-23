Import("env")

try:
    import qrcode
except ImportError:
    env.Execute("$PYTHONEXE -m pip install qrcode")

try:
    import wifi_qrcode_generator
except ImportError:
    env.Execute("$PYTHONEXE -m pip install wifi_qrcode_generator")
