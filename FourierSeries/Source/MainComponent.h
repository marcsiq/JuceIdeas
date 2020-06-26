/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define MAX_NUM_COMPONENTS 50
#define MAX_VECTOR_SIZE 300

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AnimatedAppComponent, 
    public ComboBox::Listener,
    public Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void update() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    virtual void sliderValueChanged(Slider* slider) override;
    void updateHint(void);

    // Function pointers for computations
    int (*genN)(int);
    float (*genMult)(float);

    // Hint label
    Label hint;

    // parameters
    Slider numComponents;
    ComboBox type;
    int numDisp;

    // Drawings
    Rectangle<float> centreArea, circleArea[MAX_NUM_COMPONENTS], pointArea[MAX_NUM_COMPONENTS];
    Point<float> point, centre;
    Line<float> dline, line[MAX_NUM_COMPONENTS];
    Path p;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
