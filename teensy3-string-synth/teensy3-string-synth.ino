//---------------------------------------------------------------------------------------------//
// Teensy 3 String Machine
// A 32-voice synthesizer focused on replicating the top-divider plus ensemble chorus sound
// CPU runs about 30% on a Teensy 3.6. Uncomment the CPU utilization report code if running
// it on a Teensy < 3.6.
// 
//---------------------------------------------------------------------------------------------//


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
#include "custWaveforms.h"
#include <math.h>

// how many voices do we have?
#define VOICES 32

// maxium EG time for any envelope phase in milliseconds
#define MAX_EG_TIME 3000

#define CHANNEL_SET_PIN 2
#define EE_CHECKBYTE_1 1022
#define EE_CHECKBYTE_2 1023
#define EE_MIDI_CHANNEL 11
#define LED_PIN 13

// two seconds for a long button press
#define LONG_PRESS 2000L

// 2 millseconds for the sinewave LFO update
#define LFO_PERIOD 3000L
// 2 seconds for LFO cycle
// 2 ms is 500 updates/s
// so 1000 would be 2 seconds
#define BIQUAD_LFO_RANGE 1500

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
AudioEffectEnvelope envelope17;
AudioEffectEnvelope envelope18;
AudioEffectEnvelope envelope19;
AudioEffectEnvelope envelope20;
AudioEffectEnvelope envelope21;
AudioEffectEnvelope envelope22;
AudioEffectEnvelope envelope23;
AudioEffectEnvelope envelope24;
AudioEffectEnvelope envelope25;
AudioEffectEnvelope envelope26;
AudioEffectEnvelope envelope27;
AudioEffectEnvelope envelope28;
AudioEffectEnvelope envelope29;
AudioEffectEnvelope envelope30;
AudioEffectEnvelope envelope31;
AudioEffectEnvelope envelope32;
AudioEffectEnsemble ensemble1;


// array of envelopes
AudioEffectEnvelope *myEnvelope[VOICES] = { &envelope1, &envelope2, &envelope3, &envelope4, &envelope5, &envelope6, \
&envelope7, &envelope8, &envelope9, &envelope10, &envelope11, &envelope12, &envelope13, &envelope14, &envelope15, \
&envelope16, &envelope17, &envelope18, &envelope19, &envelope20, &envelope21, &envelope22, &envelope23, &envelope24, \
&envelope25, &envelope26, &envelope27, &envelope28, &envelope29, &envelope30, &envelope31, &envelope32 };

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
AudioSynthWaveform waveform17;
AudioSynthWaveform waveform18;
AudioSynthWaveform waveform19;
AudioSynthWaveform waveform20;
AudioSynthWaveform waveform21;
AudioSynthWaveform waveform22;
AudioSynthWaveform waveform23;
AudioSynthWaveform waveform24;
AudioSynthWaveform waveform25;
AudioSynthWaveform waveform26;
AudioSynthWaveform waveform27;
AudioSynthWaveform waveform28;
AudioSynthWaveform waveform29;
AudioSynthWaveform waveform30;
AudioSynthWaveform waveform31;
AudioSynthWaveform waveform32;

// array of waveforms
AudioSynthWaveform *myWaveform[VOICES] = { &waveform1, &waveform2, &waveform3, &waveform4, &waveform5, &waveform6, \
&waveform7, &waveform8, &waveform9, &waveform10, &waveform11, &waveform12, &waveform13, &waveform14, &waveform15, \
&waveform16, &waveform17, &waveform18, &waveform19, &waveform20, &waveform21, &waveform22, &waveform23, &waveform24, \
&waveform25, &waveform26, &waveform27, &waveform28, &waveform29, &waveform30, &waveform31, &waveform32};

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
AudioConnection          patchCord17(waveform17, envelope17);
AudioConnection          patchCord18(waveform18, envelope18);
AudioConnection          patchCord19(waveform19, envelope19);
AudioConnection          patchCord20(waveform20, envelope20);
AudioConnection          patchCord21(waveform21, envelope21);
AudioConnection          patchCord22(waveform22, envelope22);
AudioConnection          patchCord23(waveform23, envelope23);
AudioConnection          patchCord24(waveform24, envelope24);
AudioConnection          patchCord25(waveform25, envelope25);
AudioConnection          patchCord26(waveform26, envelope26);
AudioConnection          patchCord27(waveform27, envelope27);
AudioConnection          patchCord28(waveform28, envelope28);
AudioConnection          patchCord29(waveform29, envelope29);
AudioConnection          patchCord30(waveform30, envelope30);
AudioConnection          patchCord31(waveform31, envelope31);
AudioConnection          patchCord32(waveform32, envelope32);

