// PJRC audio library
#include <Audio.h>

// blackaddr audio adapter library
#define TGA_PRO_REVB // Specify our hardware revision
#define TGA_PRO_EXPAND_REV2 // Specify we are using the EXPANSION CONTROL BOARD REV2
#include "BALibrary.h"
using namespace BALibrary;

BAAudioControlWM8731      codecControl;

AudioOutputI2S           i2sOut0;

#include <MIDI.h>
#include <EEPROM.h>

// how many voices do we have?
#define VOICES 16

// maxium EG time for any envelope phase in milliseconds
#define MAX_EG_TIME 1489

#define CHANNEL_SET_PIN 2
#define EE_CHECKBYTE_1 1022
#define EE_CHECKBYTE_2 1023
#define EE_MIDI_CHANNEL 11
#define LED_PIN 13

// midi channel; default to 11
unsigned int myChannel = 11;
// CC info from noteOn
unsigned int ccNumber;
unsigned int ccValue;

// note struct
// contains a pitch, a velocity, and a sequence number
// as well as frequency and level for each voice oscillator
typedef struct
{
  unsigned int myPitch;
  unsigned int myVelocity;
  unsigned int mySequence;
}
note;

// array of note values for each voice
note notes[VOICES];


// array of frequencies corresponding to MIDI notes 0 to 128
const float noteFreqs[128] = {8.176,8.662,9.177,9.723,10.301,10.913,11.562,12.25,12.978,13.75,14.568,15.434,16.352,17.324,18.354,19.445,20.602,21.827,23.125,24.5,25.957,27.5,29.135,30.868,32.703,34.648,36.708,38.891,41.203,43.654,46.249,48.999,51.913,55,58.27,61.735,65.406,69.296,73.416,77.782,82.407,87.307,92.499,97.999,103.826,110,116.541,123.471,130.813,138.591,146.832,155.563,164.814,174.614,184.997,195.998,207.652,220,233.082,246.942,261.626,277.183,293.665,311.127,329.628,349.228,369.994,391.995,415.305,440,466.164,493.883,523.251,554.365,587.33,622.254,659.255,698.456,739.989,783.991,830.609,880,932.328,987.767,1046.502,1108.731,1174.659,1244.508,1318.51,1396.913,1479.978,1567.982,1661.219,1760,1864.655,1975.533,2093.005,2217.461,2349.318,2489.016,2637.02,2793.826,2959.955,3135.963,3322.438,3520,3729.31,3951.066,4186.009,4434.922,4698.636,4978.032,5274.041,5587.652,5919.911,6271.927,6644.875,7040,7458.62,7902.133,8372.018,8869.844,9397.273,9956.063,10548.08,11175.3,11839.82,12543.85};

AudioEffectEnvelope envelope1;
AudioEffectEnvelope envelope2;
AudioEffectEnvelope envelope3;
AudioEffectEnvelope envelope4;
AudioEffectEnvelope envelope5;
AudioEffectEnvelope envelope6;
AudioEffectEnvelope envelope7;
AudioEffectEnvelope envelope8;
AudioEffectEnvelope envelope9;
AudioEffectEnvelope envelope10;
AudioEffectEnvelope envelope11;
AudioEffectEnvelope envelope12;
AudioEffectEnvelope envelope13;
AudioEffectEnvelope envelope14;
AudioEffectEnvelope envelope15;
AudioEffectEnvelope envelope16;
AudioEffectEnsemble ensemble1;


// array of envelopes
AudioEffectEnvelope *myEnvelope[16] = { &envelope1, &envelope2, &envelope3, &envelope4, &envelope5, &envelope6, &envelope7, &envelope8, &envelope9, &envelope10, &envelope11, &envelope12, &envelope13, &envelope14, &envelope15, &envelope16 };

AudioSynthWaveform waveform1;
AudioSynthWaveform waveform2;
AudioSynthWaveform waveform3;
AudioSynthWaveform waveform4;
AudioSynthWaveform waveform5;
AudioSynthWaveform waveform6;
AudioSynthWaveform waveform7;
AudioSynthWaveform waveform8;
AudioSynthWaveform waveform9;
AudioSynthWaveform waveform10;
AudioSynthWaveform waveform11;
AudioSynthWaveform waveform12;
AudioSynthWaveform waveform13;
AudioSynthWaveform waveform14;
AudioSynthWaveform waveform15;
AudioSynthWaveform waveform16;

