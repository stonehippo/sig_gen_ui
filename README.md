# Sparkfun MiniGen UI

The [Sparkfun MiniGen](https://learn.sparkfun.com/tutorials/minigen-hookup-guide) is a little waveform generator based on the Analog Devices AS9837. The board to standalone or to work paired with an [Arduino Pro Mini](https://www.sparkfun.com/arduino-pro-mini-328-3-3v-8mhz.html).

The MiniGen can be controlled entirely in code, but sometimes it's nicer to have a little physical UI. To make this possible, I paired the MiniGen with a rotary encoder to control the frequency (and the increments of adjustment) and the output waveform.

*Note:* The MiniGen is no longer availale at Sparkfun

## Hardware

- Sparkfun MiniGen
- Arduino Pro Mini 3.3/8MHZ - the MiniGen is designed to work with this board. **Do not use the 5V/16MHz version**.
- [Sparkfun Qwiic Twist](https://www.sparkfun.com/sparkfun-qwiic-twist-rgb-rotary-encoder-breakout.html) - a breakout board for a rotary encoder that uses an I2C interface
- [Sparkfun Qwiic OLED](https://www.sparkfun.com/sparkfun-qwiic-oled-display-0-91-in-128x32-lcd-24606.html) - a small OLED display at 128x32 pixels, also using I2C

To connect the encoder and OLED, I cut a Qwiic cable in half and soldered the wires to the I2C, power, and ground pins on the Arduino Pro Mini.

## Software

I developed this code using [PlatformIO](https://platformio.org/) and the [Arduino framework](https://docs.arduino.cc/language-reference/). I used the Visual Studio Code-based PlatformIO IDE, but this project will work just as well with the CLI.