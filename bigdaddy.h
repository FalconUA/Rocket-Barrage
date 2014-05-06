#ifndef BIGDADDY_H
#define BIGDADDY_H

#include <SFML/Graphics.hpp>
#include "level.h"
#include "gameobject.h"
#include <vector>

#define MAX_OBJECT_NUM 256

namespace RB
{

class BigDaddy
{    
public:
    BigDaddy();
    int CreateGame(int fps, sf::RenderWindow * window, std::string mapname);
    bool CheckCollision(std::string LevelType, int new_x, int new_y);

    std::vector< GameObject* > Objects;
private:
    Level level;


};

// end of namespace RB
};
#endif // BIGDADDY_H
