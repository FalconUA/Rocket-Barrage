#ifndef SERVEREMULATOR_H
#define SERVEREMULATOR_H
#include <level.h>
#include <gameobject.h>
#include <graphicobject.h>

/* Goal in future:
 * at this stage, we waste O(n) operations to find the owner of the rocket
 * i've made it so because in the past architecture of project, we want to have an
 * independent vectors of objects
 *
 * but now he have fully functional
 */


namespace RB
{
// RB == Rocket Barrage


const double RocketSpeed = 12.0;
const double PlayerSpeed = 8.0;
const unsigned int MAX_BOUNCE_COUNT = 6;

struct __RChainElement {
    struct __RChainElement * prev;
    Rocket * rocket;
    struct __RChainElement * next;
};
typedef struct __RChainElement RChainElement;
typedef struct {
   RChainElement * FirstRocket;
   RChainElement * LastRocket;
} RocketChain;

typedef std::pair<int,int> vector2int;
typedef std::pair<float,float> vector2float;
typedef std::pair<double,double> vector2double;

class ServerEmulator
{
public:
    ServerEmulator();
    ~ServerEmulator();
    void Init(Level * level);

// External Query
    int AddPlayer(std::string PlayerName, int team);
    int AddBounceRocket(std::string PlayerName, int mouse_x, int mouse_y);
    int AddFollowRocket(std::string PlayerName, int mouse_x, int mouse_y);
    bool AddMoveRequest(std::string PlayerName, char Direction);

/* optimization needed :
 * query functions must return something
 *
 */

    void CalcNextStep(); //note: only use after moving player (after all move request functions)
    void GetObjects(std::vector< GraphicObject >* objects );
    std::vector< std::string > GetDeadPlayers();    

private:
    std::pair<int,int> GetReflexVector(int x0, int y0, std::pair<int,int>* speedvec);
    std::pair<int,int> GetPointOfIntersection(int x0, int y0, int x1, int y1, int a0, int b0, int a1, int b1);
    std::pair<int,int> GetPointOfExplosion(RB::Rocket * rocket, RB::Player* * victim);
    std::pair<int,int> GetPointOfPlayerCollision(RB::Rocket * rocket, RB::Player* * victim);

    std::vector< Player* > player;    
    RB::RocketChain rocketChain;

    std::vector< Object > wallForPlayer;
    std::vector< Object > wallForRocket;

    /*  here we can add a vector of effects
     * that means you can store the explosions in the vector and draw them in the next frame
     */
    Level * level;
};


}; // End of namespace RB

#endif // SERVEREMULATOR_H
