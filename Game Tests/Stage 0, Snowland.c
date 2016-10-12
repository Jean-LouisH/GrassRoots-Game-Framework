#include "..\Grass (Game Framework)\Grass.h"

///////////////////////////////////
//Custom code for Game Data, Logic, AI, Inputs
///////////////////////////////////

//Global variables
char gameTitle[64]          = "Grass (Game Framework) " VERSION " [Stage 0, Snowland]";
int screenWidth             = 800;
int screenHeight            = 600;
double dpadSensitivity      = 30;
double cameraScrollSpeed    = 0.5;
double xMapSize             = 200;
double yMapSize             = 60;
bool gamePause              = false;

double friction             = 0.4;
double objectGravity        = 0.0;
double platformGravity      = 1.0;
double pointGravity[8];

void runGameScript()
{
    compute_translate();
    compute_rotate();
    compute_limitBoundary();
    compute_detectPlatformCollision();

    compute_gravitate(POLYGON, 0);
    compute_roll(POLYGON, 0);
    camera_follow(POLYGON, 0, true, false);

    for(i = 1; i < 20; i++)
        if(polygon[i].centre.yPosition < 6)
            edit_move(POLYGON, i, polygon[i].centre.xPosition, yMapSize - 1);
}

void initGameData()
{
        camera_set(25, yMapSize/2);
        edit_create(BOX, BACKGROUND, 0, 0, xMapSize - 0.01, yMapSize - 0.01, xMapSize/2, yMapSize/2, 135, 206, 250);
        edit_create(BOX, PLATFORM, 0, 0, xMapSize - 0.01, 3.0, xMapSize/2, 1.6, 83, 21, 21);
        edit_create(BOX, PLATFORM, 0, 0, xMapSize - 0.01, 3.0, xMapSize/2, 3, 255, 255, 255);
        edit_create(POLYGON, ENTITY, 6, 3.0, 0, 0, 5, 30, 255, 0, 0);
        for(i = 1; i < 20; i++)
        {
            edit_create(POLYGON, ENTITY, 8, 0.5, 0, 0, 0, 0, 255, 255, 255);
            edit_change(POLYGON, i, YVELOCITY, -5.0);
        }
}

//Controller
void readInput()
{

    if(!gamePause)
    {
        if (keyStates['w'] || keyStates['W'])
        {
            //Jumping
            for(i = 0; i < MAX_BOXES; i++)
            {
                if(box[i].properties.classification == PLATFORM)
                {
                    if(polygon[0].centre.yPosition - polygon[0].radius <=
                            box[i].centre.yPosition + (box[i].boxHeight / 2) &&
                        polygon[0].centre.yPosition - polygon[0].radius >
                            box[i].centre.yPosition - (box[i].boxHeight / 2))
                    {
                        if(polygon[0].centre.xPosition >
                                box[i].centre.xPosition - (box[i].boxWidth / 2) &&
                           polygon[0].centre.xPosition <
                                box[i].centre.xPosition + (box[i].boxWidth / 2))
                        {
                            edit_change(POLYGON, 0, YVELOCITY, 40 + box[i].properties.yVelocity);
                        }
                    }
                }
            }
        }

        if (keyStates['a'] || keyStates['A'])
            edit_change(POLYGON, 0, XVELOCITY, -1 * dpadSensitivity);

        if (keyStates['d'] || keyStates['D'])
            edit_change(POLYGON, 0, XVELOCITY, dpadSensitivity);

        ////////////////
        //Camera
        ////////////////
        if (keyStates['i'] || keyStates['I'])
            camera_scroll(0.0, cameraScrollSpeed);

        if (keyStates['j'] || keyStates['J'])
            camera_scroll(-1 * cameraScrollSpeed, 0.0);

        if (keyStates['k'] || keyStates['K'])
            camera_scroll(0.0, - 1 * cameraScrollSpeed);

        if (keyStates['l'] || keyStates['L'])
            camera_scroll(cameraScrollSpeed, 0.0);
    }

    /////////////
    //Game states
    /////////////

    if (keyStates[27])
        exit(EXIT_SUCCESS);

}

