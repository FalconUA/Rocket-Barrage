#ifndef BOTSHOOT_H
#define BOTSHOOT_H

#include <worldconstant.h>
#include <worldsimulator.h>
#include <worldtypes.h>

class BotShoot
{
public:
    //std::vector<TPlayer> victims;  //
    //std::vector<TRectangle> walls;   //

    BotShoot(rbw::WorldSimulator * server, std::vector<TPlayer> &victims, TVector bot);
    TVector getVictimCoord();

private:
    std::vector<TRectangle> walls;   //
    std::vector<TPlayer> victims;  //
    TVector bot_coord;
    std::vector<int> x_coord, y_coord;

    //std::vector<int> distance_to_victims;
    bool can_we_hit_victim(TVector victimCoord);
    std::vector<TPlayer> achievableVictims();   //victims, which aren't behind any wall;

    //TParamLineEq getParamLineEq(TVector u, TVector v); //from u to v;
    TLineEq getLineEq(TVector u, TVector v);
    TLineEq rotateLineEq(TLineEq lineEq);
    float value_y_from_x0(TLineEq& lineEq, float x0);  //if line is vertical, returns NULL;

    bool pixelIsInTheWall(TVector pixel);

    void sortVector(std::vector<int> &vector);
    std::vector<TRectangle> getWalls();
    rbw::WorldSimulator * server;
};

#endif // BOTSHOOT_H
