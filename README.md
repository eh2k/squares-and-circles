> WORK-IN-PROGRESS: At the moment where are not all engines in this repo. Feel free to leave me a message / feedback or any hints in the ["BETA TEST - FEEDBACK"](https://github.com/eh2k/squares-and-circles/issues/1) issue. More engines will follow.

# □︎●︎ [![PlatformIO CI](https://github.com/eh2k/squares-and-circles/actions/workflows/build.yml/badge.svg)](https://github.com/eh2k/squares-and-circles/actions/workflows/build.yml)
<!-- ⧉⦾ ⧇ ⟥⧂ -->
**squares-and-circles** is an alternative firmware for the Eurorack module O_C, targeting Teensy 4.0.

## ■ Demos 

[![](https://img.youtube.com/vi/WQj3YqGpxRU/0.jpg)](https://www.youtube.com/watch?v=WQj3YqGpxRU)
[![](https://img.youtube.com/vi/QdlwETEaE3A/0.jpg)](https://youtu.be/QdlwETEaE3A)
[![](https://img.youtube.com/vi/lb-pbm1ddRw/0.jpg)](https://youtu.be/lb-pbm1ddRw)

## Motivation

When I am asked why I do such projects - my general reply is: I don't know - because I like to get deeper understanding of technical things. 
Maybe the reason was the chip shortage that makes Eurorack DIY projects tricky at the moment and I used that, to get more involved with coding in the eurorack land. 
At this point, a big thanks to the people behind ornament & crime (o_C), Teensy and specially Mutable Instruments for the inspiring playground and the basis regarding hardware and software for this project.

## Challenge

Given are the following ingredients: Two buttons, two encoders and a 128x64 display. Sixteen I/O ports (4x trigs, 4x cv and 4x dac) and a Cortex-M7.

Damn many possibilities to complicate it. Mono, stereo and then again CV. Seen way too many projects that got lost in menu diving.
A tricky task to design a simple UI logic and to get as much out of the hardware as possible (work in progress).

## Concept

Similar to Monomachine and Machinedrum here we have basically 4 configurable tracks. Each track can be assigned with a synthesizer machine, controlled by a trigger and CV input. 
As there are mono and stereo machines, the generated audio signal is routed to one or two neighbour DAC outputs by default.
E.g you can chain the mono audio signal from an oscillator machine to the neighbour fx-machine with stereo-outputs.

* [Short Press Left/Right Button] scrolls through the 4 machine-tracks.
* [Long Press Left-Button] enters the machine-selection-page.
* [Long Press Right-Button] enters the machine-config-page.
* [Long Press Left and Right-Button] saves the patch - will be restored at startup.

----

## ○ Machines ([Wiki](https://github.com/eh2k/squares-and-circles/wiki/%E2%96%A1%EF%B8%8E%E2%97%8F%EF%B8%8E-Machines-&-Engines))

* **GND**
  * `---`
* **CV**
  * V/OCT, LFO, Envelope
* **Drums**
  * Analog-BD, Analog SD, Analog HH, Analog HH2
  * 909ish-BD, 909ish-SD, TR909-CH-OH, TR909-OH, TR909-Ride
  * 808ish-BD, 808ish-SD, 808ish-CH-OH, 808ish-HiHat
  * TR707, TR707-CH-OH
  * FM-Drum
  * Djembe
  * Clap
* **M-OSC**
  * Waveforms
  * Virt.Analog, Waveshaping, FM, Grain, Additive, Wavetable, Chord
  * Resonator
* **Stereo-FX**
  * Reverb, Rev-Dattorro, Delay
* **SPEECH**
  * LPC, SAM

## Machine-Config (per Track)
 * **Trig-Input**: default Trig/Gate, Midi-In (channel 1 only)
 * **CV-Input**: default V/OCT, any other control parameter
 * **Transpose**: -48 to 24  (default -24)
 * **Midi-Channel**: 1-16, one channel on mulitple machines, for polyphony
   * **Note-Hold**: True, False (Trigger)
 * **Output-Boost**: Add extra-gain to the output - can result in distortion
---------

# Build & Flash firmware
 * install VSCode + platformio extension (https://platformio.org/platformio-ide)
   - On linux: curl https://www.pjrc.com/teensy/00-teensy.rules > /etc/udev/rules.d/49-teensy.rules 
   - Open Folder or `code .` inside project directory  
   - In VSCode - choose environment e.g "OC_teensy40", press "build" or "upload" (ensure teensy connected via usb)
 * OR: use Teensy Loader to flash compiled hex: https://www.pjrc.com/teensy/loader.html
# Supported Hardware  
## Ornament-and-Crime
 * Teensy 4.x + DAC8565 + 128x64 OLED display
 * Build-guide: http://ornament-and-cri.me/ (https://github.com/jakplugg/uO_c)
   * Replace the Teensy3 with Teensy4
   > **HINT**:
   if the [POGO Pin](https://www.modwiggler.com/forum/viewtopic.php?p=2867702#p2867702) is soldered - cover the bottom of the teensy with insulating tape - all other pins are compatible with T4 to T3 (see pjrc). Be careful with connecting USB and power at the same time - if you have VIN/VUSB connected.
## Hardware setup procedure (automatically on first startup)
  >Power on the module with the [LEFT] button pressed for entering the setup procedure.
  * Encoder setup
    * Check the encoder rotation direction, press encoder for reversed setup.
  * TR/CV Test: 
    * Test/Verify your TRIG or CV inputs.
  * DAC Test: 
    * All outputs shoud have 0V.

## **⦾ Midi-Expander**
   
   Midi-In is supported on the trigger-port TR1. Although the connection does not comply with the [MIDI standard](https://minimidi.world/?fbclid=IwAR31TqOyRkvdwaLYCxoU2a89hcy2PF3hltCtRKD7IzD5HbZqzn3m9NmiZzc#types) - for me this solution is more practical than the alternative via USB.
   <img src="https://github.com/eh2k/squares-and-circles/raw/main/doc/midi2ts.png" width=80% />

### Midi-Control
  * Engines can be loaded/selected by midi program change
  * Midi channel configurable per track `*` ([Long Press Right-Button] enters the config-page)
  * Note-On behaviour configurable (HOLD, TRIGGER) 
  * Paramter CC-Mappings
    ````
    | HEX | DEC | parameter-index | CH |
    |-----|-----|-----------------|----|
    |  20 |  32 |        0        |  * |
    |  21 |  33 |        1        |  * |
    |  22 |  34 |        2        |  * |
    |  23 |  35 |        3        |  * |
    |  24 |  36 |        4        |  * |
    |  25 |  37 |        5        |  * |
    |  26 |  38 |        6        |  * |
    |  27 |  39 |        7        |  * |
    ````


## ⧉ Conclusions and the future 
 
The project was originally a kind of research that I did over half a year. The current O_C hardware could certainly be optimised. As you know, the DAC and the display share the SPI port - this is not ideal for simultaneous operation. Furthermore, the Teensy 4.0 does not have "high-end" ADCs - my focus here was to achieve operation at audio rate - the issue of noise has not been the focus so far. ADC/DAC calibration is still on my to-do list.

At the moment I want to make the project available to the community as open-source, so that everyone has the possibility to adapt and experiment with it. 
In principle, this project is a suite of apps so-called machines/engines interfacing with a system library ("libmachine").
You are welcome for any kind of feedback or colleboration. Lets extend the engine collection - where is about 60% free space at the moment.

The application code respectively the suite of machines/engines is released under the MIT licence. 

The previously mentioned "libmachine", a hardware abstraction layer, will remain "closed software" until I follow some not yet discarded ideas. This is to prevent the firmware from being ported to similar digital Eurorack modules and some theoretical licensing questions. So if you consider commercially distributing hardware with this firmware, please contact me (eh2k◯gmx.de). 

<!--
````
 _______________
|***************|
|*             *|
|*             *|
|***************|
|               |
| [BL]     [BR] |
|               |
| (EL)     (ER) |
|               |
|( ) ( ) ( ) ( )|
|               |
|( ) ( ) ( ) ( )|
|               |
|( ) ( ) ( ) ( )|
|_______________|
````
-->
