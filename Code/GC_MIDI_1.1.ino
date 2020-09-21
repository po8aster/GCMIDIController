/*
  Gamecube MIDI Controller v1.1
  By Po8aster
  Based on the GamecubeController example from the Nintendo library by NicoHood

  Required Libraries (as named in Library Manager):
  Nintendo by NicoHood
  MIDI Library by Francois Best
  MIDIUSB by Gary Grewal
*/
//----------------------------------------------------------------
//Include the Nintendo Library
#include "Nintendo.h"
//Include USB and Traditional MIDI Libraries
#include "MIDIUSB.h"
#include "MIDI.h"
#include <pitchToNote.h>  // include pitch chart from MIDIUSB library

//This makes the traditional MIDI Work
MIDI_CREATE_DEFAULT_INSTANCE();
//----------------------------------------------------------------
//*** Definitions to Update Settings/Channels/Etc
//Intended to be easily modifiable to customize the controller to your needs/preferences without having to get too deep in my awful code

// Define a Gamecube Controller and set Pin
  CGamecubeController GamecubeController(7);

// Pin definitions (only change these if you built yours differently than my schematic)
  int drumLED = 4; // pin for the drum mode LED
  int synthLED = 5; // pin for the synth mode LED
  int buttonPin = 10; // pin for the mode change button

//MIDI Channel Definitions (defaults to Channel 1 for Synth and Channel 10 for Drums on both USB and Traditional MIDI but can be changed as desired)
  int drumChU = 9; //channel for USB in Drum Mode (0-15)
  int drumChT = 10; //channel for Traditional in Drum Mode (1-16)
  int synthChU = 0; //channel for USB in Synth Mode (0-15)
  int synthChT = 1; //channel for Traditional in Synth Mode (1-16)
  byte pbUSB = 0xE0; // set last digit to channel (0-15) for pitch bend messages via USB (match synthChU in most cases)ie 0xE1 for Channel 2
  byte pbT = synthChT; // channel for Traditional Pitch Bend messages (defaults to same channel as notes, manually change to 1-16 if different channel desired)
  int dkChU = 9; //channel for USB in DK Mode (0-15)
  int dkChT = 10; //channel for Traditional in DK Mode (1-16)

//Trigger Sensitivity. Determines the value at which a trigger press is registered. Lower values raise sensitivity.
  int rThreshold = 45; //40 reccomended minimum, otherwise will always register. Increase to make trigger pulls less sensitive
  int rMax = 175; // max value for mapping, the lower the number the sooner a trigger pull will hit max value. Default of 175 sends max value just before click (continues to send max after click). 225 will send ~100 before click, 127 after (value will jump when clicked). 
  int lThreshold = 45; //40 reccomended minimum, otherwise will always register. Increase to make trigger pulls less sensitive
  int lMax = 175; // max value for mapping, the lower the number the sooner a trigger pull will hit max value. Default of 175 sends max value just before click (continues to send max after click). 225 will send ~100 before click, 127 after (value will jump when clicked).

//Drum Mode CC Definitions (defaults to Modulation control)
  //Change these as needed to change the function of the Joystick X axis in Drum Mode
  int controlVal = 1; //sets which CC message is controlled by the X axis of the joystick in Drum Mode (default 1, Modulation)

  //When true the joystick will default to zero. Pushing in either direction will move the value up towards the maximum control value. When false the joystick will default to the midpoint between min and max values. Pushing the stick left will make the value go down, right will go up.
  //TL;DR if your control defaults to 0, leave True. If your control defaults to 64, change to False.
  bool alwaysUp = false; 

  //Change these to adjust sensitivity (note that if alwaysUp is False, your control will default to the center value between these points)
  // Set 1/127 for a control with a neutral value of 64 without rounding issues
  int minVal = 1; //minimum value to send for selected control
  int maxVal = 127; //maximum value to send for selected control
  //Change these for Channel values
  int ccChU = drumChU; //sets channel for sending the CC message on USB in Drum Mode. Defaults to Drum Mode Channel but can be manually changed to 0-15 if desired
  int ccChT = drumChT; //sets channel for CC on Traditional MIDI in Drum Mode. Defaults to same channel as the Mode but can be manually changed to 1-16 if desired

//Synth Mode CC Values
  int synthXCC = 10; //CC number for x axis in synth mode, default is 10, Pan
  bool synthAlwaysUp = false; //see Drum Mode CC definitions, but set true far left = 127; false far left = 0
  int synthXMin = 1; //minimum value to send for selected control
  int synthXMax = 127; //maximum value to send for the selected control
  //set CC channels, default to synth channel
  int synthXChU = synthChU; //sets channel for sending the CC message USB, 0-15
  int synthXChT = synthChT; //sets channel for CC on Traditional MIDI, 1-16
  //Right Trigger CC values
  int synthRCC = 1; //set CC number, defaults to 1 Modulation
  int synthRMin = 0; //Minimum value, sent when trigger is not pressed so map to 0 for off
  int synthRMax = 127; //Maximum value, sent when trigget is completely pressed, clicked in
  //Set channels for Trigger CC
  int synthRChU = synthChU; //sets channel for sending the CC message USB, 0-15
  int synthRChT = synthChT; //sets channel for CC on Traditional MIDI, 1-16

//Velocity Definitions
  int vel = 64; //Default Velocity at startup, will not change performance
  int velFloor = 25; //Set velocity floor (giving full 0-127 range makes downward very sensitive. Reduce this value to increase sensitivity, increase to reduce sensitivity). Neutral velocity will change to midpoint between floor and 127
  
  //DK Mode Velocity
  //Set the velocity for notes played in DK Mode
  int dkVelocity = 100;
  int clapFloor = 70; //sets the minimum velocity for the Clap in DK mode. Will scale between this value and 127 based on volume of clap
  int clapThreshold = 125; //sets the threshold at which a clap is registered. Decrease to make more sensitive
  
  //Synth Mode Velocity
  int synthVelFloor = 64; //lowest possible velocity, sent when trigger not pulled. Pulling trigger increases up to 127

//Pitch Bend Definitions (set high and low limits for pitch bend, Defaults to 4096/12288 = 1/2 wheel range, 1/16383 is max wheel range)
  int bendLow = 4096;
  int bendHigh = 12288;

//Octave Limits (defaults to two octaves up or down from middle C in synth mode but can be adjusted as desired)
  int octaveMin = -2; //number of octaves down allowed
  int octaveMax = 2; //number of octaves up allowed
  //Transpose limits
  int transposeMin = -11; //number of half-steps down allowed
  int transposeMax = 11; //number of half-steps up allowed

//***Button to Note Mapping 
//Change number to correspond to desited MIDI Note (ie Middle C = 60). Z holds in synth mode will be 1 half-step down from whatever you map the root button to.

