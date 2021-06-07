/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ResponseCurveComponent.h"
#include "CustomRotarySlider.h"

class BypassLeftRight : public juce::Component
{
public:
	BypassLeftRight()
		:left("L"), right("R")
	{
		addAndMakeVisible(left);
		addAndMakeVisible(right);

		left.setClickingTogglesState(true);
		right.setClickingTogglesState(true);

		left.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::orange);
		left.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);

		right.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
		right.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);

		left.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
		left.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);

		right.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
		right.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);

		left.setConnectedEdges(juce::TextButton::ConnectedEdgeFlags::ConnectedOnRight);
		right.setConnectedEdges(juce::TextButton::ConnectedEdgeFlags::ConnectedOnLeft);
	}

	void setMidSides(bool midSides)
	{
		left.setButtonText(midSides ? "M" : "L");
		right.setButtonText(midSides ? "S" : "R");
	}

	void paint(juce::Graphics&) override {}

	void resized() override
	{
		auto bounds = getLocalBounds();
		left.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
		right.setBounds(bounds);
	}

	juce::TextButton left, right;
private:
};

//==============================================================================
/**
*/
class BandsEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	BandsEQAudioProcessorEditor(BandsEQAudioProcessor&);
	~BandsEQAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	BandsEQAudioProcessor& audioProcessor;

	CustomRotarySlider hmfFcSlider, hmfGainSlider, hmfQSlider;
	CustomRotarySlider lmfFcSlider, lmfGainSlider, lmfQSlider;

	CustomRotarySlider highShelfFcSlider, highShelfGainSlider, highShelfQSlider;
	CustomRotarySlider lowShelfFcSlider, lowShelfGainSlider, lowShelfQSlider;

	CustomRotarySlider lowCutSlider, lowCutSlope;
	CustomRotarySlider highCutSlider, highCutSlope;

	ResponseCurveComponent responseCurveComponent;

	BypassLeftRight bLowCut, bHighCut, bLowShelf, bHighShelf, bLmf, bHmf;

	using APVTS = juce::AudioProcessorValueTreeState;
	using SAttachment = APVTS::SliderAttachment;
	using BAttachment = APVTS::ButtonAttachment;

	SAttachment hmfFcA, hmfGainA, hmfQA;
	SAttachment lmfFcA, lmfGainA, lmfQA;
	SAttachment lowShelfFcA, lowShelfGainA, lowShelfQA;
	SAttachment highShelfFcA, highShelfGainA, highShelfQA;
	SAttachment lowCutA, lowCutSlopeA;
	SAttachment highCutA, highCutSlopeA;

	BAttachment bHmfAL, bLmfAL, bLowCutAL, bHighCutAL, bLowShelfAL, bHighShelfAL;
	BAttachment bHmfAR, bLmfAR, bLowCutAR, bHighCutAR, bLowShelfAR, bHighShelfAR;

	juce::Label lowCutL, highCutL, lowShelfL, highShelfL, lmfL, hmfL;

	std::vector<juce::Component*> getComps();
	std::vector<juce::Label*> getLabels();
	std::vector<juce::Slider*> getFrequencySliders();
	std::vector<juce::Slider*> getGainSliders();
	std::vector<juce::Slider*> getQSliders();

	void setLabel(juce::Label& l);
	void setFrequencySlider(juce::Slider& s);
	void setGainSlider(juce::Slider& s);
	void setQSlider(juce::Slider& s);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandsEQAudioProcessorEditor)
};
