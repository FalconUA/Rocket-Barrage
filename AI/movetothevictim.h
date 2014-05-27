#ifndef MOVETOTHEVICTIM_H
#define MOVETOTHEVICTIM_H

//#include "game.h"
#include <worldconstant.h>
#include <worldsimulator.h>
#include <botshoot.h>
#include <evadefromtherocket.h>
#include <worldtypes.h>

using namespace rbw;

//const int X_MIN = 0, Y_MIN = 0;
//const int X_MAX = 0, Y_MAX = 0;
const int unthinkable_time = 100; //miliseconds;

/*struct TVector
{
    int x,y;
};*/

/*typedef enum {
    DIRECTION_NODIRECTION,

    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} Direction;*/

class MoveToTheVictim
{
public:
    std::vector<TPlayer> bots, victims;  //
    std::vector<TRectangle> walls;   //

    MoveToTheVictim(WorldSimulator * server);
    //std::vector<TRectangle> getWalls(Game* game);
    bool moveToTheVictim(TPlayer bot, Player * bot_n, TVector *victim_position, sf::Vector2i* direction_bot);
    ~MoveToTheVictim();

private:

    int num;
    Direction lastVertDirection, lastHorDirection;
    sf::Vector2i* direction_bot;
    WorldSimulator * server;

    bool pixelIsInTheWall(TVector pixel);
    void sortVector(std::vector<int> &vector);
    std::vector<TVector> wave(int **cells, TVector cell_coord, int MAX_Column, int MAX_Row);
    bool victimIsInTheCellsVector(std::vector<TVector> &cells_vector/*Here are indexes of cells*/,
                                  std::vector<int> &x_coord, std::vector<int> &y_coord, TVector* victim_cell,
                                  TVector * victim_position);
    void moveToTheCell(std::vector<int> &x_coord, std::vector<int> &y_coord, TVector &cell, TPlayer &bot);

    void moveUpRight(int time/*, Player * bot_n*/);
    void moveDown(int time/*, Player *bot_n*/);
    void moveDownLeft(int time/*, Player *bot_n*/);
    void moveDownRight(int time/*, Player *bot_n*/);
    void moveLeft(int time/*, Player *bot_n*/);
    void moveRight(int time/*, Player *bot_n*/);
    void moveUp(int time/*, Player *bot_n*/);
    void moveUpLeft(int time/*, Player *bot_n*/);

    //void moveToTheNextPixel(Direction bot_direction);
};

#endif // MOVETOTHEVICTIM_H
