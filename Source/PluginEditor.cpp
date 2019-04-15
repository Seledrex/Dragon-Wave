/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "LoadingThread.h"

//==============================================================================
DragonWaveAudioProcessorEditor::DragonWaveAudioProcessorEditor(DragonWaveAudioProcessor& p) :
	AudioProcessorEditor(&p), processor(p),
	carrierOscilloscopeGUI(p,
		Constants::CARRIER_OSCILLOSCOPE_TITLE),
	fmOscilloscopeGUI(p,
		Constants::FM_OSCILLOSCOPE_TITLE),
	carrierOscGUI(p),
	fmOscGUI(p),
	carrierFilterGUI(p,
		Constants::CARRIER_FILTER_TITLE,
		Constants::CARRIER_FILTER_CUTOFF_ID,
		Constants::CARRIER_FILTER_Q_ID,
		Constants::CARRIER_FILTER_TYPE_ID),
	carrierEnvGUI(p,
		Constants::CARRIER_ENV_TITLE,
		Constants::CARRIER_ENV_ATTACK_ID,
		Constants::CARRIER_ENV_DECAY_ID,
		Constants::CARRIER_ENV_SUSTAIN_ID,
		Constants::CARRIER_ENV_RELEASE_ID,
		Constants::CARRIER_ENV_LEVEL_ID)
{
	setSize(Constants::EDITOR_WIDTH, Constants::EDITOR_HEIGHT);
	startTimerHz(60);

	addAndMakeVisible(carrierOscilloscopeGUI);
	addAndMakeVisible(fmOscilloscopeGUI);
	addAndMakeVisible(carrierOscGUI);
	addAndMakeVisible(fmOscGUI);
	addAndMakeVisible(carrierFilterGUI);
	addAndMakeVisible(carrierEnvGUI);
}

DragonWaveAudioProcessorEditor::~DragonWaveAudioProcessorEditor()
{
}

//==============================================================================
void DragonWaveAudioProcessorEditor::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void DragonWaveAudioProcessorEditor::resized()
{
	auto column1 = getLocalBounds().withTrimmedRight(Constants::COMPONENT_WIDTH * 2);
	auto column2 = getLocalBounds().withTrimmedLeft(Constants::COMPONENT_WIDTH).withTrimmedRight(Constants::COMPONENT_WIDTH);

	// Set up column one components
	carrierOscilloscopeGUI.setBounds(column1.removeFromTop(Constants::COMPONENT_HEIGHT));
	carrierOscGUI.setBounds(column1.removeFromTop(Constants::COMPONENT_HEIGHT));
	carrierFilterGUI.setBounds(column1.removeFromTop(Constants::COMPONENT_HEIGHT));
	carrierEnvGUI.setBounds(column1.removeFromTop(Constants::COMPONENT_HEIGHT));

	// Set up column two components
	fmOscilloscopeGUI.setBounds(column2.removeFromTop(Constants::COMPONENT_HEIGHT));
	fmOscGUI.setBounds(column2.removeFromTop(Constants::COMPONENT_HEIGHT));
}

//==============================================================================
void DragonWaveAudioProcessorEditor::timerCallback()
{
	carrierOscilloscopeGUI.setSound(ReferenceCountedSound::Ptr(processor.currentSound));
	fmOscilloscopeGUI.setSound(ReferenceCountedSound::Ptr(processor.currentSound));
	carrierOscilloscopeGUI.incrementFrameCount();
	fmOscilloscopeGUI.incrementFrameCount();
	carrierOscilloscopeGUI.repaint();
	fmOscilloscopeGUI.repaint();
}

