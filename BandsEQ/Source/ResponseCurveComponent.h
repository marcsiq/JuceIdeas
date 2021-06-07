/*
  ==============================================================================

	ResponseCurveComponent.h
	Created: 24 Apr 2021 1:54:11pm
	Author:  marcs

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ResponseCurveComponent : public juce::Component,
	juce::AudioProcessorParameter::Listener,
	juce::Timer
{
public:
	ResponseCurveComponent(BandsEQAudioProcessor&);
	~ResponseCurveComponent() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

	//==============================================================================
	void parameterValueChanged(int parameterIndex, float newValue) override;
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
	void timerCallback() override;

private:
	BandsEQAudioProcessor& audioProcessor;

	juce::Atomic<bool> parametersChanged;

	MonoChain chains[N_Channels];

	juce::Image background;

	void updateChain();

	juce::Rectangle<int> getRenderArea();
	juce::Rectangle<int> getAnalysisArea();
};