// array of waveforms
AudioSynthWaveform *myWaveform[16] = { &waveform1, &waveform2, &waveform3, &waveform4, &waveform5, &waveform6, &waveform7, &waveform8, &waveform9, &waveform10, &waveform11, &waveform12, &waveform13, &waveform14, &waveform15, &waveform16};

AudioConnection          patchCord1(waveform1, envelope1);
AudioConnection          patchCord2(waveform2, envelope2);
AudioConnection          patchCord3(waveform3, envelope3);
AudioConnection          patchCord4(waveform4, envelope4);
AudioConnection          patchCord5(waveform5, envelope5);
AudioConnection          patchCord6(waveform6, envelope6);
AudioConnection          patchCord7(waveform7, envelope7);
AudioConnection          patchCord8(waveform8, envelope8);
AudioConnection          patchCord9(waveform9, envelope9);
AudioConnection          patchCord10(waveform10, envelope10);
AudioConnection          patchCord11(waveform11, envelope11);
AudioConnection          patchCord12(waveform12, envelope12);
AudioConnection          patchCord13(waveform13, envelope13);
AudioConnection          patchCord14(waveform14, envelope14);
AudioConnection          patchCord15(waveform15, envelope15);
AudioConnection          patchCord16(waveform16, envelope16);

AudioMixer4              mixer_group_1;
AudioMixer4              mixer_group_2;
AudioMixer4              mixer_group_3;
AudioMixer4              mixer_group_4;
AudioMixer4              mixer_all_voices;
AudioFilterStateVariable filter1;
AudioAmplifier           amp1;

AudioConnection          patchCord17(envelope1, 0, mixer_group_1, 0);
AudioConnection          patchCord18(envelope2, 0, mixer_group_1, 1);
AudioConnection          patchCord19(envelope3, 0, mixer_group_1, 2);
AudioConnection          patchCord20(envelope4, 0, mixer_group_2, 0);
AudioConnection          patchCord21(envelope5, 0, mixer_group_2, 1);
AudioConnection          patchCord22(envelope6, 0, mixer_group_1, 3);
AudioConnection          patchCord23(envelope7, 0, mixer_group_2, 2);
AudioConnection          patchCord24(envelope8, 0, mixer_group_3, 0);
AudioConnection          patchCord25(envelope9, 0, mixer_group_3, 1);
AudioConnection          patchCord26(envelope10, 0, mixer_group_2, 3);
AudioConnection          patchCord27(envelope11, 0, mixer_group_3, 2);
AudioConnection          patchCord28(envelope12, 0, mixer_group_3, 3);
AudioConnection          patchCord29(envelope13, 0, mixer_group_4, 0);
AudioConnection          patchCord30(envelope14, 0, mixer_group_4, 1);
AudioConnection          patchCord31(envelope15, 0, mixer_group_4, 2);
AudioConnection          patchCord32(envelope16, 0, mixer_group_4, 3);
AudioConnection          patchCord33(mixer_group_1, 0, mixer_all_voices, 0);
AudioConnection          patchCord34(mixer_group_2, 0, mixer_all_voices, 1);
AudioConnection          patchCord35(mixer_group_3, 0, mixer_all_voices, 2);
AudioConnection          patchCord36(mixer_group_4, 0, mixer_all_voices, 3);
AudioConnection          patchCord37(mixer_all_voices, 0, filter1, 0);
AudioConnection          patchCord38(mixer_all_voices, 0, filter1, 1);
AudioConnection          patchCord39(filter1, 0, amp1, 0);
AudioConnection          patchCord42(amp1, 0, ensemble1, 0);
AudioConnection          patchCord40(ensemble1, 0, i2sOut0, 0);
AudioConnection          patchCord41(ensemble1, 1, i2sOut0, 1);
AudioControlWM8731       wm8731_1;       //xy=1428,56

// pitch bend
float pitchBend = 0;
int bendRange = 2;
// VCA parameters
unsigned int vcaAttack = 0;
unsigned int vcaDecay = 0;
float vcaSustain = 1.0;
unsigned int vcaRelease = 500;
// VCF parameters
float vcfCutoff = 20; // not sure if 20 Hz is the default if not defined
float vcfResonance = 0.1; // default to zero resonance

// synth_waveform.h definitions
// #define WAVEFORM_SINE      0
// #define WAVEFORM_SAWTOOTH  1
// #define WAVEFORM_SQUARE    2
// #define WAVEFORM_TRIANGLE  3
// #define WAVEFORM_ARBITRARY 4
// #define WAVEFORM_PULSE     5

