This is a 32-voice 'string machine' meant for the pjrc.com teensy 3.6 and the blackaddr audio interface. It depends on the Teensy Audio Ensemble Chorus library found here: https://github.com/quarterturn/teensy3-ensemble-chorus.

The control layout on the blackaddr audio interface control board is as follows:
- left knob is 'attack'
- middle knob is filter cutoff if left LED is not lit
- middle knob is phaser dry-wet mix if left LED is lit
- right knob is 'release'
- left button changes middle knob to phaser dry-wet (LED on = mix, LED off = cutoff)
- left button held for >2 seconds enters MIDI channel set mode
- right button cycles through the available waveforms

To set midi channel:
- enter midi set mode as above
- press the right button x times as the channel
- hold the button again for >2 seconds until the right LED blinks
- midi channel is now set and stored in EEPROM to the new channel.

I am still working out some bugs with regards to 'phantom' button and knob events at startup. The synth engine itself seems pretty stable.

quartertun@matrixwide.com
Alexander Davis
