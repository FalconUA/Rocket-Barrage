#include "evadefromtherocket.h"

EvadeFromTheRocket::EvadeFromTheRocket(rbw::WorldSimulator * server)
{
    this->server = server;
}

TRocket EvadeFromTheRocket::BouncingBomb_to_TRocket(rbw::BouncingBomb &rocket)
{
    TRocket new_rocket;
    sf::Vector2f position = rocket.GetPosition();
    sf::Vector2f speed = rocket.GetSpeed();
    new_rocket.coordinates.x = int(position.x);
    new_rocket.coordinates.y = int(position.y);
    new_rocket.speedVector.x = int(speed.x);
    new_rocket.speedVector.y = int(speed.y);
  //  rocket
    return new_rocket;
}

TRocket EvadeFromTheRocket::HomingMissile_to_TRocket(rbw::HomingMissile &rocket)
{
    TRocket new_rocket;
    sf::Vector2f position = rocket.GetPosition();
    sf::Vector2f speed = rocket.GetSpeed();
    new_rocket.coordinates.x = int(position.x);
    new_rocket.coordinates.y = int(position.y);
    new_rocket.speedVector.x = int(speed.x);
    new_rocket.speedVector.y = int(speed.y);
  //  rocket
    return new_rocket;
}
TRocket EvadeFromTheRocket::Grenade_to_TRocket(rbw::Grenade &rocket)
{
    TRocket new_rocket;
    sf::Vector2f position = rocket.GetPosition();
    sf::Vector2f speed = rocket.GetSpeed();
    new_rocket.coordinates.x = int(position.x);
    new_rocket.coordinates.y = int(position.y);
    new_rocket.speedVector.x = int(speed.x);
    new_rocket.speedVector.y = int(speed.y);
  //  rocket
    return new_rocket;
}

bool EvadeFromTheRocket::pixelIsSave(TVector pixel, int calculating_steps)
{
    //this->server->worldInfo.ElapsedTime = this->worldInfo.WorldClock.restart();
    int dx,dy;
    TRocket new_rocket;

    rbw::HM_ChainElement * HM_currElement = this->server->getWorldInfo()->homingMissiles.FirstInChain;
    while (HM_currElement != NULL){
        rbw::HomingMissile rocket = *(HM_currElement->rocket);
        /*
         *Rocket should think, that there are no players!
         */
        rbw::WorldInformation world_info;
        world_info.Players.clear();
        //rocket.worldInfo = &world_info;

        HM_currElement = HM_currElement->next;
        for(int i = 0; i < calculating_steps; i++)
        {
            new_rocket = this->HomingMissile_to_TRocket(rocket);
            dx = pixel.x - new_rocket.coordinates.x;
            dy = pixel.y - new_rocket.coordinates.y;
            if(dx*dx + dy*dy < 24*24)
                return false;
            rocket.SimulateNextStep();
        }
    }
    rbw::BB_ChainElement * BB_currElement = this->server->getWorldInfo()->bouncingBombs.FirstInChain;
    while (BB_currElement != NULL){
        rbw::BouncingBomb rocket = *(BB_currElement->rocket);
        /*
         *Rocket should think, that there are no players!
         */
        rbw::WorldInformation world_info;
        world_info.Players.clear();
        //rocket.worldInfo = &world_info;

        BB_currElement = BB_currElement->next;
        for(int i = 0; i < calculating_steps; i++)
        {
            new_rocket = this->BouncingBomb_to_TRocket(rocket);
            dx = pixel.x - new_rocket.coordinates.x;
            dy = pixel.y - new_rocket.coordinates.y;
            if(dx*dx + dy*dy < 24*24)
                return false;
            rocket.SimulateNextStep();
        }
    }
    rbw::G_ChainElement * G_currElement = this->server->getWorldInfo()->Grenades.FirstInChain;
    while (G_currElement != NULL){
        rbw::Grenade rocket = *(G_currElement->rocket);
        /*
         *Rocket should think, that there are no players!
         */
        rbw::WorldInformation world_info;
        world_info.Players.clear();
        //rocket.worldInfo = &world_info;

        G_currElement = G_currElement->next;
        for(int i = 0; i < calculating_steps; i++)
        {
            new_rocket = this->Grenade_to_TRocket(rocket);
            dx = pixel.x - new_rocket.coordinates.x;
            dy = pixel.y - new_rocket.coordinates.y;
            if(dx*dx + dy*dy < rbw::GameParam::GRENADE_RADIUS_OF_EFFECT_SQUARED + rbw::fEPS)
                return false;
            rocket.SimulateNextStep();
        }
    }
    std::cout << "using evade function " << std::endl;

    return true;
}

std::vector<TDirectionPair> EvadeFromTheRocket::saveDirection(TVector player_coord)
{
    std::vector<TDirectionPair> save_direction_pairs;
    TDirectionPair direction_pair[3][3] = {{{rbw::DIRECTION_UP,rbw::DIRECTION_LEFT},
                                            {rbw::DIRECTION_UP,rbw::DIRECTION_NODIRECTION},
                                            {rbw::DIRECTION_UP,rbw::DIRECTION_RIGHT}},
                                           {{rbw::DIRECTION_NODIRECTION,rbw::DIRECTION_LEFT},
                                            {rbw::DIRECTION_NODIRECTION,rbw::DIRECTION_NODIRECTION},
                                            {rbw::DIRECTION_NODIRECTION,rbw::DIRECTION_RIGHT}},
                                           {{rbw::DIRECTION_DOWN,rbw::DIRECTION_LEFT},
                                            {rbw::DIRECTION_DOWN,rbw::DIRECTION_NODIRECTION},
                                            {rbw::DIRECTION_DOWN,rbw::DIRECTION_RIGHT}}};
    TVector tmp_pixel;
    int r1 = 24, r2 = int(sqrt(rbw::GameParam::GRENADE_RADIUS_OF_EFFECT_SQUARED + rbw::fEPS)) + 1;
    int max_radius = ((r1 > r2) ? r1 : r2);
    for(int i = -1; i <= 1; i++)
    {
        tmp_pixel.y = player_coord.y + i*max_radius;
        for(int j = -1; j <= 1; j++)
        {
            tmp_pixel.x = player_coord.x + j*max_radius;
            if(this->pixelIsSave(tmp_pixel,max_radius))
                save_direction_pairs.push_back(direction_pair[i+1][j+1]);
        }
    }
    return save_direction_pairs;
}
