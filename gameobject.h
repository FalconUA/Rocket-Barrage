#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


namespace RB
{

typedef enum {
    TEAM_FIRST,
    TEAM_SECOND
} Team;

class GameObject
{
public:    
    void GetPosition(int* x, int* y);
    void SetPosition(int x, int y);
    std::string Type();
    std::pair<int, int> GetSpeed();
    void SetSpeed(std::pair<int, int> newSpeed);
protected:    
    std::pair<int, int> speed;
    std::string type;
    int x;
    int y;
};

class Player: public GameObject
{
public:    
    Player(std::string PlayerName);
    std::string GetName();    
private:
    RB::Team team;
    std::string PlayerName;
    int Health;    
    bool Dead;
};

class Rocket: public GameObject
{
public:
    RB::Player * GetOwner();
    std::string getOwnerName();
protected:
    RB::Player * owner;
    std::string ownerName;
};

class BounceRocket: public Rocket
{
public:    
    BounceRocket(RB::Player * owner);
    int GetBounceCount();
    void IncBounceCount();
private:
    int BounceCount;
};

class FollowRocket: public Rocket
{
public:    
    FollowRocket(RB::Player * owner, RB::Player * target);    
    std::string GetTargetName();
private:
    std::string targetName;
    RB::Player * target;
};

}; // end of namespace RB

#endif // GAMEOBJECT_H
