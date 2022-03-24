# Software Installation Instructions
## For the GC>MIDI/N64>MIDI MIDI Interfaces

This guide will help with installing the software if you build one of these MIDI Interfaces for yourself, and is intended for those less familiar using Arduino IDE. If you're already familiar with the IDE, the ReadMe should have all the info you need. You might find this a bit tedious as it's meant for beginners.

All images use the GC>MIDI sketch as an example, but the steps are exactly the same for the N64>MIDI.

This guide is also helpful if you purchased an interface from me, but want to tweak the code yourself. I highly reccomend following the full guide in the order presented below and make sure that the device is fully functional before changing any code as that will prevent any headaches if you jump right to the "Change the Code" section.

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
   * Under Tools > Board check to make sure Arduino AVR Boards shows in the menu as shown in the first image.

## Install the required Libraries using Library Manager:
Before installing the code (or sketch in Arduino lingo), we need to add a few libraries that my sketch uses. I'll include links to these libraries below for credit purposes; but the easiest way to install them is using the Library Manager, which I will explain below:

The Libraries you will need are:
 * [MIDI Library - Francois Best](https://www.arduino.cc/reference/en/libraries/midi-library/)
 * [USB MIDI Library - Gary Grewal](https://github.com/arduino-libraries/MIDIUSB)
 * [Nintendo Library - NicoHood](https://github.com/NicoHood/Nintendo)
 
To Install these libraries using Library Manager:
* In the Arduino IDE, select Sketch > Include Library > Manage Libraries
  
  ![image](https://user-images.githubusercontent.com/69469205/159942666-6f7c19db-a972-4514-a39f-a4e47fee91e9.png)

* In the Library Manager, search for the Name of the Library.
* Scroll until you find the correct one. Use the name of the author to verify it's the same one I link above.
  * In this example, I searched Nintendo, which has many results.
  * But if I scroll down, I can find the Library with the exact name "Nintendo" and verify it is by NicoHood:
    
    ![image](https://user-images.githubusercontent.com/69469205/159943739-6929f3c5-f76d-432e-b00d-9524a1e556e4.png)
    
* Click Install to install the Library. Note that your screen may look slightly different than the picture above as I already have the Library installed.
* Repeat this process for all three libraries listed above.

## Download/Copy and Paste the Code:
Now that we have our Boards and Libraries ready to go, we can start with some code! Follow the instructions below to get everything copied over:

* If this is your first time using Arduino IDE, odds are you have a sketch that looks like this open by default already:

  ![image](https://user-images.githubusercontent.com/69469205/159945718-b35566f2-3959-482e-b19d-0640e745fce4.png)

* If so, you are ready to go. If you have another sketch showing, go ahead and open a new one by selecting File > New and you should get something like the picture above.
* Save and name your sketch whatever you would like by selecting File > Save As. In this example I've named it MIDI_Instructions
* Next, go ahead and erase the default sketch code so that your sketch is completely blank:
  
  ![image](https://user-images.githubusercontent.com/69469205/159946397-f8370afc-f682-4721-8382-4e6fb441fded.png)

* In a new tab, open up the Code for whichever interface you are programming. Make sure you copy the right code for your controller or it won't work! 
  * [GC>MIDI](https://github.com/po8aster/GCMIDIController/tree/master/Code)
  * [N64>MIDI](https://github.com/po8aster/N64MIDIController/tree/master/Code)
* The easiest way to bring the code over is just to copy and paste, which is what I will do here. But for the record you could download the sketch and load it into the IDE directly if you already know how to do that.
* Open the latest version of my code by clicking the Filename.
* Once the code is open, click Copy Raw Contents to copy the whole thing:

  ![image](https://user-images.githubusercontent.com/69469205/159947552-3630e942-5738-4bdf-b687-213bde67ba92.png)

* Now paste the code into your empty sketch:

  ![image](https://user-images.githubusercontent.com/69469205/159947868-d6b882e4-913f-49f4-bafb-6555e3d35458.png)

* Now save your sketch using File > Save to make sure you don't lose your work!

## Upload the Code to the Device:
Almost there! Last thing we need to do is upload our sketch:

* In the Arduino IDE, select the Verify button that looks like a check mark
  * You should see something that looks like the image below once it is done verifying.
  * If any errors show up at this point, check to make sure the correct Libraries and Boards are installed. The error message will usually help to figure out what is going wrong.

    ![image](https://user-images.githubusercontent.com/69469205/159948878-a1a17946-805b-432e-8d47-ca0b5c9dc355.png)

* If no errors pop up, you are good to upload!
* Make sure that the Arduino is connected to your PC using a data cable (not a power only one).
* Now, click the Upload button that looks like a sideways arrow.
  * If you get any errors at this point, check the connection between your board and the computer and make sure that the correct Libraries and Boards are installed. The error message will usually help point you in the right direction.
  * If your board is connected but the IDE isn't recognizing it, sometimes closing and restarting the Arduino IDE helps fix that problem.
  * If you get "Port is Busy" type errors, make sure to close any other software that you might have running and then try again. DAWs will often cause this error, but I've had it happen with games, modding software, and even graphics programs. So just shut off everything but the IDE if you get this error.

## Test!
You should be all set! Now test out your device and make sure it's working!

Note that if a controller is not connected, the device is programmed to wait for a controller before doing anything else. So if the Drum Mode light doesn't change when you press the Mode button, it is most likely not recognizing a controller. Check the controller connections if you run into this.

## Optional: Change the Code
Before changing the code, make sure your device works! That will rule out hardware/library/etc issues if your code runs into any errors. And if you're anything like me errors are almost certainly going to happen at some point. So please save yourself a headache and test everything with my code before you change anything!

Once you've tested it out, now you can change the sketch to your liking! I've tried to make this process easy by putting all the configuration settings at the beginning of the code with lots of good descriptions to tell you what those settings do and how to change them, so you should be able to tweak a whole lot even if you don't know a thing about code or programming.

If you scroll down past the configuration settings you can absolutely reprogram the funtions themselves but I don't reccomend doing this unless you have experience with coding. 

And if you do have experience coding don't laugh to hard at how bad my code is. It works, okay?

## Optional: Change the Device Name
Once you have everything working how you want, you can change the name of your device to show as whatever you want instead of "Arduino Leonardo". This is especially helpful if you want to use multiple MIDI interfaces with one DAW as giving them different names will prevent any confusion.

You can get some really weird errors when doing this, so I can't emphasize enough that you should make sure everything else is working correctly before trying to change the name!

If you want to change the USB MIDI Device Name after you've got it working, there is an excellent guide [here](http://liveelectronics.musinou.net/MIDIdeviceName.php) on how to do so. I am not quite sure how to credit the creator properly, but it was extremely helpful for me!
