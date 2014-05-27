#include "movetothevictim.h"

MoveToTheVictim::MoveToTheVictim(WorldSimulator *server)
{
    num = 0;
    this->server = server;
}

MoveToTheVictim::~MoveToTheVictim()
{
    bots.clear();
    victims.clear();
    walls.clear();
}

bool MoveToTheVictim::moveToTheVictim(TPlayer bot, Player * bot_n,
                                      TVector * victim_position, sf::Vector2i *direction_bot)
{
    /*
     *if bot is already dead then return;
     */

    //std::cout << "MoveToTheVictim -> ";

    if(!bot_n->isAlive())
        return false;

    /*
     *We are creating vectors with x- and y-
     *coordinates of walls, bot and players
     *from opposite command;
     */

    if(num-- != 0)
    {
        //bot_n->Move(lastHorDirection);
        //bot_n->Move(lastVertDirection);
        //std::cout << "num != 0 -> ";
//        this->moveToTheNextPixel(lastHorDirection);
//        this->moveToTheNextPixel(lastVertDirection);
  //      std::cout << "return false;\n";
        *direction_bot = bot_n->lastDirection;
        return false;
    }

    this->direction_bot = direction_bot;

    lastHorDirection = DIRECTION_NODIRECTION;
    lastVertDirection = DIRECTION_NODIRECTION;
    num = 0;

    std::vector<int> x_coord;
    std::vector<int> y_coord;

    x_coord.push_back(X_MIN);
    y_coord.push_back(Y_MIN);
    for(int i = 0; i < (int)this->walls.size(); i++)
    {
        x_coord.push_back(this->walls[i].A.x);
        x_coord.push_back(this->walls[i].B.x);
        y_coord.push_back(this->walls[i].A.y);
        y_coord.push_back(this->walls[i].B.y);
    }
    x_coord.push_back(bot.coord.x); //
    y_coord.push_back(bot.coord.y); //
    for(int i = 0; i < (int)victims.size(); i++)
    {
        x_coord.push_back(victims[i].coord.x);
        y_coord.push_back(victims[i].coord.y);
    }
    x_coord.push_back(X_MAX);
    y_coord.push_back(Y_MAX);

    sortVector(x_coord);    //check!
    sortVector(y_coord);    //check!

    int rows = y_coord.size()-1, columns = x_coord.size()-1;
    int **cells = new int*[columns];
    for(int i = 0; i < columns; i++)
        cells[i] = new int[rows];
    TVector tmp_vector,bot_cell,neighCells[4];
    for(int i = 0; i < columns; i++)
        for(int j = 0; j < rows; j++)
        {

            bool cell_is_wall = true;

            for(int x = 0; x <= 1; x++)
                for(int y = 0; y <= 1; y++)
                {
                    tmp_vector.x = x_coord[i]+x;
                    tmp_vector.y = y_coord[j]+y;

                    if( !pixelIsInTheWall(tmp_vector) )
                        cell_is_wall = false;
                }

            //if( (x_coord[i] + 1 == x_coord[i+1])&&(y_coord[j] + 1 == y_coord[j+1]) )
            //    cell_is_wall = true;

            if(cell_is_wall)
                 cells[i][j] = -1;   //якщо стіна;
                     else
                 cells[i][j] = 0;    //якщо пусто;

          //  if( (tmp_vector.x == x_coord[i+1])||(tmp_vector.y == y_coord[j+1]) )
          //      cells[i][j] = -2;   //якщо внутрішність пуста;
            if( (x_coord[i] == bot.coord.x)&&(y_coord[j] == bot.coord.y) )
            {
                bot_cell.x = i;
                bot_cell.y = j;
            }
        }

    neighCells[0].x = bot_cell.x - 1;
    neighCells[0].y = bot_cell.y - 1;
   // if(/*(x_coord[neighCells[0].x+1] - x_coord[neighCells[0].x] == 1)&&*/(neighCells[0].x != 0))
     //   neighCells[0].x--;
   // if(/*(y_coord[neighCells[0].y+1] - y_coord[neighCells[0].y] == 1)&&*/(neighCells[0].y != 0))
     //   neighCells[0].y--;
    neighCells[1].x = bot_cell.x;
    neighCells[1].y = neighCells[0].y;
    /*if((x_coord[neighCells[1].x+1] - x_coord[neighCells[1].x] == 1)&&
            (neighCells[1].x != x_coord.size() - 2))
        neighCells[1].x++;*/
    neighCells[2].x = bot_cell.x/*neighCells[1].x*/;
    neighCells[2].y = bot_cell.y;
    /*if((y_coord[neighCells[2].y+1] - y_coord[neighCells[2].y] == 1)&&
            (neighCells[2].y != y_coord.size() - 2))
        neighCells[2].y++;*/
    neighCells[3].x = neighCells[0].x;
    neighCells[3].y = neighCells[2].y;

    for(int i = 0; i < 4; i++)
        if(cells[neighCells[i].x][neighCells[i].y] == 0)
            cells[neighCells[i].x][neighCells[i].y] = 1;

    int MAX_Column = x_coord.size() - 2, MAX_Row = y_coord.size() - 2;

    std::vector<TVector> next_cells,new_cells,tmp_cells;
    for(int i = 0; i < 4; i++)
        if(cells[neighCells[i].x][neighCells[i].y] == 1)
            next_cells.push_back(neighCells[i]);
    //std::cout << "Bot_x;y: " << bot.coord.x << ";" << bot.coord.y << "\n";
    //if(this->pixelIsInTheWall(bot.coord))
    //    std::cout << "Pixel is in the wall!!!!!!!!!!!!!!!!!!!!!!!!!!\n";

    /*
    next_cells.push_back(neighCells[0]);
    next_cells.push_back(neighCells[1]);
    next_cells.push_back(neighCells[2]);
    next_cells.push_back(neighCells[3]);
*/
    TVector tmp_cell;
    tmp_cell.x = -1;
    tmp_cell.y = -1;

    while((!victimIsInTheCellsVector(next_cells,x_coord,y_coord,&tmp_cell,victim_position))&&(!next_cells.empty()))
    {
        for(int i = 0; i < (int)next_cells.size(); i++)
        {
            tmp_cells = wave(cells,next_cells[i],MAX_Column,MAX_Row);
            for(int k = 0; k < (int)tmp_cells.size(); k++)
                new_cells.push_back(tmp_cells[k]);
            tmp_cells.clear();
        }
        next_cells.clear();
        next_cells = new_cells;
        new_cells.clear();

        //if(next_cells.empty())
        //    std::cout << "next_cells is empty!\n";
    }

    if((tmp_cell.x == -1)||(tmp_cell.y == -1))
    {
        //std::cout << "Victim wasn't founded! ERROR!\n";
        return false;
    }

    std::vector<TVector> way;
    for(int i = cells[tmp_cell.x][tmp_cell.y]; i > 0; i--)
    {
        way.push_back(tmp_cell);
        if( (tmp_cell.x != 0)&&
                (cells[tmp_cell.x-1][tmp_cell.y] == cells[tmp_cell.x][tmp_cell.y] - 1) )
            tmp_cell.x--;
                else
            if( (tmp_cell.y != 0)&&
                    (cells[tmp_cell.x][tmp_cell.y-1] == cells[tmp_cell.x][tmp_cell.y] - 1) )
                tmp_cell.y--;
                    else
                if( (tmp_cell.x != MAX_Column)&&
                        (cells[tmp_cell.x+1][tmp_cell.y] == cells[tmp_cell.x][tmp_cell.y] - 1) )
                    tmp_cell.x++;
                        else
                    if( (tmp_cell.y != MAX_Row)&&
                            (cells[tmp_cell.x][tmp_cell.y+1] == cells[tmp_cell.x][tmp_cell.y] - 1) )
                        tmp_cell.y++;
    }

    //std::cout << "way is: {";
    //for(int i = 0; i < (int)way.size(); i++)
    //    std::cout << "(" << way[i].x << "," << way[i].y << ")" << ",";
    //std::cout << "} -> ";


    if(way.size() <= 2)
    {
        //std::cout << "return true;\n";
        return true;
    }

    tmp_cell = way[way.size() - 2];
    way.clear();
    moveToTheCell(x_coord,y_coord,tmp_cell,bot/*,bot_n*/);
    //std::cout << "return false;\n";
    return false;
}

