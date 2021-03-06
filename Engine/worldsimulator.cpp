#include "worldsimulator.h"
#include <math.h>
#include <float.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

namespace rbw
{

/* Class WorldObject:
 * A prototype of all moving object (rockets, players, bombs) in this world
 */

sf::Vector2f WorldObject::GetPosition()
{
    return this->position;
}
sf::Vector2f WorldObject::GetSpeed()
{
    return this->speed;
}
rbw::ObjectType WorldObject::GetType()
{
    return this->type;
}
void WorldObject::SetPosition(sf::Vector2f newPosition)
{
    this->position = newPosition;
}
void WorldObject::SetSpeed(sf::Vector2f newSpeed)
{
    this->speed = newSpeed;
}
rbw::WorldInformation * const WorldObject::getWorldInfo()
{
    return this->worldInfo;
}

/* Class Player:
 * a type of objects in this GameWorld
 */

Player::Player(std::string PlayerName, Team team, rbw::spawnPos * spawnPosition, rbw::WorldInformation * worldInfo, bool bot, WorldSimulator *server)
{    
    this->PlayerName = PlayerName;
    this->team = team;
    this->type = rbw::TYPE_PLAYER;    
    this->spawnPosition = spawnPosition;
    this->spawnPosition->occupied = true; 
    this->worldInfo = worldInfo;
    this->Kill = 0;
    this->DamageDealt = 0;
    this->Death = 0;
    this->Respawn();
    this->BOT = bot;    //it's new
    this->server = server; // it's new
}
std::string Player::GetPlayerName()
{
    return this->PlayerName;
}
/*TSafeDirections Player::getSafeDirections(TVector real_speed)
{
    TSafeDirections safeDirections;
    safeDirections.Down = false;
    safeDirections.Down_Left = false;
    safeDirections.Down_Right = false;
    safeDirections.Left = false;
    safeDirections.None = false;
    safeDirections.Right = false;
    safeDirections.Up = false;
    safeDirections.Up_Left = false;
    safeDirections.Up_Right = false;
    TDirectionPair direction_pair[3][3] = {{{rbw::DIRECTION_UP,rbw::DIRECTION_LEFT},
                                            {rbw::DIRECTION_UP,rbw::DIRECTION_NODIRECTION},
                                            {rbw::DIRECTION_UP,rbw::DIRECTION_RIGHT}},
                                           {{rbw::DIRECTION_NODIRECTION,rbw::DIRECTION_LEFT},
                                            {rbw::DIRECTION_NODIRECTION,rbw::DIRECTION_NODIRECTION},
                                            {rbw::DIRECTION_NODIRECTION,rbw::DIRECTION_RIGHT}},
                                           {{rbw::DIRECTION_DOWN,rbw::DIRECTION_LEFT},
                                            {rbw::DIRECTION_DOWN,rbw::DIRECTION_NODIRECTION},
                                            {rbw::DIRECTION_DOWN,rbw::DIRECTION_RIGHT}}};
    EvadeFromTheRocket stells_system(this->server);
    std::vector<TDirectionPair> directions = stells_system.saveDirection(*this,real_speed);
    for(int i = 0; i < (int)directions.size(); i++)
        switch (directions[i])
        {
            case direction_pair[0][0]: safeDirections.Up_Left = true;
                break;
            case direction_pair[0][1]: safeDirections.Up = true;
                break;
            case direction_pair[0][2]: safeDirections.Up_Right = true;
                break;
            case direction_pair[1][0]: safeDirections.Left = true;
                break;
            case direction_pair[1][1]: safeDirections.None = true;
                break;
            case direction_pair[1][2]: safeDirections.Right = true;
                break;
            case direction_pair[2][0]: safeDirections.Down_Left = true;
                break;
            case direction_pair[2][1]: safeDirections.Down = true;
                break;
            case direction_pair[2][2]: safeDirections.Down_Right = true;
                break;
            default:
                break;
        }
    directions.clear();
    return safeDirections;
}*/
void Player::SetAdmissibleDirections(TSafeDirections &directions)   //it's new
{
    this->safe_directions = directions;
}
void Player::SetNewName(std::string name)   //it's new
{
    this->PlayerName = name;
}
void Player::Move(sf::Vector2i direction)
{
    sf::Vector2f p_new = sf::Vector2f(0.0f, 0.0f);
    /*
    if (direction == rbw::DIRECTION_NODIRECTION) return ;

    if (direction == rbw::DIRECTION_UP) p_new.y -= 1.0f;
    if (direction == rbw::DIRECTION_DOWN) p_new.y += 1.0f;
    if (direction == rbw::DIRECTION_LEFT) p_new.x -= 1.0f;
    if (direction == rbw::DIRECTION_RIGHT) p_new.x += 1.0f;
    */

    p_new.x = rbw::sgn(direction.x);
    p_new.y = rbw::sgn(direction.y);

    if (p_new == sf::Vector2f(0.f, 0.f)) return ;

    //float mSpeedTimeFactor = 1000.0 / this->worldInfo->FPS;

    // X per frame
    // 60 FPS;
    // X*60 / 1000 per milisecond
    // new FPS = k;
    // so, (X*60/1000) * (1000/k) pixels per frame, k - time per frame

    //float velocity = mSpeedTimeFactor * rbw::GameParam::MAX_PLAYER_SPEED; // in pixels

    float len = sqrt(p_new.x * p_new.x + p_new.y * p_new.y);
    float velocity = this->worldInfo->ElapsedTime.asMilliseconds() * rbw::GameParam::MAX_PLAYER_SPEED;
    float koef = velocity / len;

    //std::cout << "koef: " << koef << std::endl;

    p_new.x *= koef; //* velocity;
    p_new.y *= koef;// * velocity;

    this->speed = sf::Vector2f(p_new.x , p_new.y);

    sf::Vector2f p0 = this->position;

    float new_x;// = p0.x + p_new.x;
    float new_y;// = p0.y + p_new.y;




    if (this->BOT == false){
        bool can_move_ver = true;
        bool can_move_hor = true;

        for (int i=0; i<(int) this->worldInfo->wallForPlayer.size(); i++){
            if (this->worldInfo->wallForPlayer[i].type == "rect"){
                {
                    new_x = p0.x + p_new.x;
                    new_y = p0.y;

                    bool temp;

                    int y1 = this->worldInfo->wallForPlayer[i].rect.top;
                    int x1 = this->worldInfo->wallForPlayer[i].rect.left;
                    int y2 = y1 + this->worldInfo->wallForPlayer[i].rect.height;
                    int x2 = x1 + this->worldInfo->wallForPlayer[i].rect.width;

                    temp = ( !((new_y < y2) && (new_y > y1) && (new_x < x2) && (new_x > x1)) );
                    can_move_hor = can_move_hor && temp;
                }

                {
                    new_x = p0.x;
                    new_y = p0.y + p_new.y;

                    bool temp;

                    int y1 = this->worldInfo->wallForPlayer[i].rect.top;
                    int x1 = this->worldInfo->wallForPlayer[i].rect.left;
                    int y2 = y1 + this->worldInfo->wallForPlayer[i].rect.height;
                    int x2 = x1 + this->worldInfo->wallForPlayer[i].rect.width;

                    temp = ( !((new_y < y2) && (new_y > y1) && (new_x < x2) && (new_x > x1)) );
                    can_move_ver = can_move_ver && temp;
                }
            }
            else if (this->worldInfo->wallForPlayer[i].type == "polygon")
            {
                float new_x;// = p0.x + p_new.x;
                float new_y;// = p0.y + p_new.y;

                bool can_move = true;
                new_x = p0.x + p_new.x;
                new_y = p0.y + p_new.y;

                std::vector< sf::Vector2f > points;

                for (int k=0; k<(int) this->worldInfo->wallForPlayer[i].points.size(); k++){
                    points.push_back(sf::Vector2f(this->worldInfo->wallForPlayer[i].points[k].x,
                                                  this->worldInfo->wallForPlayer[i].points[k].y));
                }

                sf::Rect< int > tmp = this->worldInfo->wallForPlayer[i].rect;

                //has collision with i-th rectangle
                rbw::intPolygon * tmpPolygon;
                if (this->worldInfo->wallForPlayer[i].type == "rect") tmpPolygon = new rbw::intPolygon(tmp);
                if (this->worldInfo->wallForPlayer[i].type == "polygon") tmpPolygon = new rbw::intPolygon(points);
                else tmpPolygon = new rbw::intPolygon(tmp);

                sf::Vector2f pAns = tmpPolygon->CheckIntersect(this->position, sf::Vector2f(new_x, new_y));
                can_move_ver = can_move_ver && (pAns == sf::Vector2f(-1.0f, -1.0f));
                can_move_hor = can_move_hor && (pAns == sf::Vector2f(-1.0f, -1.0f));
                delete tmpPolygon;
            }
        }
        if (!(can_move_hor || can_move_ver)) return ;

        new_x = p0.x;
        new_y = p0.y;
        if (can_move_hor) new_x += p_new.x;
        if (can_move_ver) new_y += p_new.y;
        this->SetPosition(sf::Vector2f(new_x, new_y));
    }



    /* IHOR TYMCHYSHYN
     * I
     * HATE
     * YOUR CODE
     */

    else if (this->BOT == true)
    {
        //--------------------------it's new----------------------------------//
        //--------------------it's new------------------//
        bool can_move_diagonal;
        sf::Vector2f p_new_tmp = p_new;
        while( ( (p_new_tmp.x <= -2)||(p_new_tmp.x >= 2) ) && ( (p_new_tmp.y <= -2)||(p_new_tmp.y >= 2) ) /*&&*/ )
        {
            can_move_diagonal = true;
            new_x = p0.x + p_new_tmp.x;
            new_y = p0.y + p_new_tmp.y;
            for(int i = 0; i < (int)this->worldInfo->wallForBots.size()/*it's new*/; i++)
            {
                bool temp;

                int y1 = this->worldInfo->wallForBots[i].rect.top;
                int x1 = this->worldInfo->wallForBots[i].rect.left;
                int y2 = y1 + this->worldInfo->wallForBots[i].rect.height;
                int x2 = x1 + this->worldInfo->wallForBots[i].rect.width;

                temp = ( !((new_y <= y2) && (new_y >= y1) && (new_x <= x2) && (new_x >= x1)) );
                can_move_diagonal = can_move_diagonal && temp;
            }

            if(can_move_diagonal)
            {
                if(this->bot())
                if( ((p_new_tmp.x < 0) && (p_new_tmp.y < 0) && (!this->safe_directions.Up_Left)) ||
                        ((p_new_tmp.x == 0) && (p_new_tmp.y < 0) && (!this->safe_directions.Up)) ||
                        ((p_new_tmp.x > 0) && (p_new_tmp.y < 0) && (!this->safe_directions.Up_Right)) ||
                        ((p_new_tmp.x < 0) && (p_new_tmp.y == 0) && (!this->safe_directions.Left)) ||
                        /*((p_new_tmp.x == 0) && (p_new_tmp.y == 0) && (!this->safe_directions.None)) ||*/
                        ((p_new_tmp.x > 0) && (p_new_tmp.y == 0) && (!this->safe_directions.Right)) ||
                        ((p_new_tmp.x < 0) && (p_new_tmp.y > 0) && (!this->safe_directions.Down_Left)) ||
                        ((p_new_tmp.x == 0) && (p_new_tmp.y > 0) && (!this->safe_directions.Down)) ||
                        ((p_new_tmp.x > 0) && (p_new_tmp.y > 0) && (!this->safe_directions.Down_Right)) )
                    return;
                new_x = p0.x + p_new_tmp.x;
                new_y = p0.y + p_new_tmp.y;
                this->SetPosition(sf::Vector2f(new_x, new_y));
                return;
            }

            p_new_tmp.x += p_new_tmp.x < 0 ? 1 : -1;
            p_new_tmp.y += p_new_tmp.y < 0 ? 1 : -1;
        }
        //--------------------it's new------------------//

        bool can_move_hor = true;
        new_x = p0.x + p_new.x;
        new_y = p0.y;
        for (int i = 0; i < (int)this->worldInfo->wallForBots.size()/*it's new*/; i++)//it's new
        {
            bool temp;

            int y1 = this->worldInfo->wallForBots[i].rect.top;
            int x1 = this->worldInfo->wallForBots[i].rect.left;
            int y2 = y1 + this->worldInfo->wallForBots[i].rect.height;
            int x2 = x1 + this->worldInfo->wallForBots[i].rect.width;

            temp = ( !((new_y <= y2) && (new_y >= y1) && (new_x <= x2) && (new_x >= x1)) );
                //it's new ^(Was: <, >, ...; Is: <=, >=, ...)
            can_move_hor = can_move_hor && temp;
        }

        bool can_move_ver = true;
        new_x = p0.x;
        new_y = p0.y + p_new.y;
        for (int i=0; i<(int)this->worldInfo->wallForBots.size()/*it's new*/; i++){
            bool temp;

            int y1 = this->worldInfo->wallForBots[i].rect.top;
            int x1 = this->worldInfo->wallForBots[i].rect.left;
            int y2 = y1 + this->worldInfo->wallForBots[i].rect.height;
            int x2 = x1 + this->worldInfo->wallForBots[i].rect.width;

            temp = ( !((new_y <= y2) && (new_y >= y1) && (new_x <= x2) && (new_x >= x1)) );
                //it's new ^(Was: <, >, ...; Is: <=, >=, ...)
            can_move_ver = can_move_ver && temp;
        }

        if (!(can_move_hor || can_move_ver)) return ;

        //----------------it's new----------------//
        //if( (!can_move_diagonal)&&can_move_hor&&can_move_ver )
        //    return;
        //----------------it's new----------------//

        new_x = p0.x;
        new_y = p0.y;
        if (can_move_hor) new_x += p_new.x;
                else
            p_new.x = 0;
        if (can_move_ver) new_y += p_new.y;
                else
            p_new.y = 0;

        if(this->bot())
        if( ((p_new.x < 0) && (p_new.y < 0) && (!this->safe_directions.Up_Left)) ||
                ((p_new.x == 0) && (p_new.y < 0) && (!this->safe_directions.Up)) ||
                ((p_new.x > 0) && (p_new.y < 0) && (!this->safe_directions.Up_Right)) ||
                ((p_new.x < 0) && (p_new.y == 0) && (!this->safe_directions.Left)) ||
                /*((p_new.x == 0) && (p_new.y == 0) && (!this->safe_directions.None)) ||*/
                ((p_new.x > 0) && (p_new.y == 0) && (!this->safe_directions.Right)) ||
                ((p_new.x < 0) && (p_new.y > 0) && (!this->safe_directions.Down_Left)) ||
                ((p_new.x == 0) && (p_new.y > 0) && (!this->safe_directions.Down)) ||
                ((p_new.x > 0) && (p_new.y > 0) && (!this->safe_directions.Down_Right)) )
            return;
        this->SetPosition(sf::Vector2f(new_x, new_y));

        //---------------------------------------------------------------------//
    }














//    bool can_move_hor = true;
//    new_x = p0.x + p_new.x;
//    new_y = p0.y;
//    for (int i=0; i< (int)this->worldInfo->wallForPlayer.size(); i++){
//
//        /* Honestly, i have no fucking idea why this fucking algorithm
//         * doesnt work. I mean, rarely we can see some bugs on the corner
//         */
//
//        /*
//        std::vector< sf::Vector2f > points;
//
//        for (int k=0; k<(int) this->worldInfo->wallForPlayer[i].points.size(); k++){
//            points.push_back(sf::Vector2f(this->worldInfo->wallForPlayer[i].points[k].x,
//                                          this->worldInfo->wallForPlayer[i].points[k].y));
//        }
//
//        sf::Rect< int > tmp = this->worldInfo->wallForPlayer[i].rect;
//
//        //has collision with i-th rectangle
//        rbw::intPolygon * tmpPolygon;
//        if (this->worldInfo->wallForPlayer[i].type == "rect") tmpPolygon = new rbw::intPolygon(tmp);
//        if (this->worldInfo->wallForPlayer[i].type == "polygon") tmpPolygon = new rbw::intPolygon(points);
//        else tmpPolygon = new rbw::intPolygon(tmp);
//
//        sf::Vector2f pAns = tmpPolygon->CheckIntersect(this->position, sf::Vector2f(new_x, new_y));
//        can_move_hor = can_move_hor && (pAns == sf::Vector2f(-1.0f, -1.0f));
//        delete tmpPolygon;
//        */
//
//        bool temp;
//
//        int y1 = this->worldInfo->wallForPlayer[i].rect.top;
//        int x1 = this->worldInfo->wallForPlayer[i].rect.left;
//        int y2 = y1 + this->worldInfo->wallForPlayer[i].rect.height;
//        int x2 = x1 + this->worldInfo->wallForPlayer[i].rect.width;
//
//        temp = ( !((new_y < y2) && (new_y > y1) && (new_x < x2) && (new_x > x1)) );
//        can_move_hor = can_move_hor && temp;
//    }
//
//    bool can_move_ver = true;
//    new_x = p0.x;
//    new_y = p0.y + p_new.y;
//    for (int i=0; i< (int)this->worldInfo->wallForPlayer.size(); i++){
//
//        /* Honestly, i have no fucking idea why this fucking algorithm
//         * doesnt work. I mean, rarely we can see some bugs on the corner
//         */
//
//        /*
//        std::vector< sf::Vector2f > points;
//
//        for (int k=0; k<(int) this->worldInfo->wallForPlayer[i].points.size(); k++){
//            points.push_back(sf::Vector2f(this->worldInfo->wallForPlayer[i].points[k].x,
//                                          this->worldInfo->wallForPlayer[i].points[k].y));
//        }
//
//        sf::Rect< int > tmp = this->worldInfo->wallForPlayer[i].rect;
//
//        //has collision with i-th rectangle
//        rbw::intPolygon * tmpPolygon;
//        if (this->worldInfo->wallForPlayer[i].type == "rect") tmpPolygon = new rbw::intPolygon(tmp);
//        if (this->worldInfo->wallForPlayer[i].type == "polygon") tmpPolygon = new rbw::intPolygon(points);
//        else tmpPolygon = new rbw::intPolygon(tmp);
//
//        sf::Vector2f pAns = tmpPolygon->CheckIntersect(this->position, sf::Vector2f(new_x, new_y));
//        can_move_ver = can_move_ver && (pAns == sf::Vector2f(-1.0f, -1.0f));
//        delete tmpPolygon;
//        */
//        bool temp;
//
//        int y1 = this->worldInfo->wallForPlayer[i].rect.top;
//        int x1 = this->worldInfo->wallForPlayer[i].rect.left;
//        int y2 = y1 + this->worldInfo->wallForPlayer[i].rect.height;
//        int x2 = x1 + this->worldInfo->wallForPlayer[i].rect.width;
//
//        temp = ( !((new_y < y2) && (new_y > y1) && (new_x < x2) && (new_x > x1)) );
//        can_move_ver = can_move_ver && temp;
//    }
//
//    if (!(can_move_hor || can_move_ver)) return ;
//
//    new_x = p0.x;
//    new_y = p0.y;
//    if (can_move_hor) new_x += p_new.x;
//    if (can_move_ver) new_y += p_new.y;
//    this->SetPosition(sf::Vector2f(new_x, new_y));
}
bool Player::isAlive()
{
    return this->alive;
}
int Player::GetHealth()
{
    return this->health;
}
bool Player::bot()
{
    return this->BOT;
}

bool Player::Hit(int damage)
{
    if (this->health > 0){
        this->health -= damage;
        if (this->health <= 0){
            this->alive = false;

            rbw::GraphicObject newExplosion;
            newExplosion.type = rbw::Graphic::PLAYER_EXPLOSION;
            newExplosion.Name = this->PlayerName;
            newExplosion.x = this->position.x;
            newExplosion.y = this->position.y;
            this->worldInfo->Explosions.push_back(newExplosion);

            this->position = sf::Vector2f(-10000.0f, -10000.0f);
            return true;
        }
        return false;
    }
    return false;
}
void Player::Respawn()
{
    this->alive = true;
    this->health = rbw::GameParam::INITIAL_HEALTH_POINT;
    this->position.x = this->spawnPosition->x;
    this->position.y = this->spawnPosition->y;
    this->HomingMissilesLeft = rbw::GameParam::MAX_NUMBER_OF_HOMING_MISSILES_ALLOWED;
    this->BouncingBombsLeft = rbw::GameParam::MAX_NUMBER_OF_BOUNCING_BOMBS_ALLOWED;
    this->GrenadesLeft = rbw::GameParam::MAX_NUMBER_OF_GRENADES_ALLOWED;
}

rbw::Team Player::GetTeam()
{
    return this->team;
}

/* Class Homing Missile:
 * A smart rocket that can independently follow its target
 */

HomingMissile::HomingMissile(Player *owner, Player *target, rbw::HM_ChainElement * location, rbw::WorldInformation * worldInfo)
{
    this->owner = owner;
    this->target = target;
    this->position = owner->GetPosition();
    this->type = rbw::TYPE_HOMING_MISSILE;
    this->LocationInChain = location;
    this->worldInfo = worldInfo;
    this->haveToBeDestroyed = false;
    this->firstCollideWithOwner = true;
    this->owner->HomingMissilesLeft--;
}
rbw::HomingMissile::~HomingMissile()
{
    rbw::GraphicObject newExplosion;
    newExplosion.type = rbw::Graphic::HOMINGMISSILE_EXPLOSION;
    newExplosion.Name = this->owner->GetPlayerName();
    newExplosion.x = this->position.x;
    newExplosion.y = this->position.y;
    this->worldInfo->Explosions.push_back(newExplosion);

    if (rbw::GameParam::HOMING_MISSILE_RECHARGE_AFTER_EXPLOSING)
        this->owner->HomingMissilesLeft++;
}
rbw::Player * HomingMissile::GetOwner()
{
    return this->owner;
}
rbw::Player * HomingMissile::GetTarget()
{
    return this->target;
}
void HomingMissile::SimulateNextStep()
{
    rbw::Player * victim = NULL;

    sf::Vector2f targetPosition = this->target->GetPosition();
    sf::Vector2f new_speedvec;
    new_speedvec.x = targetPosition.x - this->position.x;
    new_speedvec.y = targetPosition.y - this->position.y;
    //std::cout << "target name: " << this->target->GetPlayerName() << " coord: " << targetPosition.x << " " << targetPosition.y << std::endl;
    //std::cout << "position: " << this->position.x << " " <<this->position.y << std::endl;


    double len = sqrt(new_speedvec.x * new_speedvec.x + new_speedvec.y * new_speedvec.y);  
    float velocity = this->worldInfo->ElapsedTime.asMilliseconds() * rbw::GameParam::MAX_HOMING_MISSILE_SPEED;

    double koef = velocity / len;
    //std::cout << "koef: " << koef << std::endl;
    new_speedvec.x = koef * new_speedvec.x;
    new_speedvec.y = koef * new_speedvec.y;

    //std::cout << "new speed: " << new_speedvec.x << " " << new_speedvec.y << std::endl;
    this->speed = sf::Vector2f(new_speedvec.x, new_speedvec.y);

    sf::Vector2f explosionPoint = this->getExplosionPoint(&victim);

    if (explosionPoint != sf::Vector2f(-1.0,-1.0))
    {        
        if (victim != NULL){
            bool killed = victim->Hit(rbw::GameParam::HOMING_MISSILE_DAMAGE);
            this->owner->DamageDealt += rbw::GameParam::HOMING_MISSILE_DAMAGE;
            if (killed){
                this->owner->Kill++;
                victim->Death++;
                std::string newEventMsg(this->owner->GetPlayerName() + " has killed " + victim->GetPlayerName());
                this->worldInfo->WorldEvents.push_back(newEventMsg);
            }
        }
        // here we have to destroy this current chain element (currElement)
        rbw::HM_ChainElement * previousEl = this->LocationInChain->prev;
        rbw::HM_ChainElement * nextEl = this->LocationInChain->next;
        if (previousEl != NULL)
            previousEl->next = nextEl;
        if (nextEl != NULL)
            nextEl->prev = previousEl;
        // if current element is the first rocket in chain
        if (this->worldInfo->homingMissiles.FirstInChain == this->LocationInChain){
            this->worldInfo->homingMissiles.FirstInChain = this->LocationInChain->next;
        }
        // if current element is the last rocket in chain
        if (this->worldInfo->homingMissiles.LastInChain == this->LocationInChain){
            this->worldInfo->homingMissiles.LastInChain = this->LocationInChain->prev;
        }
        delete this->LocationInChain;
        this->haveToBeDestroyed = true; // will be destroyed in next step

        this->~HomingMissile();
        return ;
    }

    this->position.x += this->speed.x;
    this->position.y += this->speed.y;
}
sf::Vector2f HomingMissile::getExplosionPoint(Player **victim)
{
    *victim = NULL;

    bool isCollidingWithOwner = false;
    for (int i=0; i< (int)this->worldInfo->Players.size(); i++){
        rbw::Player * tmpPlayer = this->worldInfo->Players[i];

        sf::Vector2f tmpPosition = tmpPlayer->GetPosition();
        sf::Vector2f vec_a;
        vec_a.x = tmpPosition.x - this->position.x;
        vec_a.y = tmpPosition.y - this->position.y;
        float distance = vec_a.x * vec_a.x + vec_a.y * vec_a.y;
        float minDistance = rbw::GameParam::PLAYER_HITBOX_RADIUS + rbw::GameParam::HOMING_MISSILE_HITBOX_RADIUS;
        if (distance < minDistance * minDistance){
            if (tmpPlayer == this->owner){
                isCollidingWithOwner = true;
                if (this->firstCollideWithOwner) continue;
            }
            *victim = tmpPlayer;
            std::cout << "victim: " << (*victim)->GetPlayerName() << std::endl;
            return this->position;
        }
    }
    if (!isCollidingWithOwner)
        this->firstCollideWithOwner = false;

    sf::Vector2f pAns = sf::Vector2f(-1.0f,-1.0f);

    for (int i=0; i< (int)this->worldInfo->wallForRocket.size(); i++){
        std::vector< sf::Vector2f > points;

        for (int k=0; k<(int) this->worldInfo->wallForRocket[i].points.size(); k++){
            points.push_back(sf::Vector2f(this->worldInfo->wallForRocket[i].points[k].x,
                                          this->worldInfo->wallForRocket[i].points[k].y));
        }

        sf::Rect< int > tmp = this->worldInfo->wallForRocket[i].rect;

        //has collision with i-th rectangle
        rbw::intPolygon * tmpPolygon;
        if (this->worldInfo->wallForRocket[i].type == "rect") tmpPolygon = new rbw::intPolygon(tmp);
        if (this->worldInfo->wallForRocket[i].type == "polygon") tmpPolygon = new rbw::intPolygon(points);
        else tmpPolygon = new rbw::intPolygon(tmp);

        pAns = tmpPolygon->CheckIntersect(this->position,
                                         sf::Vector2f(this->position.x + this->speed.x, this->position.y + this->speed.y));
        delete tmpPolygon;
        if (pAns != sf::Vector2f(-1.0f, -1.0f))
            return pAns;
    }
    // now we found that pAns - first point of intersection
    *victim = NULL;
    return pAns;
}

/* Class Bouncing Bomb
 * Very funny bomb that can bounce finite number of times
 */

BouncingBomb::BouncingBomb(Player *owner, rbw::BB_ChainElement * location, WorldInformation *worldInfo)
{
    this->BounceCount = 0;
    this->owner = owner;
    this->position = owner->GetPosition();
    this->type = rbw::TYPE_BOUNCING_BOMB;
    this->LocationInChain = location;
    this->worldInfo = worldInfo;
    this->haveToBeDestroyed = false;    
    this->firstCollideWithOwner = true;
    this->owner->BouncingBombsLeft--;
}
BouncingBomb::~BouncingBomb()
{
    rbw::GraphicObject newExplosion;
    newExplosion.type = rbw::Graphic::BOUNCINGBOMB_EXPLOSION;
    newExplosion.Name = this->owner->GetPlayerName();
    newExplosion.x = this->position.x;
    newExplosion.y = this->position.y;
    this->worldInfo->Explosions.push_back(newExplosion);

    if (rbw::GameParam::BOUNCING_BOMB_RECHARGE_AFTER_EXPLOSING)
        this->owner->BouncingBombsLeft++;
}
rbw::Player * BouncingBomb::GetOwner()
{
    return this->owner;
}
void BouncingBomb::SimulateNextStep()
{
    rbw::BB_ChainElement * currElement = this->LocationInChain;
    rbw::Player * victim = NULL;

    sf::Vector2f explosionPoint;
    if (this->BounceCount < rbw::GameParam::MAX_BOUNCE_NUMBER)
        explosionPoint = this->getCollisionWithPlayers(&victim);
    else
        explosionPoint = this->getExplosionPoint(&victim);
    if (explosionPoint != sf::Vector2f(-1.0f,-1.0f))
    {        
        if (victim != NULL){
            bool killed = victim->Hit(rbw::GameParam::BOUNCING_BOMB_DAMAGE);
            this->owner->DamageDealt += rbw::GameParam::BOUNCING_BOMB_DAMAGE;
            if (killed){
                this->owner->Kill++;
                victim->Death++;
                std::string newEventMsg(this->owner->GetPlayerName() + " has killed " + victim->GetPlayerName());
                this->worldInfo->WorldEvents.push_back(newEventMsg);

            }
        }
        rbw::BB_ChainElement * previousEl = currElement->prev;
        rbw::BB_ChainElement * nextEl = currElement->next;
        if (previousEl != NULL)
            previousEl->next = nextEl;
        if (nextEl != NULL)
            nextEl->prev = previousEl;
        // if current element is the first rocket in chain
        if (this->worldInfo->bouncingBombs.FirstInChain == currElement){
            this->worldInfo->bouncingBombs.FirstInChain = currElement->next;
        }
        // if current element is the last rocket in chain
        if (this->worldInfo->bouncingBombs.LastInChain == currElement){
            this->worldInfo->bouncingBombs.LastInChain = currElement->prev;
        }
        this->haveToBeDestroyed = true;
        delete this->LocationInChain;
        this->~BouncingBomb();
        return ;
    }    

    sf::Vector2f destPoint(this->position.x + this->speed.x,
                           this->position.y + this->speed.y);
    sf::Vector2f pointOfBounce = this->getReflexVector();

    if (pointOfBounce != sf::Vector2f(-1.0f,-1.0f)){
        ++this->BounceCount;
        destPoint = sf::Vector2f(pointOfBounce.x + rbw::sgn(this->speed.x),
                                 pointOfBounce.y + rbw::sgn(this->speed.y));        
    }
    this->position = destPoint;
}
sf::Vector2f BouncingBomb::getCollisionWithPlayers(Player **victim)
{
    std::cout << ((this->firstCollideWithOwner)? "TRUE":"FALSE") << std::endl;
    *victim = NULL;
    bool isCollidingWithOwner = false;
    for (int i=0; i< (int)this->worldInfo->Players.size(); i++){
        rbw::Player * tmpPlayer = this->worldInfo->Players[i];

        sf::Vector2f dv( tmpPlayer->GetPosition().x - this->position.x,
                         tmpPlayer->GetPosition().y - this->position.y );
        float distance = dv.x*dv.x + dv.y*dv.y;
        float minDistance = rbw::GameParam::PLAYER_HITBOX_RADIUS + rbw::GameParam::BOUNCING_BOMB_HITBOX_RADIUS;
        if (distance < minDistance*minDistance){
            //isCollidingWithOwner = (isCollidingWithOwner || (tmpPlayer == this->owner));
            if (tmpPlayer == this->owner){
                isCollidingWithOwner = true;
                if (this->firstCollideWithOwner) continue;
            }

            *victim = tmpPlayer;
            return this->position;
        }
    }
    if (!isCollidingWithOwner) this->firstCollideWithOwner = false;
    return sf::Vector2f(-1,-1);
}

sf::Vector2f BouncingBomb::getExplosionPoint(Player **victim)
{
    *victim = NULL;

    sf::Vector2f pAns = sf::Vector2f(-1.0f,-1.0f);

    pAns = this->getCollisionWithPlayers(victim);
    if (pAns != sf::Vector2f(-1.0f, -1.0f)){
        return pAns;
    }

    for (int i=0; i< (int)this->worldInfo->wallForRocket.size(); i++){
        std::vector< sf::Vector2f > points;

        for (int k=0; k<(int) this->worldInfo->wallForRocket[i].points.size(); k++){
            points.push_back(sf::Vector2f(this->worldInfo->wallForRocket[i].points[k].x,
                                          this->worldInfo->wallForRocket[i].points[k].y));
        }

        sf::Rect< int > tmp = this->worldInfo->wallForRocket[i].rect;

        //has collision with i-th rectangle
        rbw::intPolygon * tmpPolygon;
        if (this->worldInfo->wallForRocket[i].type == "rect") tmpPolygon = new rbw::intPolygon(tmp);
        if (this->worldInfo->wallForRocket[i].type == "polygon") tmpPolygon = new rbw::intPolygon(points);
        else tmpPolygon = new rbw::intPolygon(tmp);

        pAns = tmpPolygon->CheckIntersect(this->position, sf::Vector2f(this->position.x + this->speed.x,
                                                                      this->position.y + this->speed.y));
        delete tmpPolygon;
        if (pAns != sf::Vector2f(-1, -1))
            return pAns;
    }
    // now we found that pAns - first point of intersection
    *victim = NULL;
    return pAns;
}

sf::Vector2f BouncingBomb::getReflexVector()
{
    sf::Vector2f pAns = sf::Vector2f(-1.0f,-1.0f);

    for (int i=0; i< (int)this->worldInfo->wallForRocket.size(); i++){
        std::vector< sf::Vector2f > points;

        for (int k=0; k<(int) this->worldInfo->wallForRocket[i].points.size(); k++){
            points.push_back(sf::Vector2f(this->worldInfo->wallForRocket[i].points[k].x,
                                          this->worldInfo->wallForRocket[i].points[k].y));
        }

        sf::Rect< int > tmp = this->worldInfo->wallForRocket[i].rect;

        //has collision with i-th rectangle
        rbw::intPolygon * tmpPolygon;
        if (this->worldInfo->wallForRocket[i].type == "rect") tmpPolygon = new rbw::intPolygon(tmp);
        if (this->worldInfo->wallForRocket[i].type == "polygon") tmpPolygon = new rbw::intPolygon(points);
        else tmpPolygon = new rbw::intPolygon(tmp);

        sf::Vector2f tmpAns = tmpPolygon->ReflexSpeed(this->position, &(this->speed));
        if (tmpAns != sf::Vector2f(-1.0f,-1.0f)){
            pAns = tmpAns;
        }
    }
    return pAns;
}



Grenade::Grenade(Player *owner, G_ChainElement *location, WorldInformation *worldInfo, sf::Vector2f DestPoint)
{
    this->owner = owner;
    this->worldInfo = worldInfo;
    this->position = this->owner->GetPosition();
    this->StartingPoint = this->position;
    this->DestinationPoint = DestPoint;
    this->LocationInChain = location;

    if (fabs(this->StartingPoint.x - this->DestinationPoint.x) < fEPS){
        this->DestinationPoint.x += 1.0f;
    }

    /* Parabolic Equation:
     * DestinationPoint.x = dx, StartingPoint.x = sx;
     * (x - dx)(x - sx) = 0;
     * x^2 - (sx + dx)*x + dx*sx = 0;
     * ver = (sx + dx)/2
     * height = abs( (ver - dx)(ver - sx) );
     * koef = GRENADE_HEIGHT / height;
     * so, the final equation:
     *      koef*x^2 - koef*(sx + dx)*x + koef*dx*sx;
     */
    float dx = DestinationPoint.x - StartingPoint.x;
    float sx = 0.0f;

    float ver = (sx + dx)/2.;
    float height = fabs((ver - dx)*(ver - sx));
    float koef = rbw::GameParam::GRENADE_HEIGHT / height;

    this->Equation.x = koef;
    this->Equation.y = - koef * (sx + dx);
    this->Equation.z = koef*sx*dx;

    float a = (float) this->DestinationPoint.x - this->StartingPoint.x;
    float b = (float) this->DestinationPoint.y - this->StartingPoint.y;

    double len = sqrt(a*a + b*b);
    float velocity = len / (float) rbw::GameParam::GRENADE_TIME_TO_LIVE;

    velocity *= this->worldInfo->ElapsedTime.asMilliseconds();

    double koefficient = velocity / len;

    //std::cout << "speedvec " << a << " " << b << std::endl;
    a = koefficient * a;
    b = koefficient * b;

    this->speed = sf::Vector2f(a,b);
    this->ProjectionToGround = this->position;
    this->zoom_coefficient = 1.0f;

    this->owner->GrenadesLeft--;
}
Grenade::~Grenade()
{
    rbw::GraphicObject newExplosion;
    newExplosion.type = rbw::Graphic::GRENADE_EXPLOSION;
    newExplosion.Name = this->owner->GetPlayerName();

    float angle;
    float rad_angle;

    for (int k = 0; k < 10; k++){
        angle = k * 360.0f/10.0f;
        rad_angle = (angle * 1.0f /180) * M_PI;
        newExplosion.x = this->position.x + cos(rad_angle) * (rbw::GameParam::GRENADE_RADIUS_OF_EFFECT-5);
        newExplosion.y = this->position.y + sin(rad_angle) * (rbw::GameParam::GRENADE_RADIUS_OF_EFFECT-5);
        this->worldInfo->Explosions.push_back(newExplosion);
    }

    if (rbw::GameParam::GRENADE_RECHARGE_AFTER_EXPLOSING)
        this->owner->GrenadesLeft++;
}

bool Grenade::HaveToBeDestroyed()
{
    return this->haveToBeDestroyed;
}
float Grenade::GetZoom()
{
    return this->zoom_coefficient;
}

void Grenade::SimulateNextStep()
{

    float _a = this->DestinationPoint.x - this->ProjectionToGround.x;
    float _b = this->DestinationPoint.y - this->ProjectionToGround.y;

    float distanceToTarget = (_a*_a) + (_b*_b);
    float speedlen = this->speed.x * this->speed.x + this->speed.y * this->speed.y;

    if (distanceToTarget < speedlen){
        this->ProjectionToGround = this->DestinationPoint;
        this->position = this->DestinationPoint;
        this->zoom_coefficient = 1.0;

        for (int i=0; i< (int)this->worldInfo->Players.size(); i++){
            rbw::Player * tmpPlayer = this->worldInfo->Players[i];
            sf::Vector2f pos = tmpPlayer->GetPosition();
            sf::Vector2f dis (pos.x - this->position.x, pos.y - this->position.y);
            float dis_squared = (dis.x * dis.x) + (dis.y * dis.y);
            if (dis_squared < rbw::GameParam::GRENADE_RADIUS_OF_EFFECT_SQUARED + rbw::fEPS){
                bool killed = tmpPlayer->Hit(rbw::GameParam::GRENADE_DAMAGE);
                this->owner->DamageDealt += rbw::GameParam::GRENADE_DAMAGE;
                if (killed){
                    this->owner->Kill++;
                    tmpPlayer->Death++;
                    std::string newEventMsg(this->owner->GetPlayerName() + " has killed " + tmpPlayer->GetPlayerName());
                    this->worldInfo->WorldEvents.push_back(newEventMsg);
                }
            }
        }

        rbw::G_ChainElement * currElement = this->LocationInChain;
        rbw::G_ChainElement * previousEl = currElement->prev;
        rbw::G_ChainElement * nextEl = currElement->next;
        if (previousEl != NULL)
            previousEl->next = nextEl;
        if (nextEl != NULL)
            nextEl->prev = previousEl;
        // if current element is the first rocket in chain
        if (this->worldInfo->Grenades.FirstInChain == currElement){
            this->worldInfo->Grenades.FirstInChain = currElement->next;
        }
        // if current element is the last rocket in chain
        if (this->worldInfo->Grenades.LastInChain == currElement){
            this->worldInfo->Grenades.LastInChain = currElement->prev;
        }
        this->haveToBeDestroyed = true;
        delete this->LocationInChain;
        this->~Grenade();
        return ;
    }
    else
    {
        this->ProjectionToGround.x += this->speed.x;
        this->ProjectionToGround.y += this->speed.y;

        float tx = this->position.x - this->StartingPoint.x;
        float ty = this->Equation.x * tx * tx + this->Equation.y * tx + this->Equation.z;

        this->position.x = this->ProjectionToGround.x;
        this->position.y = this->ProjectionToGround.y + ty;

        this->zoom_coefficient = 1.0 + fabs(ty/rbw::GameParam::GRENADE_HEIGHT);
    }

}




/* Class World Simulator
 * It will do everything you want :D
 */
WorldSimulator::WorldSimulator()
{
    this->worldInfo.Players.clear();
    this->worldInfo.spawnPositions.clear();

    this->worldInfo.homingMissiles.FirstInChain = NULL;
    this->worldInfo.bouncingBombs.FirstInChain = NULL;

    this->worldInfo.homingMissiles.LastInChain = NULL;
    this->worldInfo.bouncingBombs.LastInChain = NULL;

    this->worldInfo.Grenades.FirstInChain = NULL;
    this->worldInfo.Grenades.LastInChain = NULL;       
}
rbw::WorldInformation * const WorldSimulator::getWorldInfo()
{
    return &(this->worldInfo);
}

void WorldSimulator::Init(Level *level, float FPS)
{
    this->worldInfo.level = level;
    this->worldInfo.FPS = FPS;
    this->worldInfo.wallForPlayer = level->GetObjects("wall");
    this->worldInfo.wallForRocket = level->GetObjects("wrocket");
    this->worldInfo.wallForBots = level->GetObjects("wbots");
    std::vector< Object > spawn = level->GetObjects("spawn");

    for (int i=0; i< (int)spawn.size(); i++){
        rbw::spawnPos * newpos = new rbw::spawnPos;
        newpos->occupied = false;
        if (spawn[i].rect.width == 0)
            newpos->team = rbw::TEAM_BLACK;
        else if (spawn[i].rect.width == 1)
            newpos->team = rbw::TEAM_WHITE;
        newpos->x = spawn[i].rect.left;
        newpos->y = spawn[i].rect.top;
        this->worldInfo.spawnPositions.push_back(newpos);
    }
}
bool WorldSimulator::AddPlayer(std::string PlayerName, Team team, bool isBot)
{
    rbw::spawnPos * PlayerSpawnPosition = NULL;
    for (int i=0; i< (int)this->worldInfo.spawnPositions.size(); i++){
        PlayerSpawnPosition = this->worldInfo.spawnPositions[i];
        if ((PlayerSpawnPosition->occupied == false) &&
                (PlayerSpawnPosition->team == team))
            break;
    }
    if ((PlayerSpawnPosition->occupied == true) || (PlayerSpawnPosition->team != team))
        return false; // no spawn position for this player
                      // maximum player number reached

    //----------it's  new-----------//
    rbw::Player * newPlayer = new Player(PlayerName, team, PlayerSpawnPosition, &(this->worldInfo), isBot, this);

    this->worldInfo.Players.push_back(newPlayer);
    std::string bot_or_player;
    if(isBot)
        bot_or_player = "bot";
            else
        bot_or_player = "player";
    this->worldInfo.WorldEvents.push_back(bot_or_player + " " + PlayerName + " has joined the game");
    //this->worldInfo.Players.push_back(newPlayer);
    //this->worldInfo.WorldEvents.push_back("player " + PlayerName + " has joined the game");
    //---------------------------//
//    rbw::Player * newPlayer = new Player(PlayerName, team, PlayerSpawnPosition, &(this->worldInfo), isBot);
//
//    this->worldInfo.Players.push_back(newPlayer);
//    this->worldInfo.WorldEvents.push_back(PlayerName + " has joined the game");
    return true;
}
bool WorldSimulator::AddBouncingBomb(std::string PlayerName, sf::Vector2i mousePosition)
{
    rbw::Player * owner = NULL;
    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        owner = worldInfo.Players[i];
        if (owner->GetPlayerName() == PlayerName) break;
    }
    if (owner == NULL) return false;
    if (owner->isAlive() == false) return false;
    if (owner->BouncingBombsLeft <= 0) return false;

