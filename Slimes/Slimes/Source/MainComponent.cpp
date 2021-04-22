#include "MainComponent.h"
#include "settings.h"


//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (Settings::width, Settings::height);
    setFramesPerSecond (Settings::fps); // This sets the frequency of the update calls.
    setRepaintsOnMouseActivity(false);

    auto w = getWidth();
    auto h = getHeight();
    image.reset(new juce::Image(juce::Image::PixelFormat::RGB, getWidth(), getHeight(), true));
    auto rand = juce::Random::getSystemRandom();
    rand.setSeedRandomly();

    slimes.clear(true);

    for (int i = 0; i < Settings::NSlimes; i++)
    {
        auto angle = juce::MathConstants<float>::twoPi * rand.nextFloat();
        if (Settings::useRandomColours)
        {
            auto randColour = juce::Colour(rand.nextInt(255), rand.nextInt(255), rand.nextInt(255));
            slimes.add(new Slime(getBounds().getCentreX(), getBounds().getCentreY(), angle, image.get(), randColour));
        }
        else
        {
            slimes.add(new Slime(getBounds().getCentreX(), getBounds().getCentreY(), angle, image.get()));
        }
    }

    float blurRadius = 2.0f;
    kernel.reset(new juce::ImageConvolutionKernel(blurRadius * 2 + 1));
    kernel->createGaussianBlur(blurRadius);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::update()
{
    processFadeOut();
    for (auto& s : slimes)
    {
        s->updatePosition();
    }
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::blue);
    static juce::Rectangle<float> textArea = { 20, 0, 100, 20 };

    g.setImageResamplingQuality(juce::Graphics::ResamplingQuality::lowResamplingQuality);
    g.drawImage(*image.get(), getBounds().toFloat(),juce::RectanglePlacement::centred);
    g.drawText("FPS: " + juce::String((int)(1000.0f / (float)getMillisecondsSinceLastUpdate())), textArea, juce::Justification::centredLeft);
}

void MainComponent::resized()
{

}


void MainComponent::processFadeOut()
{
    for (size_t y = 0; y < image->getHeight(); ++y) {
        for (size_t x = 0; x < image->getWidth(); ++x) {
            if (image->getPixelAt(x, y) != juce::Colours::black)
            {
                image->setPixelAt(x, y, image->getPixelAt(x, y).darker(Settings::darkerFactor));
            }
        }
    }

    //kernel->applyToImage(*image.get(), *image.get(), image->getBounds());

}