//Drum Mode
  int drumA = 38; //snare 2
  int drumB = 40; //snare
  int drumX = 46; //open hat
  int drumY = 44; //pedal hat
  int drumR = 35; //kick
  int drumL = 42; //closed hat
  int drumZ = 36; //kick 2
  int drumStart = 52; //chinese
  
  //D Pad
  int drumDUp = 56; //cowbell
  int drumDUpR = 50; //hi tom
  int drumDR = 48; //hi-mid tom
  int drumDDownR = 47; //low-mid tom
  int drumDDown = 45; //low tom
  int drumDDownL = 43; //hi-floor tom
  int drumDL = 41; //low-floor tom
  int drumDUpL = 54; //tambourine
  
  //C Buttons
  int drumCUp = 44; //pedal hat
  int drumCUpR = 46; //open hat
  int drumCR = 39; //clap
  int drumCDownR = 37; //side stick
  int drumCDown = 51; //ride
  int drumCDownL = 55; //splash
  int drumCL = 49; //crash
  int drumCUpL = 53; //ride bell

//Synth Mode
  //Face Buttons - Drums
  int synthA = 36; //Kick
  int synthB = 38; //Snare
  int synthX = 39; //Clap
  int synthY = 42; //Closed Hat
  
  //C Buttons
  int synthCDown = 60; //C
  int synthCDownL = 62; //D
  int synthCL = 64; //E
  int synthCUpL = 65; //F
  int synthCUp = 67; //G
  int synthCUpR = 69; //A
  int synthCR = 71; //B
  int synthCDownR = 72; //C

//DK Mode
  //Drum Kit
  int dkB = 35; //Kick
  int dkY = 38; //Snare
  int dkX = 42; //Closed Hat
  int dkAB = 46; //Open Hat
  int dkAY = 51; //Ride
  int dkAX = 44; //Pedal Hat
  int dkBY = 53; //Ride Bell
  int dkABY = 49; //Crash
  int dkABYX = 52; //Chinese
  int dkClap = 39; //Clap

  //Bongo Kit
  int dkbA = 61; //Low Bongo
  int dkbB = 60; //High Bongo
  int dkbY = 65; //High Timbale
  int dkbX = 66; //Low Timbale
  int dkbABYX = 78; //Cuica High
  int dkbClap = 39; //Clap
  
  

//----------------------------------------------------------------
// *** Here Be Dragons, the below is not intended to be easily modified but is where you will go to change how the thing works instead of simple tweaks

//Octave Control Definitions
  int octave = 0; //initial octave (0 = MIDI 4, starting at Middle C)
  int octaveUp = 0; // used to hold octave change value
  int octaveDown = 0;
  int octaveUpLast = 0; // used to detect change in octave
  int octaveDownLast = 0;
  //Transpose Defs
  int transpose = 0;
  int transposeUp = 0;
  int transposeUpLast = 0;
  int transposeDown = 0;
  int transposeDownLast = 0;

//Mode bool definitions
  bool drumMode = true;
  bool synthMode = false;
  bool dkMode = false;
  bool timeout = false;
  int timeoutLength = 500; //set the delay time for mode switch timeout

//State tracking for Mod/CC, Reset, Mode Switch, and Pitch Bend
  int ccState = 0;
  int ccStateLast = 0;
  int synthXState = 0;
  int synthXStateLast = 0;
  int synthRState = 0;
  int synthRStateLast = 0;
  int resetState = 0;
  int resetStateLast = 0;
  int modeState = 0;
  int modeStateLast = 0;
  int pbState = 0;
  int pbStateLast = 0;

//DK Mode Params for Clap velocity and Kit
  int clapVel = clapFloor;
  int bongo = 0;
  int clapOff = 0;

//Stick neutral padding parameters
  int xNeutral = 121; //the value of the left stick X axis when not held
  int xPad = 10; //how far the x value has to change up or down as registering as anything other than neutral
  int xVal = xNeutral; //holds padded value
  int yNeutral = 125; //the value of the left stick Y axis when not held
  int yPad = 10; //how far the x value has to change up or down as registering as anything other than neutral
  int yVal = yNeutral; //holds padded value

//C Stick Calibration Parameters
  int cPadding = 29; //pad determines how close to the X and Y vals for each direction. The lower the number the more precise you have to be to trigger the direction
  int xUp = 122;
  int yUp = 215;
  int xUpR = 188;
  int yUpR = 192;
  int xR = 215;
  int yR = 122;
  int xDownR = 194;
  int yDownR = 57;
  int xDown = 125;
  int yDown = 32;
  int xDownL = 54;
  int yDownL = 61;
  int xL = 36;
  int yL = 128;
  int xUpL = 64;
  int yUpL = 190;

//State Tracking for buttons
  //Face buttons
  int buttAState = 0;
  int buttAStateLast = 0; 
  int buttBState = 0;
  int buttBStateLast = 0;
  int buttXState = 0;
  int buttXStateLast = 0; 
  int buttYState = 0;
  int buttYStateLast = 0;
  
  //Combo states for DK mode
  int buttAXState = 0;
  int buttAXStateLast = 0;
  int buttABState = 0;
  int buttABStateLast = 0;
  int buttAYState = 0;
  int buttAYStateLast = 0;
  int buttBYState = 0;
  int buttBYStateLast = 0;
  int buttABYState = 0;
  int buttABYStateLast = 0;
  int buttABYXState = 0;
  int buttABYXStateLast = 0;
  
  //Back Buttons/Start
  int buttLState = 0;
  int buttLStateLast = 0;
  int buttRState = 0;
  int buttRStateLast = 0;
  int buttZState = 0;
  int buttZStateLast = 0;
  int buttStartState = 0;
  int buttStartStateLast = 0;
  
  //L and R States for Pressure Sensitivity
  int sliderLState = 0;
  int sliderLStateLast = 0;
  int sliderRState = 0;
  int sliderRStateLast = 0;
  
  //D Pad
  int buttDUpState = 0;
  int buttDUpStateLast = 0;
  int buttDUpRState = 0;
  int buttDUpRStateLast = 0;
  int buttDRState = 0;
  int buttDRStateLast = 0;
  int buttDDownRState = 0;
  int buttDDownRStateLast = 0;
  int buttDDownState = 0;
  int buttDDownStateLast = 0;
  int buttDDownLState = 0;
  int buttDDownLStateLast = 0;
  int buttDLState = 0;
  int buttDLStateLast = 0;
  int buttDUpLState = 0;
  int buttDUpLStateLast = 0;
  
  //C "Buttons"
  int buttCUpState = 0;
  int buttCUpStateLast = 0;
  int buttCUpRState = 0;
  int buttCUpRStateLast = 0;
  int buttCRState = 0;
  int buttCRStateLast = 0;
  int buttCDownRState = 0;
  int buttCDownRStateLast = 0;
  int buttCDownState = 0;
  int buttCDownStateLast = 0;
  int buttCDownLState = 0;
  int buttCDownLStateLast = 0;
  int buttCLState = 0;
  int buttCLStateLast = 0;
  int buttCUpLState = 0;
  int buttCUpLStateLast = 0;
  
  //Z held states for synth mode
  int buttCZUpState = 0;
  int buttCZUpStateLast = 0;
  int buttCZUpRState = 0;
  int buttCZUpRStateLast = 0;
  int buttCZRState = 0;
  int buttCZRStateLast = 0;
  int buttCZDownRState = 0;
  int buttCZDownRStateLast = 0;
  int buttCZDownState = 0;
  int buttCZDownStateLast = 0;
  int buttCZDownLState = 0;
  int buttCZDownLStateLast = 0;
  int buttCZLState = 0;
  int buttCZLStateLast = 0;
  int buttCZUpLState = 0;
  int buttCZUpLStateLast = 0;

