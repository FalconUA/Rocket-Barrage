#ifndef WORLDSIMULATOR_H
#define WORLDSIMULATOR_H

#include <level.h>
#include <iostream>
#include <vector>
#include <graphicobject.h>
#include <worldconstant.h>
#include <worldphysic.h>
#include <SFML/Graphics.hpp>

namespace rbw // Rocket Barrage World
{

/* pre-declaration of Game Objects
 */
class Player;
class HomingMissile;
class BouncingBomb;
class Grenade;

/* Chain of Homing Missiles
 */

struct __HM_ChainElement{
    struct __HM_ChainElement * prev;
    rbw::HomingMissile * rocket;
    struct __HM_ChainElement * next;
};
typedef struct __HM_ChainElement HM_ChainElement; // HM means "Homing Missile"

typedef struct {
   HM_ChainElement * FirstInChain;
   HM_ChainElement * LastInChain;
} HomingMissileChain;

/* Chain of Bouncing Rockets
 */
struct __BB_ChainElement{
    struct __BB_ChainElement * prev;
    rbw::BouncingBomb * rocket;
    struct __BB_ChainElement * next;
};
typedef struct __BB_ChainElement BB_ChainElement; // BB means "Bouncing Bomb

struct __G_ChainElement{
    struct __G_ChainElement * prev;
    rbw::Grenade * rocket;
    struct __G_ChainElement * next;
};
typedef struct __G_ChainElement G_ChainElement;

typedef struct {
    G_ChainElement * FirstInChain;
    G_ChainElement * LastInChain;
} GrenadeChain;



typedef struct {
   BB_ChainElement * FirstInChain;
   BB_ChainElement * LastInChain;
} BouncingBombChain;



typedef struct {
    float FPS;
    Level * level;

    std::vector< rbw::Player* > Players;

    rbw::HomingMissileChain homingMissiles;
    rbw::BouncingBombChain bouncingBombs;
    rbw::GrenadeChain Grenades;

    std::vector< Object > wallForPlayer;
    std::vector< Object > wallForRocket;

    sf::Clock WorldClock;
    sf::Time ElapsedTime;
} WorldInformation;

class WorldObject
{
public:
    void SetPosition(sf::Vector2f newPosition);
    void SetSpeed(sf::Vector2f newSpeed);

    sf::Vector2f GetPosition();
    rbw::ObjectType GetType();
    sf::Vector2f GetSpeed();
    sf::Vector2f GetIntegerPosition();

protected:
    sf::Vector2f position;
    sf::Vector2f speed;
    rbw::ObjectType type;
    rbw::WorldInformation * worldInfo;
};



class Player: public WorldObject
{
public:
    Player(std::string PlayerName,
           rbw::Team team,
           sf::Vector2f spawnPosition,
           rbw::WorldInformation * worldInfo);
    std::string GetPlayerName();

    void Move(rbw::Direction direction);

    bool isAlive();
    rbw::Team GetTeam();
    int GetHealth();
    void Hit(int damage);
private:
    rbw::Team team;
    std::string PlayerName;
    int health;
    bool alive;
};


class HomingMissile: public WorldObject
{
public:
    HomingMissile(rbw::Player * owner,
                  rbw::Player * target,
                  rbw::HM_ChainElement * location,
                  rbw::WorldInformation * worldInfo);
    ~HomingMissile();
    void SimulateNextStep();
    rbw::Player * GetOwner();
    rbw::Player * GetTarget();
    bool HaveToBeDestroyed();
private:
    bool haveToBeDestroyed;
    rbw::HM_ChainElement * LocationInChain;
    sf::Vector2f getExplosionPoint(rbw::Player ** victim);
    rbw::Player * owner;
    rbw::Player * target;
};

class BouncingBomb: public WorldObject
{
public:
    BouncingBomb(rbw::Player * owner,
                 rbw::BB_ChainElement * location,
                 rbw::WorldInformation * worldInfo);
    ~BouncingBomb();
    void SimulateNextStep();
    rbw::Player * GetOwner();
    bool HaveToBeDestroyed();
private:
    bool haveToBeDestroyed;
    sf::Vector2f getCollisionWithPlayers(rbw::Player ** victim);
    sf::Vector2f getReflexVector();
    sf::Vector2f getExplosionPoint(rbw::Player ** victim);
    rbw::BB_ChainElement * LocationInChain;
    rbw::Player * owner;
    int BounceCount;
};

class Grenade: public WorldObject
{
public:
    Grenade(rbw::Player * owner,
            rbw::G_ChainElement * location,
            rbw::WorldInformation * worldInfo,
            sf::Vector2f DestPoint);
    ~Grenade();
    void SimulateNextStep();
    rbw::Player * GetOwner();
    bool HaveToBeDestroyed();
    float GetZoom();
private:
    bool haveToBeDestroyed;
    rbw::Player * owner;
    rbw::G_ChainElement * LocationInChain;

    float zoom_coefficient;
    sf::Vector2f ProjectionToGround;
    sf::Vector2f StartingPoint;
    sf::Vector2f DestinationPoint;
    sf::Vector3f Equation;
};

class WorldSimulator
{
public:
    WorldSimulator();
    ~WorldSimulator();
    void Init(Level * level, float FPS);

/* External functions:
 * these functions will be used by server!
 */
    bool AddPlayer(std::string PlayerName, rbw::Team team);
    bool AddHomingMissile(std::string PlayerName, sf::Vector2i mousePosition);
    bool AddBouncingBomb(std::string PlayerName, sf::Vector2i mousePosition);
    bool AddGrenade(std::string PlayerName, sf::Vector2i mousePosition);
    bool AddMoveRequest(std::string PlayerName, rbw::Direction direction);    
    float SimulateNextStep(); // returns elapsed time since last world step

// this function will be used by user
    bool GetObjects(std::vector< GraphicObject >* objects);
private:    
    rbw::WorldInformation worldInfo;
};

}; // end of namespace rbw

#endif // WORLDSIMULATOR_H
