from pathlib import Path
import os
import secrets
import string
import wifi_qrcode_generator.generator
import qrcode
import re

Import("env")

fn_app_secrets_ini = Path("./app_secrets.ini")
fn_wifi_qr_code_png = Path("./data/wifi_qr_code.png")
fn_app_url_qr_png = Path("./data/app_url_qr.png")


def build_flags_as_dict(env):
    build_flags = {}
    try:
        parsed = env.ParseFlags(env["BUILD_FLAGS"])
        cpp_defines = parsed.get("CPPDEFINES")
        build_flags = dict(cpp_defines)
    except:
        pass
    return build_flags


def clean_app_secrets(target, source, env):
    fn_app_secrets_ini.unlink(missing_ok=True)


def generate_qr_codes(target, source, env):
    if not fn_app_secrets_ini.exists():
        print(
            f"ERROR: '{fn_app_secrets_ini}' has not been generated yet. "
            "Run 'Generate Secrets' first (pio run -t app_secrets_generate)"
        )
        return 1  # Fail

    build_flags = build_flags_as_dict(env)
    ssid = build_flags["ACCESS_POINT_ESSID"].replace('"', "")

    print("Instructions:")
    print()
    print(f"1) Scan QR-code to login to WIFI '{ssid}'")
    qr_code = wifi_qrcode_generator.generator.wifi_qrcode(
        ssid=ssid,
        hidden=False,
        authentication_type="WPA",
        password=build_flags["ACCESS_POINT_PSK"].replace('"', ""),
    )
    qr_code.print_ascii()
    qr_code.make_image().save(fn_wifi_qr_code_png)

    ap_ip = re.sub(r"[^0-9.]", "", build_flags["ACCESS_POINT_IP"].replace(",", "."))
    app_url = f"http://{ap_ip}/"
    print()
    print(f"2) Scan QR-code to open the web application ({app_url})")
    qr_code = qrcode.QRCode()
    qr_code.add_data(app_url)
    qr_code.print_ascii()
    qr_code.make_image().save(fn_app_url_qr_png)


def generate_app_secrets(target, source, env):
    if fn_app_secrets_ini.exists():
        print(
            f"ERROR: File '{fn_app_secrets_ini}' exists already, will not overwrite it. "
            "Run 'Delete Secrets' first (pio run -t app_secrets_clean)"
        )
        return 1  # Fail
    else:
        alphabet = string.ascii_letters + string.digits
        ap_pwd_length = int(env.GetProjectOption("custom_ap_pwd_length"))
        random_password = "".join(
            secrets.choice(alphabet) for i in range(ap_pwd_length)
        )
        with open(fn_app_secrets_ini, "w") as fh:
            fh.write(
                f"""
[app_secrets]
build_flags=
    -D ACCESS_POINT_PSK='"{random_password}"'
"""
            )


env.AddCustomTarget(
    name="app_secrets_generate",
    dependencies=None,
    actions=[generate_app_secrets],
    title="Generate Secrets",
    description="Generate app_secrets.ini (access-point password, ...)",
)

env.AddCustomTarget(
    name="wifi_qr_code_generate",
    dependencies=None,
    actions=[generate_qr_codes],
    title="Generate Wifi QR",
    description=f"Generate two QR codes: Wifi credentials '{fn_wifi_qr_code_png}' and application URL '{fn_app_url_qr_png}'",
)

env.AddCustomTarget(
    name="app_secrets_clean",
    dependencies=None,
    actions=[clean_app_secrets],
    title="Delete Secrets",
    description="Delete app_secrets.ini",
)