    //std::cout << "bounce rocket owner: " << owner->GetPlayerName() << std::endl;

    // setting speed vector
    sf::Vector2f p0 = owner->GetPosition();
    float a = (float) mousePosition.x - p0.x;
    float b = (float) mousePosition.y - p0.y;

    double len = sqrt(a*a + b*b);

    //float mSpeedTimeFactor = 1000.0 / this->worldInfo.FPS;

    // X per frame
    // 60 FPS;
    // X*60 / 1000 per milisecond
    // new FPS = k;
    // so, (X*60/1000) * (1000/k) pixels per frame, k - time per frame

    //float velocity = mSpeedTimeFactor * rbw::MAX_HOMING_MISSILE_SPEED; // in pixels

    float velocity = this->worldInfo.ElapsedTime.asMilliseconds() * rbw::GameParam::MAX_BOUNCING_BOMB_SPEED;

    double koef = velocity / len;

    //std::cout << "speedvec " << a << " " << b << std::endl;
    a = koef * a;
    b = koef * b;
    // (a,b) - speed vector
    //std::cout << "modspeed: " << a << " " << b << std::endl;

    // creating new rocket
    rbw::BB_ChainElement * newElement = new rbw::BB_ChainElement;
    rbw::BouncingBomb * newBomb = new rbw::BouncingBomb(owner, newElement, &(this->worldInfo));
    newBomb->SetSpeed(sf::Vector2f(a,b));
    newBomb->SetPosition(p0);

