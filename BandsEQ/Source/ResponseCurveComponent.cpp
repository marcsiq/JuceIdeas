/*
  ==============================================================================

	ResponseCurveComponent.cpp
	Created: 24 Apr 2021 1:54:11pm
	Author:  marcs

  ==============================================================================
*/

#include "ResponseCurveComponent.h"

ResponseCurveComponent::ResponseCurveComponent(BandsEQAudioProcessor& p)
	: audioProcessor(p), parametersChanged(false)
{
	const auto& params = audioProcessor.getParameters();

	for (auto param : params)
	{
		param->addListener(this);
	}
	updateChain();
	startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
	const auto& params = audioProcessor.getParameters();

	for (auto param : params)
	{
		param->removeListener(this);
	}
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
	parametersChanged.set(true);
}

void ResponseCurveComponent::timerCallback()
{
	if (parametersChanged.compareAndSetBool(false, true))
	{
		// updated the monochain
		updateChain();

		// signal a repaint
		repaint();
	}
}

void ResponseCurveComponent::updateChain()
{
	auto chainSettings = getChainSettings(audioProcessor.apvts);
	auto sampleRate = audioProcessor.getSampleRate();

	auto hmfCoefficients = makePeakFilter(chainSettings.hmf, sampleRate);
	updateCoefficients(chains[Left].get<ChainPositions::HMF>().coefficients, hmfCoefficients);
	updateCoefficients(chains[Right].get<ChainPositions::HMF>().coefficients, hmfCoefficients);
	chains[Left].setBypassed<ChainPositions::HMF>(chainSettings.bypass[ChainPositions::HMF][Left]);
	chains[Right].setBypassed<ChainPositions::HMF>(chainSettings.bypass[ChainPositions::HMF][Right]);

	auto lmfCoefficients = makePeakFilter(chainSettings.lmf, sampleRate);
	updateCoefficients(chains[Left].get<ChainPositions::LMF>().coefficients, lmfCoefficients);
	updateCoefficients(chains[Right].get<ChainPositions::LMF>().coefficients, lmfCoefficients);
	chains[Left].setBypassed<ChainPositions::LMF>(chainSettings.bypass[ChainPositions::LMF][Left]);
	chains[Right].setBypassed<ChainPositions::LMF>(chainSettings.bypass[ChainPositions::LMF][Right]);

	auto lowShelfCoefficients = makeLowShelf(chainSettings.lowShelf, sampleRate);
	updateCoefficients(chains[Left].get<ChainPositions::LowShelf>().coefficients, lowShelfCoefficients);
	updateCoefficients(chains[Right].get<ChainPositions::LowShelf>().coefficients, lowShelfCoefficients);
	chains[Left].setBypassed<ChainPositions::LowShelf>(chainSettings.bypass[ChainPositions::LowShelf][Left]);
	chains[Right].setBypassed<ChainPositions::LowShelf>(chainSettings.bypass[ChainPositions::LowShelf][Right]);

	auto highShelfCoefficients = makeHighShelf(chainSettings.highShelf, sampleRate);
	updateCoefficients(chains[Left].get<ChainPositions::HighShelf>().coefficients, highShelfCoefficients);
	updateCoefficients(chains[Right].get<ChainPositions::HighShelf>().coefficients, highShelfCoefficients);
	chains[Left].setBypassed<ChainPositions::HighShelf>(chainSettings.bypass[ChainPositions::HighShelf][Left]);
	chains[Right].setBypassed<ChainPositions::HighShelf>(chainSettings.bypass[ChainPositions::HighShelf][Right]);

	auto lowCutCoefficients = makeLowCutFilter(chainSettings.lowCut, sampleRate);
	updateCutFilter(chains[Left].get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCut.cutSlope);
	updateCutFilter(chains[Right].get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCut.cutSlope);
	chains[Left].setBypassed<ChainPositions::LowCut>(chainSettings.bypass[ChainPositions::LowCut][Left]);
	chains[Right].setBypassed<ChainPositions::LowCut>(chainSettings.bypass[ChainPositions::LowCut][Right]);

	auto highCutCoefficients = makeHighCutFilter(chainSettings.highCut, sampleRate);
	updateCutFilter(chains[Left].get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCut.cutSlope);
	updateCutFilter(chains[Right].get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCut.cutSlope);
	chains[Left].setBypassed<ChainPositions::HighCut>(chainSettings.bypass[ChainPositions::HighCut][Left]);
	chains[Right].setBypassed<ChainPositions::HighCut>(chainSettings.bypass[ChainPositions::HighCut][Right]);
}

