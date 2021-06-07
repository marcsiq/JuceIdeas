/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BandsEQAudioProcessorEditor::BandsEQAudioProcessorEditor(BandsEQAudioProcessor& p)
	: AudioProcessorEditor(&p),
	audioProcessor(p),
	hmfFcSlider(audioProcessor.apvts.getParameter("HMF Fc")),
	hmfGainSlider(audioProcessor.apvts.getParameter("HMF Gain")),
	hmfQSlider(audioProcessor.apvts.getParameter("HMF Q")),
	lmfFcSlider(audioProcessor.apvts.getParameter("LMF Fc")),
	lmfGainSlider(audioProcessor.apvts.getParameter("LMF Gain")),
	lmfQSlider(audioProcessor.apvts.getParameter("LMF Q")),
	lowShelfFcSlider(audioProcessor.apvts.getParameter("LowShelf Fc")),
	lowShelfGainSlider(audioProcessor.apvts.getParameter("LowShelf Gain")),
	lowShelfQSlider(audioProcessor.apvts.getParameter("LowShelf Q")),
	highShelfFcSlider(audioProcessor.apvts.getParameter("HighShelf Fc")),
	highShelfGainSlider(audioProcessor.apvts.getParameter("HighShelf Gain")),
	highShelfQSlider(audioProcessor.apvts.getParameter("HighShelf Q")),
	lowCutSlider(audioProcessor.apvts.getParameter("LowCut Fc")),
	highCutSlider(audioProcessor.apvts.getParameter("HighCut Fc")),
	lowCutSlope(audioProcessor.apvts.getParameter("LowCut Slope")),
	highCutSlope(audioProcessor.apvts.getParameter("HighCut Slope")),
	responseCurveComponent(p),
	bLowCut(),
	bHighCut(),
	bLowShelf(),
	bHighShelf(),
	bLmf(),
	bHmf(),
	hmfFcA(audioProcessor.apvts, "HMF Fc", hmfFcSlider),
	hmfGainA(audioProcessor.apvts, "HMF Gain", hmfGainSlider),
	hmfQA(audioProcessor.apvts, "HMF Q", hmfQSlider),
	lmfFcA(audioProcessor.apvts, "LMF Fc", lmfFcSlider),
	lmfGainA(audioProcessor.apvts, "LMF Gain", lmfGainSlider),
	lmfQA(audioProcessor.apvts, "LMF Q", lmfQSlider),
	lowShelfFcA(audioProcessor.apvts, "LowShelf Fc", lowShelfFcSlider),
	lowShelfGainA(audioProcessor.apvts, "LowShelf Gain", lowShelfGainSlider),
	lowShelfQA(audioProcessor.apvts, "LowShelf Q", lowShelfQSlider),
	highShelfFcA(audioProcessor.apvts, "HighShelf Fc", highShelfFcSlider),
	highShelfGainA(audioProcessor.apvts, "HighShelf Gain", highShelfGainSlider),
	highShelfQA(audioProcessor.apvts, "HighShelf Q", highShelfQSlider),
	lowCutA(audioProcessor.apvts, "LowCut Fc", lowCutSlider),
	lowCutSlopeA(audioProcessor.apvts, "LowCut Slope", lowCutSlope),
	highCutA(audioProcessor.apvts, "HighCut Fc", highCutSlider),
	highCutSlopeA(audioProcessor.apvts, "HighCut Slope", highCutSlope),
	bLowCutAL(audioProcessor.apvts, "LowCut Bypass Left", bLowCut.left),
	bLowCutAR(audioProcessor.apvts, "LowCut Bypass Right", bLowCut.right),
	bHighCutAL(audioProcessor.apvts, "HighCut Bypass Left", bHighCut.left),
	bHighCutAR(audioProcessor.apvts, "HighCut Bypass Right", bHighCut.right),
	bLowShelfAL(audioProcessor.apvts, "LowShelf Bypass Left", bLowShelf.left),
	bLowShelfAR(audioProcessor.apvts, "LowShelf Bypass Right", bLowShelf.right),
	bHighShelfAL(audioProcessor.apvts, "HighShelf Bypass Left", bHighShelf.left),
	bHighShelfAR(audioProcessor.apvts, "HighShelf Bypass Right", bHighShelf.right),
	bLmfAL(audioProcessor.apvts, "LMF Bypass Left", bLmf.left),
	bLmfAR(audioProcessor.apvts, "LMF Bypass Right", bLmf.right),
	bHmfAL(audioProcessor.apvts, "HMF Bypass Left", bHmf.left),
	bHmfAR(audioProcessor.apvts, "HMF Bypass Right", bHmf.right),
	lowCutL("Low Cut", "Low Cut"),
	highCutL("High Cut", "High Cut"),
	lowShelfL("Low Shelf", "Low Shelf"),
	highShelfL("High Shelf", "High Shelf"),
	lmfL("LMF", "LMF"),
	hmfL("HMF", "HMF")
{
	for (auto* comp : getComps())
	{
		addAndMakeVisible(comp);
	}

	for (auto* s : getFrequencySliders())
	{
		setFrequencySlider(*s);
	}

	for (auto* s : getGainSliders())
	{
		setGainSlider(*s);
	}

	for (auto* s : getQSliders())
	{
		setQSlider(*s);
	}

	for (auto* l : getLabels())
	{
		setLabel(*l);
	}

	setSize(600, 500);
}

