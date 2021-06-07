/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum Channels
{
	Left,
	Right,
	N_Channels
};

enum Slope
{
	Slope_12,
	Slope_24,
	Slope_36,
	Slope_48
};

enum ChainPositions
{
	LowCut,
	LowShelf,
	LMF,
	HMF,
	HighShelf,
	HighCut,
	N_ChainPositions
};

struct PeakSettings
{
	float fc{ 0.0f }, gainDb{ 0.0f }, quality{ 1.0f };
};

struct ShelfSettings
{
	float fc{ 0.0f }, gainDb{ 0.0f }, quality{ 1.0f };
};

struct CutFilterSettings
{
	float cutFc{ 0.0f };
	Slope cutSlope{ Slope::Slope_12 };
};

struct ChainSettings
{
	PeakSettings lmf;
	PeakSettings hmf;
	ShelfSettings lowShelf;
	ShelfSettings highShelf;
	CutFilterSettings lowCut;
	CutFilterSettings highCut;
	bool bypass[N_ChainPositions][N_Channels];
};

inline ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
	ChainSettings settings;

	settings.lowCut.cutFc = apvts.getRawParameterValue("LowCut Fc")->load();
	settings.lowCut.cutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCut Slope")->load());
	settings.highCut.cutFc = apvts.getRawParameterValue("HighCut Fc")->load();
	settings.highCut.cutSlope = static_cast<Slope>(apvts.getRawParameterValue("HighCut Slope")->load());
	settings.hmf.fc = apvts.getRawParameterValue("HMF Fc")->load();
	settings.hmf.gainDb = apvts.getRawParameterValue("HMF Gain")->load();
	settings.hmf.quality = apvts.getRawParameterValue("HMF Q")->load();
	settings.lmf.fc = apvts.getRawParameterValue("LMF Fc")->load();
	settings.lmf.gainDb = apvts.getRawParameterValue("LMF Gain")->load();
	settings.lmf.quality = apvts.getRawParameterValue("LMF Q")->load();
	settings.highShelf.fc = apvts.getRawParameterValue("HighShelf Fc")->load();
	settings.highShelf.gainDb = apvts.getRawParameterValue("HighShelf Gain")->load();
	settings.highShelf.quality = apvts.getRawParameterValue("HighShelf Q")->load();
	settings.lowShelf.fc = apvts.getRawParameterValue("LowShelf Fc")->load();
	settings.lowShelf.gainDb = apvts.getRawParameterValue("LowShelf Gain")->load();
	settings.lowShelf.quality = apvts.getRawParameterValue("LowShelf Q")->load();

	settings.bypass[ChainPositions::LMF][Left] = static_cast<bool>(apvts.getRawParameterValue("LMF Bypass Left")->load());
	settings.bypass[ChainPositions::LMF][Right] = static_cast<bool>(apvts.getRawParameterValue("LMF Bypass Right")->load());
	settings.bypass[ChainPositions::HMF][Left] = static_cast<bool>(apvts.getRawParameterValue("HMF Bypass Left")->load());
	settings.bypass[ChainPositions::HMF][Right] = static_cast<bool>(apvts.getRawParameterValue("HMF Bypass Right")->load());
	settings.bypass[ChainPositions::LowCut][Left] = static_cast<bool>(apvts.getRawParameterValue("LowCut Bypass Left")->load());
	settings.bypass[ChainPositions::LowCut][Right] = static_cast<bool>(apvts.getRawParameterValue("LowCut Bypass Right")->load());
	settings.bypass[ChainPositions::HighCut][Left] = static_cast<bool>(apvts.getRawParameterValue("HighCut Bypass Left")->load());
	settings.bypass[ChainPositions::HighCut][Right] = static_cast<bool>(apvts.getRawParameterValue("HighCut Bypass Right")->load());
	settings.bypass[ChainPositions::LowShelf][Left] = static_cast<bool>(apvts.getRawParameterValue("LowShelf Bypass Left")->load());
	settings.bypass[ChainPositions::LowShelf][Right] = static_cast<bool>(apvts.getRawParameterValue("LowShelf Bypass Right")->load());
	settings.bypass[ChainPositions::HighShelf][Left] = static_cast<bool>(apvts.getRawParameterValue("HighShelf Bypass Left")->load());
	settings.bypass[ChainPositions::HighShelf][Right] = static_cast<bool>(apvts.getRawParameterValue("HighShelf Bypass Right")->load());

	return settings;
}

using Filter = juce::dsp::IIR::Filter<float>;

using Coeffs = Filter::CoefficientsPtr;

using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, Filter, Filter, Filter, CutFilter>;

inline void updateCoefficients(Coeffs& old, const Coeffs& replacements)
{
	*old = *replacements;
}

inline Coeffs makePeakFilter(const PeakSettings& settings, double sampleRate)
{
	return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, settings.fc, settings.quality, juce::Decibels::decibelsToGain(settings.gainDb));
}

inline Coeffs makeHighShelf(const ShelfSettings& settings, double sampleRate)
{
	return juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, settings.fc, settings.quality, juce::Decibels::decibelsToGain(settings.gainDb));
}

inline Coeffs makeLowShelf(const ShelfSettings& settings, double sampleRate)
{
	return juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, settings.fc, settings.quality, juce::Decibels::decibelsToGain(settings.gainDb));
}

template<int Index, typename ChainType, typename CoefficientType>
inline void update(ChainType& chain, const CoefficientType& coefficients)
{
	updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
	chain.template setBypassed<Index>(false);
}

template<typename ChainType, typename CoefficientType>
inline void updateCutFilter(ChainType& chain, const CoefficientType& cutCoefficients, const Slope& slope)
{
	chain.template setBypassed<0>(true);
	chain.template setBypassed<1>(true);
	chain.template setBypassed<2>(true);
	chain.template setBypassed<3>(true);

	switch (slope)
	{
	case Slope::Slope_48:
	{
		update<3>(chain, cutCoefficients);
	}
	case Slope::Slope_36:
	{
		update<2>(chain, cutCoefficients);
	}
	case Slope::Slope_24:
	{
		update<1>(chain, cutCoefficients);
	}
	case Slope::Slope_12:
	{
		update<0>(chain, cutCoefficients);
	}
	}
}

inline auto makeLowCutFilter(const CutFilterSettings& settings, double sampleRate)
{
	return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(settings.cutFc, sampleRate, 2 * (settings.cutSlope + 1));
}

inline auto makeHighCutFilter(const CutFilterSettings& settings, double sampleRate)
{
	return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(settings.cutFc, sampleRate, 2 * (settings.cutSlope + 1));
}

//==============================================================================
/**
*/
class BandsEQAudioProcessor : public juce::AudioProcessor
{
public:
	//==============================================================================
	BandsEQAudioProcessor();
	~BandsEQAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	juce::AudioProcessorValueTreeState apvts;

	MonoChain chains[N_Channels];
private:

	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

	void updateHMFPeakFilter(const ChainSettings& chainSettings, const float& sampleRate);
	void updateLMFPeakFilter(const ChainSettings& chainSettings, const float& sampleRate);
	void updateLowCutFilter(const ChainSettings& chainSettings, const float& sampleRate);
	void updateHighCutFilter(const ChainSettings& chainSettings, const float& sampleRate);
	void updateLowShelfFilter(const ChainSettings& chainSettings, const float& sampleRate);
	void updateHighShelfFilter(const ChainSettings& chainSettings, const float& sampleRate);

	void updateAllFilters(float sampleRate);

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandsEQAudioProcessor)
};
