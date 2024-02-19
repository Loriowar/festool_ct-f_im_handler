# Festool CT-F I/M handler
Description of Festool CT-F I/M Set (button + receiver, [item number 202097](https://www.festool.com/accessory/dust-extraction/additions-to-the-system/modules/202097---ct-f-btm-set)) commands. Plus Arduino firmware to convert receiver commands into more friendly digital output.

## Use cases
- Use an original Festool Button and receiver to manipulate any kind of devices, not only other Festool equipment.
- Improve a Festool devices like vacuum CTL SYS (220V version) to get the same remove control as exists in the battery variation.

## Similar projects
- [Here](https://github.com/tjob/ft-vac-control) is a good project with applying CT-F I/M to any vacuum.
- And [here](https://github.com/jenskueper/festool-ctl-sys-bluetooth) is remote control for Festool CTL SYS vacuum.

## Hard logic implementation
More about such approach you can find in a [separate doc](https://github.com/Loriowar/festool_ct-f_im_handler/blob/main/hard_logic_elements_implementation.md) within the repo. Also there are references to other variants of the device.

## How to assemble and get it works?
- Upload firmware to Arduino.
- Attach Festool receive to Arduino `D4` (mid wire on the receiver). You can use 5V from Arduino to power the receiver (lowest receiver wire is ground, uppest +5V).
- Optional, add pull-down resistor (10kOm) to `D4`.
- Attach any action/control device to `D8`. For example, Arduino relay or logical MOSFET with you custom load.
- Attach 8-12V to Arduino Vin.
- Pair button and receiver.
- Push green Festool button and enjoy!

## How it works?
It reads data from Festool receiver (`D4` port), convert it into command code (see the codes description below) and set `D8` accordion to the command.

After power-on `D8` has low level. In case of receiving `0x177F` it sets high level on `D8`. And in case of `0x17CA` command it set low on `D8`. All other commands are ignored.

## Hardware
Just an Arduino Nano.

## Software
Arduino firmware can be found [in the repo](https://github.com/Loriowar/festool_ct-f_im_handler/blob/main/festool_receiver_manchester.ino).

### Pins description
- `D4` - input data from the receiver.
- `D8` - output data.
- `D13` (led) - duplicates a D8 status.

## Receiver data protocol
Here is a [good explanation](https://github.com/tjob/ft-vac-control?tab=readme-ov-file#how-it-works) of the data protocol together with a [serial trace](https://github.com/tjob/ft-vac-control/tree/main/software/traces). Below you can find more details about the protocol. It was collected using the mentioned Arduino firmware (see commented debug code in the firmware).

### Receiver messages
NOTE: there might be wrong parsing and/or wrong bits order. All below data taken from only one Festool buttion+receiver pair. Please, verify the commands on your device before usage.

~600ms after power on
```
Received value hex (bin) = 47 (1000111)
```
Then, after each button click
```
Received value hex (bin) = 177F (1011101111111)
Received value hex (bin) = 3B00 (11101100000000)
Received value hex (bin) = 177F (1011101111111)

Received value hex (bin) = 17CA (1011111001010)
Received value hex (bin) = 3B7FFF (1110110111111111111111)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 2FF (1011111111)
Received value hex (bin) = 3BFF (11101111111111)

Received value hex (bin) = 177F (1011101111111)
Received value hex (bin) = 3B00 (11101100000000)
Received value hex (bin) = 177F (1011101111111)

Received value hex (bin) = 17CA (1011111001010)
Received value hex (bin) = 3B7FFF (1110110111111111111111)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 2FF (1011111111)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 3BFF (11101111111111)
```
Zero messages are artifacts of the parsing. You can ignor it.

Different set of messages has different delay between messages within the set (from ~50ms to hundreds ms).

Fast double click might generate one additional message `0x2FF`:
```
// double click
Received value hex (bin) = 177F (1011101111111)
Received value hex (bin) = 3B00 (11101100000000)
Received value hex (bin) = 177F (1011101111111)
Received value hex (bin) = 17CA (1011111001010)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 2FF (1011111111)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 3BFF (11101111111111)

// click
Received value hex (bin) = 177F (1011101111111)
Received value hex (bin) = 3B00 (11101100000000)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 177F (1011101111111)

// double click
Received value hex (bin) = 17CA (1011111001010)
Received value hex (bin) = 3B7FFF (1110110111111111111111)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 2FF (1011111111)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 3BFF (11101111111111)
Received value hex (bin) = 177F (1011101111111)
Received value hex (bin) = 3B00 (11101100000000)
Received value hex (bin) = 177F (1011101111111)

// double click
Received value hex (bin) = 17CA (1011111001010)
Received value hex (bin) = 3B7FFF (1110110111111111111111)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 2FF (1011111111)
Received value hex (bin) = 3BFF (11101111111111)

// click
Received value hex (bin) = 177F (1011101111111)
Received value hex (bin) = 3B00 (11101100000000)
Received value hex (bin) = 177F (1011101111111)

// double click
Received value hex (bin) = 17CA (1011111001010)
Received value hex (bin) = 3B7FFF (1110110111111111111111)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 0 (0)
Received value hex (bin) = 2FF (1011111111)
Received value hex (bin) = 3BFF (11101111111111)
Received value hex (bin) = 177F (1011101111111)
Received value hex (bin) = 3B00 (11101100000000)
Received value hex (bin) = 177F (1011101111111)
```

### Found commands
- `0x47` - on receiver power on
- `0x177F` - start
- `0x17CA` - stop
- `0x3B00` - to full speed?
- `0x3B7FFF` - reduce speed with rate?
- `0x3BFF` - reduce speed to zero?
- `0x2FF` - unknown (between reduce speed with rate and reduce speed to zero)
- `0x17CA` - unknown
