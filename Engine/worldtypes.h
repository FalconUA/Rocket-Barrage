#ifndef WORLDTYPES_H
#define WORLDTYPES_H

#include <iostream>
#include <SFML/System.hpp>

struct TVector
{
    int x, y;
};

struct TRectangle
{
    TVector A,B;
};

struct TPlayer{
    TVector coord;
    int speed;
    std::string name;
};   //

//struct TParamLineEq
//{
//    float x_0, a, y_0, b;
//    /*
//     *x = x_0 + a*t;
//     *y = y_0 + b*t;
//     */
//};

struct TLineEq
{
    float k, c;
    /*
     *y = k*x + c;
     */
    bool vertical;  //if line is vertical then x = k;
};

struct TRocket
{
    TVector coordinates;  //current coordinates of the rocket;
    TVector speedVector;   //speed vector of the rocket;
};

struct TSafeDirections
{
    bool Up_Left, Up, Up_Right,
            Left, None, Right,
       Down_Left, Down, Down_Right;
};

struct TDirectionPair
{
    rbw::Direction vert,hor;
};

struct TPair_PlayerDirection
{
    std::string playerName;
    sf::Vector2i direction;
};

#endif // WORLDTYPES_H
