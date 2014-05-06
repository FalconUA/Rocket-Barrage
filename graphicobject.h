#ifndef GRAPHICOBJECT_H
#define GRAPHICOBJECT_H

#include <SFML/Graphics.hpp>
#include <worldconstant.h>
#include <string>

typedef struct {
    int x;
    int y;
    rbw::Graphic::GraphicObjectType type;
    std::string Name;

    // only for "Player" type:
    rbw::Team team;
    int HealthPoint;

    // only for "Grenade" type:
    float zoom_coefficient;

} GraphicObject;

void DrawGraphicObject(sf::RenderWindow * window, GraphicObject * object);

#endif // GRAPHICOBJECT_H