void MoveToTheVictim::moveToTheCell(std::vector<int> &x_coord, std::vector<int> &y_coord, TVector &cell, TPlayer &bot/*,
                   rbw::Player * bot_n*/)
{
    //std::cout << "moveToTheCell -> ";
    TVector new_coord,old_coord;

    //----------------it's new---------------------//
    int x_start, x_end, y_start, y_end;
    if(bot.coord.x != x_coord[cell.x] + 1)
    {
        new_coord.x = (x_coord[cell.x] + x_coord[cell.x+1]) / 2 + 1;
        x_start = new_coord.x - 1;
        x_end = new_coord.x;
    }
            else
    {
        new_coord.x = (x_coord[cell.x] + x_coord[cell.x+1]) / 2;
        x_start = new_coord.x;
        x_end = new_coord.x + 1;
    }
    if(bot.coord.y != y_coord[cell.y] + 1)
    {
        new_coord.y = (y_coord[cell.y] + y_coord[cell.y+1]) / 2 + 1;
        y_start = new_coord.y - 1;
        y_end = new_coord.y;
    }
            else
    {
        new_coord.y = (y_coord[cell.y] + y_coord[cell.y+1]) / 2;
        y_start = new_coord.y;
        y_end = new_coord.y + 1;
    }
    if( this->pixelIsInTheWall(new_coord) )
        for(int i = x_start; (i < x_end)&&(this->pixelIsInTheWall(new_coord))&&
            (new_coord.x == bot.coord.x)&&(new_coord.y == bot.coord.y); i++)
            for(int j = y_start; (j < y_end)&&(this->pixelIsInTheWall(new_coord))&&
                (new_coord.x == bot.coord.x)&&(new_coord.y == bot.coord.y); j++)
            {
                new_coord.x = i;
                new_coord.y = j;
            }
    //------------------------------------------//

       /* if(this->pixelIsInTheWall(new_coord))
        {
            if(this->)
        }
*/
      //std::cout << "Bot want move to (" << new_coord.x << "," <<
      //             new_coord.y << ");" << std::endl;
     // std::cout << "This pixel is in the wall: " << this->pixelIsInTheWall(new_coord) << ";" << std::endl;
      //std::cout << "x_coord: " << x_coord[cell.x] << " - " << x_coord[cell.x+1] << ";" << std::endl;
      //std::cout << "y_coord: " << y_coord[cell.y] << " - " << y_coord[cell.y+1] << ";" << std::endl;
        //else
//        new_coord.y = (y_coord[cell.y] + y_coord[cell.y+1]) / 2;
    old_coord = bot.coord;
    std::cout << "Bot_coord: (" << old_coord.x << "," << old_coord.y << ");\n";
    int way_part1,way_part2,time1,time2;

    /*TVector real_speed;
    sf::Vector2f speed = bot_n->GetSpeed();
    real_speed.x = (int)speed.x;
    real_speed.y = (int)speed.y;
    EvadeFromTheRocket stells_system(this->server);
    stells_system.saveDirection(*bot_n,real_speed);
    */
    if( (new_coord.x >= bot.coord.x)&&(new_coord.y <= bot.coord.y ) )
    {
       // std::cout << "IF\n";
        if(new_coord.x - bot.coord.x <  bot.coord.y - new_coord.y)
        {
            way_part1 = new_coord.x - bot.coord.x;
            way_part2 = bot.coord.y - new_coord.y - way_part1;
            num = (way_part1 + way_part2)/2 + 1; // --- //
            time1 = int(way_part1 / bot.speed * 1000) + 1;
            time2 = int(way_part2 / bot.speed * 1000);
            if(time1 < unthinkable_time)
                time2 = (unthinkable_time - time1 < time2 ? unthinkable_time - time1 : time2);
                        else
            {
                time1 = unthinkable_time;
                time2 = 0;
            }
            //if(time1 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x + 1 << "," << old_coord.y - 1 << ");\n";
            moveUpRight(time1);
            //if(time2 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x << "," << old_coord.y - 1 << ");\n";
            moveUp(time2);
            return;
        }
                else
        {
            way_part1 = bot.coord.y - new_coord.y;
            way_part2 = new_coord.x - bot.coord.x - way_part1;
            num = (way_part1 + way_part2)/2 + 1; // --- //
            time1 = int(way_part1 / bot.speed * 1000) + 1;
            time2 = int(way_part2 / bot.speed * 1000);
            if(time1 < unthinkable_time)
                time2 = (unthinkable_time - time1 < time2 ? unthinkable_time - time1 : time2);
                        else
            {
                time1 = unthinkable_time;
                time2 = 0;
            }
            //if(time1 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x + 1 << "," << old_coord.y - 1 << ");\n";
            moveUpRight(time1);
            //if(time2 != 0)
            //   std::cout << "Next_coord: (" << old_coord.x + 1 << "," << old_coord.y << ");\n";
            moveRight(time2);
            return;
        }
    }
    if( (new_coord.x >= bot.coord.x)&&(new_coord.y >= bot.coord.y ) )
    {
        //std::cout << "IF\n";
        if(new_coord.x - bot.coord.x < new_coord.y - bot.coord.y)
        {
            way_part1 = new_coord.x - bot.coord.x;
            way_part2 = new_coord.y - bot.coord.y - way_part1;
            num = (way_part1 + way_part2)/2 + 1; // --- //
            time1 = int(way_part1 / bot.speed * 1000) + 1;
            time2 = int(way_part2 / bot.speed * 1000);
            if(time1 < unthinkable_time)
                time2 = (unthinkable_time - time1 < time2 ? unthinkable_time - time1 : time2);
                        else
            {
                time1 = unthinkable_time;
                time2 = 0;
            }
            //if(time1 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x + 1 << "," << old_coord.y + 1 << ");\n";
            moveDownRight(time1);
            //if(time2 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x << "," << old_coord.y - 1 << ");\n";
            moveDown(time2);
            return;
        }
                else
        {
            way_part1 = new_coord.y - bot.coord.y;
            way_part2 = new_coord.x - bot.coord.x - way_part1;
            num = (way_part1 + way_part2)/2 + 1; // --- //
            time1 = int(way_part1 / bot.speed * 1000) + 1;
            time2 = int(way_part2 / bot.speed * 1000);
            if(time1 < unthinkable_time)
                time2 = (unthinkable_time - time1 < time2 ? unthinkable_time - time1 : time2);
                        else
            {
                time1 = unthinkable_time;
                time2 = 0;
            }
            //if(time1 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x + 1 << "," << old_coord.y + 1 << ");\n";
            moveDownRight(time1);
            //if(time2 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x + 1 << "," << old_coord.y << ");\n";
            moveRight(time2);
            return;
        }
    }
    if( (new_coord.x <= bot.coord.x)&&(new_coord.y >= bot.coord.y ) )
    {
        //std::cout << "IF\n";
        if(bot.coord.x - new_coord.x < new_coord.y - bot.coord.y)
        {
            way_part1 = bot.coord.x - new_coord.x;
            way_part2 = new_coord.y - bot.coord.y - way_part1;
            num = (way_part1 + way_part2)/2 + 1; // --- //
            time1 = int(way_part1 / bot.speed * 1000) + 1;
            time2 = int(way_part2 / bot.speed * 1000);
            if(time1 < unthinkable_time)
                time2 = (unthinkable_time - time1 < time2 ? unthinkable_time - time1 : time2);
                        else
            {
                time1 = unthinkable_time;
                time2 = 0;
            }
            //if(time1 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x - 1 << "," << old_coord.y + 1 << ");\n";
            moveDownLeft(time1);
            //if(time2 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x << "," << old_coord.y + 1 << ");\n";
            moveDown(time2);
            return;
        }
                else
        {
            way_part1 = new_coord.y - bot.coord.y;
            way_part2 = bot.coord.x - new_coord.x - way_part1;
            num = (way_part1 + way_part2)/2 + 1; // --- //
            time1 = int(way_part1 / bot.speed * 1000) + 1;
            time2 = int(way_part2 / bot.speed * 1000);
            if(time1 < unthinkable_time)
                time2 = (unthinkable_time - time1 < time2 ? unthinkable_time - time1 : time2);
                        else
            {
                time1 = unthinkable_time;
                time2 = 0;
            }
            //if(time1 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x - 1 << "," << old_coord.y + 1 << ");\n";
            moveDownLeft(time1);
            //if(time1 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x - 1 << "," << old_coord.y << ");\n";
            moveLeft(time2);
            return;
        }
    }
    if( (new_coord.x <= bot.coord.x)&&(new_coord.y <= bot.coord.y ) )
    {
        //std::cout << "IF\n";
        if(bot.coord.x - new_coord.x < bot.coord.y - new_coord.y)
        {
            way_part1 = bot.coord.x - new_coord.x;
            way_part2 = bot.coord.y - new_coord.y - way_part1;
            num = (way_part1 + way_part2)/2 + 1; // --- //
            time1 = int(way_part1 / bot.speed * 1000) + 1;
            time2 = int(way_part2 / bot.speed * 1000);
            if(time1 < unthinkable_time)
                time2 = (unthinkable_time - time1 < time2 ? unthinkable_time - time1 : time2);
                        else
            {
                time1 = unthinkable_time;
                time2 = 0;
            }
            //if(time1 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x - 1 << "," << old_coord.y - 1 << ");\n";
            moveUpLeft(time1);
            //if(time2 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x << "," << old_coord.y - 1 << ");\n";
            moveUp(time2);
            return;
        }
                else
        {
            way_part1 = bot.coord.y - new_coord.y;
            way_part2 = bot.coord.x - new_coord.x - way_part1;
            num = (way_part1 + way_part2)/2 + 1; // --- //
            time1 = int(way_part1 / bot.speed * 1000) + 1;
            time2 = int(way_part2 / bot.speed * 1000);
            if(time1 < unthinkable_time)
                time2 = (unthinkable_time - time1 < time2 ? unthinkable_time - time1 : time2);
                        else
            {
                time1 = unthinkable_time;
                time2 = 0;
            }
            //if(time1 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x - 1 << "," << old_coord.y - 1 << ");\n";
            moveUpLeft(time1);
            //if(time2 != 0)
            //    std::cout << "Next_coord: (" << old_coord.x - 1 << "," << old_coord.y << ");\n";
            moveLeft(time2);
            return;
        }
    }
}

