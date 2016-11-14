#include "Definitions.h"

////////////////////////////////////////
//GLUT Callback function implementations
///////////////////////////////////////
void timer(int milliseconds)
{
    glutTimerFunc(milliseconds, timer, milliseconds);
    glutPostRedisplay();
}

void keyPressed(unsigned char key, int x, int y)
{
    keyStates[key] = true; //registers a button press in the input buffer.

    if (keyStates[27]) //Escape key
        exit(EXIT_SUCCESS);

    if(keyStates[9]) //Tab key
        glutPositionWindow(((glutGet(GLUT_SCREEN_WIDTH) * 0.250)/ 2) + 1,
                           ((glutGet(GLUT_SCREEN_HEIGHT) * 0.250 * 0.563) / 2) + 22);
}

void keyUp(unsigned char key, int x, int y)
{
    if((keyStates['p'] || keyStates['P']) && (!isGamePaused))
    {
        isGamePaused = true;
        gameState = MENU;
    }
    else if((keyStates['p'] || keyStates['P'] ) && (isGamePaused))
    {
        isGamePaused = false;
        gameState = GAMEPLAY;
    }

    keyStates[key] = false;
}

void keySpecial(unsigned char key, int x, int y)
{
    specialKeyStates[key] = true;
}

void keyUpSpecial(unsigned char key, int x, int y)
{
    specialKeyStates[key] = false;
}

void resize(int width, int height)
{
    glutReshapeWindow(glutGet(GLUT_SCREEN_WIDTH) * 0.750, (glutGet(GLUT_SCREEN_WIDTH) * 0.750) * 0.563);
}