AudioMixer4              mixer_group_1;
AudioMixer4              mixer_group_2;
AudioMixer4              mixer_group_3;
AudioMixer4              mixer_group_4;
AudioMixer4              mixer_group_5;
AudioMixer4              mixer_group_6;
AudioMixer4              mixer_group_7;
AudioMixer4              mixer_group_8;

// array of waveforms
AudioMixer4 *myMixerGroup[8] = { &mixer_group_1, &mixer_group_2, &mixer_group_3, &mixer_group_4, &mixer_group_5, \
&mixer_group_6, &mixer_group_7, &mixer_group_8};

AudioMixer4              mixer_combo_1;
AudioMixer4              mixer_combo_2;

AudioMixer4              mixer_all_voices;

AudioMixer4              mixer_phaser_1;
AudioMixer4              mixer_phaser_2;

AudioFilterStateVariable filter1;
AudioAmplifier           amp1;
AudioAmplifier           amp2;

// lfo
AudioSynthWaveformSine   sine1;
// for phaser effect
AudioFilterBiquad        biquad1;
AudioFilterBiquad        biquad2;

AudioConnection          patchCord34(envelope1, 0, mixer_group_1, 0);
AudioConnection          patchCord35(envelope2, 0, mixer_group_1, 1);
AudioConnection          patchCord36(envelope3, 0, mixer_group_1, 2);
AudioConnection          patchCord37(envelope4, 0, mixer_group_1, 3);
AudioConnection          patchCord38(envelope5, 0, mixer_group_2, 0);
AudioConnection          patchCord39(envelope6, 0, mixer_group_2, 1);
AudioConnection          patchCord40(envelope7, 0, mixer_group_2, 2);
AudioConnection          patchCord41(envelope8, 0, mixer_group_2, 3);
AudioConnection          patchCord42(envelope9, 0, mixer_group_3, 0);
AudioConnection          patchCord43(envelope10, 0, mixer_group_3, 1);
AudioConnection          patchCord44(envelope11, 0, mixer_group_3, 2);
AudioConnection          patchCord45(envelope12, 0, mixer_group_3, 3);
AudioConnection          patchCord46(envelope13, 0, mixer_group_4, 0);
AudioConnection          patchCord47(envelope14, 0, mixer_group_4, 1);
AudioConnection          patchCord48(envelope15, 0, mixer_group_4, 2);
AudioConnection          patchCord49(envelope16, 0, mixer_group_4, 3);
AudioConnection          patchCord50(envelope17, 0, mixer_group_5, 0);
AudioConnection          patchCord51(envelope18, 0, mixer_group_5, 1);
AudioConnection          patchCord52(envelope19, 0, mixer_group_5, 2);
AudioConnection          patchCord53(envelope20, 0, mixer_group_5, 3);
AudioConnection          patchCord54(envelope21, 0, mixer_group_6, 0);
AudioConnection          patchCord55(envelope22, 0, mixer_group_6, 1);
AudioConnection          patchCord56(envelope23, 0, mixer_group_6, 2);
AudioConnection          patchCord57(envelope24, 0, mixer_group_6, 3);
AudioConnection          patchCord58(envelope25, 0, mixer_group_7, 0);
AudioConnection          patchCord59(envelope26, 0, mixer_group_7, 1);
AudioConnection          patchCord60(envelope27, 0, mixer_group_7, 2);
AudioConnection          patchCord61(envelope28, 0, mixer_group_7, 3);
AudioConnection          patchCord62(envelope29, 0, mixer_group_8, 0);
AudioConnection          patchCord63(envelope30, 0, mixer_group_8, 1);
AudioConnection          patchCord64(envelope31, 0, mixer_group_8, 2);
AudioConnection          patchCord65(envelope32, 0, mixer_group_8, 3);