//----------------------------------------------------------------
//*** MIDI Functions
//Note Functions
// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

//CC Function
// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

//Pitch Bend Function
void midiCommand(byte cmd, byte data1, byte  data2) {
  // First parameter is the event type (top 4 bits of the command byte).
  // Second parameter is command byte combined with the channel.
  // Third parameter is the first data byte
  // Fourth parameter second data byte

  midiEventPacket_t midiMsg = {cmd >> 4, cmd, data1, data2};
  MidiUSB.sendMIDI(midiMsg);
}

//----------------------------------------------------------------
//Mode Toggle Function
void toggleMode(){
  //set timeout
  timeout = true;
  
  // trigger all notes off
  allNotesOff();
  
  //If in drum mode, switch to synth and change LEDs
  if(drumMode==true){
    drumMode = false;
    synthMode = true;
    digitalWrite(drumLED, LOW);
    digitalWrite(synthLED, HIGH);
  }
  //If in synth mode, switch to DK
  else if(synthMode==true){
    dkMode = true;
    synthMode = false;
    digitalWrite(drumLED, HIGH);
    digitalWrite(synthLED, HIGH);
    //Set DK Velocity - this value is static so just need to set it once
    vel = dkVelocity;
    
  }
  //If in DK mode, switch to drum
  else if(dkMode==true){
    dkMode = false;
    drumMode = true;
    digitalWrite(drumLED, HIGH);
    digitalWrite(synthLED, LOW);
  }
  //wait then toggle timeout
  delay(timeoutLength);
  timeout = false;
}

//All Notes Off on all channels
void allNotesOff(){
  //Send all notes off (CC 123) to prevent held notes
  //Sends to all channels for all modes

  //Drum Mode
  //Traditional
  MIDI.sendControlChange(123, 0, drumChT); //CC, value, channel (1-16)
  //USB
  controlChange(drumChU, 123, 0); //channel (0-15), CC, value
  MidiUSB.flush();

  //Synth Mode
  //Traditional
  MIDI.sendControlChange(123,0,synthChT); //CC, value, channel (1-16)
  //USB
  controlChange(synthChU, 123, 0); //channel (0-15), CC, value
  MidiUSB.flush();

  //DK Mode
  //Traditional
  MIDI.sendControlChange(123, 0, dkChT); //CC, value, channel (1-16)
  //USB
  controlChange(dkChU, 123, 0); //channel (0-15), CC, value
  MidiUSB.flush();
}

//Change DK Kits
void kitToggle(){
  //stop all notes to prevent held notes
  allNotesOff();
  
  //If in Drum Kit with clap on, disable clap
  if(bongo == 0 && clapOff == 0){
    clapOff = 1;
  }
  //If in Drum Kit with clap off, switch to Bongo, clap on
  else if(bongo == 0 && clapOff == 1){
    clapOff = 0;
    bongo = 1;
  }
  //If in Bongo Kit with clap on, disable clap
  else if(bongo == 1 && clapOff == 0){
    clapOff = 1;
  }
  //If in Bongo Kit with clap off, go back to Drum, clap on
  else if(bongo == 1 && clapOff == 1){
    clapOff = 0;
    bongo = 0;
  }
}

//Get Padded Joystick Values
int joyY(int yAxis){
 //Uncomment to show vals for yAxis (25 all down, 225 all up, 125 neutral)
  //Serial.println(gc_report.yAxis, DEC);
//If the value is outside of the padded range, return the value plus or minus the pad to prevent jumpiness when leaving the pad range
if(yAxis < (yNeutral + yPad) && yAxis > (yNeutral - yPad)){
  yVal = yNeutral;
}else{
    if (yAxis>yNeutral){
      yVal = yAxis - yPad;
    }else if (yAxis<yNeutral){
      yVal = yAxis + yPad;
    }
}
return yVal;
//Values returned: +/- 100
}

int joyX(int xAxis){
 //Uncomment to show vals for yAxis (25 all down, 225 all up, 125 neutral)
  //Serial.println(gc_report.yAxis, DEC);
//If the value is outside of the padded range, return the value plus or minus the pad to prevent jumpiness when leaving the pad range
if(xAxis < (xNeutral + xPad) && xAxis > (xNeutral - xPad)){
  xVal = xNeutral;
}else{
    if (xAxis>xNeutral){
      xVal = xAxis - xPad;
    }else if (xAxis<xNeutral){
      xVal = xAxis + xPad;
    }
}
return xVal;
//Values returned: 
}

//----------------------------------------------------------------
//Functions for sending Drum and Synth Notes
void synthNoteOn(int note, int octave, int transpose){ 
    //Send on USB
    noteOn(synthChU, (note + (octave*12) + transpose), vel);   // Channel (0-15), pitch, velo
    
    MidiUSB.flush();
    //Send on Traditional MIDI
    MIDI.sendNoteOn((note + (octave*12) + transpose), vel, synthChT); // pitch, velo, channel (1-16)
  }

void drumNoteOn(int note){ 
    //Send on USB
    noteOn(drumChU, note, vel);   // Channel (0-15), pitch, velo
    MidiUSB.flush();
    //Send on Traditional MIDI
    MIDI.sendNoteOn(note, vel, drumChT); // pitch, velo, channel (1-16)
  }
  
void dkNoteOn(int note){ 
    //Send on USB
    noteOn(dkChU, note, vel);   // Channel (0-15), pitch, velo
    MidiUSB.flush();
    //Send on Traditional MIDI
    MIDI.sendNoteOn(note, vel, dkChT); // pitch, velo, channel (1-16)
  }

void synthNoteOff(int note, int octave, int transpose){
    // NoteOff for USB
    noteOff(synthChU, note + (octave*12) + transpose, vel);  // Channel (0-15), pitch, velo
    MidiUSB.flush();
    // NoteOff for Traditional MIDI
    MIDI.sendNoteOff(note + (octave*12) + transpose, vel, synthChT); // pitch, velo, channel (1-16)
 }

void drumNoteOff(int note){
    // NoteOff for USB
    noteOff(drumChU, note, vel);  // Channel (0-15), pitch, velo
    MidiUSB.flush();
    // NoteOff for Traditional MIDI
    MIDI.sendNoteOff(note, vel, drumChT); // pitch, velo, channel (1-16)
 }

 void dkNoteOff(int note){
    // NoteOff for USB
    noteOff(dkChU, note, vel);  // Channel (0-15), pitch, velo
    MidiUSB.flush();
    // NoteOff for Traditional MIDI
    MIDI.sendNoteOff(note, vel, dkChT); // pitch, velo, channel (1-16)
 }

