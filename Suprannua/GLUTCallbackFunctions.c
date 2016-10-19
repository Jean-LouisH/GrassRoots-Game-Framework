#include "Definitions.h"
#include "GLUTCallbackFunctions.h"

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
}

void keyUp(unsigned char key, int x, int y)
{
    if((keyStates['p'] || keyStates['P']) && (!isGamePaused))
        isGamePaused = true;

    else if((keyStates['p'] || keyStates['P'] ) && (isGamePaused))
        isGamePaused = false;

    keyStates[key] = false;
    polygon[0].properties.xVelocity = 0; //stops player movement immediately after releasing.
}
