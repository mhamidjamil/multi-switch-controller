# 74HC595 with Arduino Nano for Controlling 64+ Switches

This project demonstrates how to control over 64 switches (or LEDs) using an Arduino Nano and the **74HC595 shift register**. The 74HC595 allows you to expand the number of digital outputs using just 3 data pins on the Arduino, making it an efficient solution for projects with many outputs.

## What is the 74HC595?

The **74HC595** is an 8-bit shift register that expands the number of output pins by "shifting" data through its internal storage. It allows multiple registers to be chained together, so you can control a large number of outputs with only a few microcontroller pins.

### Features:
- 8 outputs per shift register
- Easily chainable for more outputs
- Requires only 3 data pins (Data, Clock, Latch)

## How This Project Works

In this project, we use multiple 74HC595 shift registers to control **64+ switches** with just 3 Arduino Nano pins:
- **Data Pin**: Sends the serial data to the registers.
- **Clock Pin**: Synchronizes the data flow.
- **Latch Pin**: Updates the outputs based on the data.

By chaining shift registers, you can control more outputs as needed.

## Wiring

- Connect the **Data**, **Clock**, and **Latch** pins from the Arduino Nano to the first 74HC595.
- Chain additional 74HC595s by connecting their **serial out** to the next shift register's **serial in**.
- Connect each switch/LED to the outputs of the 74HC595.

## Usage

1. Clone or download this repository.
2. Upload the provided code to your Arduino Nano.
3. Wire the shift registers according to the diagram in the code comments.
4. Control your switches/LEDs by updating the data sent to the 74HC595 registers.
