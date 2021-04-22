/*
  ==============================================================================

    Slime.h
    Created: 25 Mar 2021 3:13:23pm
    Author:  marcs

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Slime
{
public: 
    Slime(const int initialX, const int initialY, const float initialAngle, juce::Image* im, const juce::Colour c = juce::Colours::white);

    int getX() const;
    int getY() const;

    void updatePosition();

private:
    juce::Point<float> p;
    float angle;
    juce::Image* image;
    juce::Colour colour;

};