#ifndef GRAPHICOBJECT_H
#define GRAPHICOBJECT_H

#include <SFML/Graphics.hpp>
#include <worldconstant.h>
#include <string>

namespace rbw
{

typedef struct {
    int x;
    int y;
    float velocity_x;
    float velocity_y;
    rbw::Graphic::GraphicObjectType type;
    std::string Name; // if type == player, then


    // only for "Player" type:
    rbw::Team team;
    int HealthPoint;

    // only for "Grenade" type:
    float zoom_coefficient;

} GraphicObject;

void DrawGraphicObject(sf::RenderWindow * window, GraphicObject * object, bool spray = false);
}

#endif // GRAPHICOBJECT_H