BandsEQAudioProcessorEditor::~BandsEQAudioProcessorEditor()
{
}

void BandsEQAudioProcessorEditor::setLabel(juce::Label& l)
{
	l.setJustificationType(juce::Justification::centred);
	l.setColour(juce::Label::ColourIds::textColourId, juce::Colours::orange);
}
void BandsEQAudioProcessorEditor::setFrequencySlider(juce::Slider& s)
{
	s.setTextValueSuffix("Hz");
}
void BandsEQAudioProcessorEditor::setGainSlider(juce::Slider& s)
{
	s.setTextValueSuffix("dB");
	s.setNumDecimalPlacesToDisplay(1);
}
void BandsEQAudioProcessorEditor::setQSlider(juce::Slider& s)
{
	s.setNumDecimalPlacesToDisplay(2);
}

//==============================================================================
void BandsEQAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(juce::Colours::black);
}

void BandsEQAudioProcessorEditor::resized()
{
	auto bounds = getLocalBounds();

	auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.40f);
	responseCurveComponent.setBounds(responseArea.reduced(10));

	auto filtW = bounds.getWidth() / 6;

	auto lowCutArea = bounds.removeFromLeft(filtW);
	auto highCutArea = bounds.removeFromRight(filtW);

	auto lowShelfArea = bounds.removeFromLeft(filtW);
	auto highShelfArea = bounds.removeFromRight(filtW);

	auto lmfArea = bounds.removeFromLeft(filtW);
	auto hmfArea = bounds;

	auto labelH = 14;

	lowCutL.setBounds(lowCutArea.removeFromTop(labelH));
	highCutL.setBounds(highCutArea.removeFromTop(labelH));
	lowShelfL.setBounds(lowShelfArea.removeFromTop(labelH));
	highShelfL.setBounds(highShelfArea.removeFromTop(labelH));
	lmfL.setBounds(lmfArea.removeFromTop(labelH));
	hmfL.setBounds(hmfArea.removeFromTop(labelH));

	auto paddingH = 8;

	lowCutArea.removeFromTop(paddingH);
	highCutArea.removeFromTop(paddingH);
	lowShelfArea.removeFromTop(paddingH);
	highShelfArea.removeFromTop(paddingH);
	lmfArea.removeFromTop(paddingH);
	hmfArea.removeFromTop(paddingH);

	auto buttonsH = 20;
	auto buttonsR = 20;

	bLowCut.setBounds(lowCutArea.removeFromTop(buttonsH).reduced(buttonsR, 0));
	bHighCut.setBounds(highCutArea.removeFromTop(buttonsH).reduced(buttonsR, 0));
	bLowShelf.setBounds(lowShelfArea.removeFromTop(buttonsH).reduced(buttonsR, 0));
	bHighShelf.setBounds(highShelfArea.removeFromTop(buttonsH).reduced(buttonsR, 0));
	bLmf.setBounds(lmfArea.removeFromTop(buttonsH).reduced(buttonsR, 0));
	bHmf.setBounds(hmfArea.removeFromTop(buttonsH).reduced(buttonsR, 0));

	lowCutSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5f));
	lowCutSlope.setBounds(lowCutArea);

	highCutSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5f));
	highCutSlope.setBounds(highCutArea);

	hmfFcSlider.setBounds(hmfArea.removeFromTop(hmfArea.getHeight() * 0.33f));
	hmfGainSlider.setBounds(hmfArea.removeFromTop(hmfArea.getHeight() * 0.5f));
	hmfQSlider.setBounds(hmfArea);

	lmfFcSlider.setBounds(lmfArea.removeFromTop(lmfArea.getHeight() * 0.33f));
	lmfGainSlider.setBounds(lmfArea.removeFromTop(lmfArea.getHeight() * 0.5f));
	lmfQSlider.setBounds(lmfArea);

	lowShelfFcSlider.setBounds(lowShelfArea.removeFromTop(lowShelfArea.getHeight() * 0.33f));
	lowShelfGainSlider.setBounds(lowShelfArea.removeFromTop(lowShelfArea.getHeight() * 0.5f));
	lowShelfQSlider.setBounds(lowShelfArea);

	highShelfFcSlider.setBounds(highShelfArea.removeFromTop(highShelfArea.getHeight() * 0.33f));
	highShelfGainSlider.setBounds(highShelfArea.removeFromTop(highShelfArea.getHeight() * 0.5f));
	highShelfQSlider.setBounds(highShelfArea);
}

