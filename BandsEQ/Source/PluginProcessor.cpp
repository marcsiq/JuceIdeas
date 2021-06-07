/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BandsEQAudioProcessor::BandsEQAudioProcessor()
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	),
	apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

BandsEQAudioProcessor::~BandsEQAudioProcessor()
{
}

//==============================================================================
const juce::String BandsEQAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool BandsEQAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool BandsEQAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool BandsEQAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double BandsEQAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int BandsEQAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int BandsEQAudioProcessor::getCurrentProgram()
{
	return 0;
}

void BandsEQAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String BandsEQAudioProcessor::getProgramName(int index)
{
	return {};
}

void BandsEQAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================

void BandsEQAudioProcessor::updateAllFilters(float sampleRate)
{
	auto chainSettings = getChainSettings(apvts);

	updateHMFPeakFilter(chainSettings, sampleRate);
	updateLMFPeakFilter(chainSettings, sampleRate);
	updateLowCutFilter(chainSettings, sampleRate);
	updateHighCutFilter(chainSettings, sampleRate);
	updateLowShelfFilter(chainSettings, sampleRate);
	updateHighShelfFilter(chainSettings, sampleRate);
}

void BandsEQAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	juce::dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = 1;
	spec.sampleRate = sampleRate;

	chains[Left].prepare(spec);
	chains[Right].prepare(spec);

	updateAllFilters(getSampleRate());
}

void BandsEQAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BandsEQAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void BandsEQAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	updateAllFilters(getSampleRate());

	juce::dsp::AudioBlock<float> block(buffer);

	auto leftBlock = block.getSingleChannelBlock(0);
	auto rightBlock = block.getSingleChannelBlock(1);

	juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
	juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

	chains[Left].process(leftContext);
	chains[Right].process(rightContext);
}

//==============================================================================
bool BandsEQAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BandsEQAudioProcessor::createEditor()
{
	return new BandsEQAudioProcessorEditor(*this);
	//return new juce::GenericAudioProcessorEditor(this);
}

//==============================================================================
void BandsEQAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	juce::MemoryOutputStream mos(destData, true);
	apvts.state.writeToStream(mos);
}

void BandsEQAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
	if (tree.isValid())
	{
		apvts.replaceState(tree);
		updateAllFilters(getSampleRate());
	}
}

void BandsEQAudioProcessor::updateHMFPeakFilter(const ChainSettings& chainSettings, const float& sampleRate)
{
	auto coeffs = makePeakFilter(chainSettings.hmf, sampleRate);
	const auto position = (int)ChainPositions::HMF;
	for (int i = 0; i < N_Channels; i++)
	{
		updateCoefficients(chains[i].get<position>().coefficients, coeffs);
		chains[i].setBypassed<position>(chainSettings.bypass[position][i]);
	}
}

void BandsEQAudioProcessor::updateLMFPeakFilter(const ChainSettings& chainSettings, const float& sampleRate)
{
	auto coeffs = makePeakFilter(chainSettings.lmf, sampleRate);
	const auto position = (int)ChainPositions::LMF;
	for (int i = 0; i < N_Channels; i++)
	{
		updateCoefficients(chains[i].get<position>().coefficients, coeffs);
		chains[i].setBypassed<position>(chainSettings.bypass[position][i]);
	}
}

void BandsEQAudioProcessor::updateLowShelfFilter(const ChainSettings& chainSettings, const float& sampleRate)
{
	auto coeffs = makeLowShelf(chainSettings.lowShelf, sampleRate);
	const auto position = (int)ChainPositions::LowShelf;
	for (int i = 0; i < N_Channels; i++)
	{
		updateCoefficients(chains[i].get<position>().coefficients, coeffs);
		chains[i].setBypassed<position>(chainSettings.bypass[position][i]);
	}
}

void BandsEQAudioProcessor::updateHighShelfFilter(const ChainSettings& chainSettings, const float& sampleRate)
{
	auto coeffs = makeHighShelf(chainSettings.highShelf, sampleRate);
	const auto position = (int)ChainPositions::HighShelf;
	for (int i = 0; i < N_Channels; i++)
	{
		updateCoefficients(chains[i].get<position>().coefficients, coeffs);
		chains[i].setBypassed<position>(chainSettings.bypass[position][i]);
	}
}

void BandsEQAudioProcessor::updateLowCutFilter(const ChainSettings& chainSettings, const float& sampleRate)
{
	auto lowCutCoefficients = makeLowCutFilter(chainSettings.lowCut, sampleRate);
	const auto position = (int)ChainPositions::LowCut;
	for (int i = 0; i < N_Channels; i++)
	{
		auto& lowCut = chains[i].get<position>();
		updateCutFilter(lowCut, lowCutCoefficients, chainSettings.lowCut.cutSlope);
		chains[i].setBypassed<position>(chainSettings.bypass[position][i]);
	}
}

