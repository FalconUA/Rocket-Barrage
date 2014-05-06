#include "serveremulator.h"
#include <math.h>
#include <iostream>
#include <algorithm>

using namespace RB;

ServerEmulator::ServerEmulator()
{
    this->player.clear();   
    this->rocketChain.FirstRocket = NULL;
    this->rocketChain.LastRocket = NULL;
}

ServerEmulator::~ServerEmulator(){
    for (int i=0; i<this->player.size(); i++){
        RB::Player * tmpPlayer = this->player[i];
        delete tmpPlayer;
    }
    RB::RChainElement * tmpElement = this->rocketChain.FirstRocket;
    while (tmpElement != NULL){
        RB::Rocket * tmpRocket = tmpElement->rocket;
        delete tmpRocket;
        tmpElement = tmpElement->next;
    }

}

void ServerEmulator::Init(Level *level){
    this->level = level;
    this->wallForPlayer = level->GetObjects("wall");
}

int ServerEmulator::AddPlayer(std::string PlayerName, int team){
    Player * newplayer = new Player(PlayerName);
    newplayer->SetPosition(40, 40);
std::cout << "player added : " << newplayer << std::endl;
    this->player.push_back(newplayer);
    return this->player.size()-1;
}

int ServerEmulator::AddBounceRocket(std::string PlayerName, int mouse_x, int mouse_y){
    Player* owner = NULL;
    for (int i=0; i<player.size(); i++){
        owner = player[i];
        if (owner->GetName() == PlayerName) break;
    }
    if (owner == NULL) return -1;
    std::cout << "bounce rocket owner: " << owner->GetName() << std::endl;

    // setting speed vector
    int x0, y0;
    owner->GetPosition(&x0, &y0);
    int a = mouse_x - x0;
    int b = mouse_y - y0;

    std::cout << "speedvec " << a << " " << b << std::endl;

    double len = sqrt(a*a + b*b);
    double koef = RocketSpeed / len;
    a = koef * a;
    b = koef * b;
    // (a,b) - speed vector
    std::cout << "modspeed: " << a << " " << b << std::endl;

    // creating new rocket
    Rocket* newRocket = new BounceRocket(owner);
    newRocket->SetSpeed(std::make_pair(a,b));
    newRocket->SetPosition(x0, y0);

    // new chain element
    RChainElement * newElement = new RChainElement;
    newElement->rocket = newRocket;

    if (this->rocketChain.LastRocket == NULL){
        this->rocketChain.FirstRocket = newElement;
        this->rocketChain.LastRocket = newElement;
        newElement->next = NULL;
        newElement->prev = NULL;
    }
    else {
        newElement->prev = this->rocketChain.LastRocket;
        newElement->next = NULL;
        this->rocketChain.LastRocket->next = newElement;
        this->rocketChain.LastRocket = newElement;
    }


    return 0;
    /* need to be changed to something like:
     * "return the pointer to newElement
     */

    //this->rocket.push_back(newRocket);
    //return this->rocket.size() - 1;
}

int ServerEmulator::AddFollowRocket(std::string PlayerName, int mouse_x, int mouse_y){       
    int sx, sy; // starting coordinates

    Player* owner = NULL;
    for (int i=0; i<player.size(); i++){
        owner = player[i];
        if (owner->GetName() == PlayerName) break;
    }
    if (owner == NULL) return -1;
    std::cout << "Homing missile owner: " << owner->GetName() << std::endl;
    owner->GetPosition(&sx, &sy);

    Player* target = NULL;
    double distance = 1000000000.0;
    for (int i=0; i<player.size(); i++){
        Player* tmpPlayer = player[i];        

        if (tmpPlayer->GetName() == owner->GetName())
            continue;

        int x0, y0;
        tmpPlayer->GetPosition(&x0, &y0);
        int a, b;
        a = mouse_x - x0;
        b = mouse_y - y0;
        double newdistance = a*a + b*b;

        //if that bastard is closer
        if (newdistance < distance){
            distance = newdistance;
            target = tmpPlayer;
        }        
    }
    if (target == NULL){
        std::cout << "no victim found";
        return 0;
    }

    std::cout << PlayerName << " " << target->GetName() << std::endl;

    Rocket* newRocket = new FollowRocket(owner, target);
    newRocket->SetPosition(sx, sy);

    RB::RChainElement * newElement = new RB::RChainElement;
    newElement->rocket = newRocket;

    if (this->rocketChain.FirstRocket == NULL){
        this->rocketChain.FirstRocket = newElement;
        this->rocketChain.LastRocket = newElement;
        newElement->next = NULL;
        newElement->prev = NULL;
    }
    else {
        newElement->prev = this->rocketChain.LastRocket;
        newElement->next = NULL;
        this->rocketChain.LastRocket->next = newElement;
        this->rocketChain.LastRocket = newElement;
    }

    //this->rocket.push_back(newrocket);
    //return this->rocket.size() - 1;
}

