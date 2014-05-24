#ifndef EVADEFROMTHEROCKET_H
#define EVADEFROMTHEROCKET_H

#include <worldconstant.h>
#include <worldsimulator.h>
#include <math.h>

struct TVector
{
    int x, y;
};

//struct TRectangle
//{
//    TVector A,B;
//};

struct TRocket
{
    TVector coordinates;  //current coordinates of the rocket;
    TVector speedVector;   //speed vector of the rocket;
};

/*
 *Trajectory will be a parametric system {x(t),y(t)},
 *where (x,y) is a coordinate of the rocket
 *on the t-s milisecond of the round;
 */

//struct TParamLine
//{
//    double x_const,  y_const, x_tan, y_tan;
    /*
     *x(t) = x_const + x_tan * t;
     *y(t) = y_const + y_tan * t;
     */
//};

//typedef TParamLine* TTrajectory;

struct TDirectionPair
{
    rbw::Direction vert,hor;
};

//---------------------------------//


class EvadeFromTheRocket
{
public:
    //std::vector<TRectangle> rocket_walls;

    EvadeFromTheRocket(rbw::WorldSimulator * server);
   // TTrajectory createRocketTrajectory(TRocket rocket, int kicksNumber /*how many times rocket have
   //                                                                         imagingly kicked the wall*/,
   //                                    double playTime /*current milisecond of the round*/,
   //                                    TParamLine* pLine /*pointer to i place in the massive trajectory*/);
    bool pixelIsSave(TVector pixel, int calculating_steps);
    std::vector<TDirectionPair> saveDirection(TVector player_coord);

private:
    rbw::WorldSimulator * server;
    bool pixelIsInTheWall(TVector pixel);   //if (x,y) is a wall then 'true' else 'false';
    void sortVector(std::vector<int> &vector);
    TRocket BouncingBomb_to_TRocket(rbw::BouncingBomb &rocket);
    TRocket HomingMissile_to_TRocket(rbw::HomingMissile &rocket);
    TRocket Grenade_to_TRocket(rbw::Grenade &rocket);
};

#endif // EVADEFROMTHEROCKET_H
