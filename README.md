# Cat Lamp http server

This repository contains code to my tiny server running on esp8266 module,
which is embedded in a *sliglty hacked* RGB cat lamp.

The source code is based on the FreeRTOS based sdk provided by espressif.


## Set up the repository

ESP8266_RTOS_SDK is added as a submodule and should be set-up as instructed by
espressif in their
[README](https://github.com/espressif/ESP8266_RTOS_SDK#readme), after cloning
the submodule.

```
python -m venv esp_venv
. esp_venv/bin/activate
git submodule update --init --checkout
cd ESP8266_RTOS_SDK
pip install -r requirements.txt
./install.sh
. export.sh
```

Downlaod the toolchain they provide and either add it to PATH or edit the build
command using `make menuconfig`.

> If it's set up correctly, you should be able to build one of the examples,
> like:
> ```
> make -D ESP8266_RTOS_SDK/examples/hello_world
> ```
>

## Building

When the repository is set up, simply run:

```
make
```

To flash the board, run:

```
make flash
```

To get the serial console run:

```
make monitor
```