//----------------------------------------------------------------
//Functions for setting states and triggering notes

//set C State against padded values
int getCState(Gamecube_Report_t gc_report, int x, int y){
  if((gc_report.cxAxis < x + cPadding && gc_report.cxAxis > x - cPadding) && (gc_report.cyAxis < y + cPadding && gc_report.cyAxis > y - cPadding)){
    return 1;
  }else{
    return 0;
  }
}

//set C Stick states
void setCStates(Gamecube_Report_t gc_report){
  //Uncomment to read C-Stick values 
  //Serial.print(gc_report.cxAxis, DEC);
  //Serial.print(" / ");
  //Serial.println(gc_report.cyAxis, DEC);
 
  //Check C values against padded settings to return state
  buttCUpState = getCState(gc_report, xUp, yUp);
  buttCUpRState = getCState(gc_report, xUpR, yUpR);
  buttCRState = getCState(gc_report, xR, yR);
  buttCDownRState = getCState(gc_report, xDownR, yDownR);
  buttCDownState = getCState(gc_report, xDown, yDown);
  buttCDownLState = getCState(gc_report, xDownL, yDownL);
  buttCLState = getCState(gc_report, xL, yL);
  buttCUpLState = getCState(gc_report, xUpL, yUpL);
}

//setCStates for Synth Mode
void synthCStates(Gamecube_Report_t gc_report){
  //Read C Position values to temporary ints
  int buttCUp = getCState(gc_report, xUp, yUp);
  int buttCUpR = getCState(gc_report, xUpR, yUpR);
  int buttCR = getCState(gc_report, xR, yR);
  int buttCDownR = getCState(gc_report, xDownR, yDownR);
  int buttCDown = getCState(gc_report, xDown, yDown);
  int buttCDownL = getCState(gc_report, xDownL, yDownL);
  int buttCL = getCState(gc_report, xL, yL);
  int buttCUpL = getCState(gc_report, xUpL, yUpL);
  
  //Check temp states and Z state to see which state is true
  //Up
  //Z Hold
  if(buttZState == 1 && buttCUp == 1){
    buttCZUpState = 1;
  }else{
    buttCZUpState = 0;
  }
  //Reg
  if(buttZState == 0 && buttCUp == 1){
    buttCUpState = 1;
  }else{
    buttCUpState = 0;
  }

  //Up + R
    if(buttZState == 1 && buttCUpR == 1){
    buttCZUpRState = 1;
  }else{
    buttCZUpRState = 0;
  }
  //Reg
  if(buttZState == 0 && buttCUpR == 1){
    buttCUpRState = 1;
  }else{
    buttCUpRState = 0;
  }

   //R
    if(buttZState == 1 && buttCR == 1){
    buttCZRState = 1;
  }else{
    buttCZRState = 0;
  }
  //Reg
  if(buttZState == 0 && buttCR == 1){
    buttCRState = 1;
  }else{
    buttCRState = 0;
  }

   //Down + R
    if(buttZState == 1 && buttCDownR == 1){
    buttCZDownRState = 1;
  }else{
    buttCZDownRState = 0;
  }
  //Reg
  if(buttZState == 0 && buttCDownR == 1){
    buttCDownRState = 1;
  }else{
    buttCDownRState = 0;
  }

  //Down
    if(buttZState == 1 && buttCDown == 1){
    buttCZDownState = 1;
  }else{
    buttCZDownState = 0;
  }
  //Reg
  if(buttZState == 0 && buttCDown == 1){
    buttCDownState = 1;
  }else{
    buttCDownState = 0;
  }

  //Down + L
    if(buttZState == 1 && buttCDownL == 1){
    buttCZDownLState = 1;
  }else{
    buttCZDownLState = 0;
  }
  //Reg
  if(buttZState == 0 && buttCDownL == 1){
    buttCDownLState = 1;
  }else{
    buttCDownLState = 0;
  }

  //L
    if(buttZState == 1 && buttCL == 1){
    buttCZLState = 1;
  }else{
    buttCZLState = 0;
  }
  //Reg
  if(buttZState == 0 && buttCL == 1){
    buttCLState = 1;
  }else{
    buttCLState = 0;
  }

  //Up + L
    if(buttZState == 1 && buttCUpL == 1){
    buttCZUpLState = 1;
  }else{
    buttCZUpLState = 0;
  }
  //Reg
  if(buttZState == 0 && buttCUpL == 1){
    buttCUpLState = 1;
  }else{
    buttCUpLState = 0;
  }
}

//Set D Pad States
void setDStates(Gamecube_Report_t gc_report){
  //DPad States
  //D Up
  if(gc_report.dup == 1 && gc_report.dright == 0 && gc_report.ddown == 0 && gc_report.dleft == 0){
    buttDUpState = 1;
    buttDUpRState = 0;
    buttDRState = 0;
    buttDDownRState = 0;
    buttDDownState = 0;
    buttDDownLState = 0;
    buttDLState = 0;
    buttDUpLState = 0;
  }
  //D Up Right
  else if(gc_report.dup == 1 && gc_report.dright == 1 && gc_report.ddown == 0 && gc_report.dleft == 0){
    buttDUpState = 0;
    buttDUpRState = 1;
    buttDRState = 0;
    buttDDownRState = 0;
    buttDDownState = 0;
    buttDDownLState = 0;
    buttDLState = 0;
    buttDUpLState = 0;
  }
  //D Right
  else if(gc_report.dup == 0 && gc_report.dright == 1 && gc_report.ddown == 0 && gc_report.dleft == 0){
    buttDUpState = 0;
    buttDUpRState = 0;
    buttDRState = 1;
    buttDDownRState = 0;
    buttDDownState = 0;
    buttDDownLState = 0;
    buttDLState = 0;
    buttDUpLState = 0;
  }
  //D Down Right
  else if(gc_report.dup == 0 && gc_report.dright == 1 && gc_report.ddown == 1 && gc_report.dleft == 0){
    buttDUpState = 0;
    buttDUpRState = 0;
    buttDRState = 0;
    buttDDownRState = 1;
    buttDDownState = 0;
    buttDDownLState = 0;
    buttDLState = 0;
    buttDUpLState = 0;
  }
  //D Down
  else if(gc_report.dup == 0 && gc_report.dright == 0 && gc_report.ddown == 1 && gc_report.dleft == 0){
    buttDUpState = 0;
    buttDUpRState = 0;
    buttDRState = 0;
    buttDDownRState = 0;
    buttDDownState = 1;
    buttDDownLState = 0;
    buttDLState = 0;
    buttDUpLState = 0;
  }
  //D Down Left
  else if(gc_report.dup == 0 && gc_report.dright == 0 && gc_report.ddown == 1 && gc_report.dleft == 1){
    buttDUpState = 0;
    buttDUpRState = 0;
    buttDRState = 0;
    buttDDownRState = 0;
    buttDDownState = 0;
    buttDDownLState = 1;
    buttDLState = 0;
    buttDUpLState = 0;
  }
  //D Left
  else if(gc_report.dup == 0 && gc_report.dright == 0 && gc_report.ddown == 0 && gc_report.dleft == 1){
    buttDUpState = 0;
    buttDUpRState = 0;
    buttDRState = 0;
    buttDDownRState = 0;
    buttDDownState = 0;
    buttDDownLState = 0;
    buttDLState = 1;
    buttDUpLState = 0;
  }
   //D Up Left
  else if(gc_report.dup == 1 && gc_report.dright == 0 && gc_report.ddown == 0 && gc_report.dleft == 1){
    buttDUpState = 0;
    buttDUpRState = 0;
    buttDRState = 0;
    buttDDownRState = 0;
    buttDDownState = 0;
    buttDDownLState = 0;
    buttDLState = 0;
    buttDUpLState = 1;
  }
  //Set all null otherwise
  else{
    buttDUpState = 0;
    buttDUpRState = 0;
    buttDRState = 0;
    buttDDownRState = 0;
    buttDDownState = 0;
    buttDDownLState = 0;
    buttDLState = 0;
    buttDUpLState = 0;
  }
}

