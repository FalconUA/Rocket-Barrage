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
    rbw::Team team;
    int x;
    int y;
    bool occupied;
} spawnPos;

typedef struct
{
    std::string PlayerName;
    rbw::Team team;
    int Kill;
    int Death;
    int DamageDealt;

    int HomingMissilesLeft;
    int BouncingBombsLeft;
    int GrenadesLeft;

    bool isDead;
} PlayerExportInformation;


typedef struct {
    float FPS;

    sf::Clock WorldClock;
    sf::Time ElapsedTime;

    rbw::HomingMissileChain homingMissiles;
    rbw::BouncingBombChain bouncingBombs;
    rbw::GrenadeChain Grenades;

    Level * level;

    std::vector< rbw::Player* > Players;    
    std::vector< Object > wallForPlayer;
    std::vector< Object > wallForRocket;
    std::vector< rbw::spawnPos* > spawnPositions;
    std::vector< std::string > WorldEvents;

    std::vector< rbw::GraphicObject > Explosions;
} WorldInformation;

class WorldObject
{
public:
    void SetPosition(sf::Vector2f newPosition);
    void SetSpeed(sf::Vector2f newSpeed);

    sf::Vector2f GetPosition();
    rbw::ObjectType GetType();
    sf::Vector2f GetSpeed();    
    rbw::WorldInformation * const getWorldInfo();

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
           rbw::spawnPos * spawnPosition,
           rbw::WorldInformation * worldInfo,
           bool bot);   //it's new
    std::string GetPlayerName();

    void Move(sf::Vector2i direction);
    void Respawn();

    bool isAlive();
    rbw::Team GetTeam();
    int GetHealth();
    bool Hit(int damage); // the true result means that this player was killed right after calling this function
    bool bot(); //true if bot; false if real player;
                //it's new


    int Kill;
    int Death;    
    int DamageDealt;

    int HomingMissilesLeft;
    int BouncingBombsLeft;
    int GrenadesLeft;

    rbw::PlayerExportInformation Export();
private:
    rbw::Team team;
    std::string PlayerName;
    int health;
    bool alive;

    bool BOT;   //true if bot; false if real player;
                //it's new
    rbw::spawnPos * spawnPosition;
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
    bool AddPlayer(std::string PlayerName, rbw::Team team, bool isBot); // the false return means that current map is not designed for more players
    bool AddHomingMissile(std::string PlayerName, sf::Vector2i mousePosition);
    bool AddBouncingBomb(std::string PlayerName, sf::Vector2i mousePosition);
    bool AddGrenade(std::string PlayerName, sf::Vector2i mousePosition);
    bool AddMoveRequest(std::string PlayerName, sf::Vector2i direction);
    float SimulateNextStep(); // returns elapsed time since last world step

    std::vector< std::string > ExportEvents();
    std::vector< rbw::PlayerExportInformation > ExportPlayerInfo();
    bool GetObjects(std::vector< GraphicObject >* objects);

    bool RoundEnded(rbw::Team * WinningTeam);
    bool RoundDraw();
    rbw::WorldInformation * const getWorldInfo();
private:    
    rbw::WorldInformation worldInfo;    
};

}; // end of namespace rbw

#endif // WORLDSIMULATOR_H
