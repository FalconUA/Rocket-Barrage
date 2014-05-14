#include "graphicobject.h"

namespace rbw
{

void DrawGraphicObject(sf::RenderWindow *window, GraphicObject *object){
    if (object->type == rbw::Graphic::PLAYER){
        sf::CircleShape circle;
        if (object->team == rbw::TEAM_BLACK)
            circle.setFillColor(sf::Color::Blue);
        else
            circle.setFillColor(sf::Color::Red);
        circle.setRadius(16.0);
        circle.setPosition(object->x - 16, object->y - 16);

        sf::RectangleShape hpBar;
        hpBar.setFillColor(sf::Color::Black);
        hpBar.setSize(sf::Vector2f(42.0f, 5.0f));
        hpBar.setPosition(object->x - 21, object->y - 25);

        sf::RectangleShape health;
        health.setFillColor(sf::Color::Yellow);
        health.setSize(sf::Vector2f(40.0f * (object->HealthPoint/100.0f), 3.0f));
        health.setPosition(object->x - 20, object->y - 24);

        window->draw(circle);
        window->draw(hpBar);
        window->draw(health);
    }
    if (object->type == rbw::Graphic::FOLLOW_ROCKET){
        sf::CircleShape circle;
        circle.setFillColor(sf::Color::White);
        circle.setRadius(8.0);
        circle.setPosition(object->x - 8, object->y - 8);
        window->draw(circle);
    }
    if (object->type == rbw::Graphic::BOUNCE_ROCKET){
        sf::CircleShape circle;
        circle.setFillColor(sf::Color::Black);
        circle.setRadius(8.0);
        circle.setPosition(object->x - 8, object->y - 8);
        window->draw(circle);
    }
    if (object->type == rbw::Graphic::GRENADE){
        sf::CircleShape circle;
        circle.setFillColor(sf::Color::Green);
        circle.setRadius(8.0 * object->zoom_coefficient);
        circle.setPosition(object->x - 8, object->y - 8);
        window->draw(circle);
    }
    if (object->type == rbw::Graphic::MOUSE_POINTER_NORMAL){
        sf::RectangleShape up_ver;
        sf::Color cursorColor = sf::Color::Yellow;
        up_ver.setFillColor(cursorColor);
        up_ver.setSize(sf::Vector2f(1.0f, 6.0f));
        up_ver.setOutlineThickness(1.0f);
        up_ver.setOutlineColor(sf::Color::Black);
        up_ver.setPosition(sf::Vector2f(object->x, object->y - 9));

        sf::RectangleShape down_ver;
        down_ver.setFillColor(cursorColor);
        down_ver.setSize(sf::Vector2f(1.0f, 6.0f));
        down_ver.setOutlineThickness(1.0f);
        down_ver.setOutlineColor(sf::Color::Black);
        down_ver.setPosition(sf::Vector2f(object->x, object->y+4));

        sf::RectangleShape left_hor;
        left_hor.setFillColor(cursorColor);
        left_hor.setSize(sf::Vector2f(6.0f, 1.0f));
        left_hor.setOutlineThickness(1.0f);
        left_hor.setOutlineColor(sf::Color::Black);
        left_hor.setPosition(sf::Vector2f(object->x - 9, object->y));

        sf::RectangleShape right_hor;
        right_hor.setFillColor(cursorColor);
        right_hor.setSize(sf::Vector2f(6.0f, 1.0f));
        right_hor.setOutlineThickness(1.0f);
        right_hor.setOutlineColor(sf::Color::Black);
        right_hor.setPosition(sf::Vector2f(object->x + 4, object->y));
        window->draw(up_ver);
        window->draw(down_ver);
        window->draw(left_hor);
        window->draw(right_hor);
    }
    if (object->type == rbw::Graphic::MOUSE_POINTER_SWITCHED){
        sf::RectangleShape up_ver;
        sf::Color cursorColor = sf::Color::Yellow;
        up_ver.setFillColor(cursorColor);
        up_ver.setSize(sf::Vector2f(1.0f, 6.0f));
        up_ver.setOutlineThickness(1.0f);
        up_ver.setOutlineColor(sf::Color::Black);
        up_ver.setPosition(sf::Vector2f(object->x, object->y - 9));

        sf::RectangleShape down_ver;
        down_ver.setFillColor(cursorColor);
        down_ver.setSize(sf::Vector2f(1.0f, 6.0f));
        down_ver.setOutlineThickness(1.0f);
        down_ver.setOutlineColor(sf::Color::Black);
        down_ver.setPosition(sf::Vector2f(object->x, object->y+4));

        sf::RectangleShape left_hor;
        left_hor.setFillColor(cursorColor);
        left_hor.setSize(sf::Vector2f(6.0f, 1.0f));
        left_hor.setOutlineThickness(1.0f);
        left_hor.setOutlineColor(sf::Color::Black);
        left_hor.setPosition(sf::Vector2f(object->x - 9, object->y));

        sf::RectangleShape right_hor;
        right_hor.setFillColor(cursorColor);
        right_hor.setSize(sf::Vector2f(6.0f, 1.0f));
        right_hor.setOutlineThickness(1.0f);
        right_hor.setOutlineColor(sf::Color::Black);
        right_hor.setPosition(sf::Vector2f(object->x + 4, object->y));
        window->draw(up_ver);
        window->draw(down_ver);
        window->draw(left_hor);
        window->draw(right_hor);

        sf::CircleShape circle;
        //circle.setFillColor(sf::Color::Transparent);
        sf::Color color;
        color.g = 0;
        color.r = 0;
        color.b = 255;
        color.a = 50;

        circle.setFillColor(color);
        circle.setOutlineThickness(1.0f);
        circle.setOutlineColor(sf::Color::Cyan);
        circle.setRadius(rbw::GameParam::GRENADE_RADIUS_OF_EFFECT - 16.0f);
        circle.setPosition(sf::Vector2f(object->x - rbw::GameParam::GRENADE_RADIUS_OF_EFFECT + 16.0f,
                                        object->y - rbw::GameParam::GRENADE_RADIUS_OF_EFFECT + 16.0f));
        window->draw(circle);

    }
}

}; // end of namespace rbw
