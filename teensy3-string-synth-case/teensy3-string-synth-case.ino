#include <Audio.h>
// blackaddr audio adapter library
#define TGA_PRO_REVB // Specify our hardware revision
#define TGA_PRO_EXPAND_REV2 // Specify we are using the EXPANSION CONTROL BOARD REV2
#include "BALibrary.h"
using namespace BALibrary;

BAAudioControlWM8731      codecControl;

AudioOutputI2S           i2sOut0, i2sOut1;

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
//AudioConnection          patchCord38(mixer_all_voices, 0, filter1, 1);
AudioConnection          patchCord39(filter1, 0, ensemble1, 0);
AudioConnection          patchCord40(ensemble1, 0, i2sOut0, 0);
AudioConnection          patchCord41(ensemble1, 1, i2sOut1, 1);
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
int oscWF = 5;
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
constexpr bool potSwapDirection = false;

// track if knob 3 (center) controls pwm
int isPwm = 0;

// Create physical controls for Expansion Board, 2 switches, 3 pots, 0 encoders, 2 LEDs
BAPhysicalControls controls(BA_EXPAND_NUM_SW, BA_EXPAND_NUM_POT, 0, BA_EXPAND_NUM_LED);

int pwmHandle, waveformHandle, attackHandle, releaseHandle, filterHandle, led1Handle, led2Handle;

float potValue;