// oscillator waveform (see above)
int oscWF = 1;
// oscillator frequency
float oscFreq = 0.0;
// oscillator level
float oscLevel = 1.0;
// pwm
float pwmLevel = 0.5;


//////////////////////////////////////////
// SETUP PHYSICAL CONTROLS
// - POT1 (left) will control the amount of attack
// - POT2 (right) will control the amount of release
// - POT3 (centre) will control the filter
// - SW1  (left) will be used to enable vox humana
// - LED1 (left) will be illuminated when vox humana is selected
// - SW2  (right) will be used to add sub-octave
// - LED2 (right) will illuminate when pressing SW2.
//////////////////////////////////////////
constexpr int  potCalibMin = 1;
constexpr int  potCalibMax = 1023;
constexpr bool potSwapDirection = true;

// track if knob 3 (center) controls pwm
int isPwm = 0;
// track led2
int isLed2 = 0;

// Create physical controls for Expansion Board, 2 switches, 3 pots, 0 encoders, 2 LEDs
BAPhysicalControls controls(BA_EXPAND_NUM_SW, BA_EXPAND_NUM_POT, BA_EXPAND_NUM_ENC, BA_EXPAND_NUM_LED);

int pwmHandle, waveformHandle, attackHandle, releaseHandle, filterHandle, led1Handle, led2Handle;

float potValue;

//---------------------------------------------------------------------------------------------//
// setup
//---------------------------------------------------------------------------------------------//
void setup() {
  //Serial.begin(115200);

  // set up the LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  wm8731_1.enable();
  AudioMemory(128);
  // disable and enable the codec
  codecControl.disable();
  delay(100);
  codecControl.enable();
  codecControl.setHeadphoneVolume(1.0);

  // initialize the notes struct
  // NOTE: MIDI pitch 0 signifies no note is playing
  // despite that being a valid note.
  for (int j = 0; j < VOICES; j++)
  {
    notes[j].myPitch = 0;
    notes[j].myVelocity = 0;
    notes[j].mySequence = 0;
  }

  mixer_group_1.gain(0, 0.0625);
  mixer_group_1.gain(1, 0.0625);
  mixer_group_1.gain(2, 0.0625);
  mixer_group_1.gain(3, 0.0625);
  mixer_group_2.gain(0, 0.0625);
  mixer_group_2.gain(1, 0.0625);
  mixer_group_2.gain(2, 0.0625);
  mixer_group_2.gain(3, 0.0625);
  mixer_group_3.gain(0, 0.0625);
  mixer_group_3.gain(1, 0.0625);
  mixer_group_3.gain(2, 0.0625);
  mixer_group_3.gain(3, 0.0625);
  mixer_group_4.gain(0, 0.0625);
  mixer_group_4.gain(1, 0.0625);
  mixer_group_4.gain(2, 0.0625);
  mixer_group_4.gain(3, 0.0625);
  mixer_all_voices.gain(0, 0.4);
  mixer_all_voices.gain(1, 0.4);
  mixer_all_voices.gain(2, 0.4);
  mixer_all_voices.gain(3, 0.4);
  amp1.gain(2.5);

  // stop processing while configuring things
  AudioNoInterrupts();

  // initialize envelopes
  for (int h = 0; h < VOICES; h++)
  {
    myEnvelope[h]->attack(vcaAttack);
    myEnvelope[h]->hold(0.0);
    myEnvelope[h]->decay(vcaDecay);
    myEnvelope[h]->sustain(vcaSustain);
    myEnvelope[h]->release(vcaRelease);
  }

  // initialize filters
  filter1.frequency(vcfCutoff);
  filter1.resonance(vcfResonance);
  filter1.octaveControl(7); // default to the maximum range

  // resume processing
  AudioInterrupts();

  // read the EEPROM checkbytes
  if ((EEPROM.read(EE_CHECKBYTE_1) == 0xBA) && (EEPROM.read(EE_CHECKBYTE_2) == 0xDA))
  {
    // read the MIDI channel from EEPROM
    myChannel = EEPROM.read(EE_MIDI_CHANNEL);
  }
  else
  {
    // set the checkbytes
    EEPROM.write(EE_CHECKBYTE_1, 0xBA);
    EEPROM.write(EE_CHECKBYTE_2, 0xDA);
    // the MIDI channel will default to 1
    EEPROM.write(EE_MIDI_CHANNEL, myChannel);
  }

  myChannel = 11;
  
  // MIDI setup
  MIDI.begin(myChannel);  
  MIDI.setHandleNoteOn(doNoteOn); 
  MIDI.setHandleControlChange(doCC); 
  MIDI.setHandleNoteOff(doNoteOff); 
  MIDI.setHandlePitchBend(doBend);
  
  // Setup the controls. The return value is the handle to use when checking for control changes, etc.
  // pushbuttons
  pwmHandle = controls.addSwitch(BA_EXPAND_SW1_PIN); // will be used for vox humana control
  waveformHandle = controls.addSwitch(BA_EXPAND_SW2_PIN); // will be used for sub-octave
  // pots
  attackHandle = controls.addPot(BA_EXPAND_POT1_PIN, potCalibMin, potCalibMax, potSwapDirection); // control the amount of delay
  releaseHandle = controls.addPot(BA_EXPAND_POT2_PIN, potCalibMin, potCalibMax, potSwapDirection); 
  filterHandle = controls.addPot(BA_EXPAND_POT3_PIN, potCalibMin, potCalibMax, potSwapDirection); 
  // leds
  led1Handle = controls.addOutput(BA_EXPAND_LED1_PIN);
  controls.setOutput(led1Handle, 0);
  led2Handle = controls.addOutput(BA_EXPAND_LED2_PIN);
  controls.setOutput(led2Handle, 0);
  
  controls.setOutput(led1Handle, 1);
  delay(200);
  controls.setOutput(led1Handle, 0);
  delay(200);
  controls.setOutput(led2Handle, 1);
  delay(200);
  controls.setOutput(led2Handle, 0);

  //Serial.println("end of setup");
  
}

