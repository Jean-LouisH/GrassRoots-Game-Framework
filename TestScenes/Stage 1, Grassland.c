#include "..\Suprannua\Definitions.h" //All the enums, structs and variables the framework and script needs to know.
#include "..\Suprannua\GameScript.h" //Script and asset functions.
#include "..\Suprannua\2DCamera.h" //Camera functions.
#include "..\Suprannua\AI.h" //Artificial Intelligence of agents.
#include "..\Suprannua\Input.h"
#include "..\Suprannua\Physics.h" //All algorithms and physics calculations.
#include "..\Suprannua\Logic.h"
#include "..\Suprannua\Geometry.h"
#include "..\Suprannua\Editor.h" //Functions used for data manipulation in game script.

///////////////////////////////////
//Custom code for Game Data, Logic, AI, Inputs
///////////////////////////////////

//Global variables
char gameTitle[64]          = SOFTWARE VERSION "[Stage 1, Grassland]";
double dpadSensitivity      = 30;
double cameraScrollSpeed    = 0.5;

Rect worldSizeMetres        = {100,80};

double platformGravity      = 50.0;

bool isGamePaused           = false;

void initGameData()
{
        camera_setWidth(worldSizeMetres.width);
        camera_setTarget(camera2D.viewport.width/2, camera2D.viewport.height/2);

        edit_create(BLOCK, BACKGROUND, 0, 0, worldSizeMetres.width - 0.01, worldSizeMetres.height - 0.01,
                    worldSizeMetres.width/2, worldSizeMetres.height/2, 135, 206, 250, 255);
        edit_create(BLOCK, PLATFORM, 0, 0, 60, 1.0, 30, 1.0, 0, 150, 0, 255);
        edit_create(BLOCK, PLATFORM, 0, 0, 60, 1.0, 30, 0.505, 165, 42, 42, 255);
        edit_create(POLYGON, ENTITY, 6, 3.0, 0, 0, 5, 30, 255, 0, 0, 255);
        polygon[0].properties.bouncePercentage = 0.3;
        edit_create(POLYGON, ENTITY, 3, 2.0, 0, 0, 10, 40 + 5.0, 0, 200, 0, 255);
        edit_create(BLOCK, PLATFORM, 0, 0, 10.0, 1.0, 10, 40, 255, 0, 0, 255);
        edit_create(BLOCK, PLATFORM, 0, 0, 12.0, 1.0, 80, 10, 0, 0, 100, 255);
        edit_create(BLOCK, PLATFORM, 0, 0, 6.0, 1.0, 95, 20, 0, 0, 100, 255);
        edit_create(BLOCK, PLATFORM, 0, 0, 10.0, 1.0, 85, 32, 0, 0, 100, 255);
        edit_create(BLOCK, PLATFORM, 0, 0, 6.0, 1.0, 46, 30, 0, 0, 100, 255);
}

void runGameScript()
{
    int i;

    physics_limitBoundary();
    physics_detectPlatformCollision();

    physics_gravitate(POLYGON, 1, DOWN);
    AI_spin(POLYGON, 1, ANTICLOCKWISE, 180);

    physics_roll(POLYGON, 0);

    if(logic_isWithinPlatformWidth(POLYGON, 0, 7))
    {
        if(block[7].centre.yPosition > polygon[0].centre.yPosition)
            physics_gravitate(POLYGON, 0, UP);
        else
            physics_gravitate(POLYGON, 0, DOWN);
    }
    else
        physics_gravitate(POLYGON, 0, DOWN);

    //Currently testing scripted sequencing, before developing a function.
    if(timeCount >= 3 && timeCount < 10 && camera2D.viewport.width > 15)
    {
        camera_setTarget(polygon[1].centre.xPosition, polygon[1].centre.yPosition);
        camera_zoomBy(-0.4);
    }
    if(timeCount > 10 && timeCount < 11)
        camera_setWidth(100);
    if(timeCount >= 10 && timeCount < 15 && camera2D.viewport.width > 15)
    {
        camera_setTarget(polygon[0].centre.xPosition, polygon[0].centre.yPosition);
        camera_zoomBy(-0.6);
    }
    if(timeCount > 15 && timeCount < 16)
    {
        camera_setWidth(worldSizeMetres.width);
        camera_setTarget(camera2D.viewport.width/2, camera2D.viewport.height/2);
    }

    //To do: add a distance function for simplicity.
    if(hypot(polygon[0].centre.xPosition - polygon[1].centre.xPosition,
        polygon[0].centre.yPosition - polygon[1].centre.yPosition) < polygon[1].radius)
    {
        edit_remove(POLYGON, 1);
        edit_colour(POLYGON, 0, 0, 225, 0, 255);
    }

    //To do: add a timing function for simplicity.
    if(frameCount % (int)(FRAME_RATE * 1) == 0)
    {
        if(polygon[0].properties.colour[GREEN] > 0)
            edit_colour(BLOCK, 3, 0, 225, 0, 255);
    }
    if(frameCount % (int)(FRAME_RATE * 1) == 25)
    //
    {
        if(block[3].properties.colour[GREEN] > 0)
            edit_colour(BLOCK, 3, 255, 0, 0, 255);
    }

    //To do: add a platform scroller function for simplicity.
    if(block[7].centre.yPosition <= 30)
        edit_adjust(BLOCK, 7, YVELOCITY, 5);
    if(block[7].centre.yPosition >= 45)
        edit_adjust(BLOCK, 7, YVELOCITY, -5);


    if(polygon[0].centre.yPosition < -5)
    {
        edit_move(POLYGON, 0, 5, 30);
        edit_change(POLYGON, 0, YVELOCITY, 0);
    }
    if(polygon[0].properties.colour[GREEN] > 0 && camera2D.viewport.width < 200)
    {
        camera_zoomBy(0.1);
    }
}

//Controller
void readInput()
{
    int i;

    if(gameState == GAMEPLAY)
    {
        if(input_isPressed('w'))
        {
            for(i = 0; i < MAX_BLOCKS; i++)
            {
                if(logic_isOnPlatform(POLYGON, 0, i))
                    edit_change(POLYGON, 0, YVELOCITY, 40 + block[i].properties.yVelocity); //Jumping
            }
        }

        if(input_isPressed('a'))
            edit_change(POLYGON, 0, XVELOCITY, -1 * dpadSensitivity);
        else if(polygon[0].properties.xVelocity > 0)
                edit_adjust(POLYGON, 0, XVELOCITY, -3);
            //polygon[0].properties.xVelocity = 0;

        if(input_isPressed('s'))
        {
            for(i = 0; i < MAX_BLOCKS; i++)
            {
                if(logic_isTouchingUnderPlatform(POLYGON, 0, i))
                    edit_change(POLYGON, 0, YVELOCITY, -40 + block[i].properties.yVelocity);
            }
        }

        if(input_isPressed('d'))
            edit_change(POLYGON, 0, XVELOCITY, dpadSensitivity);
        else if(polygon[0].properties.xVelocity < 0)
                edit_adjust(POLYGON, 0, XVELOCITY, 3);

    }
    else if (gameState == MENU)
    {
        ////////////////
        //Camera
        ////////////////
        if(input_isPressed('i'))
            camera_scroll(0.0, cameraScrollSpeed);

        if(input_isPressed('j'))
            camera_scroll(-1 * cameraScrollSpeed, 0.0);

        if(input_isPressed('k'))
            camera_scroll(0.0, - 1 * cameraScrollSpeed);

        if(input_isPressed('l'))
            camera_scroll(cameraScrollSpeed, 0.0);
    }
}