void BandsEQAudioProcessor::updateHighCutFilter(const ChainSettings& chainSettings, const float& sampleRate)
{
	auto highCutCoefficients = makeHighCutFilter(chainSettings.highCut, sampleRate);
	const auto position = (int)ChainPositions::HighCut;
	for (int i = 0; i < N_Channels; i++)
	{
		auto& highCut = chains[i].get<position>();
		updateCutFilter(highCut, highCutCoefficients, chainSettings.highCut.cutSlope);
		chains[i].setBypassed<position>(chainSettings.bypass[position][i]);
	}
}

juce::AudioProcessorValueTreeState::ParameterLayout BandsEQAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	juce::NormalisableRange<float> lowCutFcRange(10.0f, 350.0f, 1.0f, 0.6f);
	juce::NormalisableRange<float> highCutFcRange(3000.0f, 20000.0f, 1.0f, 0.3f);

	juce::NormalisableRange<float> lowShelfFcRange(30.0f, 450.0f, 1.0f, 0.6f);
	juce::NormalisableRange<float> highShelfFcRange(1500.0f, 16000.0f, 1.0f, 0.3f);

	juce::NormalisableRange<float> hmfCutFcRange(600.0f, 7000.0f, 1.0f, 1.0f);
	juce::NormalisableRange<float> lmfCutFcRange(200.0f, 2500.0f, 1.0f, 1.0f);

	juce::NormalisableRange<float> gainRange(-15.0f, 15.0f, 0.01f, 1.0f);
	juce::NormalisableRange<float> qRange(0.5f, 3.0f, 0.01f, 1.0f);

	juce::StringArray slopes("12 dB/Oct", "24 dB/Oct", "36 dB/Oct", "48 dB/Oct");

	layout.add(std::make_unique<juce::AudioParameterBool>("LowCut Bypass Left", "LowCut Bypass Left", false));

	layout.add(std::make_unique<juce::AudioParameterBool>("LowCut Bypass Right", "LowCut Bypass Right", false));

	layout.add(std::make_unique<juce::AudioParameterFloat>("LowCut Fc", "LowCut Fc", lowCutFcRange, 10.0f));

	layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCut Slope", slopes, slopes.indexOf("12 dB/Oct")));

	layout.add(std::make_unique<juce::AudioParameterBool>("HighCut Bypass Left", "HighCut Bypass Left", false));

	layout.add(std::make_unique<juce::AudioParameterBool>("HighCut Bypass Right", "HighCut Bypass Right", false));

	layout.add(std::make_unique<juce::AudioParameterFloat>("HighCut Fc", "HighCut Fc", highCutFcRange, 20000.0f));

	layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", slopes, slopes.indexOf("12 dB/Oct")));

	layout.add(std::make_unique<juce::AudioParameterBool>("HMF Bypass Left", "HMF Bypass Left", false));

	layout.add(std::make_unique<juce::AudioParameterBool>("HMF Bypass Right", "HMF Bypass Right", false));

	layout.add(std::make_unique<juce::AudioParameterFloat>("HMF Fc", "HMF Fc", hmfCutFcRange, 1000.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>("HMF Gain", "HMF Gain", gainRange, 0.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>("HMF Q", "HMF Q", qRange, 1.0f));

	layout.add(std::make_unique<juce::AudioParameterBool>("LMF Bypass Left", "LMF Bypass Left", false));

	layout.add(std::make_unique<juce::AudioParameterBool>("LMF Bypass Right", "LMF Bypass Right", false));

	layout.add(std::make_unique<juce::AudioParameterFloat>("LMF Fc", "LMF Fc", lmfCutFcRange, 1000.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>("LMF Gain", "LMF Gain", gainRange, 0.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>("LMF Q", "LMF Q", qRange, 1.0f));

	layout.add(std::make_unique<juce::AudioParameterBool>("LowShelf Bypass Left", "LowShelf Bypass Left", false));

	layout.add(std::make_unique<juce::AudioParameterBool>("LowShelf Bypass Right", "LowShelf Bypass Right", false));

	layout.add(std::make_unique<juce::AudioParameterFloat>("LowShelf Fc", "LowShelf Fc", lowShelfFcRange, 300.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>("LowShelf Gain", "LowShelf Gain", gainRange, 0.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>("LowShelf Q", "LowShelf Q", qRange, 1.0f));

	layout.add(std::make_unique<juce::AudioParameterBool>("HighShelf Bypass Left", "HighShelf Bypass Left", false));

	layout.add(std::make_unique<juce::AudioParameterBool>("HighShelf Bypass Right", "HighShelf Bypass Right", false));

	layout.add(std::make_unique<juce::AudioParameterFloat>("HighShelf Fc", "HighShelf Fc", highShelfFcRange, 2000.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>("HighShelf Gain", "HighShelf Gain", gainRange, 0.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>("HighShelf Q", "HighShelf Q", qRange, 1.0f));

	return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new BandsEQAudioProcessor();
}