bool ServerEmulator::AddMoveRequest(std::string PlayerName, char Direction){

    Player* currPlayer = NULL;
    for (int i=0; i<player.size(); i++){
        Player* tmp = player[i];
        if (tmp->GetName() == PlayerName){
            currPlayer = tmp;
            break;
        }
    }
    if (currPlayer == NULL) return false;

    int x0, y0;
    currPlayer->GetPosition(&x0, &y0);

    int new_x = x0;
    int new_y = y0;
    if (Direction == 'a') new_x += -8;
    if (Direction == 'd') new_x += +8;
    if (Direction == 'w') new_y += -8;
    if (Direction == 's') new_y += +8;

    bool can_move = true;
    for (int i=0; i<wallForPlayer.size(); i++){
        bool temp;

        int y1 = wallForPlayer[i].rect.top;
        int x1 = wallForPlayer[i].rect.left;
        int y2 = y1 + wallForPlayer[i].rect.height;
        int x2 = x1 + wallForPlayer[i].rect.width;

        temp = ( !((new_y < y2) && (new_y > y1) && (new_x < x2) && (new_x > x1)) );
        can_move = can_move && temp;
    }
    if (!can_move) return false;

    currPlayer->SetPosition(new_x, new_y);
    return true;
}
void ServerEmulator::CalcNextStep(){

    RB::RChainElement * currElement = this->rocketChain.FirstRocket;
    while (currElement != NULL){
        RB::Rocket * tmp = currElement->rocket;
        if (tmp->Type() == "BounceRocket"){
            RB::BounceRocket * bounce = (RB::BounceRocket*) tmp;
            int x0, y0;
            tmp->GetPosition(&x0, &y0);

            std::pair<int,int> explosionPoint;
            RB::Player * victim;

            if (bounce->GetBounceCount() < RB::MAX_BOUNCE_COUNT)
                explosionPoint = this->GetPointOfPlayerCollision(bounce, &victim);
            else explosionPoint = this->GetPointOfExplosion(bounce, &victim);
            if (explosionPoint != std::make_pair(-1,-1))
            {
                std::cout << "expoint is not -1 -1" << std::endl;
                // here we have to destroy this current chain element (currElement)
                RB::RChainElement * previousEl = currElement->prev;
                RB::RChainElement * nextEl = currElement->next;
                if (previousEl != NULL)
                    previousEl->next = nextEl;
                if (nextEl != NULL)
                    nextEl->prev = previousEl;
                // if current element is the first rocket in chain
                if (this->rocketChain.FirstRocket == currElement){
                    this->rocketChain.FirstRocket = currElement->next;
                }
                // if current element is the last rocket in chain
                if (this->rocketChain.LastRocket == currElement){
                    this->rocketChain.LastRocket = currElement->prev;
                }

                delete currElement;
                currElement = nextEl;
                continue; // continue the main loop while (currElement != NULL) { ... }
            }

            std::pair<int,int> speedvec = tmp->GetSpeed();
            int next_x = x0 + speedvec.first;
            int next_y = y0 + speedvec.second;

            std::pair< int, int > POI = this->GetReflexVector(x0, y0, &speedvec);
            if ((POI.first >=0) && (POI.second >= 0)){
                next_x = POI.first;
                next_y = POI.second;
                tmp->SetSpeed(speedvec);
                bounce->IncBounceCount();
            }
            tmp->SetPosition(next_x, next_y);
        }
/*  Need optimization
 *  we have to add a pointer to player to class "follow rocket"
 *  it will decrease the number of computations
 *  therefore, increase the speed of the game
 */

        if (tmp->Type() == "FollowRocket"){
            FollowRocket* follow = (FollowRocket*) tmp;
            int x0, y0;
            tmp->GetPosition(&x0, &y0);

            std::pair<int,int> explosionPoint;
            RB::Player * victim;

            explosionPoint = this->GetPointOfExplosion(follow, &victim);
            if (explosionPoint != std::make_pair(-1,-1))
            {
                std::cout << "expoint is not -1 -1" << std::endl;
                // here we have to destroy this current chain element (currElement)
                RB::RChainElement * previousEl = currElement->prev;
                RB::RChainElement * nextEl = currElement->next;
                if (previousEl != NULL)
                    previousEl->next = nextEl;
                if (nextEl != NULL)
                    nextEl->prev = previousEl;
                // if current element is the first rocket in chain
                if (this->rocketChain.FirstRocket == currElement){
                    this->rocketChain.FirstRocket = currElement->next;
                }
                // if current element is the last rocket in chain
                if (this->rocketChain.LastRocket == currElement){
                    this->rocketChain.LastRocket = currElement->prev;
                }

                delete currElement;
                currElement = nextEl;
                continue; // continue the main loop while (currElement != NULL) { ... }
            }

            int tx, ty;
            for (int k=0; k<this->player.size(); k++){
                Player * tmpp;
                tmpp = player[k];
                if (tmpp->GetName() == follow->GetTargetName())
                    tmpp->GetPosition(&tx, &ty);
            }
            int a = tx - x0;
            int b = ty - y0;

            double len = sqrt(a*a + b*b);
            double koef = RocketSpeed / len;
            a = koef * a;
            b = koef * b;
            // (a,b) - speed vector

            tmp->SetSpeed(std::make_pair(a,b));
            tmp->SetPosition(x0 + a, y0 + b);
        }
        currElement = currElement->next;
    }
}