    // new chain element    
    newElement->rocket = newBomb;

    if (this->worldInfo.bouncingBombs.LastInChain == NULL){
        this->worldInfo.bouncingBombs.FirstInChain = newElement;
        this->worldInfo.bouncingBombs.LastInChain = newElement;
        newElement->next = NULL;
        newElement->prev = NULL;
    }
    else {
        newElement->prev = this->worldInfo.bouncingBombs.LastInChain;
        newElement->next = NULL;
        this->worldInfo.bouncingBombs.LastInChain->next = newElement;
        this->worldInfo.bouncingBombs.LastInChain = newElement;
    }
    return true;
}
bool WorldSimulator::AddHomingMissile(std::string PlayerName, sf::Vector2i mousePosition)
{    
    Player * owner = NULL;
    std::cout << "fuck dat" << std::endl;
    std::cout << " #HM debug: " << &this->worldInfo << std::endl;
    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        owner = this->worldInfo.Players[i];
        if (owner->GetPlayerName() == PlayerName) break;
    }
    if (owner == NULL) return false;
    if (owner->isAlive() == false) return false;
    if (owner->HomingMissilesLeft <= 0) return false;

    std::cout << "Homing missile owner: " << owner->GetPlayerName() << std::endl;
    sf::Vector2f sp = owner->GetPosition();

    Player * target = NULL;
    double distance = 1000000000.0f;
    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        Player * tmpPlayer = this->worldInfo.Players[i];

        if ((tmpPlayer->GetPlayerName() == owner->GetPlayerName())||tmpPlayer->GetTeam() == owner->GetTeam())
            continue;

        sf::Vector2f p0 = tmpPlayer->GetPosition();
        float a, b;
        a = mousePosition.x - p0.x;
        b = mousePosition.y - p0.y;
        double newdistance = a*a + b*b;
