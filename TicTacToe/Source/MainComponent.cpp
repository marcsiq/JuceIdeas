/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

#define WIDTH_HEIGHT 400
#define GAME_SIZE 3
#define ELEMENTS_PADDING 10

char board[GAME_SIZE][GAME_SIZE] = { "" };

char playerX = 'X';
char playerO = 'O';

char players[2] = { playerX, playerO};

int currentPlayer = 0;

//==============================================================================
MainComponent::MainComponent()
{

    setSize (WIDTH_HEIGHT, WIDTH_HEIGHT);
    currentPlayer = players[0];

    addMouseListener(this, false);

}

MainComponent::~MainComponent()
{
}

void MainComponent::mouseDown(const MouseEvent& event)
{
    int x = event.getMouseDownX();
    int y = event.getMouseDownY();

    x = x / (getWidth()/GAME_SIZE);
    y = y / (getHeight()/GAME_SIZE);

    if (board[x][y] != playerX && board[x][y] != playerO)
    {
        board[x][y] = players[currentPlayer];
        currentPlayer = currentPlayer == 0 ? 1 : 0;
    
    }
   
    updateSymbols();
    repaint();

}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.strokePath(grid, PathStrokeType (4.0f));
    g.setColour(Colours::antiquewhite);
    g.strokePath(symbolsO, PathStrokeType(6.0f));
    g.setColour(Colours::indianred);
    g.strokePath(symbolsX, PathStrokeType(6.0f));

}

void MainComponent::updateSymbols(void)
{
    int w = getWidth() / GAME_SIZE;
    int h = getHeight() / GAME_SIZE;

    symbolsO.clear();
    symbolsO.clear();

    for (int i = 0; i < GAME_SIZE; i++)
    {
        for (int j = 0; j < GAME_SIZE; j++)
        {
            int x = i * w;
            int y = j * h;
            char s = board[i][j];
            if (s == playerO)
            {
                symbolsO.addEllipse(Rectangle<float>{(float)x + ELEMENTS_PADDING, (float)y + ELEMENTS_PADDING, (float)(w)-2 * ELEMENTS_PADDING, (float)(h)-2 * ELEMENTS_PADDING});
            }
            else if (s == playerX)
            {
                symbolsX.startNewSubPath(x + ELEMENTS_PADDING, y + ELEMENTS_PADDING);
                symbolsX.lineTo(x + w - ELEMENTS_PADDING, y + w - ELEMENTS_PADDING);
                symbolsX.startNewSubPath(x + w - ELEMENTS_PADDING, y + ELEMENTS_PADDING);
                symbolsX.lineTo(x + ELEMENTS_PADDING, y + h - ELEMENTS_PADDING);
            }
        }
    }
}

void MainComponent::resized()
{
    int w = getWidth();
    int h = getHeight();

    grid.clear();

    for (int i = 1; i < GAME_SIZE; i++)
    {
        grid.startNewSubPath(i*w / GAME_SIZE, 0);
        grid.lineTo(i*w / GAME_SIZE, w);

        grid.startNewSubPath(0, i * h / GAME_SIZE);
        grid.lineTo(h, i * h / GAME_SIZE);

    }
}