AudioConnection          patchCord66(mixer_group_1, 0, mixer_combo_1, 0);
AudioConnection          patchCord67(mixer_group_2, 0, mixer_combo_1, 1);
AudioConnection          patchCord68(mixer_group_3, 0, mixer_combo_1, 2);
AudioConnection          patchCord69(mixer_group_4, 0, mixer_combo_1, 3);
AudioConnection          patchCord70(mixer_group_5, 0, mixer_combo_2, 0);
AudioConnection          patchCord71(mixer_group_6, 0, mixer_combo_2, 1);
AudioConnection          patchCord72(mixer_group_7, 0, mixer_combo_2, 2);
AudioConnection          patchCord73(mixer_group_8, 0, mixer_combo_2, 3);

AudioConnection          patchCord74(mixer_combo_1, 0, mixer_all_voices, 0);
AudioConnection          patchCord75(mixer_combo_2, 0, mixer_all_voices, 1);

AudioConnection          patchCord76(mixer_all_voices, 0, filter1, 0);
AudioConnection          patchCord77(mixer_all_voices, 0, filter1, 1);

AudioConnection          patchCord78(filter1, 0, ensemble1, 0);



AudioConnection          patchCord80(ensemble1, 0, mixer_phaser_1, 0);
AudioConnection          patchCord81(ensemble1, 1, mixer_phaser_2, 0);
AudioConnection          patchCord82(ensemble1, 0, biquad1, 0);
AudioConnection          patchCord83(ensemble1, 1, biquad2, 0);
AudioConnection          patchCord84(biquad1, 0, mixer_phaser_1, 1);
AudioConnection          patchCord85(biquad2, 0, mixer_phaser_2, 1);

AudioConnection          patchCord86(mixer_phaser_1, 0, amp1, 0);
AudioConnection          patchCord87(mixer_phaser_2, 0, amp2, 0);

AudioConnection          patchCord88(amp1, 0, i2sOut0, 0);
AudioConnection          patchCord89(amp2, 0, i2sOut0, 1);


AudioControlWM8731       wm8731_1;       //xy=1428,56

const int16_t *customWaveforms[8] = { custWave1, custWave2, custWave3, custWave4, custWave5, custWave6, custWave7, custWave8 };

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



// oscillator waveform index
int oscWF = 0;
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

// track led1
int isLed1 = 0;
// track led2
int isLed2 = 0;

// Create physical controls for Expansion Board, 2 switches, 3 pots, 0 encoders, 2 LEDs
BAPhysicalControls controls(BA_EXPAND_NUM_SW, BA_EXPAND_NUM_POT, BA_EXPAND_NUM_ENC, BA_EXPAND_NUM_LED);

int buttonRHandle, centerKnobHandle, attackHandle, releaseHandle, buttonLHandle, ledLHandle, ledRHandle;

float potValue;

long buttonTimer;
int lPressed = 0;

float biquadFrequency = 3000.0;
long biquadLfoUpdateTimer;
int lfoIndex = 0;
float phaserDryWet = 0.0;

//---------------------------------------------------------------------------------------------//
// setup
//---------------------------------------------------------------------------------------------//
void setup() {
  Serial.begin(115200);

  // set up the LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  wm8731_1.enable();
  AudioMemory(128);
  // disable and enable the codec
  codecControl.disable();
  delay(100);
  codecControl.enable();
  delay(100);
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

  // set gain
  for (int gc = 0; gc < 8; gc++)
  {
    for (int cc = 0; cc < 4; cc++)
    {
      myMixerGroup[gc]->gain(cc, 0.25);
    }  
  }

  
  mixer_combo_1.gain(0, 0.25);
  mixer_combo_1.gain(1, 0.25);
  mixer_combo_1.gain(2, 0.25);
  mixer_combo_1.gain(3, 0.25);
  mixer_combo_2.gain(0, 0.25);
  mixer_combo_2.gain(1, 0.25);
  mixer_combo_2.gain(2, 0.25);
  mixer_combo_2.gain(3, 0.25);
  
  mixer_all_voices.gain(0, 0.5);
  mixer_all_voices.gain(1, 0.5);
  amp1.gain(2.8);
  amp2.gain(2.8);

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

  biquad1.setNotch(0, biquadFrequency, 0.3);
  biquad2.setNotch(0, biquadFrequency, 0.3);

  // initialize LFO
  sine1.amplitude(1.0);
  sine1.frequency(0.5);

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
  
  // MIDI setup
  MIDI.begin(myChannel);  
  MIDI.setHandleNoteOn(doNoteOn); 
  MIDI.setHandleControlChange(doCC); 
  MIDI.setHandleNoteOff(doNoteOff); 
  MIDI.setHandlePitchBend(doBend);
  
  // Setup the controls. The return value is the handle to use when checking for control changes, etc.
  // pushbuttons
  buttonLHandle = controls.addSwitch(BA_EXPAND_SW1_PIN); // will be used for vox humana control
  buttonRHandle = controls.addSwitch(BA_EXPAND_SW2_PIN); // will be used for sub-octave
  // pots
  attackHandle = controls.addPot(BA_EXPAND_POT1_PIN, potCalibMin, potCalibMax, potSwapDirection); // control the amount of delay
  releaseHandle = controls.addPot(BA_EXPAND_POT2_PIN, potCalibMin, potCalibMax, potSwapDirection); 
  centerKnobHandle = controls.addPot(BA_EXPAND_POT3_PIN, potCalibMin, potCalibMax, potSwapDirection); 
  // leds
  ledLHandle = controls.addOutput(BA_EXPAND_LED1_PIN);
  controls.setOutput(ledLHandle, 0);
  ledRHandle = controls.addOutput(BA_EXPAND_LED2_PIN);
  controls.setOutput(ledRHandle, 0);
  
  controls.setOutput(ledLHandle, 1);
  delay(200);
  controls.setOutput(ledLHandle, 0);
  delay(200);
  controls.setOutput(ledRHandle, 1);
  delay(200);
  controls.setOutput(ledRHandle, 0);
  delay(50);

  biquadLfoUpdateTimer = micros();
  //Serial.println("end of setup");
  
}

