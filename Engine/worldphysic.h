#ifndef WORLDPHYSIC_H
#define WORLDPHYSIC_H

#include <vector>
#include <math.h>
#include <SFML/Graphics.hpp>

namespace rbw
{

const double EPS = 1E-9;
const double fEPS = 1E-6;

class intPolygon
{
public:
    intPolygon(std::vector< sf::Vector2f > point);
    intPolygon(sf::Rect < int > rect);
    sf::Vector2f CheckIntersect(sf::Vector2f p0, sf::Vector2f);
    sf::Vector2f ReflexSpeed(sf::Vector2f p0, sf::Vector2f * speedvec);
    float GetArea();
private:
    std::vector< sf::Vector2f > Points;
    float Area;
};

sf::Vector2f GetPointOfIntersection(sf::Vector2f a0, sf::Vector2f a1, sf::Vector2f b0, sf::Vector2f b1);
int sgn(float x);

};

#endif // WORLDPHYSIC_H