std::vector< std::string > ServerEmulator::GetDeadPlayers(){

}


void ServerEmulator::GetObjects(std::vector< GraphicObject > * objects){
    using namespace rbw::Graphic;
    objects->clear();
    for (int i=0; i<player.size(); i++){
        GraphicObject newobject;
        Player* tmp = player[i];
        int x0, y0;
        tmp->GetPosition(&x0, &y0);
        newobject.Name = tmp->GetName();
        newobject.type = PLAYER;
        newobject.x = x0;
        newobject.y = y0;
        objects->push_back(newobject);
    }
    RB::RChainElement * currElement = this->rocketChain.FirstRocket;
    while (currElement != NULL){
        GraphicObject newobject;
        Rocket* tmp = currElement->rocket;
        int x0, y0;
        tmp->GetPosition(&x0, &y0);

        GraphicObjectType newtype;
        if (tmp->Type() == "BounceRocket") newtype = BOUNCE_ROCKET;
        if (tmp->Type() == "FollowRocket") newtype = FOLLOW_ROCKET;

        newobject.Name = "";
        newobject.type = newtype;
        newobject.x = x0;
        newobject.y = y0;
        objects->push_back(newobject);
        currElement = currElement->next;
    }
}

// private math functions


//note: this function is only for Homing Missiles

std::pair<int,int> ServerEmulator::GetPointOfPlayerCollision(RB::Rocket * rocket, RB::Player* * victim){
    int x0, y0;
    std::pair<int,int> speedvec;
    rocket->GetPosition(&x0, &y0);
    speedvec = rocket->GetSpeed();

    int x1 = x0 + speedvec.first;
    int y1 = y0 + speedvec.second;

    for (int i=0; i<this->player.size(); i++){
        int px, py;
        RB::Player * tmp = player[i];

        if (rocket->getOwnerName() == tmp->GetName()) continue;

        tmp->GetPosition(&px, &py);
        int a = px - x0;
        int b = py - y0;
        int distance = a*a + b*b;
        if (distance < 24*24){
            *victim = tmp;
            std::cout << "victim: " << (*victim)->GetName() << std::endl;
            return std::make_pair(x0, y0);
            break;
        }
    }
    return std::make_pair(-1,-1);
}

