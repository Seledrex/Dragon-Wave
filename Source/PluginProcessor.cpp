/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WavetableVoice.h"
#include "LoadingThread.h"

//==============================================================================
DragonWaveAudioProcessor::DragonWaveAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	),
	parameters(*this, nullptr, "PARAMETERS", createParameterLayout()) // Initialize parameters
#endif
{
	// Add voices to the synth
	int numVoices = (int)*parameters.getRawParameterValue(Constants::OSCILLATOR_VOICES_ID);
	for (auto i = 0; i < numVoices; i++) {
		synth.addVoice(new WavetableVoice());
	}

	// Start the loading thread
	loadingThread = std::unique_ptr<LoadingThread>(new LoadingThread(*this));
}

DragonWaveAudioProcessor::~DragonWaveAudioProcessor()
{
}

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout DragonWaveAudioProcessor::createParameterLayout()
{
	// Define parameters here
	std::vector<std::unique_ptr<RangedAudioParameter>> params;

	auto oscillatorPitch = std::make_unique<AudioParameterInt>(
		Constants::OSCILLATOR_PITCH_ID,
		Constants::OSCILLATOR_PITCH_NAME,
		-24, 24, 0
	);
	params.push_back(std::move(oscillatorPitch));

	auto oscillatorVoices = std::make_unique<AudioParameterInt>(
		Constants::OSCILLATOR_VOICES_ID,
		Constants::OSCILLATOR_VOICES_NAME,
		1, 32, 8
	);
	params.push_back(std::move(oscillatorVoices));

	auto filterType = std::make_unique<AudioParameterChoice>(
		Constants::FILTER_TYPE_ID,
		Constants::FILTER_TYPE_NAME,
		StringArray("Lowpass", "Highpass", "Bandpass"),
		0
	);
	params.push_back(std::move(filterType));

	auto filterCutoff = std::make_unique<AudioParameterFloat>(
		Constants::FILTER_CUTOFF_ID,
		Constants::FILTER_CUTOFF_NAME,
		NormalisableRange<float>(20.0f, 22050.0f, 0.01f, 0.5f),
		22050.0f
	);
	params.push_back(std::move(filterCutoff));

	auto filterQ = std::make_unique<AudioParameterFloat>(
		Constants::FILTER_Q_ID,
		Constants::FILTER_Q_NAME,
		NormalisableRange<float>(1.0f, 5.0f, 1.0f),
		1.0f
	);
	params.push_back(std::move(filterQ));

	return { params.begin(), params.end() };
}

//==============================================================================
const String DragonWaveAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool DragonWaveAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool DragonWaveAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool DragonWaveAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double DragonWaveAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int DragonWaveAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int DragonWaveAudioProcessor::getCurrentProgram()
{
	return 0;
}

void DragonWaveAudioProcessor::setCurrentProgram(int /*index*/)
{
}

const String DragonWaveAudioProcessor::getProgramName(int /*index*/)
{
	return {};
}

void DragonWaveAudioProcessor::changeProgramName(int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void DragonWaveAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
	synth.setCurrentPlaybackSampleRate(sampleRate);
}

void DragonWaveAudioProcessor::releaseResources()
{
	currentSound = nullptr;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DragonWaveAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void DragonWaveAudioProcessor::processBlock(AudioBuffer<float> & buffer, MidiBuffer & midiMessages)
{
	buffer.clear();

	int time;
	MidiMessage m;

	// Keep track of note on count for the editor animation
	for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
	{
		if (m.isNoteOn())
			noteOnCount++;
		else if (m.isNoteOff())
			noteOnCount--;
	}

	// Make sure count does not go negative!
	if (noteOnCount < 0)
		noteOnCount = 0;

	int numVoices = (int)* parameters.getRawParameterValue(Constants::OSCILLATOR_VOICES_ID);
	if (numVoices < synth.getNumVoices())
	{
		for (int i = synth.getNumVoices() - 1; i >= numVoices; i--)
			synth.removeVoice(i);
	}
	else if (numVoices > synth.getNumVoices())
	{
		int numVoicesToAdd = numVoices - synth.getNumVoices();
		for (int i = 0; i < numVoicesToAdd; i++)
			synth.addVoice(new WavetableVoice());
	}

	WavetableVoice* voice;
	for (int i = 0; i < synth.getNumVoices(); i++)
	{
		voice = dynamic_cast<WavetableVoice*>(synth.getVoice(i));

		if (voice != nullptr)
		{
			voice->setPitchShift(parameters.getRawParameterValue(Constants::OSCILLATOR_PITCH_ID));
		}
	}

	synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool DragonWaveAudioProcessor::hasEditor() const
{
	return true;
}

AudioProcessorEditor* DragonWaveAudioProcessor::createEditor()
{
	return new DragonWaveAudioProcessorEditor(*this);
}

//==============================================================================
void DragonWaveAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// Copy state and convert to XML
	auto state = parameters.copyState();
	std::unique_ptr<XmlElement> xml(state.createXml());

	// Include waveform choice
	int enumVal = chosenWaveform;
	xml->setAttribute(Constants::WAVEFORM_TYPE_ID, enumVal);

	// Include wavetable file path
	if (currentSound != nullptr)
		xml->setAttribute(Constants::WAVETABLE_PATH_ID, currentSound->getPath());
	else
		xml->setAttribute(Constants::WAVETABLE_PATH_ID, "");

	// Store parameters in the memory block
	copyXmlToBinary(*xml, destData);
}

// Restores parameters from this memory block,
// whose contents will have been created by the getStateInformation() call.
void DragonWaveAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// Read memory block into XML
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
	{
		if (xmlState->hasTagName(parameters.state.getType()))
		{
			// Replace parameter state with that of the XML
			parameters.replaceState(ValueTree::fromXml(*xmlState));

			// Get the chosen waveform and file path for the wavetable
			int chosenWavefromIndex = std::stoi(xmlState->getStringAttribute(Constants::WAVEFORM_TYPE_ID).toStdString());
			chosenWaveform = static_cast<WavetableSound::Waveform>(chosenWavefromIndex);
			chosenPath = xmlState->getStringAttribute(Constants::WAVETABLE_PATH_ID);

			// Notify the thread to load
			loadingThread->notify();
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new DragonWaveAudioProcessor();
}