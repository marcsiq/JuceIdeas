/*
  ==============================================================================

	CustomRotarySlider.cpp
	Created: 24 Apr 2021 2:13:35pm
	Author:  marcs

  ==============================================================================
*/

#include "CustomRotarySlider.h"

void MyLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
	float sliderPosProportional, float rotaryStartAngle,
	float rotaryEndAngle, juce::Slider& s)
{
	using namespace juce;

	auto fullBounds = Rectangle<float>(x, y, width, height);

	auto size = jmin(fullBounds.getWidth(), fullBounds.getHeight());
	size -= getTextHeight() * 2;

	juce::Rectangle<float> bounds;
	bounds.setSize(size, size);
	bounds.setCentre(fullBounds.getCentreX(), fullBounds.getCentreY());

	g.setColour(Colours::white);
	g.drawEllipse(bounds, 2.0f);

	Path p;

	p.startNewSubPath(bounds.getCentreX(), bounds.getCentreY() - size / 4);
	p.lineTo(bounds.getCentreX(), bounds.getCentreY() - size / 2);

	auto sliderAngRad = jmap(sliderPosProportional, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);

	p.applyTransform(AffineTransform().rotated(sliderAngRad, bounds.getCentreX(), bounds.getCentreY()));

	g.strokePath(p, PathStrokeType(2.0f));

	g.setColour(Colours::orange);
	g.setFont(getTextHeight());
	auto text = getDisplayString(s);
	auto strWidth = g.getCurrentFont().getStringWidth(text);

	bounds.setSize(strWidth, getTextHeight());
	bounds.setCentre(fullBounds.getCentreX(), fullBounds.getCentreY());
	g.drawFittedText(text, bounds.toNearestInt(), juce::Justification::centred, 1);
}

juce::String MyLookAndFeel::getDisplayString(juce::Slider& s)
{
	if (auto* mySlider = dynamic_cast<CustomRotarySlider*>(&s))
	{
		return mySlider->getDisplayString();
	}
	else
	{
		return juce::String(s.getValue()) + " " + s.getTextValueSuffix();
	}
}

CustomRotarySlider::CustomRotarySlider(juce::RangedAudioParameter* p)
	: juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
		juce::Slider::TextEntryBoxPosition::NoTextBox),
	param(p)
{
	setLookAndFeel(&lnf);
}

CustomRotarySlider::~CustomRotarySlider()
{
	setLookAndFeel(nullptr);
}

juce::String CustomRotarySlider::getDisplayString()
{
	if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
	{
		return choiceParam->getCurrentChoiceName();
	}

	juce::String str;
	bool addK = false;
	auto suffix = getTextValueSuffix();

	if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
	{
		auto val = floatParam->get();
		if (suffix == "Hz")
		{
			if (val >= 1000.0f)
			{
				val /= 1000.0f;
				addK = true;
			}
			str = juce::String(val, addK ? 2 : 0);
			str << (addK ? (" k") : " ") << suffix;
		}
		else
		{
			str = juce::String(val, getNumDecimalPlacesToDisplay());
			if (suffix.isNotEmpty())
			{
				str << " " << suffix;
			}
		}
	}

	return str;
}