//---------------------------------------------------------------------------------------------//
// main loop
//---------------------------------------------------------------------------------------------//
void loop()
{
  // get MIDI notes if available
  MIDI.read();

  // check button 1 (left)
  if (controls.isSwitchToggled(pwmHandle))
  {
    if (isPwm)
    {
      controls.setOutput(led1Handle, 0);
      isPwm = 0;
    }
    else
    {
      controls.setOutput(led1Handle, 1);
      isPwm = 1;
    }   
  }
  // check button 2 (right)
  // just toggele the led for now
  if (controls.isSwitchToggled(waveformHandle))
  {
    if (isLed2)
    {
      controls.setOutput(led2Handle, 0);
      isLed2 = 0;
    }
    else
    {
      controls.setOutput(led2Handle, 1);
      isLed2 = 1;
    }
  }
  
//  // set the MIDI channel if the button is pressed
//  if (channelButton.update())
//  {
//    if (channelButton.read() == LOW)
//    {
//      setMIDIChannel();
//    }
//  }

  // read the controls
  // attack
  if (controls.checkPotValue(attackHandle, potValue))
  {
    vcaAttack = (potValue * MAX_EG_TIME);
    //Serial.println(String("vcaAttack: ") + vcaAttack);
    for (int g = 0; g < VOICES; g++)
    {
      myEnvelope[g]->attack(vcaAttack);
    }
  }
  
  // release
  if (controls.checkPotValue(releaseHandle, potValue))
  {
    vcaRelease = (potValue * MAX_EG_TIME);
    //Serial.println(String("vcaRelease: ") + vcaRelease);
    for (int f = 0; f < VOICES; f++)
    {
      myEnvelope[f]->release(vcaRelease);
    }
  }
  
  // cutoff or pwm
  if (controls.checkPotValue(filterHandle, potValue))
  {
    if (isPwm)
    {
      pwmLevel = potValue;
      //Serial.println(String("pwmLevel: ") + pwmLevel);
      for (int e = 0; e < VOICES; e++)
      {
        AudioNoInterrupts();
        myWaveform[e]->pulseWidth(pwmLevel);
        AudioInterrupts();
      }
    }
    else
    {
      vcfCutoff = (potValue * 10000 + 20);
      //Serial.println(String("vcfCutoff: ") + vcfCutoff);
      filter1.frequency(vcfCutoff);
    }
  }
  
}