/*
        bool is_visible = true;

        for (int i=0; i< (int)this->worldInfo.wallForRocket.size(); i++){
            sf::Rect< int > tmp = this->worldInfo.wallForRocket[i].rect;
            //has collision with i-th rectangle
            rbw::intPolygon tmpPolygon(tmp);

            sf::Vector2f pInt = tmpPolygon.CheckIntersect(owner->GetPosition(), tmpPlayer->GetPosition());
            if (pInt != sf::Vector2f(-1.0f, -1.0f)){
                is_visible = false;
                break;
            }
        }
        if (!is_visible) continue;
*/

        //if that bastard is closer
        if (newdistance < distance){
            distance = newdistance;
            target = tmpPlayer;
        }
    }
    if ((target == NULL)||(target->isAlive() == false)){
        std::cout << "no victim found";
        return false;
    }

    /*
    // Check if owner can see his victim
    for (int i=0; i< (int)this->worldInfo->wallForRocket.size(); i++){
        sf::Rect< int > tmp = this->worldInfo->wallForRocket[i].rect;
        //has collision with i-th rectangle
        rbw::intPolygon tmpPolygon(tmp);

        pAns = tmpPolygon.CheckIntersect(this->position,
                                         sf::Vector2f(this->position.x + this->speed.x, this->position.y + this->speed.y));
        if (pAns != sf::Vector2f(-1.0f, -1.0f))
            return pAns;
    }
    */

    std::cout << PlayerName << " has sent a gift to " << target->GetPlayerName() << std::endl;

    rbw::HM_ChainElement * newElement = new rbw::HM_ChainElement;
    rbw::HomingMissile * newRocket = new rbw::HomingMissile(owner, target, newElement, &(this->worldInfo));
    newRocket->SetPosition(sp);

    newElement->rocket = newRocket;

    if (this->worldInfo.homingMissiles.FirstInChain == NULL){
        this->worldInfo.homingMissiles.FirstInChain = newElement;
        this->worldInfo.homingMissiles.LastInChain = newElement;
        newElement->next = NULL;
        newElement->prev = NULL;
    }
    else {
        newElement->prev = this->worldInfo.homingMissiles.LastInChain;
        newElement->next = NULL;
        this->worldInfo.homingMissiles.LastInChain->next = newElement;
        this->worldInfo.homingMissiles.LastInChain = newElement;
    }
    return true;
}
bool WorldSimulator::AddGrenade(std::string PlayerName, sf::Vector2i mousePosition)
{
    Player * owner = NULL;
    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        owner = this->worldInfo.Players[i];
        if (owner->GetPlayerName() == PlayerName) break;
    }
    if (owner == NULL) return false;
    if (owner->isAlive() == false) return false;
    if (owner->GrenadesLeft <= 0) return false;

    //std::cout << "Grenade owner: " << owner->GetPlayerName() << std::endl;

    // creating new rocket
    rbw::G_ChainElement * newElement = new rbw::G_ChainElement;
    rbw::Grenade * newBomb = new rbw::Grenade(owner, newElement, &(this->worldInfo), sf::Vector2f(mousePosition.x,
                                                                                                  mousePosition.y));

    // new chain element
    newElement->rocket = newBomb;

    if (this->worldInfo.Grenades.LastInChain == NULL){
        this->worldInfo.Grenades.FirstInChain = newElement;
        this->worldInfo.Grenades.LastInChain = newElement;
        newElement->next = NULL;
        newElement->prev = NULL;
    }
    else {
        newElement->prev = this->worldInfo.Grenades.LastInChain;
        newElement->next = NULL;
        this->worldInfo.Grenades.LastInChain->next = newElement;
        this->worldInfo.Grenades.LastInChain = newElement;
    }
    return true;
}

