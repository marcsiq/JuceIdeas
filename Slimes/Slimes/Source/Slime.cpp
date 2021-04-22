/*
  ==============================================================================

    Slime.cpp
    Created: 25 Mar 2021 3:13:23pm
    Author:  marcs

  ==============================================================================
*/

#include "Slime.h"
#include "settings.h"

Slime::Slime(const int initialX, const int initialY, const float initialAngle, juce::Image* im, const juce::Colour c)
    :p(initialX, initialY), angle(initialAngle), image(im), colour(c)
{

}

int Slime::getX() const
{
    return p.getX();
}

int Slime::getY() const
{
    return p.getY();
}

void Slime::updatePosition()
{
    auto newX = p.getX() + Settings::amountSpeed * cosf(angle);
    auto newY = p.getY() + Settings::amountSpeed * sinf(angle);

    float w = image->getWidth();
    float h = image->getHeight();

    // Clamp to image bounds and pick a new random angle if hit boundary
    if (newX < 0 || newX > w || newY < 0 || newY > h)
    {
        newX = juce::jmin(w, juce::jmax(0.0f, newX));
        newY = juce::jmin(h, juce::jmax(0.0f, newY));
        angle = juce::MathConstants<float>::twoPi * juce::Random::getSystemRandom().nextFloat();
        
        // Direction change, same angle
        // angle -= juce::MathConstants<float>::pi;

        // Direction change, 90 degrees 
        //angle += juce::MathConstants<float>::halfPi;
    }

    p.setXY(newX, newY);
    
    image->setPixelAt(p.getX(), p.getY(), colour);
}