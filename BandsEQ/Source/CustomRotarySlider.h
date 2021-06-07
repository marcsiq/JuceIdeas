/*
  ==============================================================================

	CustomRotarySlider.h
	Created: 24 Apr 2021 2:13:35pm
	Author:  marcs

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MyLookAndFeel : public juce::LookAndFeel_V4
{
public:
	void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
		float sliderPosProportional, float rotaryStartAngle,
		float rotaryEndAngle, juce::Slider&) override;

private:
	int getTextHeight() const { return 12; }
	juce::String getDisplayString(juce::Slider& s);
};

class CustomRotarySlider : public juce::Slider
{
public:
	CustomRotarySlider(juce::RangedAudioParameter* p);
	~CustomRotarySlider() override;

	juce::String getDisplayString();

private:
	juce::RangedAudioParameter* param;
	MyLookAndFeel lnf;
};