//---------------------------------------------------------------------------------------------//
// main loop
//---------------------------------------------------------------------------------------------//
void loop()
{

  // find completed notes and mark the voice as available
  for (int p = 0; p < VOICES; p++)
  {
    // first clean up completed notes
    if (!(myEnvelope[p]->isActive()))
      notes[p].mySequence = 0;  
  }
  
  // get MIDI notes if available
  MIDI.read();

  // check for long or short press
  if ((controls.isSwitchHeld(buttonLHandle)))
  {
    buttonTimer = millis();
    while (controls.isSwitchHeld(buttonLHandle));
    lPressed = 1;
  }

  // check button 1 (left)
  if (lPressed)
  {
    if ((millis() - buttonTimer) < LONG_PRESS)
    {
      if (isLed1)
      {
        controls.setOutput(ledLHandle, 0);
        isLed1 = 0;
      }
      else
      {
        controls.setOutput(ledLHandle, 1);
        isLed1 = 1;
      }
    }
    else
    {
      isLed1 = setMIDIChannel(isLed1);
      MIDI.begin(myChannel);
    }
    lPressed = 0;
  }
  
  // check button 2 (right)
  if (controls.isSwitchToggled(buttonRHandle))
  {
    oscWF++;
    if (oscWF > 7)
      oscWF = 0;
    controls.setOutput(ledRHandle, 1);
    delay(100);
    controls.setOutput(ledRHandle, 0);
  }

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
  
  // cutoff
  if (controls.checkPotValue(centerKnobHandle, potValue))
  {
    if (isLed1 == 0)
    {
      vcfCutoff = (potValue * 10000 + 20);
      filter1.frequency(vcfCutoff);
    }
    else
    {
      mixer_phaser_1.gain(0, 1 - potValue);
      mixer_phaser_1.gain(1, potValue);
      mixer_phaser_2.gain(0, 1 - potValue);
      mixer_phaser_2.gain(1, potValue);
    }
  }

  
  // update the biquad filter frequency
  if ((micros() - biquadLfoUpdateTimer) >= LFO_PERIOD)
  {
    lfoIndex++;
    if (lfoIndex > BIQUAD_LFO_RANGE)
      lfoIndex = 0;
    biquad1.setNotch(0, ((sin((( 2.0 * M_PI) / BIQUAD_LFO_RANGE) * lfoIndex)) * 750) + 1000, 0.9);
    biquad2.setNotch(0, ((sin((( 2.0 * M_PI) / BIQUAD_LFO_RANGE) * lfoIndex)) * 750) + 1000, 0.9);
    biquad1.setNotch(1, ((sin((( 2.0 * M_PI) / BIQUAD_LFO_RANGE) * lfoIndex)) * 1000) + 2000, 0.9);
    biquad2.setNotch(1, ((sin((( 2.0 * M_PI) / BIQUAD_LFO_RANGE) * lfoIndex)) * 1000) + 2000, 0.9);
    biquad1.setNotch(2, ((sin((( 2.0 * M_PI) / BIQUAD_LFO_RANGE) * lfoIndex)) * 3000) + 5000, 0.9);
    biquad2.setNotch(2, ((sin((( 2.0 * M_PI) / BIQUAD_LFO_RANGE) * lfoIndex)) * 3000) + 5000, 0.9);
    biquadLfoUpdateTimer = micros();
  }
  
} 