std::vector<juce::Component*> BandsEQAudioProcessorEditor::getComps()
{
	return
	{
		&hmfFcSlider,
		&hmfGainSlider,
		&hmfQSlider,
		&lmfFcSlider,
		&lmfGainSlider,
		&lmfQSlider,
		&lowCutSlider,
		&highCutSlider,
		&highCutSlope,
		&lowCutSlope,
		&lowShelfFcSlider,
		&lowShelfGainSlider,
		&lowShelfQSlider,
		&highShelfFcSlider,
		&highShelfGainSlider,
		&highShelfQSlider,
		&responseCurveComponent,
		&lowCutL,
		&highCutL,
		&lowShelfL,
		&highShelfL,
		&lmfL,
		&hmfL,
		&bLowCut,
		&bHighCut,
		&bLowShelf,
		&bHighShelf,
		&bHmf,
		&bLmf
	};
}

std::vector<juce::Label*> BandsEQAudioProcessorEditor::getLabels()
{
	return
	{
		&lowCutL,
		&highCutL,
		&lowShelfL,
		&highShelfL,
		&lmfL,
		&hmfL
	};
}

std::vector<juce::Slider*> BandsEQAudioProcessorEditor::getFrequencySliders()
{
	return
	{
		&hmfFcSlider,
		&lmfFcSlider,
		&lowCutSlider,
		&highCutSlider,
		&lowShelfFcSlider,
		&highShelfFcSlider,
	};
}

std::vector<juce::Slider*> BandsEQAudioProcessorEditor::getGainSliders()
{
	return
	{
		&hmfGainSlider,
		&lmfGainSlider,
		&lowShelfGainSlider,
		&highShelfGainSlider,
	};
}

std::vector<juce::Slider*> BandsEQAudioProcessorEditor::getQSliders()
{
	return
	{
		&hmfQSlider,
		&lmfQSlider,
		&lowShelfQSlider,
		&highShelfQSlider,
	};
}