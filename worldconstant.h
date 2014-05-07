#ifndef WORLDCONSTANT_H
#define WORLDCONSTANT_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

namespace rbw
{

namespace GameParam
{
    const int INITIAL_HEALTH_POINT = 100;

    const int BOUNCING_BOMB_DAMAGE = 2;
    const int HOMING_MISSILE_DAMAGE = 2;
    const int GRENADE_DAMAGE = 10;

    const int MAX_BOUNCE_NUMBER = 6;
    const float MAX_PLAYER_SPEED = 0.36f; // speed per milisecond, is equal to 8 pixels per 60ms
    const float MAX_HOMING_MISSILE_SPEED = 0.64f; // speed per milisecond, is equal to 12 pixels per 60ms
    const float MAX_BOUNCING_BOMB_SPEED = 0.64f; // speed per milisecond, is equal to 12 pixels per 60ms

    const int GRENADE_TIME_TO_LIVE = 1000; // in miliseconds;
    const float GRENADE_HEIGHT = 200; // in pixels
    const float GRENADE_RADIUS_OF_EFFECT = 150;
    const float GRENADE_RADIUS_OF_EFFECT_SQUARED = GRENADE_RADIUS_OF_EFFECT * GRENADE_RADIUS_OF_EFFECT; // equal to radius 200

    const int MAX_NUMBER_OF_HOMING_MISSILES_ALLOWED = 5;
    const int MAX_NUMBER_OF_BOUNCING_BOMBS_ALLOWED = 10;
    const int MAX_NUMBER_OF_GRENADES_ALLOWED = 3;

    const bool HOMING_MISSILE_RECHARGE_AFTER_EXPLOSING = true;
    const bool BOUNCING_BOMB_RECHARGE_AFTER_EXPLOSING = true;
    const bool GRENADE_RECHARGE_AFTER_EXPLOSING = true;
};

typedef enum {
    TYPE_PLAYER,
    TYPE_HOMING_MISSILE,
    TYPE_BOUNCING_BOMB
} ObjectType;

typedef enum {
    DIRECTION_NODIRECTION,
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} Direction;

typedef enum {
    TEAM_BLACK,
    TEAM_WHITE
} Team;

namespace Graphic{
enum GraphicObjectType {
    PLAYER,
    FOLLOW_ROCKET,
    BOUNCE_ROCKET,
    GRENADE,
    MOUSE_POINTER_NORMAL,
    MOUSE_POINTER_SWITCHED
};

}

};

#endif // WORLDCONSTANT_H