//---------------------------------------------------------------------------------------------//
// function doNoteOn
//---------------------------------------------------------------------------------------------//
void doNoteOn(byte channel, byte pitch, byte velocity)
{
  //Serial.println("got a note");
  
  // flag if a free voice was found 
  unsigned int voicesFree = 0;
  // the voice to be used
  unsigned int voiceUsed = 0;
  // temporary variable for finding oldest note
  byte seqNo = 0;
  int i;

  //Serial.println("doNoteOn");

//  digitalWrite(LED_PIN, HIGH);

  // try to find a free voice to play the note
  for (i = 0; i < VOICES; i++)
  {
    // first clean up completed notes
    if (!(myEnvelope[i]->isActive()))
      notes[i].mySequence = 0;
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
      
      //Serial.print("Using voice ");
      //Serial.println(voiceUsed);
      
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
        
    //Serial.print("Stealing voice ");
    //Serial.println(voiceUsed);

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
  myWaveform[voiceUsed]->begin(WAVEFORM_ARBITRARY);
  myWaveform[voiceUsed]->arbitraryWaveform(customWaveforms[oscWF], 172.0);
  myWaveform[voiceUsed]->frequency(oscFreq);
  myWaveform[voiceUsed]->amplitude((velocity / 127.0) * oscLevel);
  myEnvelope[voiceUsed]->noteOn();
  AudioInterrupts();

  
//  digitalWrite(LED_PIN, LOW);
//
//  Serial.println("CPU: ");
//  Serial.print(AudioProcessorUsage());
//  Serial.print("    ");
//  Serial.print("Memory: ");
//  Serial.print(AudioMemoryUsage());
//  Serial.println(" ");
  
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
  
  //Serial.print("Releasing voice ");
  //Serial.println(voiceUsed);

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
      //myWaveform[k]->begin(oscWF);
      //myWaveform[k]->arbitraryWaveform(custWave1, 172.0);
      myWaveform[k]->frequency(oscFreq);
      myWaveform[k]->amplitude((notes[k].myVelocity / 127.0) * oscLevel);
      //myEnvelope[k]->noteOn();
    }
  }
  AudioInterrupts();
}

//---------------------------------------------------------------------------------------------//
// function setMIDIChannel
//---------------------------------------------------------------------------------------------//
int setMIDIChannel(int ledState)
{
  int clickCount = 0;
  unsigned long previousMillis;
    
  // light the led for 1.0 sec to indicate set mode
  if (ledState)
  {
    controls.setOutput(ledLHandle, 0);
    delay(500);
  }
  controls.setOutput(ledLHandle, 1);
  delay(1000);
  controls.setOutput(ledLHandle, 0);
  
  // loop until the channel button is held down
  while (1)
  {
    // time each press
    if ((controls.isSwitchHeld(buttonLHandle)))
    {
      previousMillis = millis();
      while (controls.isSwitchHeld(buttonLHandle));
      if ((millis() - previousMillis) < LONG_PRESS)
      {
        clickCount++;
        Serial.println(clickCount);
      }
      // if long press and at least one short press
      if (((millis() - previousMillis) > LONG_PRESS) && (clickCount > 0))
      {
        Serial.println((millis() - previousMillis));
        break;
      }
    }    
  }
      
  // light the led for 1.0 sec to indicate set mode done
  controls.setOutput(ledLHandle, 1);
  delay(1000);
  controls.setOutput(ledLHandle, 0);
  
  // blink back the channel
  for (int r = 0; r <= clickCount; r++)
  {
    controls.setOutput(ledLHandle, 1);
    delay(50);
    controls.setOutput(ledLHandle, 0);
    delay(500);
  }
 
  // store the setting to EEPROM if not zero or greater than 15
  if ((clickCount > 0) && (clickCount < 16))
  {
    EEPROM.write(EE_MIDI_CHANNEL, clickCount);
    myChannel = clickCount;
  }
 
  return ledState;
}
