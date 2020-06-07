/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

#define WIDTH_HEIGHT 400
#define GAME_SIZE 10
#define ELEMENTS_PADDING (int)(0.2*WIDTH_HEIGHT/GAME_SIZE)

char board[GAME_SIZE][GAME_SIZE];

char playerX = 'X';
char playerO = 'O';

char players[2] = { playerX, playerO};
int playersWins[2] = { 0,0 };

int currentPlayer = 0;
int turns;

bool check3(char a, char b, char c)
{
    return (a == b && b == c && a!=NULL);
}

//==============================================================================
MainComponent::MainComponent()
{
    setLookAndFeel(&tttlf);

    setSize (WIDTH_HEIGHT, WIDTH_HEIGHT);
       
    reset();

    currentPlayer = 0;
}

MainComponent::~MainComponent()
{
    setLookAndFeel(nullptr);
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
        turns--;
    }
   
    updateSymbols();

    char winner = checkWinner();

    if (winner != NULL || turns == 0)
    {
    
        String s;
        if (winner != NULL)
        {
            playersWins[winner == playerX ? 0 : 1]++;
            s << "PLAYER [ " << winner  << " ] WINS\n\n";
            s << " [ " << playerX << " ] --> " << playersWins[0] << " wins" << "\n";
            s << " [ " << playerO << " ] --> " << playersWins[1] << " wins";
        }
        else
        {
            s << "It's a TIE! \n";
        }

        AlertWindow::showMessageBox(AlertWindow::AlertIconType::NoIcon, "GAME FINISHED", s, "Play Again!", this);

        reset();
    }

}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.strokePath(grid, PathStrokeType (4.0f));
    g.setColour(Colours::antiquewhite);
    g.strokePath(symbolsO, PathStrokeType(7.0f));
    g.setColour(Colours::indianred);
    g.strokePath(symbolsX, PathStrokeType(7.0f));
    g.setColour(Colours::green);
    g.setOpacity(0.7);
    g.strokePath(winnerp, PathStrokeType(7.0f));
}

void MainComponent::reset( void )
{
    turns = GAME_SIZE * GAME_SIZE;

    for (int i = 0; i < GAME_SIZE; i++)
    {
        for (int j = 0; j < GAME_SIZE; j++)
        {
            board[i][j] = NULL;
        }
    }
    winnerp.clear();
    updateSymbols();
}

char MainComponent::checkWinner(void)
{
    for (int i = 0; i < GAME_SIZE; i++)
    {
        for (int j = 0; j < GAME_SIZE; j++)
        {
            if (j - 1 >= 0 && j + 1 < GAME_SIZE)
            {
                if (check3(board[i][j - 1], board[i][j], board[i][j + 1]))
                {
                    addWinnerPath(i, j - 1, i, j + 1);
                    return board[i][j];
                }
            }
            if (i - 1 >= 0 && i + 1 < GAME_SIZE)
            {
                if (check3(board[i - 1][j], board[i][j], board[i + 1][j]))
                {
                    addWinnerPath(i - 1, j, i + 1, j);
                    return board[i][j];
                }
            }
            if (i - 1 >= 0 && j - 1 >= 0 && i + 1 < GAME_SIZE && j + 1 < GAME_SIZE)
            {
                // First diagonal
                if (check3(board[i - 1][j - 1], board[i][j], board[i + 1][j + 1]))
                {
                    addWinnerPath(i - 1, j - 1, i + 1, j + 1);
                    return board[i][j];
                }
                // Second diagonal
                if (check3(board[i - 1][j + 1], board[i][j], board[i + 1][j - 1]))
                {
                    addWinnerPath(i + 1, j - 1, i - 1, j + 1);
                    return board[i][j];
                }
            }
        }
    }
    return NULL;
}


void MainComponent::addWinnerPath(int si, int sj, int ei, int ej)
{
    int w = getWidth() / GAME_SIZE;
    int h = getHeight() / GAME_SIZE;

    int sx=0, sy=0, ex=0, ey=0;

    if (si == ei)
    {
        //vertical
        ex = sx = si * w + w / 2;
        sy = sj * h + ELEMENTS_PADDING ;
        ey = ej * h + h - ELEMENTS_PADDING;
    }
    else if (sj == ej)
    {
        //horizontal
        ey = sy = sj * h + h / 2;
        sx = si * w + ELEMENTS_PADDING;
        ex = ei * w + w - ELEMENTS_PADDING;
    }
    else
    {
        //diagonal1
        if (si < ei)
        {
            sx = si * w + ELEMENTS_PADDING;
            ex = ei * w + w - ELEMENTS_PADDING;
            sy = sj * h + ELEMENTS_PADDING;
            ey = ej * h + h - ELEMENTS_PADDING;
        }
        else
        {
            sx = si * w + w - ELEMENTS_PADDING;
            ex = ei * w + ELEMENTS_PADDING;
            sy = sj * h + ELEMENTS_PADDING;
            ey = ej * h + h - ELEMENTS_PADDING;
        }
    }


    winnerp.startNewSubPath(sx, sy);
    winnerp.lineTo(ex, ey);
}

void MainComponent::updateSymbols(void)
{
    int w = getWidth() / GAME_SIZE;
    int h = getHeight() / GAME_SIZE;

    symbolsO.clear();
    symbolsX.clear();

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

    repaint();
}

void MainComponent::resized()
{
    int w = getWidth();
    int h = getHeight();

    grid.clear();

    for (int i = 0; i < GAME_SIZE+1; i++)
    {
        grid.startNewSubPath(i*w / GAME_SIZE, 0);
        grid.lineTo(i*w / GAME_SIZE, w);

        grid.startNewSubPath(0, i * h / GAME_SIZE);
        grid.lineTo(h, i * h / GAME_SIZE);

    }
}
