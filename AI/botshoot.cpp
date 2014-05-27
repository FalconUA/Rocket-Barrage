#include "botshoot.h"

BotShoot::BotShoot(rbw::WorldSimulator *server, std::vector<TPlayer> &victims, TVector bot)
{
    this->server = server;
    this->victims = victims;
    this->walls = this->getWalls();
    this->bot_coord = bot;

    this->x_coord.push_back(X_MIN);
    this->y_coord.push_back(Y_MIN);
    for(int i = 0; i < (int)walls.size(); i++)
    {
        this->x_coord.push_back(walls[i].A.x);
        this->x_coord.push_back(walls[i].B.x);
        this->y_coord.push_back(walls[i].A.y);
        this->y_coord.push_back(walls[i].B.y);
    }
    this->x_coord.push_back(this->bot_coord.x); //
    this->y_coord.push_back(this->bot_coord.y); //
    for(int i = 0; i < (int)this->victims.size(); i++)
    {
        this->x_coord.push_back(this->victims[i].coord.x);
        this->y_coord.push_back(this->victims[i].coord.y);
    }
    this->x_coord.push_back(X_MAX);
    this->y_coord.push_back(Y_MAX);
    sortVector(this->x_coord);    //check!
    sortVector(this->y_coord);    //check!
}

void BotShoot::sortVector(std::vector<int> &vector)
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

std::vector<TRectangle> BotShoot::getWalls()
{
    std::vector< Object > walls = this->server->worldInfo.wallForRocket;
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
        my_walls.push_back(my_wall);
    }
    return my_walls;
}

/*TParamLineEq BotShoot::getParamLineEq(TVector u, TVector v)
{
    int dx = v.x - u.x, dy = v.y - u.y;

}*/

TLineEq BotShoot::getLineEq(TVector u, TVector v)
{
    float dx = float(v.x - u.x), dy = float(v.y - u.y);
    TLineEq lineEq;
    if(dx == 0)
    {
        lineEq.k = u.x;
        lineEq.vertical = true;
        return lineEq;
    }
    lineEq.k = dy / dx;
    lineEq.c = float(u.y) - lineEq.k*u.x;
    lineEq.vertical = false;
    return lineEq;
}

TLineEq BotShoot::rotateLineEq(TLineEq lineEq)
{
    TLineEq rotatedLineEq;
    if(lineEq.vertical)
    {
        rotatedLineEq.k = 0;
        rotatedLineEq.c = lineEq.k;
        rotatedLineEq.vertical = false;
        return rotatedLineEq;
    }
    if(lineEq.k == 0)
    {
        rotatedLineEq.k = lineEq.c;
        rotatedLineEq.vertical = true;
        return rotatedLineEq;
    }
    rotatedLineEq.k = 1/lineEq.k;
    rotatedLineEq.c = - lineEq.c / lineEq.k;
    rotatedLineEq.vertical = false;
    return rotatedLineEq;
}

float BotShoot::value_y_from_x0(TLineEq& lineEq, float x0)
{
    if(lineEq.vertical)
        return float(NULL);
    return (lineEq.k * x0 + lineEq.c);
}

bool BotShoot::pixelIsInTheWall(TVector pixel)
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

bool BotShoot::can_we_hit_victim(TVector victimCoord)
{
    TLineEq line_bot_victim_y_from_x = getLineEq(this->bot_coord,victimCoord);
    TLineEq line_bot_victim_x_from_y = rotateLineEq(line_bot_victim_y_from_x);

    TVector intersection;

    if(!line_bot_victim_y_from_x.vertical)
        for(int i = 0; i < int(this->x_coord.size()); i++)
        {
            intersection.x = int( this->x_coord[i] );
            intersection.y = int( this->value_y_from_x0(line_bot_victim_y_from_x,float(intersection.x)) );
            if( ( ( (intersection.x >= victimCoord.x)&&(intersection.x <= this->bot_coord.x) )||
                  ( (intersection.x <= victimCoord.x)&&(intersection.x >= this->bot_coord.x) )   )&&
                ( ( (intersection.y >= victimCoord.y)&&(intersection.y <= this->bot_coord.y) )||
                  ( (intersection.y <= victimCoord.y)&&(intersection.y >= this->bot_coord.y) )   )&&
                    this->pixelIsInTheWall(intersection)    )
                return false;
        }

    if(!line_bot_victim_x_from_y.vertical)
        for(int i = 0; i < int(this->y_coord.size()); i++)
        {
            intersection.y = int( this->y_coord[i] );
            intersection.x = this->value_y_from_x0(line_bot_victim_x_from_y,float(intersection.y));
            if( ( ( (intersection.x >= victimCoord.x)&&(intersection.x <= this->bot_coord.x) )||
                  ( (intersection.x <= victimCoord.x)&&(intersection.x >= this->bot_coord.x) )   )&&
                ( ( (intersection.y >= victimCoord.y)&&(intersection.y <= this->bot_coord.y) )||
                  ( (intersection.y <= victimCoord.y)&&(intersection.y >= this->bot_coord.y) )   )&&
                    this->pixelIsInTheWall(intersection)    )
                return false;
        }

    return true;
}

std::vector<TPlayer> BotShoot::achievableVictims()
{
    std::vector<TPlayer> achiev_victims;

    for(int i = 0; i < int(this->victims.size()); i++)
        if(this->can_we_hit_victim(this->victims[i].coord))
            achiev_victims.push_back(this->victims[i]);

    return achiev_victims;
}

TVector BotShoot::getVictimCoord()
{
    std::vector<TPlayer> achiev_victims = this->achievableVictims();
    if(achiev_victims.empty())
    {
        TVector no_victim;

        //char* p_no_victim = (char*)(&no_victim);
        //for(char* i = p_no_victim; i < p_no_victim + sizeof(no_victim); i++)
        //    *i = char(NULL);
        no_victim.x = 0;
        no_victim.y = 0;

        return no_victim;
    }

    TPlayer victim = achiev_victims[0];
    for(int i = 1, dx, dy, min_distance_2 = ~(1 >> 1); i < int(achiev_victims.size()); i++)
    {
        dx = victim.coord.x - achiev_victims[i].coord.x;
        dy = victim.coord.y - achiev_victims[i].coord.y;
        if(int sqr = dx*dx + dy*dy < min_distance_2)
        {
            min_distance_2 = sqr;
            victim = achiev_victims[i];
        }
    }

    return victim.coord;
}