bool MoveToTheVictim::victimIsInTheCellsVector(std::vector<TVector> &cells_vector/*Here are indexes of cells*/,
                              std::vector<int> &x_coord, std::vector<int> &y_coord, TVector *victim_cell,
                                               TVector * victim_position)
{
    TVector min_coord, max_coord;
    for(int i = 0; i < (int)cells_vector.size(); i++)
    {
        min_coord.x = x_coord[cells_vector[i].x];
        min_coord.y = y_coord[cells_vector[i].y];
        max_coord.x = x_coord[cells_vector[i].x + 1];//
        max_coord.y = y_coord[cells_vector[i].y + 1];//
        for(int j = 0; j < (int)victims.size(); j++)
            if( (victims[j].coord.x >= min_coord.x)&&(victims[j].coord.x <= max_coord.x)&&
                    (victims[j].coord.y >= min_coord.y)&&(victims[j].coord.y <= max_coord.y) )
            {
                *victim_cell = cells_vector[i];
                victim_position->x = victims[j].coord.x;
                victim_position->y = victims[j].coord.y;
                //std::cout << "Victim is founded: (" << victim_position->x << ","
                //          << victim_position->y << ");\n";
                return true;
            }
    }
    //std::cout << "Victim isn't founded!\n";
    return false;
}

