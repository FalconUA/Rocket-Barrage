#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <level.h>
#include <graphicobject.h>
#include <worldsimulator.h>
#include <SFML/System.hpp>
#include <graphicengine.h>
#include <evadefromtherocket.h> //it's new
#include <movetothevictim.h>    //it's new
#include <botshoot.h>   //it's new

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
    sf::Vector2u winSize;

    GraphicObject mouse_object;
    std::string MyName;
    sf::RenderWindow * window;

    rbw::WorldSimulator * server;
    rbw::GraphicEngine * graphic;

    std::vector< GraphicObject > Objects;
    Level  * level;
    bool Switch_mouse;
    bool ShowScore;

    std::vector<TRectangle> getWalls(); //it's new
    void botyara(MoveToTheVictim * moveToTheVictim, std::vector<TPair_PlayerDirection> &directions, Team team_name, bool shoot_function);   //it's new
    MoveToTheVictim * moveToTheVictim,/*temporary*/
        * moveToTheVictim_first_team, * moveToTheVictim_second_team;  //it's new
    TSafeDirections getSafeDirections(rbw::Player botyara, TVector real_speed);  //it's new
};

#endif // GAME_H