bool WorldSimulator::AddMoveRequest(std::string PlayerName, sf::Vector2i direction)
{
    rbw::Player * currPlayer = NULL;
    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        rbw::Player * tmp = this->worldInfo.Players[i];
        if (tmp->GetPlayerName() == PlayerName){
            currPlayer = tmp;
            break;
        }
    }
    if (currPlayer == NULL) return false;
    if (currPlayer->isAlive() == false) return false;

    currPlayer->Move(direction);

    return true;
}

float WorldSimulator::SimulateNextStep()
{
    /* Now we're going to simulate the trajectory of all homing missiles
     */
    this->worldInfo.ElapsedTime = this->worldInfo.WorldClock.restart();

    rbw::HM_ChainElement * HM_currElement = this->worldInfo.homingMissiles.FirstInChain;
    while (HM_currElement != NULL){
        rbw::HomingMissile * rocket = HM_currElement->rocket;
        HM_currElement = HM_currElement->next;
        rocket->SimulateNextStep();
    }
    rbw::BB_ChainElement * BB_currElement = this->worldInfo.bouncingBombs.FirstInChain;
    while (BB_currElement != NULL){
        rbw::BouncingBomb * rocket = BB_currElement->rocket;        
        BB_currElement = BB_currElement->next;
        rocket->SimulateNextStep();
    }
    rbw::G_ChainElement * G_currElement = this->worldInfo.Grenades.FirstInChain;
    while (G_currElement != NULL){
        rbw::Grenade * rocket = G_currElement->rocket;
        G_currElement = G_currElement->next;
        rocket->SimulateNextStep();
    }
    return this->worldInfo.ElapsedTime.asMilliseconds();
}

