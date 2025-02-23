# dlopes_weather_bot

Created based on smartnode/telebot and open-meteo-api. Code test for Nxlog opportunity.


## Building

Similar as https://github.com/smartnode/telebot, you need to install libraries and
build tools such as CMake.
On Debian-based Linux distributions you can do it as follows:

```sh
sudo apt-get install libcurl4-openssl-dev libjson-c-dev cmake binutils make
```

Also is necessary to set up Rust build environment.<br>
Obs.: To avoid an error with no_mangle (unsafe attribute) during Rust build,
I've used the 1.84.0 version:

```sh
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

rustup install 1.84.0
rustup override set 1.84.0

```

Finally, to build the program run following commands:

```sh
cd [your repository]
mkdir -p build && cd build
cmake ../
make
```

The command builds the C app and Rust lib api for open-meteo together.


## Clean

It is also possible to clean the C and Rust objects with the command:

```sh
make clean-all
```


## Running

After building, you can run the app in the test folder:
```sh
./dlopes_bot
```

(!) It is needed to create a `.token` file in the build/test folder.
A token ID was generated with Telegram botfather - the dlopes_weather - and
can be found at the header on test/dlopes_bot.c. Or use another token of your convenience.


## Simple Test

Running the app, open Telegram and send messages to dlopes_weather
bot: https://t.me/dlopes_weather_bot


## Unitary Tests
Some simple unit tests have been created that can be built and run by configuring cmake with:
```sh
cmake ../ -DRUN_TESTING=ON
make
```

To clear the environment variable, use:
```sh
cmake ../ -DRUN_TESTING=OFF
make
```


## CI/CD with Github

Just an initial version - Please check it out in .github/workflows/ci.yml
