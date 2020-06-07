/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (600, 400);
    setFramesPerSecond(100); // This sets the frequency of the update calls.
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

    Rectangle<float> pointArea, centreArea;
    Point<float> point, centre;
    Line<float> line1, line2;
    Path p, circle;

    int x = 200;
    int y = 200;
    int radius = 100;
    circle.addEllipse(x - radius, y - radius, radius * 2, radius * 2);

    centre.setXY(x, y);
    centreArea.setSize(5, 5);
    centreArea.setCentre(centre);
  
    x = x + radius * cos(getFrameCounter() * 0.05);
    y = y + radius * sin(getFrameCounter() * 0.05);
    point.setXY(x, y);

    arr.insert(arr.begin(), y);

    p.startNewSubPath(400, arr[0]);
    for (int i = 1; i < arr.size(); i++)
    {
        p.lineTo(400+i, arr[i]);
    }


    pointArea.setSize(7,7);
    pointArea.setCentre(point);

    line1.setStart(centre);
    line1.setEnd(point);
     
    line2.setStart(point);
    line2.setEnd(400, arr[0]);

    if (arr.size() > 200)
    {
        arr.pop_back();
    }

    g.setColour(Colours::white.withAlpha(0.3f));
    g.strokePath(circle, PathStrokeType(1.0f));
    g.drawLine(line1);
    g.drawLine(line2);

    g.setColour(Colours::white);
    g.fillEllipse(centreArea);

    g.setColour(Colours::green);
    g.fillEllipse(pointArea);


    g.setColour(Colours::antiquewhite);
    g.strokePath(p, PathStrokeType(1.0f));

}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
