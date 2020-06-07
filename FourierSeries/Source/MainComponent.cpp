/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

#define PI 3.141692
#define MAX_NUM_COMPONENTS 50
#define MAX_VECTOR_SIZE 300

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (700, 400);
    setFramesPerSecond(60); // This sets the frequency of the update calls.

    addAndMakeVisible(numComponents);
    numComponents.setSliderStyle(Slider::SliderStyle::IncDecButtons);
    numComponents.setTextBoxStyle(Slider::TextBoxLeft, false, 50, 25);
    numComponents.setRange(0, MAX_NUM_COMPONENTS, 1);
    numComponents.setValue(1);
    numComponents.setNumDecimalPlacesToDisplay(0);
    numComponents.setBounds(250, 20, 120, 25);

    addAndMakeVisible(type);
    type.addItemList({ "Square", "Sawtooth", "Sawtooth back", "Triangle" }, 1);
    type.setBounds(50, 20, 150, 25);
    type.setSelectedId(1);

}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::update()
{
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);

    // You can add your drawing code here!

    Rectangle<float> centreArea, circleArea[MAX_NUM_COMPONENTS], pointArea[MAX_NUM_COMPONENTS];
    Point<float> point, centre;
    Line<float> dline, line[MAX_NUM_COMPONENTS];
    Path p;
    int n;
    float mult;

    float x = 200;
    float y = 220;
    centre.setXY(x, y);
    centreArea.setSize(5, 5);
    centreArea.setCentre(centre);

    int radius = 70;
   
    //init at centre
    point = centre;

    int numDisp = numComponents.getValue();

    for (int i = 0; i < numDisp; i++)
    {
        int wave = type.getSelectedId();
        if (wave == 1)
        {
            // Square
            n = i * 2 + 1;
            mult = 4 / (n * PI);
        }
        else if (wave == 2)
        {
            // Sawtooth
            n = i + 1;
            mult = pow(-1, n + 1) * 2.0f / (n * PI);
        }
        else if (wave == 3)
        {
            // Sawtooth back
            n = i+1;
            mult = 2 / (n * PI);
        }
        else if (wave == 4)
        {
            // Triangle
            n = i * 2 + 1;
            mult = pow(-1, (n-1)/2) * 8.0f / (n * PI *n * PI);
        }

        circleArea[i].setSize(abs(mult * radius * 2), abs(mult * radius * 2));
        circleArea[i].setCentre(point);

        line[i].setStart(point);

        x += mult * radius * cos(getFrameCounter() * n * 0.04f);
        y += mult * radius * sin(getFrameCounter() * n * 0.04f);

        point.setXY(x, y);

        line[i].setEnd(point);

        pointArea[i].setSize(7, 7);
        pointArea[i].setCentre(point);

    }
    arr.insert(arr.begin(), y);

    dline.setStart(point);
    dline.setEnd(400, arr[0]);

    p.startNewSubPath(400, arr[0]);

    for (int i = 1; i < arr.size(); i++)
    {
        p.lineTo(400+i, arr[i]);
    }

    if (arr.size() > MAX_VECTOR_SIZE)
    {
        arr.pop_back();
    }

    g.setColour(Colours::white.withAlpha(0.2f));
    for (int i = 0; i < numDisp; i++)
    {
        g.drawEllipse(circleArea[i], 1.5f);
        g.drawLine(line[i]);
    }
    g.drawLine(dline);

    g.setColour(Colours::white);
    g.fillEllipse(centreArea);

    g.setColour(Colours::white.withAlpha(0.2f));
    for (int i = 0; i < numDisp; i++)
    {
        if (i == numDisp - 1)
        {
            g.setColour(Colours::green);
        }
        g.fillEllipse(pointArea[i]);
    }


    g.setColour(Colours::antiquewhite);
    g.strokePath(p, PathStrokeType(1.0f));

}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
