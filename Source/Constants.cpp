/*
  ==============================================================================

    Constants.cpp
    Created: 7 Apr 2019 9:57:26pm
    Author:  Eric

  ==============================================================================
*/

#include "Constants.h"

//==============================================================================
const String Constants::WAVEFORM_TYPE_ID = "waveformType";
const String Constants::WAVETABLE_PATH_ID = "wavetablePath";
const String Constants::OSCILLATOR_PITCH_ID = "oscillatorPitch";
const String Constants::OSCILLATOR_PITCH_NAME = "Pitch";
const String Constants::OSCILLATOR_VOICES_ID = "oscillatorVoices";
const String Constants::OSCILLATOR_VOICES_NAME = "Voices";
const String Constants::CARRIER_FILTER_TYPE_ID = "carrierFilterType";
const String Constants::CARRIER_FILTER_TYPE_NAME = "CF Type";
const String Constants::CARRIER_FILTER_CUTOFF_ID = "carrierFilterCutoff";
const String Constants::CARRIER_FILTER_CUTOFF_NAME = "CF Cutoff";
const String Constants::CARRIER_FILTER_Q_ID = "carrierFilterQ";
const String Constants::CARRIER_FILTER_Q_NAME = "CF Q";
const String Constants::CARRIER_ENV_ATTACK_ID = "carrierEnvAttack";
const String Constants::CARRIER_ENV_ATTACK_NAME = "CE Attack";
const String Constants::CARRIER_ENV_DECAY_ID = "carrierEnvDecay";
const String Constants::CARRIER_ENV_DECAY_NAME = "CE Decay";
const String Constants::CARRIER_ENV_SUSTAIN_ID = "carrierEnvSustain";
const String Constants::CARRIER_ENV_SUSTAIN_NAME = "CE Sustain";
const String Constants::CARRIER_ENV_RELEASE_ID = "carrierEnvRelease";
const String Constants::CARRIER_ENV_RELEASE_NAME = "CE Release";
const String Constants::CARRIER_ENV_LEVEL_ID = "carrierEnvLevel";
const String Constants::CARRIER_ENV_LEVEL_NAME = "CE Level";

//==============================================================================
const int Constants::EDITOR_WIDTH = 900;
const int Constants::EDITOR_HEIGHT = 700;
const int Constants::COMPONENT_WIDTH = EDITOR_WIDTH / 3;
const int Constants::COMPONENT_HEIGHT = EDITOR_HEIGHT / 4;
const int Constants::PADDING = 10;

//==============================================================================
const String Constants::CARRIER_OSCILLOSCOPE_TITLE = "Carrier Oscillator";
const String Constants::FM_OSCILLOSCOPE_TITLE = "FM Oscillator";
const String Constants::CARRIER_OSCILLATOR_TITLE = "Carrier Oscillator Settings";
const String Constants::FM_OSCILLATOR_TITLE = "FM Oscillator Settings";
const String Constants::CARRIER_FILTER_TITLE = "Carrier Filter";
const String Constants::CARRIER_ENV_TITLE = "Carrier Envelope";

//==============================================================================
const String Constants::SINE = "Sin";
const String Constants::TRIANGLE = "Tri";
const String Constants::SAWTOOTH = "Saw";
const String Constants::SQUARE = "Square";
const String Constants::NOISE = "Noise";

//==============================================================================
const String Constants::OPEN_WAVETABLE = "Open wavetable...";
const String Constants::SELECT_WAVETABLE = "Select wavetable...";

//==============================================================================
const String Constants::LOWPASS = "Lowpass";
const String Constants::HIGHPASS = "Highpass";
const String Constants::BANDPASS = "Bandpass";
const String Constants::BANDREJECT = "Bandreject";
const String Constants::ALLPASS = "Allpass";

//==============================================================================
const String Constants::FILTER_TYPE = "Type";
const String Constants::FILTER_CUTOFF = "Cutoff";
const String Constants::FILTER_Q = "Q";
