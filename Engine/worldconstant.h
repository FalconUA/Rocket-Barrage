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
    const float MAX_PLAYER_SPEED = 0.40f; // speed per milisecond, 0.36f is equal to 8 pixels per 60ms
    const float MAX_HOMING_MISSILE_SPEED = 0.50f; // speed per milisecond, 0.64f is equal to 12 pixels per 60ms
    const float MAX_BOUNCING_BOMB_SPEED = 0.50f; // speed per milisecond, 0.64f is equal to 12 pixels per 60ms

    const int GRENADE_TIME_TO_LIVE = 1000; // in miliseconds;
    const float GRENADE_HEIGHT = 200; // in pixels
    const float GRENADE_RADIUS_OF_EFFECT = 150;
    const float GRENADE_RADIUS_OF_EFFECT_SQUARED = GRENADE_RADIUS_OF_EFFECT * GRENADE_RADIUS_OF_EFFECT; // equal to radius 200

    const int MAX_NUMBER_OF_HOMING_MISSILES_ALLOWED = 8;
    const int MAX_NUMBER_OF_BOUNCING_BOMBS_ALLOWED = 10;
    const int MAX_NUMBER_OF_GRENADES_ALLOWED = 3;

    const bool HOMING_MISSILE_RECHARGE_AFTER_EXPLOSING = true;
    const bool BOUNCING_BOMB_RECHARGE_AFTER_EXPLOSING = true;
    const bool GRENADE_RECHARGE_AFTER_EXPLOSING = true;

    const float PLAYER_HITBOX_RADIUS = 16.0f;
    const float HOMING_MISSILE_HITBOX_RADIUS = 8.0f;
    const float BOUNCING_BOMB_HITBOX_RADIUS = 8.0f;
    const float GRENADE_HITBOX_RADIUS = 8.0f;
}; // end of namespace gameparam

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
    DIRECTION_RIGHT,
} Direction;

typedef enum {
    TEAM_BLACK,
    TEAM_WHITE,
    TEAM_NOTEAM
} Team;

namespace Graphic{
    enum GraphicObjectType {
        PLAYER,
        FOLLOW_ROCKET,
        BOUNCE_ROCKET,
        GRENADE,
        MOUSE_POINTER_NORMAL,
        MOUSE_POINTER_SWITCHED,

        HOMINGMISSILE_EXPLOSION,
        BOUNCINGBOMB_EXPLOSION,
        GRENADE_EXPLOSION,
        PLAYER_EXPLOSION
    };

    const std::string BLACK_TANK_IMAGE_FILENAME = "tank_black.png";
    const std::string WHITE_TANK_IMAGE_FILENAME = "tank_white.png";
    const std::string HOMING_MISSILE_IMAGE_FILENAME = "homingmissile.png";
    const std::string BOUNCING_BOMB_IMAGE_FILENAME = "bouncingbomb.gif";
    const std::string GRENADE_IMAGE_FILENAME = "grenade.png";
    const std::string HM_EXPLOSION_ANIMATION_FILENAME = "explosion_hm.png";
    const std::string BB_EXPLOSION_ANIMATION_FILENAME = "explosion_bb.png";
    const std::string P_EXPLOSION_ANIMATION_FILENAME = "explosion_p.png";
    const std::string G_EXPLOSION_ANIMATION_FILENAME = "explosion_g.png";

    #ifdef __linux__
        const std::string ModelPath = "Resources/models/";
        const std::string AnimationPath = "Resources/animations/";
    #else
        const std::string ModelPath = "Resources\models\\";
        const std::string AnimationPath = "Resources\animations\\";
    #endif

}; // end of namespace graphic

}; // end of namespace rbw

#endif // WORLDCONSTANT_H