//---------------------------------------------------------------------------------------------//
// function doNoteOn
//---------------------------------------------------------------------------------------------//
void doNoteOn(byte channel, byte pitch, byte velocity)
{
  Serial.println("got a note");
  
  // flag if a free voice was found 
  unsigned int voicesFree = 0;
  // the voice to be used
  unsigned int voiceUsed = 0;
  // temporary variable for finding oldest note
  byte seqNo = 0;
  int i;

  Serial.println("doNoteOn");

  digitalWrite(LED_PIN, HIGH);

  // try to find a free voice to play the note
  for (i = 0; i < VOICES; i++)
  {
    // if a free voice is found, use it
    if ((notes[i].mySequence == 0) && !(myEnvelope[i]->isActive()))
    {
      notes[i].myPitch = pitch;
      notes[i].myVelocity = velocity;
      // store the time
      notes[i].mySequence = 1;
      // flag it
      voicesFree = 1;
      // store the voice used so the right oscillators can be turned on
      voiceUsed = i;
      
      Serial.print("Using voice ");
      Serial.println(voiceUsed);
      
      break;
    }
    // increment the sequence number if the note is being used
    else
    {
      notes[i].mySequence = notes[i].mySequence + 1;
    }
  }

  // if no free voice was found
  // find the oldest note and use that
  if (voicesFree == 0)
  {
    for (i = 0; i < VOICES; i++)
    {
      if (notes[i].mySequence > seqNo)
      {
        seqNo = notes[i].mySequence;
        voiceUsed = i;
      }
    }

    // assign seqNo to mySequence
    notes[voiceUsed].mySequence = 1;
    // assign pitch and velocity to myPitch and myVelocity
    notes[voiceUsed].myPitch = pitch;
    notes[voiceUsed].myVelocity = velocity;
        
    Serial.print("Stealing voice ");
    Serial.println(voiceUsed);

    AudioNoInterrupts();

    // turn off the voice being stolen
    myEnvelope[voiceUsed]->noteOff();

    AudioInterrupts();
  }  
  // voiceUsed is now the voice that will play the note

  // convert MIDI note to frequency
  oscFreq = noteFreqs[pitch];

  // bend if necessary 
  oscFreq = noteFreqs[notes[voiceUsed].myPitch];
  // bend up
  if (pitchBend > 0)
  {
    oscFreq = oscFreq + (oscFreq * pitchBend);
  }
  // bend down
  else
  {
    oscFreq = oscFreq + ((oscFreq / 2) * pitchBend);
  }

  AudioNoInterrupts();
  myWaveform[voiceUsed]->begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
  myWaveform[voiceUsed]->pulseWidth(pwmLevel);
  myEnvelope[voiceUsed]->noteOn();
  AudioInterrupts();

  
  digitalWrite(LED_PIN, LOW);
  
}

//---------------------------------------------------------------------------------------------//
// function doNoteOff
//---------------------------------------------------------------------------------------------//
void doNoteOff(byte channel, byte pitch, byte velocity)
{
  // holds the voice playing the pitch
  byte voiceUsed = 0;


  // find the voice which is playing the note
  for (int i = 0; i < VOICES; i++)
  {
     if (pitch == notes[i].myPitch)
     {
       voiceUsed = i;
       // reset the voice values
       notes[i].myPitch = 0;
       notes[i].myVelocity = 0;
       notes[i].mySequence = 0;
      }
  }
  
  Serial.print("Releasing voice ");
  Serial.println(voiceUsed);

  AudioNoInterrupts();
  myEnvelope[voiceUsed]->noteOff();
  AudioInterrupts();
}

//---------------------------------------------------------------------------------------------//
// function doCC
//---------------------------------------------------------------------------------------------//
void doCC(byte channel, byte ccnumber, byte ccvalue)
{
  return;
}

//---------------------------------------------------------------------------------------------//
// function doBend
//---------------------------------------------------------------------------------------------//
void doBend(byte channel, int bend)
{
  float oscFreq;
  float pitchBend;

  // bend is apparently -8192 to 8192
  // calculate bend ratio
  pitchBend = bend / 8192.0 / 6;

  // bend all the voices
  AudioNoInterrupts();
  for (int k = 0; k < VOICES; k++)
  {
    // only bend active notes
    if (notes[k].mySequence > 0)
    {
      oscFreq = noteFreqs[notes[k].myPitch];
      // bend up
      if (pitchBend > 0)
      {
        oscFreq = oscFreq + (oscFreq * pitchBend);
      }
      // bend down
      else
      {
        oscFreq = oscFreq + ((oscFreq / 2) * pitchBend);
      }
      myWaveform[k]->begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
    }
  }
  AudioInterrupts();
}
