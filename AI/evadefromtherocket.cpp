#include "evadefromtherocket.h"

EvadeFromTheRocket::EvadeFromTheRocket(rbw::WorldSimulator * server)
{
    this->server = server;
    this->walls = this->getWalls();
}

EvadeFromTheRocket::~EvadeFromTheRocket()
{
    walls.clear();
}

std::vector<TRectangle> EvadeFromTheRocket::getWalls()
{
    std::vector< Object > walls = this->server->worldInfo.wallForPlayer;
    Object wall;
    std::vector<TRectangle> my_walls;
    TRectangle my_wall;
    for(int i = 0; i < (int)walls.size(); i++)
    {
        wall = walls[i];
        my_wall.A.x = wall.rect.left;
        my_wall.A.y = wall.rect.top;
        my_wall.B.x = my_wall.A.x + wall.rect.width;
        my_wall.B.y = my_wall.A.y + wall.rect.height;
        //
        //my_wall.A.x--;
        //my_wall.A.y--;
        //my_wall.B.x++;
        //my_wall.B.y++;
        //
        my_walls.push_back(my_wall);
    }
    return my_walls;
}

bool EvadeFromTheRocket::pixelIsInTheWall(TVector pixel)
{
    TVector minCoord,maxCoord;
    for(int i = 0; i < (int)this->walls.size(); i++)
    {
        if(this->walls[i].A.x < this->walls[i].B.x)
        {
            minCoord.x = this->walls[i].A.x;
            maxCoord.x = this->walls[i].B.x;
        }
            else
        {
            minCoord.x = this->walls[i].B.x;
            maxCoord.x = this->walls[i].A.x;
        }
        if(this->walls[i].A.y < this->walls[i].B.y)
        {
            minCoord.y = this->walls[i].A.y;
            maxCoord.y = this->walls[i].B.y;
        }
            else
        {
            minCoord.y = this->walls[i].B.y;
            maxCoord.y = this->walls[i].A.y;
        }
        if( (pixel.x >= minCoord.x)&&(pixel.x <= maxCoord.x)&&
                (pixel.y >= minCoord.y)&&(pixel.y <= maxCoord.y) )
            return true;
    }
    return false;
}

