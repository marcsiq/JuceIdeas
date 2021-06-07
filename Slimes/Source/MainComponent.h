#pragma once

#include <JuceHeader.h>
#include "Slime.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AnimatedAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void update() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:

    void processFadeOut();
    //==============================================================================
    // Your private member variables go here...
    std::unique_ptr<juce::Image> image;
    std::unique_ptr<juce::ImageConvolutionKernel> kernel;
    juce::OwnedArray<Slime> slimes;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