std::vector<TVector> MoveToTheVictim::wave(int **cells, TVector cell_coord, int MAX_Column, int MAX_Row)
{
    std::vector<TVector> next_cells;
    TVector tmp_vector;
    if( (cell_coord.x > 0)&&(cells[cell_coord.x - 1][cell_coord.y] == 0) )
    {
        tmp_vector.x = cell_coord.x - 1;
        tmp_vector.y = cell_coord.y;
        cells[tmp_vector.x][tmp_vector.y] = cells[cell_coord.x][cell_coord.y] + 1;
        next_cells.push_back(tmp_vector);
    }
    if( (cell_coord.y > 0)&&(cells[cell_coord.x][cell_coord.y - 1] == 0) )
    {
        tmp_vector.x = cell_coord.x;
        tmp_vector.y = cell_coord.y - 1;
        cells[tmp_vector.x][tmp_vector.y] = cells[cell_coord.x][cell_coord.y] + 1;
        next_cells.push_back(tmp_vector);
    }
    if( (cell_coord.x < MAX_Column)&&(cells[cell_coord.x + 1][cell_coord.y] == 0) )
    {
        tmp_vector.x = cell_coord.x + 1;
        tmp_vector.y = cell_coord.y;
        cells[tmp_vector.x][tmp_vector.y] = cells[cell_coord.x][cell_coord.y] + 1;
        next_cells.push_back(tmp_vector);
    }
    if( (cell_coord.y < MAX_Row)&&(cells[cell_coord.x][cell_coord.y + 1] == 0) )
    {
        tmp_vector.x = cell_coord.x;
        tmp_vector.y = cell_coord.y + 1;
        cells[tmp_vector.x][tmp_vector.y] = cells[cell_coord.x][cell_coord.y] + 1;
        next_cells.push_back(tmp_vector);
    }
    return next_cells;
}