//---------------------------------------------------------------------------------------------//
// setup
//---------------------------------------------------------------------------------------------//
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("HELLOOOOO?!?");

    // Disable the audio codec first
  codecControl.disable();
  delay(100);
  codecControl.enable();
  AudioMemory(128);
  wm8731_1.enable();

  // stop processing while configuring things
  AudioNoInterrupts();

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

  envelope1.attack(vcaAttack);
  envelope1.hold(0.0);
  envelope1.decay(vcaDecay);
  envelope1.sustain(vcaSustain);
  envelope1.release(vcaRelease);

  envelope2.attack(vcaAttack);
  envelope2.hold(0.0);
  envelope2.decay(vcaDecay);
  envelope2.sustain(vcaSustain);
  envelope2.release(vcaRelease);

  envelope3.attack(vcaAttack);
  envelope3.hold(0.0);
  envelope3.decay(vcaDecay);
  envelope3.sustain(vcaSustain);
  envelope3.release(vcaRelease);

  envelope4.attack(vcaAttack);
  envelope4.hold(0.0);
  envelope4.decay(vcaDecay);
  envelope4.sustain(vcaSustain);
  envelope4.release(vcaRelease);

  envelope5.attack(vcaAttack);
  envelope5.hold(0.0);
  envelope5.decay(vcaDecay);
  envelope5.sustain(vcaSustain);
  envelope5.release(vcaRelease);

  envelope6.attack(vcaAttack);
  envelope6.hold(0.0);
  envelope6.decay(vcaDecay);
  envelope6.sustain(vcaSustain);
  envelope6.release(vcaRelease);

  envelope7.attack(vcaAttack);
  envelope7.hold(0.0);
  envelope7.decay(vcaDecay);
  envelope7.sustain(vcaSustain);
  envelope7.release(vcaRelease);

  envelope8.attack(vcaAttack);
  envelope8.hold(0.0);
  envelope8.decay(vcaDecay);
  envelope8.sustain(vcaSustain);
  envelope8.release(vcaRelease);

  envelope9.attack(vcaAttack);
  envelope9.hold(0.0);
  envelope9.decay(vcaDecay);
  envelope9.sustain(vcaSustain);
  envelope9.release(vcaRelease);

  envelope10.attack(vcaAttack);
  envelope10.hold(0.0);
  envelope10.decay(vcaDecay);
  envelope10.sustain(vcaSustain);
  envelope10.release(vcaRelease);

  envelope11.attack(vcaAttack);
  envelope11.hold(0.0);
  envelope11.decay(vcaDecay);
  envelope11.sustain(vcaSustain);
  envelope11.release(vcaRelease);

  envelope12.attack(vcaAttack);
  envelope12.hold(0.0);
  envelope12.decay(vcaDecay);
  envelope12.sustain(vcaSustain);
  envelope12.release(vcaRelease);

  envelope13.attack(vcaAttack);
  envelope13.hold(0.0);
  envelope13.decay(vcaDecay);
  envelope13.sustain(vcaSustain);
  envelope13.release(vcaRelease);

  envelope14.attack(vcaAttack);
  envelope14.hold(0.0);
  envelope14.decay(vcaDecay);
  envelope14.sustain(vcaSustain);
  envelope14.release(vcaRelease);

  envelope15.attack(vcaAttack);
  envelope15.hold(0.0);
  envelope15.decay(vcaDecay);
  envelope15.sustain(vcaSustain);
  envelope15.release(vcaRelease);

  envelope16.attack(vcaAttack);
  envelope16.hold(0.0);
  envelope16.decay(vcaDecay);
  envelope16.sustain(vcaSustain);
  envelope16.release(vcaRelease);
  
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

  // MIDI setup
  MIDI.begin(myChannel);  
  MIDI.setHandleNoteOn(doNoteOn); 
  MIDI.setHandleControlChange(doCC); 
  MIDI.setHandleNoteOff(doNoteOff); 
  MIDI.setHandlePitchBend(doBend);

  controls.setOutput(led1Handle, 1);
  delay(500);
  controls.setOutput(led1Handle, 0);
  delay(500);
  controls.setOutput(led2Handle, 1);
  delay(500);
  controls.setOutput(led2Handle, 0);
  
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
  controls.setOutput(led2Handle, controls.getSwitchValue(led2Handle));
  
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
    vcaAttack = (potValue / 1023.0) * MAX_EG_TIME;
    for (int g = 0; g < VOICES; g++)
    {
      switch (g)
      {
        case 0:
          envelope1.attack(vcaAttack);
          break;
        case 1:
          envelope2.attack(vcaAttack);
          break;
        case 2:
          envelope3.attack(vcaAttack);
          break;
        case 3:
          envelope4.attack(vcaAttack);
          break;
        case 4:
          envelope5.attack(vcaAttack);
          break;
        case 5:
          envelope6.attack(vcaAttack);
          break;
        case 6:
          envelope7.attack(vcaAttack);
          break;
        case 7:
          envelope8.attack(vcaAttack);
          break;
        case 8:
          envelope9.attack(vcaAttack);
          break;
        case 9:
          envelope10.attack(vcaAttack);
          break;
        case 10:
          envelope11.attack(vcaAttack);
          break;
        case 11:
          envelope12.attack(vcaAttack);
          break;
        case 12:
          envelope13.attack(vcaAttack);
          break;
        case 13:
          envelope14.attack(vcaAttack);
          break;
        case 14:
          envelope15.attack(vcaAttack);
          break;
        case 15:
          envelope16.attack(vcaAttack);
          break;
        default:
          break;
      };
    }
  }
  // release
  if (controls.checkPotValue(releaseHandle, potValue))
  {
    vcaRelease = (potValue / 1023.0) * MAX_EG_TIME;
    for (int f = 0; f < VOICES; f++)
    {
      switch (f)
      {
        case 0:
          envelope1.release(vcaAttack);
          break;
        case 1:
          envelope2.release(vcaAttack);
          break;
        case 2:
          envelope3.release(vcaAttack);
          break;
        case 3:
          envelope4.release(vcaAttack);
          break;
        case 4:
          envelope5.release(vcaAttack);
          break;
        case 5:
          envelope6.release(vcaAttack);
          break;
        case 6:
          envelope7.release(vcaAttack);
          break;
        case 7:
          envelope8.release(vcaAttack);
          break;
        case 8:
          envelope9.release(vcaAttack);
          break;
        case 9:
          envelope10.release(vcaAttack);
          break;
        case 10:
          envelope11.release(vcaAttack);
          break;
        case 11:
          envelope12.release(vcaAttack);
          break;
        case 12:
          envelope13.release(vcaAttack);
          break;
        case 13:
          envelope14.release(vcaAttack);
          break;
        case 14:
          envelope15.release(vcaAttack);
          break;
        case 15:
          envelope16.release(vcaAttack);
          break;
        default:
          break;
      };
    }
  }
  // cutoff or pwm
  if (controls.checkPotValue(releaseHandle, potValue))
  {
    if (isPwm)
    {
      pwmLevel = potValue / 1023.0;
      for (int e = 0; e < VOICES; e++)
      {
        AudioNoInterrupts();
        switch (e)
        {
          case 0:
            waveform1.pulseWidth(pwmLevel);
            break;
          case 1:
            waveform2.pulseWidth(pwmLevel);
            break;
          case 2:
            waveform3.pulseWidth(pwmLevel);
            break;
          case 3:
            waveform4.pulseWidth(pwmLevel);
            break;
          case 4:
            waveform5.pulseWidth(pwmLevel);
            break;
          case 5:
            waveform6.pulseWidth(pwmLevel);
            break;
          case 6:
            waveform7.pulseWidth(pwmLevel);
            break;
          case 7:
            waveform8.pulseWidth(pwmLevel);
            break;
          case 8:
            waveform9.pulseWidth(pwmLevel);
            break;
          case 9:
            waveform10.pulseWidth(pwmLevel);
            break;
          case 10:
            waveform11.pulseWidth(pwmLevel);
            break;
          case 11:
            waveform12.pulseWidth(pwmLevel);
            break;
          case 12:
            waveform13.pulseWidth(pwmLevel);
            break;
          case 13:
            waveform14.pulseWidth(pwmLevel);
            break;
          case 14:
            waveform15.pulseWidth(pwmLevel);
            break;
          case 15:
            waveform16.pulseWidth(pwmLevel);
            break;
          default:
            break;
        };
        AudioInterrupts();
      }
    }
    else
    {
      vcfCutoff = (potValue / 1023.0) * 10000 + 20;
      filter1.frequency(vcfCutoff);
    }
  }
  
}

