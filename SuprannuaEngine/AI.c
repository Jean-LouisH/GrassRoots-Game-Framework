#include "SuprannuaEngine.h"

void AI_avoid(unsigned char agent, int agentNumber, unsigned char object, int objectNumber, double range)
{
	double positionAngle = geometry_findAngleBetween(agent, agentNumber, object, objectNumber);

	/*If the object is closer to the agent than the range permits, the AI avoids the object.*/
	if (geometry_findDistance(agent, agentNumber, object, objectNumber) < range)
	{
		polygon[agentNumber].properties.xVelocity = -1 * dpadSensitivity * cos(positionAngle);

		/*Flies if airbourne, jumps if not.*/
		if (polygon[agentNumber].properties.classification == FLOATING)
		{
			polygon[agentNumber].properties.yVelocity = -1 * dpadSensitivity * sin(positionAngle);
		}
		else if (event_isPolygonHigher(agentNumber, objectNumber))
		{
			AI_jump(POLYGON, agentNumber, 10.0);
		}
	}
}

void AI_catch(unsigned char agent, int agentNumber, unsigned char object, int objectNumber, bool followX, bool followY)
{
	double positionAngle = geometry_findAngleBetween(agent, agentNumber, object, objectNumber);

	if (followY)
	{
		/*If the object is approaching the agent from the left or right side.*/
		if ((event_hasPolygonPastXLocation(agentNumber, polygon[objectNumber].centre.xPosition) &&
			polygon[objectNumber].properties.xVelocity > 0) ||
			(!event_hasPolygonPastXLocation(agentNumber, polygon[objectNumber].centre.xPosition) &&
				polygon[objectNumber].properties.xVelocity < 0))
		{
			/*Fly to it.*/
			if (polygon[agentNumber].properties.classification == FLOATING)
			{
				polygon[agentNumber].properties.yVelocity = dpadSensitivity * sin(positionAngle);
			}
			/*Jump to it.*/
			else if (polygon[agentNumber].properties.classification == ENTITY && event_isPolygonHigher(objectNumber, agentNumber))
			{
				AI_jump(POLYGON, agentNumber, 10.0);
			}
		}
	}

	if (followX)
	{
		if ((event_hasPolygonPastXLocation(agentNumber, polygon[objectNumber].centre.xPosition) &&
			polygon[objectNumber].properties.xVelocity < 0) ||
			(!event_hasPolygonPastXLocation(agentNumber, polygon[objectNumber].centre.xPosition) &&
				polygon[objectNumber].properties.xVelocity > 0))
		{
			polygon[agentNumber].properties.yVelocity = dpadSensitivity * sin(positionAngle);
		}
	}
}

void AI_fly(unsigned char agent, int agentNumber, double height)
{
	if (edit_get(agent, agentNumber, TYPE) == FLOATING)
	{
		/*Continuously alternates the polygon's velocity while around the targeted height.*/
		if (polygon[agentNumber].centre.yPosition < height - 0.5)
		{
			edit_adjust(POLYGON, agentNumber, YVELOCITY, (dpadSensitivity * 2));
		}
		else if (polygon[agentNumber].centre.yPosition > height + 0.5)
		{
			edit_adjust(POLYGON, agentNumber, YVELOCITY, (-1 * polygon[agentNumber].properties.yVelocity));
		}
	}
}

void AI_follow(unsigned char agent, int agentNumber, unsigned char object, int objectNumber, double detectionRange, double targetRange)
{
	double positionAngle = geometry_findAngleBetween(agent, agentNumber, object, objectNumber);

	/*If it is close enough to detect but further than the desired distance*/
	if (geometry_findDistance(agent, agentNumber, object, objectNumber) < detectionRange &&
		(geometry_findDistance(agent, agentNumber, object, objectNumber) > targetRange))
	{
		polygon[agentNumber].properties.xVelocity = dpadSensitivity * cos(positionAngle);

		/*In the Y-axis.*/
		/*Fly to it.*/
		if (polygon[agentNumber].properties.classification == FLOATING)
		{
			polygon[agentNumber].properties.yVelocity = dpadSensitivity * sin(positionAngle);
		}
		/*Jump to it.*/
		else if (event_isPolygonHigher(objectNumber, agentNumber))
		{
			AI_jump(POLYGON, agentNumber, 10);
		}
	}
}