bool WorldSimulator::GetObjects(std::vector< GraphicObject > * objects)
{
    objects->clear();
    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        GraphicObject newobject;
        Player* tmp = this->worldInfo.Players[i];
        sf::Vector2f p0 = tmp->GetPosition();
        newobject.Name = tmp->GetPlayerName();

        newobject.HealthPoint = tmp->GetHealth();
        newobject.team = tmp->GetTeam();

        newobject.type = rbw::Graphic::PLAYER;
        newobject.x = (int) p0.x;
        newobject.y = (int) p0.y;
        newobject.velocity_x = tmp->GetSpeed().x;
        newobject.velocity_y = tmp->GetSpeed().y;

        objects->push_back(newobject);
    }
    rbw::HM_ChainElement * HM_currElement = this->worldInfo.homingMissiles.FirstInChain;
    while (HM_currElement != NULL){
        rbw::HomingMissile * rocket = HM_currElement->rocket;
        HM_currElement = HM_currElement->next;

        GraphicObject newobject;
        sf::Vector2f p0 = rocket->GetPosition();
        newobject.Name = "Homing Missile";
        newobject.type = rbw::Graphic::FOLLOW_ROCKET;
        newobject.x = (int) p0.x;
        newobject.y = (int) p0.y;
        newobject.velocity_x = rocket->GetSpeed().x;
        newobject.velocity_y = rocket->GetSpeed().y;
        objects->push_back(newobject);
    }
    rbw::BB_ChainElement * BB_currElement = this->worldInfo.bouncingBombs.FirstInChain;
    while (BB_currElement != NULL){
        rbw::BouncingBomb * rocket = BB_currElement->rocket;
        BB_currElement = BB_currElement->next;

        GraphicObject newobject;
        sf::Vector2f p0 = rocket->GetPosition();
        newobject.Name = "Bouncing Bomb";
        newobject.type = rbw::Graphic::BOUNCE_ROCKET;
        newobject.x = (int) p0.x;
        newobject.y = (int) p0.y;
        newobject.velocity_x = rocket->GetSpeed().x;
        newobject.velocity_y = rocket->GetSpeed().y;
        objects->push_back(newobject);
    }
    rbw::G_ChainElement * G_currElement = this->worldInfo.Grenades.FirstInChain;
    while (G_currElement != NULL){
        rbw::Grenade * rocket = G_currElement->rocket;
        G_currElement = G_currElement->next;

        GraphicObject newobject;
        sf::Vector2f p0 = rocket->GetPosition();
        newobject.Name = "Grenade";
        newobject.zoom_coefficient = rocket->GetZoom();
        newobject.type = rbw::Graphic::GRENADE;
        newobject.x = (int) p0.x;
        newobject.y = (int) p0.y;
        newobject.velocity_x = rocket->GetSpeed().x;
        newobject.velocity_y = rocket->GetSpeed().y;
        objects->push_back(newobject);
    }

    for (int i=0; i< (int)this->worldInfo.Explosions.size(); i++){
        objects->push_back(this->worldInfo.Explosions[i]);
    }
    this->worldInfo.Explosions.clear();
    return true;
}
std::vector< rbw::PlayerExportInformation > WorldSimulator::ExportPlayerInfo()
{
    std::vector< rbw::PlayerExportInformation > answer(0);
    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        rbw::Player * tmpPlayer = this->worldInfo.Players[i];
        rbw::PlayerExportInformation tmpExportInfo;
        tmpExportInfo.DamageDealt = tmpPlayer->DamageDealt;
        tmpExportInfo.Death = tmpPlayer->Death;
        tmpExportInfo.isDead = !tmpPlayer->isAlive();
        tmpExportInfo.Kill = tmpPlayer->Kill;
        tmpExportInfo.PlayerName = tmpPlayer->GetPlayerName();

        tmpExportInfo.BouncingBombsLeft = tmpPlayer->BouncingBombsLeft;
        tmpExportInfo.HomingMissilesLeft = tmpPlayer->HomingMissilesLeft;
        tmpExportInfo.GrenadesLeft = tmpPlayer->GrenadesLeft;

        tmpExportInfo.team = tmpPlayer->GetTeam();
        answer.push_back(tmpExportInfo);
    }
    return answer;
}
std::vector< std::string > WorldSimulator::ExportEvents()
{
    std::vector< std::string > answer;
    answer = this->worldInfo.WorldEvents;
    this->worldInfo.WorldEvents.clear();
    return answer;
}
bool WorldSimulator::RoundEnded(rbw::Team * WinningTeam)
{
    bool allBlackAreDead = true;
    int BlackNum = 0;
    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        rbw::Player * tmpPlayer = this->worldInfo.Players[i];
        if (tmpPlayer->GetTeam() == rbw::TEAM_BLACK){
            BlackNum++;
            allBlackAreDead = (allBlackAreDead && (tmpPlayer->isAlive() == false));
        }
    }
    bool allWhiteAreDead = true;
    int WhiteNum = 0;
    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        rbw::Player * tmpPlayer = this->worldInfo.Players[i];
        if (tmpPlayer->GetTeam() == rbw::TEAM_WHITE){
            WhiteNum++;
            allWhiteAreDead = (allWhiteAreDead && (tmpPlayer->isAlive() == false));
        }
    }
    if (allBlackAreDead && allWhiteAreDead) *WinningTeam = rbw::TEAM_NOTEAM;
    else if (allBlackAreDead) *WinningTeam = rbw::TEAM_WHITE;
    else if (allWhiteAreDead) *WinningTeam = rbw::TEAM_BLACK;

    if (((WhiteNum == 0)&&(!allBlackAreDead))||((BlackNum == 0)&&(!allWhiteAreDead))) return false;
    return (allBlackAreDead || allWhiteAreDead);
}

