# dlopes_weather_bot

Created based on telebot and open-meteo-api. Code test for Nxlog opportunity.



## Building

Similar as https://github.com/smartnode/telebot, you need to install libraries and
build tools such as CMake.
On Debian-based Linux distributions you can do it as follows:

```sh
sudo apt-get install libcurl4-openssl-dev libjson-c-dev cmake binutils make
```


To build the program run following commands:

```sh
cd [your repository]
mkdir -p build && cd build
cmake ../
make
```

## Running

After building, you can run the app in the test folder:
```sh
./dlopes_bot
```
It's is needed to have a `.token` file in the same location.
This token was generated with Telegram botfather - the dlopes_weather


## Simple Test

Running the app, open Telegram and send messages to dlopes_weather bot: https://t.me/dlopes_weather_bot