std::pair<int,int> ServerEmulator::GetPointOfExplosion(RB::Rocket * rocket, RB::Player ** victim){
    int x0, y0;
    std::pair<int,int> speedvec;
    rocket->GetPosition(&x0, &y0);
    speedvec = rocket->GetSpeed();

    int x1 = x0 + speedvec.first;
    int y1 = y0 + speedvec.second;    

    std::pair<int,int> pAns = std::make_pair(-1,-1);

    pAns = this->GetPointOfPlayerCollision(rocket, victim);
    if (pAns != std::make_pair(-1,-1)) return pAns;

    int minLen = 1000000000;

    int CollisionCount = 0;
    bool is_ver = false, is_hor = false;

    for (int i=0; i<this->wallForPlayer.size(); i++){
        sf::Rect< int > tmp = wallForPlayer[i].rect;
        //has collision with i-th rectangle
        if ((x1 > tmp.left) && (x1 < tmp.left + tmp.width) &&
                (y1 > tmp.top) && (y1 < tmp.top + tmp.height)){
            int a0, a1, b0, b1;
            std::pair<int,int> p1, p2, p3, p4;

            //top horizontal
            a0 = tmp.left; a1 = tmp.left + tmp.width;
            b0 = tmp.top;  b1 = tmp.top;
            p1 = this->GetPointOfIntersection(x0, y0, x1, y1, a0, b0, a1, b1);
            if (p1 != std::make_pair(-1,-1)){
                CollisionCount ++;
                int txm = p1.first - x0;
                int tym = p1.second - y0;
                int len = txm*txm + tym*tym;
                if (len < minLen){
                    minLen = len;
                    pAns = p1;
                    is_hor = true;
                }
            }


            // bottom horizontal
            a0 = tmp.left; a1 = tmp.left + tmp.width;
            b0 = tmp.top + tmp.height;  b1 = tmp.top + tmp.height;
            p2 = this->GetPointOfIntersection(x0, y0, x1, y1, a0, b0, a1, b1);
            if (p2 != std::make_pair(-1,-1)){
                CollisionCount ++;
                int txm = p2.first - x0;
                int tym = p2.second - y0;
                int len = txm*txm + tym*tym;
                if (len < minLen){
                    minLen = len;
                    pAns = p2;
                    is_hor = true;
                }
            }

            // left vertical
            a0 = tmp.left; a1 = tmp.left;
            b0 = tmp.top;  b1 = tmp.top + tmp.height;
            p3 = this->GetPointOfIntersection(x0, y0, x1, y1, a0, b0, a1, b1);
            if (p3 != std::make_pair(-1,-1)){
                CollisionCount ++;
                int txm = p3.first - x0;
                int tym = p3.second - y0;
                int len = txm*txm + tym*tym;
                if (len < minLen){
                    minLen = len;
                    pAns = p3;
                    is_ver = true;
                }
            }

            // right vertical
            a0 = tmp.left + tmp.width; a1 = tmp.left + tmp.width;
            b0 = tmp.top;  b1 = tmp.top + tmp.height;
            p4 = this->GetPointOfIntersection(x0, y0, x1, y1, a0, b0, a1, b1);
            if (p4 != std::make_pair(-1,-1)){
                CollisionCount ++;
                int txm = p4.first - x0;
                int tym = p4.second - y0;
                int len = txm*txm + tym*tym;
                if (len < minLen){
                    minLen = len;
                    pAns = p4;
                    is_ver = true;
                }
            }
        }
    }

    // now we found that pAns - first point of intersection
    *victim = NULL;
    return pAns;
}