bool WorldSimulator::RoundDraw()
{

    rbw::HM_ChainElement * HM_currElement = this->worldInfo.homingMissiles.FirstInChain;
    while (HM_currElement != NULL){
        rbw::HM_ChainElement * tmp = HM_currElement;
        HM_currElement = HM_currElement->next;
        delete tmp->rocket;
        delete tmp;
    }
    rbw::BB_ChainElement * BB_currElement = this->worldInfo.bouncingBombs.FirstInChain;
    while (BB_currElement != NULL){
        rbw::BB_ChainElement * tmp = BB_currElement;
        BB_currElement = BB_currElement->next;
        delete tmp->rocket;
        delete tmp;
    }
    rbw::G_ChainElement * G_currElement = this->worldInfo.Grenades.FirstInChain;
    while (G_currElement != NULL){
        rbw::G_ChainElement * tmp = G_currElement;
        G_currElement = G_currElement->next;
        delete tmp->rocket;
        delete tmp;
    }
    this->worldInfo.homingMissiles.FirstInChain = NULL;
    this->worldInfo.bouncingBombs.FirstInChain = NULL;

    this->worldInfo.homingMissiles.LastInChain = NULL;
    this->worldInfo.bouncingBombs.LastInChain = NULL;

    this->worldInfo.Grenades.FirstInChain = NULL;
    this->worldInfo.Grenades.LastInChain = NULL;

    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        rbw::Player * tmpPlayer = this->worldInfo.Players[i];
        tmpPlayer->Respawn();
    }
    return true;
}