//set button states for drum mode
void setStatesDrum(Gamecube_Report_t gc_report){
  //Face buttons
  buttAState = gc_report.a;
  buttBState = gc_report.b;
  buttXState = gc_report.x;
  buttYState = gc_report.y;

  //Back Buttons/Start
  buttLState = gc_report.l;
  buttRState = gc_report.r;
  buttZState = gc_report.z;
  buttStartState = gc_report.start;

  //Set D Pad States
  setDStates(gc_report);
  
  //Set C States
  setCStates(gc_report); 
}

//Set States for Synth Mode
void setStatesSynth(Gamecube_Report_t gc_report){
  //Face buttons
  buttAState = gc_report.a;
  buttBState = gc_report.b;
  buttXState = gc_report.x;
  buttYState = gc_report.y;
  buttZState = gc_report.z;

  //C Stick States
  synthCStates(gc_report);
}

//Set States for DK Mode - Drum Kit
void setStatesDKDrum(Gamecube_Report_t gc_report){
//Start State
  buttStartState = gc_report.start;

//Set Pad States
  //All Pads
  if(gc_report.a == 1 && gc_report.b == 1 && gc_report.y == 1 && gc_report.x == 1){
    buttABYXState = 1;
  }else{
    buttABYXState = 0;
  }
  //ABY
  if(gc_report.a == 1 && gc_report.b == 1 && gc_report.y == 1 && gc_report.x == 0){
    buttABYState = 1;
  }else{
    buttABYState = 0;
  }
  //BY
  if(gc_report.a == 0 && gc_report.b == 1 && gc_report.y == 1 && gc_report.x == 0){
    buttBYState = 1;
  }else{
    buttBYState = 0;
  }
  //AX
  if(gc_report.a == 1 && gc_report.b == 0 && gc_report.y == 0 && gc_report.x == 1){
    buttAXState = 1;
  }else{
    buttAXState = 0;
  }
  //AB
  if(gc_report.a == 1 && gc_report.b == 1 && gc_report.y == 0 && gc_report.x == 0){
    buttABState = 1;
  }else{
    buttABState = 0;
  }
  //AY
  if(gc_report.a == 1 && gc_report.b == 0 && gc_report.y == 1 && gc_report.x == 0){
    buttAYState = 1;
  }else{
    buttAYState = 0;
  }
  //B
  if(gc_report.a == 0 && gc_report.b == 1 && gc_report.y == 0 && gc_report.x == 0){
    buttBState = 1;
  }else{
    buttBState = 0;
  }
  //Y
  if(gc_report.a == 0 && gc_report.b == 0 && gc_report.y == 1 && gc_report.x == 0){
    buttYState = 1;
  }else{
    buttYState = 0;
  }
  //X
  if(gc_report.a == 0 && gc_report.b == 0 && gc_report.y == 0 && gc_report.x == 1){
    buttXState = 1;
  }else{
    buttXState = 0;
  }

//Clap State
  //If R value is above threshold, set clap to positive and set the velocity for the clap based on the volume
  if(gc_report.right > clapThreshold){
    buttRState = 1;
    clapVel = constrain(map(gc_report.right, clapThreshold, 200, clapFloor, 127), clapFloor, 127);
    Serial.println(gc_report.right); 
  }else{
    buttRState = 0;
  }  
}

//Set States for DK Mode - Bongo Kit
void setStatesDKBongo(Gamecube_Report_t gc_report){
//Start State
  buttStartState = gc_report.start;

//Set Pad States
  //All Pads
  if(gc_report.a == 1 && gc_report.b == 1 && gc_report.y == 1 && gc_report.x == 1){
    buttABYXState = 1;
  }else{
    buttABYXState = 0;
  }
  //A
  if(gc_report.a == 1 && gc_report.b == 0 && gc_report.y == 0 && gc_report.x == 0){
    buttAState = 1;
  }else{
    buttAState = 0;
  }
  //B
  if(gc_report.a == 0 && gc_report.b == 1 && gc_report.y == 0 && gc_report.x == 0){
    buttBState = 1;
  }else{
    buttBState = 0;
  }
  //Y
  if(gc_report.a == 0 && gc_report.b == 0 && gc_report.y == 1 && gc_report.x == 0){
    buttYState = 1;
  }else{
    buttYState = 0;
  }
  //X
  if(gc_report.a == 0 && gc_report.b == 0 && gc_report.y == 0 && gc_report.x == 1){
    buttXState = 1;
  }else{
    buttXState = 0;
  }

//Clap State
  //If R value is above threshold, set clap to positive and set the velocity for the clap based on the volume
  if(gc_report.right > clapThreshold){
    buttRState = 1;
    clapVel = constrain(gc_report.right, clapFloor, 127);
    //Serial.println(clapVel); 
  }else{
    buttRState = 0;
  }  
  
}

//Update Last States used in Drum Mode
void updateStatesDrum(){
  //Face buttons
  buttAStateLast = buttAState;
  buttBStateLast = buttBState;
  buttXStateLast = buttXState;
  buttYStateLast = buttYState;

  //Back Buttons/Start
  buttLStateLast = buttLState;
  buttRStateLast = buttRState;
  buttZStateLast = buttZState;
  buttStartStateLast = buttStartState;

  //D Pad States
  updateStatesDPad();

  //C Stick States
  updateStatesCStick();
}

//Update Last States used in Synth Mode
void updateStatesSynth(){
  //Face buttons
  buttAStateLast = buttAState;
  buttBStateLast = buttBState;
  buttXStateLast = buttXState;
  buttYStateLast = buttYState;
  buttZStateLast = buttZState;

  //C Stick States
  updateStatesCStick();

  //C Stick States Z Holds
  updateZStatesCStick();
}