std::pair< int,int > ServerEmulator::GetReflexVector(int x0, int y0, std::pair<int, int>* speedvec){
    int x1 = x0+ (*speedvec).first;
    int y1 = y0+ (*speedvec).second;

    std::pair<int,int> pAns = std::make_pair(-1,-1);
    int minLen = 1000000000;

    int CollisionCount = 0;
    bool is_ver = false, is_hor = false;

    for (int i=0; i<this->wallForPlayer.size(); i++){        
        sf::Rect< int > tmp = wallForPlayer[i].rect;
        //has collision with i-th rectangle
        if ((x1 > tmp.left) && (x1 < tmp.left + tmp.width) &&
                (y1 > tmp.top) && (y1 < tmp.top + tmp.height)){
            int a0, a1, b0, b1;
            std::pair<int,int> p1, p2, p3, p4;

            //top horizontal
            a0 = tmp.left; a1 = tmp.left + tmp.width;
            b0 = tmp.top;  b1 = tmp.top;
            p1 = this->GetPointOfIntersection(x0, y0, x1, y1, a0, b0, a1, b1);
            if (p1 != std::make_pair(-1,-1)){
                CollisionCount ++;
                int txm = p1.first - x0;
                int tym = p1.second - y0;
                int len = txm*txm + tym*tym;
                if (len < minLen){
                    minLen = len;
                    pAns = p1;
                    is_hor = true;
                }
            }


            // bottom horizontal
            a0 = tmp.left; a1 = tmp.left + tmp.width;
            b0 = tmp.top + tmp.height;  b1 = tmp.top + tmp.height;
            p2 = this->GetPointOfIntersection(x0, y0, x1, y1, a0, b0, a1, b1);
            if (p2 != std::make_pair(-1,-1)){
                CollisionCount ++;
                int txm = p2.first - x0;
                int tym = p2.second - y0;
                int len = txm*txm + tym*tym;
                if (len < minLen){
                    minLen = len;
                    pAns = p2;
                    is_hor = true;
                }
            }

            // left vertical
            a0 = tmp.left; a1 = tmp.left;
            b0 = tmp.top;  b1 = tmp.top + tmp.height;
            p3 = this->GetPointOfIntersection(x0, y0, x1, y1, a0, b0, a1, b1);
            if (p3 != std::make_pair(-1,-1)){
                CollisionCount ++;
                int txm = p3.first - x0;
                int tym = p3.second - y0;
                int len = txm*txm + tym*tym;
                if (len < minLen){
                    minLen = len;
                    pAns = p3;
                    is_ver = true;
                }
            }

            // right vertical
            a0 = tmp.left + tmp.width; a1 = tmp.left + tmp.width;
            b0 = tmp.top;  b1 = tmp.top + tmp.height;
            p4 = this->GetPointOfIntersection(x0, y0, x1, y1, a0, b0, a1, b1);
            if (p4 != std::make_pair(-1,-1)){
                CollisionCount ++;
                int txm = p4.first - x0;
                int tym = p4.second - y0;
                int len = txm*txm + tym*tym;
                if (len < minLen){
                    minLen = len;
                    pAns = p4;
                    is_ver = true;
                }
            }
        }
    }

    // now we found that pAns - first point of intersection
    if (is_hor){
        (*speedvec).second *= -1;
    }
    if (is_ver){
        (*speedvec).first *= -1;
    }
    return pAns;
}

struct pt {
    int x, y;
};

const double EPS = 1E-9;

inline int det (int a, int b, int c, int d) {
    return a * d - b * c;
}

inline bool between (int a, int b, double c) {
    return std::min(a,b) <= c + EPS && c <= std::max(a,b) + EPS;
}

inline bool intersect_1 (int a, int b, int c, int d) {
    if (a > b)  std::swap (a, b);
    if (c > d)  std::swap (c, d);
    return (std::max(a,c) <= std::min(b,d));
}

bool intersect (pt a, pt b, pt c, pt d, double * ans_x, double * ans_y) {
    *ans_x = -1.0;
    *ans_y = -1.0;
    int A1 = a.y-b.y,  B1 = b.x-a.x,  C1 = -A1*a.x - B1*a.y;
    int A2 = c.y-d.y,  B2 = d.x-c.x,  C2 = -A2*c.x - B2*c.y;
    int zn = det (A1, B1, A2, B2);
    if (zn != 0) {
        double x = - det (C1, B1, C2, B2) * 1. / zn;
        double y = - det (A1, C1, A2, C2) * 1. / zn;
        *ans_x = x;
        *ans_y = y;
        return between (a.x, b.x, x) && between (a.y, b.y, y)
            && between (c.x, d.x, x) && between (c.y, d.y, y);
    }
    else return false;
    /*
        return det (A1, C1, A2, C2) == 0 && det (B1, C1, B2, C2) == 0
            && intersect_1 (a.x, b.x, c.x, d.x)
            && intersect_1 (a.y, b.y, c.y, d.y);
    */
}
std::pair<int,int> ServerEmulator::GetPointOfIntersection(int x0, int y0, int x1, int y1, int a0, int b0, int a1, int b1){
    pt a = {x0, y0};
    pt b = {x1, y1};
    pt c = {a0, b0};
    pt d = {a1, b1};
    double x,y;
    bool is_intersect = intersect(a,b,c,d,&x, &y);
    if (is_intersect) return std::make_pair((int)x, (int)y);
    return std::make_pair(-1,-1);
}