WorldSimulator::~WorldSimulator()
{
    rbw::HM_ChainElement * HM_currElement = this->worldInfo.homingMissiles.FirstInChain;
    while (HM_currElement != NULL){
        rbw::HM_ChainElement * tmp = HM_currElement;
        HM_currElement = HM_currElement->next;
        delete tmp->rocket;
        delete tmp;
    }
    rbw::BB_ChainElement * BB_currElement = this->worldInfo.bouncingBombs.FirstInChain;
    while (BB_currElement != NULL){
        rbw::BB_ChainElement * tmp = BB_currElement;
        BB_currElement = BB_currElement->next;
        delete tmp->rocket;
        delete tmp;
    }
    rbw::G_ChainElement * G_currElement = this->worldInfo.Grenades.FirstInChain;
    while (G_currElement != NULL){
        rbw::G_ChainElement * tmp = G_currElement;
        G_currElement = G_currElement->next;
        delete tmp->rocket;
        delete tmp;
    }

    for (int i=0; i< (int)this->worldInfo.spawnPositions.size(); i++){
        rbw::spawnPos * tmpPos = this->worldInfo.spawnPositions[i];
        delete tmpPos;
    }

    for (int i=0; i< (int)this->worldInfo.Players.size(); i++){
        rbw::Player * tmpPlayer = this->worldInfo.Players[i];
        delete tmpPlayer;
    }    
}

}; // end of namespace rbw