//==============================================================================
void ResponseCurveComponent::paint(juce::Graphics& g)
{
	using namespace juce;

	g.drawImage(background, getLocalBounds().toFloat());

	auto bounds = getAnalysisArea();

	auto w = bounds.getWidth();

	Path responseCurve[N_Channels];
	std::vector<double> mags;

	for (int c = 0; c < N_Channels; c++)
	{
		auto& lowcut = chains[c].get<ChainPositions::LowCut>();
		auto& hmf = chains[c].get<ChainPositions::HMF>();
		auto& lmf = chains[c].get<ChainPositions::LMF>();
		auto& lowShelf = chains[c].get<ChainPositions::LowShelf>();
		auto& highShelf = chains[c].get<ChainPositions::HighShelf>();
		auto& highcut = chains[c].get<ChainPositions::HighCut>();

		auto sampleRate = audioProcessor.getSampleRate();

		mags.clear();
		mags.resize(w);

		for (int i = 0; i < w; i++)
		{
			double mag = 1.0f;
			auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

			if (!chains[c].isBypassed<ChainPositions::HMF>())
			{
				mag *= hmf.coefficients->getMagnitudeForFrequency(freq, sampleRate);
			}

			if (!chains[c].isBypassed<ChainPositions::LMF>())
			{
				mag *= lmf.coefficients->getMagnitudeForFrequency(freq, sampleRate);
			}

			if (!chains[c].isBypassed<ChainPositions::HighShelf>())
			{
				mag *= highShelf.coefficients->getMagnitudeForFrequency(freq, sampleRate);
			}

			if (!chains[c].isBypassed<ChainPositions::LowShelf>())
			{
				mag *= lowShelf.coefficients->getMagnitudeForFrequency(freq, sampleRate);
			}

			if (!chains[c].isBypassed<ChainPositions::LowCut>())
			{
				if (!lowcut.isBypassed<0>())
				{
					mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
				}
				if (!lowcut.isBypassed<1>())
				{
					mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
				}
				if (!lowcut.isBypassed<2>())
				{
					mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
				}
				if (!lowcut.isBypassed<3>())
				{
					mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
				}
			}

			if (!chains[c].isBypassed<ChainPositions::HighCut>())
			{
				if (!highcut.isBypassed<0>())
				{
					mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
				}
				if (!highcut.isBypassed<1>())
				{
					mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
				}
				if (!highcut.isBypassed<2>())
				{
					mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
				}
				if (!highcut.isBypassed<3>())
				{
					mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
				}
			}

			mags[i] = Decibels::gainToDecibels(mag);
		}

		const double outputMin = bounds.getBottom();
		const double outputMax = bounds.getY();

		auto map = [outputMin, outputMax](double input)
		{
			return jmap(input, -48.0, 48.0, outputMin, outputMax);
		};

		responseCurve[c].startNewSubPath(bounds.getX(), map(mags.front()));

		for (size_t i = 1; i < mags.size(); ++i)
		{
			responseCurve[c].lineTo(bounds.getX() + i, map(mags[i]));
		}

		g.setColour(c == 0 ? Colours::orange : Colours::red);
		g.strokePath(responseCurve[c], PathStrokeType(2.0f));
	}
	g.setColour(Colours::orange);
	g.drawRoundedRectangle(getRenderArea().toFloat(), 0.0f, 2.0f);
}

void ResponseCurveComponent::resized()
{
	using namespace juce;
	background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);

	Graphics g(background);

	Array<float> freqs
	{
		20, 50, 100,
		200, 500, 1000,
		2000, 5000, 10000, 20000
	};

	Array<float> gains
	{
		-48, -36, -24, -12, 0, 12, 24, 36, 48
	};

	auto renderArea = getAnalysisArea();
	auto left = renderArea.getX();
	auto right = renderArea.getRight();
	auto top = renderArea.getY();
	auto bottom = renderArea.getBottom();
	auto w = renderArea.getWidth();
	auto h = renderArea.getHeight();

	Array<float> xs;
	for (auto f : freqs)
	{
		auto normX = mapFromLog10(f, 20.0f, 20000.0f);
		xs.add(left + w * normX);
	}

	for (auto gdB : gains)
	{
		auto y = jmap(gdB, -48.0f, 48.0f, float(bottom), float(top));
		g.setColour(Colours::white.withAlpha((gdB == 0.0f ? 1.0f : 0.4f)));
		g.drawHorizontalLine(y, left, right);
	}

	g.setColour(Colours::white.withAlpha(0.4f));
	for (auto x : xs)
	{
		g.drawVerticalLine(x, top, bottom);
	}

	const int fontHeight = 10;
	g.setFont(fontHeight);

	for (int i = 0; i < freqs.size(); i++)
	{
		auto f = freqs[i];
		auto x = xs[i];

		bool addK = false;
		String str;
		if (f >= 1000.0f)
		{
			f /= 1000.0f;
			addK = true;
		}
		str = juce::String(f, 0);
		str << (addK ? (" kHz") : " Hz");

		auto textWidth = g.getCurrentFont().getStringWidth(str);

		Rectangle<int> r;
		r.setSize(textWidth, fontHeight);
		r.setCentre(x, 0);
		r.setY(1);
		g.drawFittedText(str, r, juce::Justification::centred, 1);
	}

	for (auto gdB : gains)
	{
		auto y = jmap(gdB, -48.0f, 48.0f, float(bottom), float(top));

		String str;
		if (gdB > 0)
		{
			str << "+";
		}
		str << gdB;
		auto textWidth = g.getCurrentFont().getStringWidth(str);
		Rectangle<int> r;
		r.setSize(textWidth, fontHeight);
		r.setX(getWidth() - textWidth);
		r.setCentre(r.getCentreX(), y);
		g.drawFittedText(str, r, juce::Justification::centred, 1);

		str.clear();
		str << (gdB - 48);
		r.setX(1);
		textWidth = g.getCurrentFont().getStringWidth(str);
		r.setSize(textWidth, fontHeight);
		g.drawFittedText(str, r, juce::Justification::centred, 1);
	}
}

juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
	auto bounds = getLocalBounds();

	bounds.removeFromTop(12);
	bounds.removeFromLeft(20);
	bounds.removeFromRight(20);

	return bounds;
}

juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
	auto bounds = getRenderArea();

	bounds.removeFromTop(4);
	bounds.removeFromBottom(4);

	return bounds;
}