//---------------------------------------------------------------------------------------------//
// function doNoteOn
//---------------------------------------------------------------------------------------------//
void doNoteOn(byte channel, byte pitch, byte velocity)
{
  // flag if a free voice was found 
  unsigned int voicesFree = 0;
  // the voice to be used
  unsigned int voiceUsed = 0;
  // temporary variable for finding oldest note
  byte seqNo = 0;
  int i;

  digitalWrite(LED_PIN, HIGH);

  // try to find a free voice to play the note
  for (i = 0; i < VOICES; i++)
  {
    // if a free voice is found, use it
    if (notes[i].mySequence == 0)
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
    switch (voiceUsed)
    {
      case 0:
        envelope1.noteOff();
        break;
      case 1:
        envelope2.noteOff();
        break;
      case 2:
        envelope3.noteOff();
        break;
      case 3:
        envelope4.noteOff();
        break;
      case 4:
        envelope5.noteOff();
        break;
      case 5:
        envelope6.noteOff();
        break;
      case 6:
        envelope7.noteOff();
        break;
      case 7:
        envelope8.noteOff();
        break;
      case 8:
        envelope9.noteOff();
        break;
      case 9:
        envelope10.noteOff();
        break;
      case 10:
        envelope11.noteOff();
        break;
      case 11:
        envelope12.noteOff();
        break;
      case 12:
        envelope13.noteOff();
        break;
      case 13:
        envelope14.noteOff();
        break;
      case 14:
        envelope15.noteOff();
        break;
      case 15:
        envelope16.noteOff();
        break;
      default:
        break;
    };

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
  switch (voiceUsed)
  {
    case 0:
      waveform1.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform1.pulseWidth(pwmLevel);
      envelope1.noteOn();
      break;
    case 1:
      waveform2.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform2.pulseWidth(pwmLevel);
      envelope2.noteOn();
      break;
    case 2:
      waveform3.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform3.pulseWidth(pwmLevel);
      envelope3.noteOn();
      break;
    case 3:
      waveform4.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform4.pulseWidth(pwmLevel);
      envelope4.noteOn();
      break;
    case 4:
      waveform5.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform5.pulseWidth(pwmLevel);
      envelope5.noteOn();
      break;
    case 5:
      waveform6.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform6.pulseWidth(pwmLevel);
      envelope6.noteOn();
      break;
    case 6:
      waveform7.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform7.pulseWidth(pwmLevel);
      envelope7.noteOn();
      break;
    case 7:
      waveform8.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform8.pulseWidth(pwmLevel);
      envelope8.noteOn();
      break;
    case 8:
      waveform9.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform9.pulseWidth(pwmLevel);
      envelope9.noteOn();
      break;
    case 9:
      waveform10.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform10.pulseWidth(pwmLevel);
      envelope10.noteOn();
      break;
    case 10:
      waveform11.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform11.pulseWidth(pwmLevel);
      envelope11.noteOn();
      break;
    case 11:
      waveform12.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform12.pulseWidth(pwmLevel);
      envelope12.noteOn();
      break;
    case 12:
      waveform13.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform13.pulseWidth(pwmLevel);
      envelope13.noteOn();
      break;
    case 13:
      waveform14.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform14.pulseWidth(pwmLevel);
      envelope14.noteOn();
      break;
    case 14:
      waveform15.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform15.pulseWidth(pwmLevel);
      envelope15.noteOn();
      break;
    case 15:
      waveform16.begin(((velocity / 127.0) * oscLevel), oscFreq, oscWF);
      waveform16.pulseWidth(pwmLevel);
      envelope16.noteOn();
      break;
    default:
      break;
  };
  
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
    switch (voiceUsed)
  {
    case 0:
      envelope1.noteOff();
      break;
    case 1:
      envelope2.noteOff();
      break;
    case 2:
      envelope3.noteOff();
      break;
    case 3:
      envelope4.noteOff();
      break;
    case 4:
      envelope5.noteOff();
      break;
    case 5:
      envelope6.noteOff();
      break;
    case 6:
      envelope7.noteOff();
      break;
    case 7:
      envelope8.noteOff();
      break;
    case 8:
      envelope9.noteOff();
      break;
    case 9:
      envelope10.noteOff();
      break;
    case 10:
      envelope11.noteOff();
      break;
    case 11:
      envelope12.noteOff();
      break;
    case 12:
      envelope13.noteOff();
      break;
    case 13:
      envelope14.noteOff();
      break;
    case 14:
      envelope15.noteOff();
      break;
    case 15:
      envelope16.noteOff();
      break;
    default:
      break;
  };
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
      switch (k)
      {
        case 0:
          waveform1.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform1.pulseWidth(pwmLevel);
          envelope1.noteOn();
          break;
        case 1:
          waveform2.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform2.pulseWidth(pwmLevel);
          envelope2.noteOn();
          break;
        case 2:
          waveform3.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform3.pulseWidth(pwmLevel);
          envelope3.noteOn();
          break;
        case 3:
          waveform4.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform4.pulseWidth(pwmLevel);
          envelope4.noteOn();
          break;
        case 4:
          waveform5.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform5.pulseWidth(pwmLevel);
          envelope5.noteOn();
          break;
        case 5:
          waveform6.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform6.pulseWidth(pwmLevel);
          envelope6.noteOn();
          break;
        case 6:
          waveform7.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform7.pulseWidth(pwmLevel);
          envelope7.noteOn();
          break;
        case 7:
          waveform8.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform8.pulseWidth(pwmLevel);
          envelope8.noteOn();
          break;
        case 8:
          waveform9.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform9.pulseWidth(pwmLevel);
          envelope9.noteOn();
          break;
        case 9:
          waveform10.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform10.pulseWidth(pwmLevel);
          envelope10.noteOn();
          break;
        case 10:
          waveform11.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform11.pulseWidth(pwmLevel);
          envelope11.noteOn();
          break;
        case 11:
          waveform12.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform12.pulseWidth(pwmLevel);
          envelope12.noteOn();
          break;
        case 12:
          waveform13.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform13.pulseWidth(pwmLevel);
          envelope13.noteOn();
          break;
        case 13:
          waveform14.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform14.pulseWidth(pwmLevel);
          envelope14.noteOn();
          break;
        case 14:
          waveform15.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform15.pulseWidth(pwmLevel);
          envelope15.noteOn();
          break;
        case 15:
          waveform16.begin(((notes[k].myVelocity / 127.0) * oscLevel), oscFreq, oscWF);
          waveform16.pulseWidth(pwmLevel);
          envelope16.noteOn();
          break;
        default:
          break;
      };
    }
  }
  AudioInterrupts();
}
