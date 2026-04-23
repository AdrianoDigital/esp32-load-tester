import os

# This loads PlatformIO's environment
Import("env")


def vite_run(target, source, env):
    os.chdir("web_interface")
    try:
        env.Execute("npm install")
        env.Execute("npx vite")
    finally:
        os.chdir("..")


def vite_build(target, source, env):
    os.chdir("web_interface")
    try:
        env.Execute("npx vite build --emptyOutDir --outDir ../data/www")
    finally:
        os.chdir("..")


env.AddCustomTarget(
    name="web_if_devel",
    dependencies=None,
    actions=[vite_run],
    title="Run web-if",
    description="Start web interface locally in node.js",
)

env.AddCustomTarget(
    name="web_if_build",
    dependencies=None,
    actions=[vite_build],
    title="Build web-if",
    description="Build the React web application into a production release for upload",
)


# Note: Pre "buildfs" didn't work, see
#       https://github.com/platformio/platform-espressif32/issues/834#issuecomment-1158760192
env.AddPreAction(".pio/build/esp12e/littlefs.bin", vite_build)

