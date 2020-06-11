/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

#define PI 3.141692f

inline int nadd1(int i) { return (i + 1); }
inline int nodd(int i) { return (i * 2 + 1); }

inline float squareMult(float n) { return (4 / (n * PI));}
inline float sawtoothMult(float n) { return (pow(-1, n + 1) * 2.0f / ((float)n * PI)); };
inline float sawtoothbackMult(float n) { return (2 / (n * PI)); };
inline float triangleMult(float n) { return (pow(-1, (n - 1) / 2.0f) * 8.0f / (n * PI * n * PI)); };


//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (700, 400);
    setFramesPerSecond(30); // This sets the frequency of the update calls.

    // hint Label
    addAndMakeVisible(hint);
    hint.setJustificationType(Justification::topRight);
    hint.setBounds(700 - 180 - 50, 30, 180, 100);
    hint.setFont(Font(15.0f, Font::FontStyleFlags::italic));
    hint.setMinimumHorizontalScale(1.0f);

    addAndMakeVisible(numComponents);
    numComponents.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    numComponents.setTextBoxStyle(Slider::TextBoxLeft, false, 50, 25);
    numComponents.setRange(0, MAX_NUM_COMPONENTS, 1);
    numComponents.setValue(1);
    numComponents.setNumDecimalPlacesToDisplay(0);
    numComponents.setBounds(220, 30, 200, 25);
    numComponents.setColour(Slider::ColourIds::thumbColourId, Colours::white);
    numComponents.addListener(this);

    addAndMakeVisible(type);
    type.addItemList({ "Square", "Sawtooth", "Sawtooth back", "Triangle" }, 1);
    type.setBounds(50, 30, 150, 25);
    type.addListener(this);
    type.setSelectedId(1, NotificationType::sendNotification);

}

MainComponent::~MainComponent()
{
}

void MainComponent::updateHint()
{
    
    static Component* c;

    if ( numComponents.isMouseOver())
    {
        if (c != (Component*)&numComponents)
        {
            hint.setText("Modify the number of Fourier Series components to plot!", NotificationType::dontSendNotification);
            c = (Component*)&numComponents;
        }
    }
    else if( type.isMouseOver(true))
    {
        if (c != (Component*)&type)
        {
            hint.setText("Changes the current Fourier Series", NotificationType::dontSendNotification);
            c = (Component*)&type;
        }
    }
    else if (c != nullptr)
    {
        hint.setText("", NotificationType::dontSendNotification);
        c = nullptr;
    }
}

//==============================================================================
void MainComponent::update()
{

    // init drawings
    float x = 200;
    float y = 240;
    float radius = 75;
    float velocity = 0.04f;
    static std::vector< int > arr;

    centre.setXY(x, y);
    centreArea.setSize(5, 5);
    centreArea.setCentre(centre);
    point = centre;

    for (int i = 0; i < numDisp; i++)
    {
        int n = genN(i);
        float mult = genMult(n);

        circleArea[i].setSize(abs(mult * radius * 2), abs(mult * radius * 2));
        circleArea[i].setCentre(point);

        line[i].setStart(point);

        x += mult * radius * cos(getFrameCounter() * n * velocity);
        y += mult * radius * sin(getFrameCounter() * n * velocity);

        point.setXY(x, y);

        line[i].setEnd(point);

        pointArea[i].setSize(3, 3);
        pointArea[i].setCentre(point);

    }
    // last point
    pointArea[numDisp-1].setSize(7, 7);
    pointArea[numDisp-1].setCentre(point);

    arr.insert(arr.begin(), y);

    dline.setStart(point);
    dline.setEnd(400, arr[0]);

    if (arr.size() > MAX_VECTOR_SIZE)
    {
        arr.pop_back();
    }

    p.clear();
    p.startNewSubPath(400, arr[0]);

    for (int i = 1; i < arr.size(); i++)
    {
        p.lineTo(400 + i, arr[i]);
    }

    updateHint();

}


//==============================================================================
void MainComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &type)
    {
        switch (type.getSelectedId())
        {
        case 1:
        {
            // Square
            genN = &nodd;
            genMult = &squareMult;
            break;
        }
        case 2:
        {
            // Sawtooth
            genN = &nadd1;
            genMult = &sawtoothMult;
            break;
        }
        case 3:
        {
            // Sawtooth back
            genN = &nadd1;
            genMult = &sawtoothbackMult;
            break;
        }
        case 4:
        {
            // Triangle
            genN = &nodd;
            genMult = &triangleMult;
            break;
        }
        }
    }
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    if (slider == &numComponents)
    {
        numDisp = numComponents.getValue();
    }

}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black);
    
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