//Update Last States used in DK Mode - Drum Kit
void updateStatesDKDrum(){
  //Start
  buttStartStateLast = buttStartState;
  //Pad States
  buttBStateLast = buttBState;
  buttXStateLast = buttXState;
  buttYStateLast = buttYState;
  buttAXStateLast = buttAXState;
  buttBYStateLast = buttBYState;
  buttABYXStateLast = buttABYXState;
  buttABYStateLast = buttABYState;
  buttABStateLast = buttABState;
  buttAYStateLast = buttAYState;
  //R
  buttRStateLast = buttRState;
}

//Update Last States used in DK Mode - Bongo Kit
void updateStatesDKBongo(){
  //Start
  buttStartStateLast = buttStartState;
  //Pad States
  buttAStateLast = buttAState;
  buttBStateLast = buttBState;
  buttXStateLast = buttXState;
  buttYStateLast = buttYState;
  buttABYXStateLast = buttABYXState;
  //R
  buttRStateLast = buttRState;
}

//Check states and trigger note on and off messages for drum mode
void drumTrigger(int state, int stateLast, int note){
  //Check if state has changed
  if(state != stateLast){
    //If state is 1 trigger note on
    if(state == 1){
      drumNoteOn(note);
    }
    //Send note off if not
    else{
      drumNoteOff(note);
    }
  }
}

//Check states and trigger note on and off messages for drum mode
void synthTrigger(int state, int stateLast, int note){
  
  //Check if state has changed
  if (state != stateLast){
    //If state is 1 trigger note on
    if (state == 1){
      synthNoteOn(note, octave, transpose);
    }
    //Send note off if not
    else{
      synthNoteOff(note, octave, transpose);
    }
  }
}

//Check states and trigger note on and off messages for DK Mode
//Check states and trigger note on and off messages for drum mode
void dkTrigger(int state, int stateLast, int note){
  //Check if state has changed
  if(state != stateLast){
    //If state is 1 trigger note on
    if(state == 1){
      dkNoteOn(note);
    }
    //Send note off if not
    else{
      dkNoteOff(note);
    }
  }
}

//Update D Pad Last States
void updateStatesDPad(){
  //D Pad
  buttDUpStateLast = buttDUpState;
  buttDUpRStateLast = buttDUpRState;
  buttDRStateLast = buttDRState;
  buttDDownRStateLast = buttDDownRState;
  buttDDownStateLast = buttDDownState;
  buttDDownLStateLast = buttDDownLState;
  buttDLStateLast = buttDLState;
  buttDUpLStateLast = buttDUpLState;
}

//Update C Stick Last States
void updateStatesCStick(){
  buttCUpStateLast = buttCUpState;
  buttCUpRStateLast = buttCUpRState;
  buttCRStateLast = buttCRState;
  buttCDownRStateLast = buttCDownRState;
  buttCDownStateLast = buttCDownState;
  buttCDownLStateLast = buttCDownLState;
  buttCLStateLast = buttCLState;
  buttCUpLStateLast = buttCUpLState;
}

//Update Z Hold C Stick Last States 
void updateZStatesCStick(){
  buttCZUpStateLast = buttCZUpState;
  buttCZUpRStateLast = buttCZUpRState;
  buttCZRStateLast = buttCZRState;
  buttCZDownRStateLast = buttCZDownRState;
  buttCZDownStateLast = buttCZDownState;
  buttCZDownLStateLast = buttCZDownLState;
  buttCZLStateLast = buttCZLState;
  buttCZUpLStateLast = buttCZUpLState;
}

//----------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  
  // Set up LEDs
  pinMode(drumLED, OUTPUT);
  pinMode(synthLED, OUTPUT);

  //Set up mode button
  pinMode(buttonPin, INPUT_PULLUP);

  //start traditional MIDI on Channel 1
  MIDI.begin(synthChT);

  //Set the default LED
  digitalWrite(drumLED, HIGH);
}

//----------------------------------------------------------------

void loop()
{
  // Try to read the controller data
  if (GamecubeController.read())
  {
    // Print Controller information
    auto status = GamecubeController.getStatus();
    auto report = GamecubeController.getReport();
    print_gc_report(report, status);
  }
  else
  {
    // Add debounce if reading failed
    Serial.println(F("Error reading Gamecube controller."));
    delay(1500);
  }
}