void MoveToTheVictim::sortVector(std::vector<int> &vector)
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

bool MoveToTheVictim::pixelIsInTheWall(TVector pixel)
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

void MoveToTheVictim::moveUpRight(int time)
{
    if(time == 0)
        return;
    this->direction_bot->x++;
    this->direction_bot->y--;
    //bot_n->Move(DIRECTION_UP);
    lastVertDirection = DIRECTION_UP;
    //bot_n->Move(DIRECTION_RIGHT);
    lastHorDirection = DIRECTION_RIGHT;

}//miliseconds;

void MoveToTheVictim::moveDownRight(int time)
{
    if(time == 0)
        return;
    this->direction_bot->x++;
    this->direction_bot->y++;
    //bot_n->Move(DIRECTION_DOWN);
    lastVertDirection = DIRECTION_DOWN;
    //bot_n->Move(DIRECTION_RIGHT);
    lastHorDirection = DIRECTION_RIGHT;
}//

void MoveToTheVictim::moveDownLeft(int time)
{
    if(time == 0)
        return;
    this->direction_bot->x--;
    this->direction_bot->y++;
    //bot_n->Move(DIRECTION_DOWN);
    lastVertDirection = DIRECTION_DOWN;
    //bot_n->Move(DIRECTION_LEFT);
    lastHorDirection = DIRECTION_LEFT;
}//

