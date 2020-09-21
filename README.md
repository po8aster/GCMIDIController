# GC>MIDI
## Gamecube MIDI Controller

GC>MIDI is an Arduino-based interface that takes input from any standard Gamecube Controller, translates those inputs to MIDI notes and CC data, and transmits them via both USB and 5-pin MIDI.

GC>MIDI has three modes, a Drum Mode, a Synth Mode, and a mode specifically for the Donkey Konga Bongo controller. See the [Controller Mapping](https://github.com/po8aster/GCMIDIController/tree/master/Controller%20Mapping) section for details on how each mode functions. Pressing the button switches between modes, indicated by the LEDs.

## Software:
* See the [Code](https://github.com/po8aster/GCMIDIController/tree/master/Code) section to download or copy the code.
  * If you aren't familiar with Ardunio IDE, loading sketches, and loading libraries there are lots of great tutorials out there, and I won't try to recreate that info here. But Google those terms and you should be on your way!
* Required Libraries:
  * [MIDI Library - Francois Best](https://www.arduino.cc/reference/en/libraries/midi-library/)
  * [USB MIDI Library - Gary Grewal](https://github.com/arduino-libraries/MIDIUSB)
  * [Nintendo Library - NicoHood](https://github.com/NicoHood/Nintendo)
* If you want to change the USB MIDI Device Name after you've got it working, there is an excellent guide [here](http://liveelectronics.musinou.net/MIDIdeviceName.php) on how to do so. I am not quite sure how to credit the creator properly, but it was extremely helpful for me.

## Hardware:
* See the [Schematic](https://github.com/po8aster/GCMIDIController/tree/master/Schematic) section for wiring diagram.
* Parts list:
  * Arduino Pro Micro - 5v Version
  * 220 Ohm Resistor x3 (1/4 watt is fine)
  * Bi-Directional Logic Level Converter (it doesn't actually have to be bi-directional, but that's what's in my wiring diagram so just go with this if you don't know the different wiring for a unidirectional one)
  * 5v to 3.3v Voltage Regulator Module (this module contains all the capacitors etc for voltage conversion. You can of course create your own, but these things are dirt cheap so if you aren't sure how to do that I'd just grab the little module)
  * LEDs x2 (choose whatever color you want, but for what it's worth I use Green for Drum Mode and Blue for Synth Mode)
  * Momentary Pushbutton
  * MIDI (5 Pin DIN) Connector
  * Gamecube Controller Extension Cable
* For more details on hooking things up, take a look at NicoHood's library linked above. My project simply modifies that project to convert inputs to MIDI signals on the code side, so reading over those guides is well worth your time.
  
#### If you build it and like it, please consider a small donation using the Sponsor button to buy me a beer for my efforts. Obviously not required at all, but always appreciated! 