void print_gc_report(Gamecube_Report_t &gc_report, Gamecube_Status_t &gc_status)
{
  // Print device information
  switch (gc_status.device) {
    case NINTENDO_DEVICE_GC_NONE:
      break;
    case NINTENDO_DEVICE_GC_WIRED:
      break;

    default:
      break;
  }

  //Mode Button Trigger
  //read button value
  modeState = digitalRead(buttonPin);
  //if button state has changed, switch mode
  if(modeState != modeStateLast){
  //Serial.println(buttonVal);
  //If button pressed (0) and timeout done switch mode
  if(modeState==0 && timeout == false){
    toggleMode();
    }
  }
 //update last state
modeStateLast = modeState;

//----------------------------------------------------------------
//***Drum Mode Code
if(drumMode==true){

//Velocity
//Get padded stick value to use for velocity
int velPadVal = joyY(gc_report.yAxis);
//Set velocity based on joystick Y val, use constrain as lower limits may go into negatives
vel = constrain(map(velPadVal, yNeutral - 95, yNeutral + 95, velFloor, 127),velFloor, 127);

//CC (Modulation) Control
  //get padded X Axis value to use for CC
  int ccPadVal = joyX(gc_report.xAxis);

  //Calculate CC Value and set as state
    if(alwaysUp==true){
      ccState = abs(constrain(map (ccPadVal, xNeutral - 100, xNeutral + 100, -maxVal, maxVal), -maxVal, maxVal)); //translate value from controller input based on config settings
    }else if (alwaysUp==false){
      ccState = constrain(map (ccPadVal, xNeutral - 100, xNeutral + 100, minVal, maxVal),minVal, maxVal); // translate value from controller input based on config settings
    }
  //check if CC Value has changed, if so, calculate and send CC value
    if(ccState != ccStateLast){
    //Send the CC Message  
      //Traditional
      MIDI.sendControlChange(controlVal, ccState, drumChT); //CC, value, channel (1-16)
      //USB
      controlChange(drumChU, controlVal, ccState); //channel (0-15), CC, value
      MidiUSB.flush();
    }
    //update CC Last State
    ccStateLast = ccState;

//**** Begin Drum Note Triggers

//Set states based on buttons
  setStatesDrum(gc_report);

//Run Triggers for Drum Mode
//Face and Triggers
  //A
  drumTrigger(buttAState, buttAStateLast, drumA);
  //B
  drumTrigger(buttBState, buttBStateLast, drumB);
  //X
  drumTrigger(buttXState, buttXStateLast, drumX);
  //Y
  drumTrigger(buttYState, buttYStateLast, drumY);
  //L
  drumTrigger(buttLState, buttLStateLast, drumL);
  //R
  drumTrigger(buttRState, buttRStateLast, drumR);
  //Z
  drumTrigger(buttZState, buttZStateLast, drumZ);
  //Start
  drumTrigger(buttStartState, buttStartStateLast, drumStart);

//DPad
  //Up
  drumTrigger(buttDUpState, buttDUpStateLast, drumDUp);
  //Up+R
  drumTrigger(buttDUpRState, buttDUpRStateLast, drumDUpR);
  //R
  drumTrigger(buttDRState, buttDRStateLast, drumDR);
  //Down+R
  drumTrigger(buttDDownRState, buttDDownRStateLast, drumDDownR);
  //Down
  drumTrigger(buttDDownState, buttDDownStateLast, drumDDown);
  //Down+L
  drumTrigger(buttDDownLState, buttDDownLStateLast, drumDDownL);
  //L
  drumTrigger(buttDLState, buttDLStateLast, drumDL);
  //Up+L
  //L
  drumTrigger(buttDUpLState, buttDUpLStateLast, drumDUpL);

//C Stick
  //Up
  drumTrigger(buttCUpState, buttCUpStateLast, drumCUp);
  //Up+R
  drumTrigger(buttCUpRState, buttCUpRStateLast, drumCUpR);
  //R
  drumTrigger(buttCRState, buttCRStateLast, drumCR);
  //Down+R
  drumTrigger(buttCDownRState, buttCDownRStateLast, drumCDownR);
  //Down
  drumTrigger(buttCDownState, buttCDownStateLast, drumCDown);
  //Down+L
  drumTrigger(buttCDownLState, buttCDownLStateLast, drumCDownL);
  //L
  drumTrigger(buttCLState, buttCLStateLast, drumCL);
  //Up+L
  drumTrigger(buttCUpLState, buttCUpLStateLast, drumCUpL);

//Update Last States
  updateStatesDrum();

}

//----------------------------------------------------------------
//***Synth Mode Code
if(synthMode == true){

//Y Axis Pitch Bend Control
  //get padded Y Axis value for PB
  int yPadVal = joyY(gc_report.yAxis);

  //Calculate the PB value. If it has changed, send it
  int pitchBend = constrain(map (yPadVal, yNeutral - 90, yNeutral + 90, bendLow, bendHigh), bendLow, bendHigh);
  //Serial.println(pitchBend);

  //Set and check states
  pbState = pitchBend;
  if(pbState != pbStateLast){
    int pitchBendUSBval = pbState; //make a seperate value to modify for USB 
    //Convert 8 bit pitchbend value to 7 bit
      pitchBendUSBval = pitchBendUSBval << 1;     // shift low byte's msb to high byte
      byte msb = highByte(pitchBendUSBval);      // get the high bits
      byte lsb = lowByte(pitchBendUSBval) >> 1;  // get the low 8 bits
    
      // send the pitch bend message:
      midiCommand(pbUSB, lsb, msb); //number after E is the channel for the message
      MidiUSB.flush();
      
      //Pitch Bend for Traditional MIDI
      int pitchBendTrad = pbState - 8192; //make a new value for Traditional MIDI
      MIDI.sendPitchBend(pitchBendTrad, pbT);
  }
  //update last state
  pbStateLast = pbState;
  
//X Axis CC (Pan) Control
  //get padded X Axis value to use for CC
  int ccXPadVal = joyX(gc_report.xAxis);

//Calculate the CC value and set the state
  if(synthAlwaysUp==true){
    synthXState = abs(constrain(map (ccXPadVal, xNeutral - 95, xNeutral + 95, -synthXMax, synthXMax), -synthXMax, synthXMax)); //translate value from controller input based on config settings
    }else if (synthAlwaysUp==false){
    synthXState = constrain(map (ccXPadVal, xNeutral - 95, xNeutral + 95, synthXMin, synthXMax), synthXMin, synthXMax); // translate value from controller input based on config settings
    }
//check if CC Value has changed, if so, send CC value
  if(synthXState != synthXStateLast){
  //Send the CC Message  
    //Traditional
    MIDI.sendControlChange(synthXCC, synthXState, synthXChT); //CC, value, channel (1-16)
    //USB
    controlChange(synthXChU, synthXCC, synthXState); //channel (0-15), CC, value
    MidiUSB.flush();
  }
  //update CC Last State
  synthXStateLast = synthXState;

//R Trigger CC (Modulation) Control
  
  //Get Trigger value and calculate CC value for state, set state to zero if below threshold
  if(gc_report.right> rThreshold){
    int slider = gc_report.right;
    //Calculate CC Value and set as state
    sliderRState = constrain(map (slider, rThreshold, rMax, synthRMin, synthRMax), synthRMin, synthRMax); //translate value from controller input based on config settings

  }else{
    sliderRState = 0;
  }
  
  //Uncomment to get control range, 36 neutral (45 seems like good threshold), 225 max
  //Serial.println(sliderRState);

  //if state has changed, send CC Value
  if(sliderRState != sliderRStateLast){
  //Send the CC Message  
    //Traditional
    MIDI.sendControlChange(synthRCC, sliderRState, synthRChT); //CC, value, channel (1-16)
    //USB
    controlChange(synthRChU, synthRCC, sliderRState); //channel (0-15), CC, value
    MidiUSB.flush();
  }
  //update last state
  sliderRStateLast = sliderRState;

//L Trigger Velocity Control
  //Get Trigger value and set to floor if lower than threshold
  if(gc_report.left < lThreshold){
    vel = synthVelFloor;
    //Serial.println(vel);
  }
  //If above floor, map to trigger values
  else{
    vel = constrain(map(gc_report.left, lThreshold, lMax, synthVelFloor, 127), synthVelFloor, 127);
    //Serial.println(vel);
  }

//Octave Control
  //Octave Up
  //set to 1 if up pressed
  octaveUp = gc_report.dup;
  
  //if that has changed since the last loop, update the octave
  if (octaveUp != octaveUpLast){
    //if the change is the button being pressed, move the octave up one within the constraints
    if(octaveUp == 1){
      // stop all notes to prevent held notes in the old octave
      allNotesOff();
      // set octave up by 1
      octave = constrain((octave +1), octaveMin, octaveMax);
    }
  }
  octaveUpLast = octaveUp; //update the last state

  //Octave Down
  //set to 1 if down pressed
  octaveDown = gc_report.ddown;
  
  //if that has changed since the last loop, update the octave
  if (octaveDown != octaveDownLast){
    //if the change is the button being pressed, move the octave up one within the constraints
    if(octaveDown == 1){
      // stop all notes to prevent held notes in the old octave
      allNotesOff();
      // set octave down by 1
      octave = constrain((octave -1), octaveMin, octaveMax);
    }
  }
  octaveDownLast = octaveDown; //update the last state
  //Serial.println(octave);

//Transpose Control
  //Transpose Up
  //set to 1 if right pressed
  transposeUp = gc_report.dright;
  
  //if that has changed since the last loop, update the octave
  if (transposeUp != transposeUpLast){
    //if the change is the button being pressed, move the transpose up one within the constraints
    if(transposeUp == 1){
      // stop all notes to prevent held notes in the old range
      allNotesOff();
      // set transpose up by 1
      transpose = constrain((transpose +1), transposeMin, transposeMax);
    }
  }
  transposeUpLast = transposeUp; //update the last state

  //Transpose Down
  //set to 1 if left pressed
  transposeDown = gc_report.dleft;
  
  //if that has changed since the last loop, update the octave
  if (transposeDown != transposeDownLast){
    //if the change is the button being pressed, move the transpose up one within the constraints
    if(transposeDown == 1){
      // stop all notes to prevent held notes in the old range
      allNotesOff();
      // set transpose down by 1
      transpose = constrain((transpose -1), transposeMin, transposeMax);
    }
  }
  transposeDownLast = transposeDown; //update the last state
  //Serial.println(transpose);
//Start Button Reset octave and kill notes if pressed
  //check if button state changed
  resetState = gc_report.start;
  if(resetState != resetStateLast){
    //if pressed trigger reset
  if(resetState==1){
    //stop all notes on all mode channels
    allNotesOff();
    //reset octave value to zero
    octave=0;
    //reset transpose value to zero
    transpose=0;
  }
 }
 //update last state
 resetStateLast = resetState;

//**** Begin Synth Note Triggers

//Set Button States
  setStatesSynth(gc_report);

//Trigger notes
  //Face (Drum Triggers)
    //A
    drumTrigger(buttAState, buttAStateLast, synthA);
    //B
    drumTrigger(buttBState, buttBStateLast, synthB);
    //X
    drumTrigger(buttXState, buttXStateLast, synthX);
    //Y
    drumTrigger(buttYState, buttYStateLast, synthY);

  //C Stick Triggers
    //Up
    synthTrigger(buttCUpState, buttCUpStateLast, synthCUp);
    //Up+R
    synthTrigger(buttCUpRState, buttCUpRStateLast, synthCUpR);
    //R
    synthTrigger(buttCRState, buttCRStateLast, synthCR);
    //Down+R
    synthTrigger(buttCDownRState, buttCDownRStateLast, synthCDownR);
    //Down
    synthTrigger(buttCDownState, buttCDownStateLast, synthCDown);
    //Down+L
    synthTrigger(buttCDownLState, buttCDownLStateLast, synthCDownL);
    //L
    synthTrigger(buttCLState, buttCLStateLast, synthCL);
    //Up+L
    synthTrigger(buttCUpLState, buttCUpLStateLast, synthCUpL);

  //C + Z Triggers
    //Up
    synthTrigger(buttCZUpState, buttCZUpStateLast, synthCUp - 1);
    //Up+R
    synthTrigger(buttCZUpRState, buttCZUpRStateLast, synthCUpR - 1);
    //R
    synthTrigger(buttCZRState, buttCZRStateLast, synthCR - 1);
    //Down+R
    synthTrigger(buttCZDownRState, buttCZDownRStateLast, synthCDownR - 1);
    //Down
    synthTrigger(buttCZDownState, buttCZDownStateLast, synthCDown - 1);
    //Down+L
    synthTrigger(buttCZDownLState, buttCZDownLStateLast, synthCDownL - 1);
    //L
    synthTrigger(buttCZLState, buttCZLStateLast, synthCL - 1);
    //Up+L
    synthTrigger(buttCZUpLState, buttCZUpLStateLast, synthCUpL - 1);

//Update Button Last States
  updateStatesSynth();
}
 //----------------------------------------------------------------
//*** DK Mode Code
if(dkMode == true){
  
//Get Button States for Kit
  if(bongo == 0){
    setStatesDKDrum(gc_report);
  }else if (bongo == 1){
    setStatesDKBongo(gc_report);
  }
  
//Switch Kits
  //check start state
  if(buttStartState != buttStartStateLast){
    //if state has changed to 1, trigger a kit change
    if(buttStartState == 1){
      kitToggle();
    }
  }
//Serial.print(bongo);
//Serial.println(clapOff);

//Trigger Notes
  //Drum Kit
  if(bongo == 0){
    //Pads
    //B
      dkTrigger(buttBState, buttBStateLast, dkB);
    //Y
      dkTrigger(buttYState, buttYStateLast, dkY);
    //X
      dkTrigger(buttXState, buttXStateLast, dkX);
    //BY
      dkTrigger(buttBYState, buttBYStateLast, dkBY);
    //AB
      dkTrigger(buttABState, buttABStateLast, dkAB);
    //AY
      dkTrigger(buttAYState, buttAYStateLast, dkAY);
    //AX
      dkTrigger(buttAXState, buttAXStateLast, dkAX);
    //ABY
      dkTrigger(buttABYState, buttABYStateLast, dkABY);
    //ABYX
      dkTrigger(buttABYXState, buttABYXStateLast, dkABYX);

    //Clap - All Code Here for Custom Velocity
      //If Clap isn't disabled, check if state changed
      if(clapOff == 0){
        if(buttRState != buttRStateLast){
          //if state changed to On send note
          if(buttRState == 1){ 
            //Send on USB
            noteOn(dkChU, dkClap, clapVel);   // Channel (0-15), pitch, velo
            MidiUSB.flush();
            //Send on Traditional MIDI
            MIDI.sendNoteOn(dkClap, clapVel, dkChT); // pitch, velo, channel (1-16)
          }
          //otherwise send note off
          else{
            // NoteOff for USB
            noteOff(dkChU, dkClap, clapVel);  // Channel (0-15), pitch, velo
            MidiUSB.flush();
            // NoteOff for Traditional MIDI
            MIDI.sendNoteOff(dkClap, clapVel, dkChT); // pitch, velo, channel (1-16)
          }
        }
      }
  }
  
  //Bongo Kit
  else if (bongo == 1){
    //Pads
    //A
      dkTrigger(buttAState, buttAStateLast, dkbA);
    //B
      dkTrigger(buttBState, buttBStateLast, dkbB);
    //Y
      dkTrigger(buttYState, buttYStateLast, dkbY);
    //X
      dkTrigger(buttXState, buttXStateLast, dkbX);
    //ABYX
      dkTrigger(buttABYXState, buttABYXStateLast, dkbABYX);

    //Clap - All Code Here for Custom Velocity
      //If Clap isn't disabled, check if state changed
      if(clapOff == 0){
        if(buttRState != buttRStateLast){
          //if state changed to On send note
          if(buttRState == 1){ 
            //Send on USB
            noteOn(dkChU, dkbClap, clapVel);   // Channel (0-15), pitch, velo
            MidiUSB.flush();
            //Send on Traditional MIDI
            MIDI.sendNoteOn(dkbClap, clapVel, dkChT); // pitch, velo, channel (1-16)
          }
          //otherwise send note off
          else{
            // NoteOff for USB
            noteOff(dkChU, dkbClap, clapVel);  // Channel (0-15), pitch, velo
            MidiUSB.flush();
            // NoteOff for Traditional MIDI
            MIDI.sendNoteOff(dkbClap, clapVel, dkChT); // pitch, velo, channel (1-16)
          }
        }
      }
  }

//Update Last States for Kit
    if (bongo == 0){
      updateStatesDKDrum();
    } else if (bongo == 1){
      updateStatesDKBongo();
    }
  }
}
