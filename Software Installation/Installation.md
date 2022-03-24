# Software Installation Instructions
## GC>MIDI Gamecube MIDI Controller

This guide will help with installing the software when you build your MIDI Interface, and is intended for those less familiar using Arduino IDE. If you're already familiar with the IDE, the ReadMe should have all the info you need, you might find this a bit tedious as it's meant for beginners.

## Install the Arduino IDE:
This step should be pretty straightforward, and there are lots of guides out there if you get stuck. So I will simply say to install the Arduino IDE.

You can use this link, or just Google "Install Arduino IDE" if this link ends up breaking down the road: [https://docs.arduino.cc/software/ide-v1](https://docs.arduino.cc/software/ide-v1)

## Install the Arduino Leonardo board using Boards Manager:
Once you have the IDE installed and open, the first thing we will do is make sure the Arduino Leonardo board support is installed. Note that you can use other boards if you know what you're doing; but since this guide is intended for beginners I will assume you are using the same Leonardo/Pro-Micro board that I use.

* In the Arduino IDE, open Tools > Board > Boards Manager
  
  ![image](https://user-images.githubusercontent.com/69469205/159937764-fea0df9d-dbef-4b32-8f78-6643f8db8e82.png)
 
* In the Boards Manager, make sure the Arduino AVR Boards package is installed:
  * Search "Arduino AVR" in the Search Bar
  * Select the latest version from the dropdown menu and click Install (note that Install might be greyed out if you already have the latest version).
      
      ![image](https://user-images.githubusercontent.com/69469205/159939755-4ff138a8-8c71-4bb8-a80f-4e4f56ade881.png)
 
 * Check to make sure the Arduino Leonardo is now available.
   * Under Tools > Board check to make sure Arduino AVR shows in the menu as shown in the first image.

## Install the required Libraries using Library Manager:

## Download/Copy and Paste the Code:

## Upload the Code to the Device:

##Test!

## Software:
* See the [Code](https://github.com/po8aster/GCMIDIController/tree/master/Code) section to download or copy the code.
  * If you aren't familiar with Ardunio IDE, loading sketches, and loading libraries there are lots of great tutorials out there, and I won't try to recreate that info here. But Google those terms and you should be on your way!
* Required Libraries:
  * [MIDI Library - Francois Best](https://www.arduino.cc/reference/en/libraries/midi-library/)
  * [USB MIDI Library - Gary Grewal](https://github.com/arduino-libraries/MIDIUSB)
  * [Nintendo Library - NicoHood](https://github.com/NicoHood/Nintendo)
* If you want to change the USB MIDI Device Name after you've got it working, there is an excellent guide [here](http://liveelectronics.musinou.net/MIDIdeviceName.php) on how to do so. I am not quite sure how to credit the creator properly, but it was extremely helpful for me.