void EvadeFromTheRocket::sortVector(std::vector<int> &vector)
{
    int tmp,steps = (int)vector.size()-1;
    std::sort(vector.begin(),vector.end());
    /*for(int i = 0; i < steps; i++)
        for(int j = 0; j < steps; j++)
            if(vector[j] > vector[j+1])
            {
                tmp = vector[j];
                vector[j] = vector[j+1];
                vector[j+1] = tmp;
            }*/
    tmp = 0;
    for(int i = 0; i < steps; i++)
        if(vector[i-tmp] == vector[i+1-tmp])
        {
            vector.erase(vector.begin() + i - tmp);
            tmp++;
        }
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

bool EvadeFromTheRocket::pixelIsSave(TVector pixel, rbw::Player bot, int calculating_steps)
{
    //this->server->worldInfo.ElapsedTime = this->worldInfo.WorldClock.restart();
    //int dx,dy;
    //TRocket new_rocket;

   // rbw::WorldSimulator  ss = *(this->server);

    //rbw::HM_ChainElement tmpYo = *(this->server->worldInfo.homingMissiles.FirstInChain);
    //std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\n";
    rbw::HM_ChainElement * HM_currElement = this->server->worldInfo.homingMissiles.FirstInChain;
    while (HM_currElement != NULL)
    {
        //std::cout << "IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII\n";
        //rbw::HM_ChainElement yo = * HM_currElement;
        //std::cout << "Left1: " << HM_currElement->rocket->GetOwner()->HomingMissilesLeft << "\n";
        //int yo = HM_currElement->rocket->GetOwner()->HomingMissilesLeft;
        rbw::WorldInformation world_info;

        rbw::Player tmp_bot = bot;
        tmp_bot.SetNewName("");
        world_info.Players.clear();
        world_info.Players.push_back(&tmp_bot);

        rbw::HomingMissile rocket(HM_currElement->rocket->GetOwner(),
                                  HM_currElement->rocket->GetTarget(),
                                  HM_currElement, &world_info); //= *(HM_currElement->rocket);
        //HM_currElement->rocket->GetOwner()->HomingMissilesLeft--;
        //std::cout << "Left2: " << HM_currElement->rocket->GetOwner()->HomingMissilesLeft << "\n";
        /*
         *Rocket should think, that there are no players!
         */
        //rocket.worldInfo = &world_info;
        //--------------------------------------------------------------------------------//
        bot.SetPosition(sf::Vector2f(pixel.x,pixel.y));
        rbw::Player * tmpPlayer = &bot;
        if (tmpPlayer->GetPlayerName() == rocket.GetOwner()->GetPlayerName())
        {
            //std::cout << "SUKASUKASUKASUKASUKASUKASUKASUKASUKASUKASUKASUKA\n";
            HM_currElement = HM_currElement->next;
            continue;
        }
        for(int i = 0; i < calculating_steps; i++)
        {
            sf::Vector2f tmpPosition = tmpPlayer->GetPosition();
            sf::Vector2f vec_a;
            vec_a.x = tmpPosition.x - rocket.GetPosition().x;
            vec_a.y = tmpPosition.y - rocket.GetPosition().y;
            float distance = vec_a.x * vec_a.x + vec_a.y * vec_a.y;
            float minDistance = rbw::GameParam::PLAYER_HITBOX_RADIUS + rbw::GameParam::HOMING_MISSILE_HITBOX_RADIUS;
            if (distance < minDistance * minDistance)
            {
                std::cout << "YOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOs\n";
                return false;
            }
            //std::cout << "PosBot: " << tmpPosition.x << ";" << tmpPosition.y << "\n";
            //std::cout << "PosRoc:" << rocket.GetPosition().x << ";" << rocket.GetPosition().y << "\n";
            rocket.SimulateNextStep();
        }
       // rocket.~HomingMissile();
        //--------------------------------------------------------------------------------------------//

        //HM_currElement->rocket->GetOwner()->HomingMissilesLeft = yo;
        HM_currElement = HM_currElement->next;
        /*for(int i = 0; i < calculating_steps; i++)
        {
            new_rocket = this->HomingMissile_to_TRocket(rocket);
            dx = pixel.x - new_rocket.coordinates.x;
            dy = pixel.y - new_rocket.coordinates.y;
            if(dx*dx + dy*dy < 24*24)
                return false;
            rocket.SimulateNextStep();
        }*/
    }

    rbw::BB_ChainElement * BB_currElement = this->server->worldInfo.bouncingBombs.FirstInChain;
    while (BB_currElement != NULL)
    {
        rbw::WorldInformation world_info;
        world_info.Players.clear();

        rbw::BouncingBomb rocket(BB_currElement->rocket->GetOwner(),
                                  BB_currElement, &world_info); //= *(HM_currElement->rocket);

        //rbw::BouncingBomb rocket = *(BB_currElement->rocket);
        /*
         *Rocket should think, that there are no players!
         */
        //rbw::WorldInformation world_info;
        //world_info.Players.clear();
        //rocket.worldInfo = &world_info;
        //--------------------------------------------------------------------------------//
        bot.SetPosition(sf::Vector2f(pixel.x,pixel.y));
        rbw::Player * tmpPlayer = &bot;
        if (tmpPlayer->GetPlayerName() == rocket.GetOwner()->GetPlayerName())
        {
            BB_currElement = BB_currElement->next;
            continue;
        }

        for(int i = 0; i < calculating_steps; i++)
        {
            sf::Vector2f tmpPosition = tmpPlayer->GetPosition();
            sf::Vector2f vec_a;
            vec_a.x = tmpPosition.x - rocket.GetPosition().x;
            vec_a.y = tmpPosition.y - rocket.GetPosition().y;
            float distance = vec_a.x * vec_a.x + vec_a.y * vec_a.y;
            float minDistance = rbw::GameParam::PLAYER_HITBOX_RADIUS + rbw::GameParam::BOUNCING_BOMB_HITBOX_RADIUS;
            if (distance < minDistance * minDistance)
                return false;
            rocket.SimulateNextStep();
        }
        BB_currElement = BB_currElement->next;
        //--------------------------------------------------------------------------------------------//
    }

    //rbw::BB_ChainElement * BB_currElement = this->server->worldInfo.bouncingBombs.FirstInChain;
    //while (BB_currElement != NULL){
    //    rbw::BouncingBomb rocket = *(BB_currElement->rocket);
    //    /*
    //     *Rocket should think, that there are no players!
    //     */
    /*    rbw::WorldInformation world_info;
        world_info.Players.clear();
        rocket.worldInfo = &world_info;

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
    */

    rbw::G_ChainElement * G_currElement = this->server->worldInfo.Grenades.FirstInChain;
    while (G_currElement != NULL)
    {
        rbw::Grenade rocket = *(G_currElement->rocket);
        /*
         *Rocket should think, that there are no players!
         */
        rbw::WorldInformation world_info;
        world_info.Players.clear();
        rocket.worldInfo = &world_info;
        //--------------------------------------------------------------------------------//
        bot.SetPosition(sf::Vector2f(pixel.x,pixel.y));
        rbw::Player * tmpPlayer = &bot;
        //if (tmpPlayer == rocket.GetOwner())
        //    continue;

        for(int i = 0; i < calculating_steps; i++)
        {
            //sf::Vector2f tmpPosition = tmpPlayer->GetPosition();

            sf::Vector2f pos = tmpPlayer->GetPosition();
            sf::Vector2f dis (pos.x - rocket.GetPosition().x, pos.y - rocket.GetPosition().y);
            float dis_squared = (dis.x * dis.x) + (dis.y * dis.y);
            if (dis_squared < rbw::GameParam::GRENADE_RADIUS_OF_EFFECT_SQUARED + rbw::fEPS)
                return false;

            //sf::Vector2f vec_a;
            //vec_a.x = tmpPosition.x - rocket.GetPosition().x;
            //vec_a.y = tmpPosition.y - rocket.GetPosition().y;
            //float distance = vec_a.x * vec_a.x + vec_a.y * vec_a.y;
            //float minDistance = rbw::GameParam::PLAYER_HITBOX_RADIUS + rbw::GameParam::GRENADE_HITBOX_RADIUS;
            //if (distance < minDistance * minDistance)
            //    return false;
            rocket.SimulateNextStep();
        }
        G_currElement = G_currElement->next;
        //--------------------------------------------------------------------------------------------//
    }

    //rbw::G_ChainElement * G_currElement = this->server->worldInfo.Grenades.FirstInChain;
    //while (G_currElement != NULL){
    //    rbw::Grenade rocket = *(G_currElement->rocket);
        /*
         *Rocket should think, that there are no players!
         */
    /*    rbw::WorldInformation world_info;
        world_info.Players.clear();
        rocket.worldInfo = &world_info;

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

    }*/

    return true;
}

std::vector<TDirectionPair> EvadeFromTheRocket::saveDirection(/*TVector player_coord, */rbw::Player bot, TVector real_speed)
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
    TVector tmp_pixel, player_coord;
    sf::Vector2f bot_position = bot.GetPosition();
    player_coord.x = int(bot_position.x);
    player_coord.y = int(bot_position.y);
    int r1 = int(rbw::GameParam::PLAYER_HITBOX_RADIUS + rbw::GameParam::HOMING_MISSILE_HITBOX_RADIUS) + 1,
            r2 = int(rbw::GameParam::PLAYER_HITBOX_RADIUS + rbw::GameParam::BOUNCING_BOMB_HITBOX_RADIUS) + 1,
            r3 = int(sqrt(rbw::GameParam::GRENADE_RADIUS_OF_EFFECT_SQUARED + rbw::fEPS)) + 1;
    int max_radius = ( ((r1 > r2) ? r1 : r2) >  r3 ? ((r1 > r2) ? r1 : r2) : r3 );
    for(int i = -1; i <= 1; i++)
    {
        tmp_pixel.y = player_coord.y + i*real_speed.y/*max_radius*/;
        for(int j = -1; j <= 1; j++)
        {
            tmp_pixel.x = player_coord.x + j*real_speed.x/*max_radius*/;
            if(this->pixelIsSave(tmp_pixel,bot,max_radius))
                save_direction_pairs.push_back(direction_pair[i+1][j+1]);
        }
    }
    return save_direction_pairs;
}