void AI_jump(unsigned char agent, int agentNumber, double jumpVelocity)
{
	/*To jump off of platforms*/
	for (int i = 0; i <= storedBlocks; i++)
	{
		if (event_isOnPlatform(POLYGON, agentNumber, i))
		{
			edit_change(POLYGON, agentNumber, YVELOCITY, jumpVelocity + block[i].properties.yVelocity);
			break; //Once a single platform is identified, there's no need to look for another.
		}
		/*Jumps from under a platform, especially if gravity is reversed.*/
		else if (event_isTouchingUnderPlatform(POLYGON, agentNumber, i))
		{
			edit_change(POLYGON, agentNumber, YVELOCITY, -1 * jumpVelocity + block[i].properties.yVelocity);
			break;
		}
	}
	/*To jump off of other polygons.*/
	for (int i = 0; i <= storedPolygons; i++)
	{
		if (polygon[agentNumber].properties.classification == polygon[i].properties.classification)
		{
			if (event_isPolygonHigher(agentNumber, i) && event_arePolygonsTouching(agentNumber, i))
			{
				edit_change(POLYGON, agentNumber, YVELOCITY, jumpVelocity + polygon[i].properties.yVelocity);
				break;
			}
		}
	}
}

void AI_mimic(unsigned char agent, int agentNumber, unsigned char object, int objectNumber)
{
	polygon[agentNumber].properties.xVelocity = polygon[objectNumber].properties.xVelocity;
	polygon[agentNumber].properties.yVelocity = polygon[objectNumber].properties.yVelocity;
	polygon[agentNumber].properties.angle = polygon[objectNumber].properties.angle;
}

void AI_shoot(unsigned char agent, int agentNumber, unsigned char object, int objectNumber, double speed)
{
	static int lastPolygon = 0;

	double positionAngle = atan2(polygon[objectNumber].centre.yPosition -
		polygon[agentNumber].centre.yPosition - polygon[agentNumber].radius - 1.0,
		polygon[objectNumber].centre.xPosition - polygon[agentNumber].centre.xPosition);

	/*Retains the memory cell value of the last polygon to replace with a new shot.*/
	if (lastPolygon != 0)
	{
		edit_remove(POLYGON, lastPolygon);
	}

	edit_createPolygon(ENTITY, 12, 0.3, polygon[agentNumber].centre.xPosition +
		polygon[agentNumber].radius, polygon[agentNumber].centre.yPosition + polygon[agentNumber].radius + 1.0, WHITE);

	lastPolygon = storedPolygons;
	edit_change(POLYGON, lastPolygon, BOUNCE, 0.99);
	edit_change(POLYGON, lastPolygon, MASS, 0.0001);

	polygon[lastPolygon].properties.xVelocity = speed * cos(positionAngle);
	polygon[lastPolygon].properties.yVelocity = speed * sin(positionAngle);
}

void AI_spin(unsigned char agent, int agentNumber, bool direction, double degreesPerSecond)
{
	switch (agent)
	{
		/*((direction * 2) - 1) is used to alternate between -1 and +1 from
		* direction values of either 0 or 1.
		*
		* ((0 * 2) - 1) = -1
		* ((1 * 2) - 1) = +1
		*/

	case POLYGON:   polygon[agentNumber].properties.angle +=
		((direction * 2) - 1) * (degreesPerSecond / (FRAME_RATE));
		break;
	}
}

void AI_travel(unsigned char agent, int agentNumber, double toXPosition, double toYPosition, double Velocity)
{
	double positionAngle = atan2(toYPosition - polygon[agentNumber].centre.yPosition,
		toXPosition - polygon[agentNumber].centre.xPosition);

	polygon[agentNumber].properties.xVelocity = dpadSensitivity * cos(positionAngle);

	if (polygon[agentNumber].properties.classification == FLOATING)
	{
		polygon[agentNumber].properties.yVelocity = dpadSensitivity * sin(positionAngle);
	}
	else if (polygon[agentNumber].centre.yPosition < toYPosition)
	{
		AI_jump(POLYGON, agentNumber, 40);
	}
}