void MoveToTheVictim::moveUpLeft(int time)
{
    if(time == 0)
        return;
    this->direction_bot->x--;
    this->direction_bot->y--;
    //bot_n->Move(DIRECTION_LEFT);
    lastVertDirection = DIRECTION_UP;
    //bot_n->Move(DIRECTION_UP);
    lastHorDirection = DIRECTION_LEFT;
}//

void MoveToTheVictim::moveUp(int time)
{
    if(time == 0)
        return;
    this->direction_bot->y--;
    //bot_n->Move(DIRECTION_UP);
    lastVertDirection = DIRECTION_UP;
}//

void MoveToTheVictim::moveRight(int time)
{
    if(time == 0)
        return;
    this->direction_bot->x++;
    //bot_n->Move(DIRECTION_RIGHT);
    lastHorDirection = DIRECTION_RIGHT;
}//

void MoveToTheVictim::moveDown(int time)
{
    if(time == 0)
        return;
    this->direction_bot->y++;
    //bot_n->Move(DIRECTION_DOWN);
    lastVertDirection = DIRECTION_DOWN;
}//

void MoveToTheVictim::moveLeft(int time)
{
    if(time == 0)
        return;
    this->direction_bot->x--;
    //bot_n->Move(DIRECTION_LEFT);
    lastHorDirection = DIRECTION_LEFT;
}//

/*void MoveToTheVictim::moveToTheNextPixel(Direction bot_direction)
{
    switch (bot_direction){
        case DIRECTION_DOWN:
                        this->moveDown(1);
                        break;
        case DIRECTION_LEFT:
                        this->moveLeft(1);
                        break;
        case DIRECTION_RIGHT:
                        this->moveRight(1);
                        break;
        case DIRECTION_UP:
                        this->moveUp(1);
                        break;
        default: break;
    }
}
*/
