/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class TicTacToeLF : public LookAndFeel_V4
{
public:
    TicTacToeLF() 
    {
        
    }
private:
    //==============================================================================
    AlertWindow* createAlertWindow(const String& title, const String& message,
        const String& button1, const String& button2, const String& button3,
        AlertWindow::AlertIconType iconType,
        int numButtons, Component* associatedComponent) override
    {
        auto* aw = LookAndFeel_V2::createAlertWindow(title, message, button1, button2, button3,
            iconType, numButtons, associatedComponent);

        auto bounds = aw->getBounds();
        bounds = bounds.withSizeKeepingCentre(bounds.getWidth() -100, bounds.getHeight());
        aw->setBounds(bounds);

        aw->setOpaque(false);
        aw->setColour(AlertWindow::ColourIds::backgroundColourId, findColour(ResizableWindow::backgroundColourId).withAlpha(0.85f));

        for (auto* child : aw->getChildren())
            if (auto* button = dynamic_cast<TextButton*> (child))
                button->setBounds(button->getBounds() + Point<int>(-50, -5));

        return aw;
    }
};


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component, public MouseListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...

    Path grid;
    Path symbolsX, symbolsO;
    Path winnerp;

    void mouseDown(const MouseEvent& event) override;
    void updateSymbols(void);    
    char checkWinner(void);
    void reset( void );
    void addWinnerPath(int si, int sj, int ei, int ej);

    TicTacToeLF tttlf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
