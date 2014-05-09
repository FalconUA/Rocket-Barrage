#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <level.h>
#include <graphicobject.h>
#include <worldsimulator.h>
#include <SFML/System.hpp>
#include <graphicengine.h>

using namespace rbw;

class Game
{
public:
    Game(std::string PlayerName);    
    Game(WorldSimulator* server, std::string PlayerName, sf::RenderWindow* window, Level * level);
    bool Init(WorldSimulator * server, sf::RenderWindow* window, Level * level);

    void RespondToEvent(sf::Event * event);
    void CheckKey();
    void GenerateNextFrame();

private:
    GraphicObject mouse_object;
    std::string MyName;
    sf::RenderWindow * window;

    rbw::WorldSimulator * server;
    rbw::GraphicEngine * graphic;

    std::vector< GraphicObject > Objects;
    Level  * level;
    bool Switch_mouse;
};

#endif // GAME_H
