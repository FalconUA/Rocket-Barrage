#include "game.h"
#include <iostream>
#include <sstream>

using namespace rbw;

Game::Game(std::string PlayerName){
    this->MyName = PlayerName;    
    moveToTheVictim = new MoveToTheVictim(this->server);  //it's new

    moveToTheVictim_first_team = new MoveToTheVictim(this->server);  //it's new
    moveToTheVictim_second_team = new MoveToTheVictim(this->server);  //it's new
}

Game::Game(WorldSimulator * server, std::string PlayerName, sf::RenderWindow *window, Level * level){
    this->MyName = PlayerName;
    this->Init(server, window, level);
}

//---------------------------------------it's new-----------------------------------//
std::vector<TRectangle> Game::getWalls()
{
    std::vector< Object > walls = this->server->worldInfo.wallForBots;
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
//----------------------------------------------------------------------------------//

bool Game::Init(WorldSimulator * server, sf::RenderWindow *window, Level * level){
    this->Switch_mouse = false;
    this->ShowScore = false;
    this->mouse_object.type = rbw::Graphic::MOUSE_POINTER_NORMAL;
    window->setMouseCursorVisible(false);
    this->level = level;
    this->server = server;
    this->window = window;

    this->graphic = new rbw::GraphicEngine;
    this->graphic->initOutputWindow(this->window);
    this->graphic->initModels();
    this->graphic->initAnimations();

    this->moveToTheVictim->walls = this->getWalls();// ------------it's new //
    this->moveToTheVictim_first_team->walls = this->getWalls();// ------------it's new //
    this->moveToTheVictim_second_team->walls = this->getWalls();// ------------it's new //

    this->server->AddPlayer(this->MyName, rbw::TEAM_BLACK, false);//it's new(player isn't a bot - false)
    this->server->AddPlayer("Bot", rbw::TEAM_WHITE, true);//it's new(player is bot - true)
    this->server->AddPlayer("Bot2", rbw::TEAM_WHITE, true);//it's new(player is bot - true)
    this->server->AddPlayer("My_Bot1", rbw::TEAM_BLACK, true);//it's new(player isn't a bot - false)

    this->winSize = this->window->getSize();
    return true;
}

//---------------------------------------it's new-------------------------------------//
TSafeDirections Game::getSafeDirections(rbw::Player botyara, TVector real_speed)
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
    /*TDirectionPair direction_pair[3][3] = {{{rbw::DIRECTION_UP,rbw::DIRECTION_LEFT},
                                            {rbw::DIRECTION_UP,rbw::DIRECTION_NODIRECTION},
                                            {rbw::DIRECTION_UP,rbw::DIRECTION_RIGHT}},
                                           {{rbw::DIRECTION_NODIRECTION,rbw::DIRECTION_LEFT},
                                            {rbw::DIRECTION_NODIRECTION,rbw::DIRECTION_NODIRECTION},
                                            {rbw::DIRECTION_NODIRECTION,rbw::DIRECTION_RIGHT}},
                                           {{rbw::DIRECTION_DOWN,rbw::DIRECTION_LEFT},
                                            {rbw::DIRECTION_DOWN,rbw::DIRECTION_NODIRECTION},
                                            {rbw::DIRECTION_DOWN,rbw::DIRECTION_RIGHT}}};*/
    EvadeFromTheRocket stells_system(botyara.server);
    std::vector<TDirectionPair> directions = stells_system.saveDirection(botyara,real_speed);
    for(int i = 0; i < (int)directions.size(); i++)
    {
        if((directions[i].vert == rbw::DIRECTION_UP)&&(directions[i].hor == rbw::DIRECTION_LEFT)) safeDirections.Up_Left = true;
        if((directions[i].vert == rbw::DIRECTION_UP)&&(directions[i].hor == rbw::DIRECTION_NODIRECTION)) safeDirections.Up = true;
        if((directions[i].vert == rbw::DIRECTION_UP)&&(directions[i].hor == rbw::DIRECTION_RIGHT)) safeDirections.Up_Right = true;
        if((directions[i].vert == rbw::DIRECTION_NODIRECTION)&&(directions[i].hor == rbw::DIRECTION_LEFT)) safeDirections.Left = true;
        if((directions[i].vert == rbw::DIRECTION_NODIRECTION)&&(directions[i].hor == rbw::DIRECTION_NODIRECTION)) safeDirections.None = true;
        if((directions[i].vert == rbw::DIRECTION_NODIRECTION)&&(directions[i].hor == rbw::DIRECTION_RIGHT)) safeDirections.Right = true;
        if((directions[i].vert == rbw::DIRECTION_DOWN)&&(directions[i].hor == rbw::DIRECTION_LEFT)) safeDirections.Down_Left = true;
        if((directions[i].vert == rbw::DIRECTION_DOWN)&&(directions[i].hor == rbw::DIRECTION_NODIRECTION)) safeDirections.Down = true;
        if((directions[i].vert == rbw::DIRECTION_DOWN)&&(directions[i].hor == rbw::DIRECTION_RIGHT)) safeDirections.Down_Right = true;
    }
    directions.clear();
    return safeDirections;
}

void Game::botyara(MoveToTheVictim * moveToTheVictim,
                   std::vector< TPair_PlayerDirection > &directions,
                   rbw::Team team_name, bool shoot_function)
{
    sf::Vector2i direction_bot(0,0);//it's new
    TPair_PlayerDirection bot_name_direction;

    rbw::Player * bot;
    TPlayer _bot;
    std::vector< rbw::Player* > _bots;
    sf::Vector2f tmp;

    for(int i = 0; i < (int)this->server->worldInfo.Players.size(); i++)
    {
        bot = this->server->worldInfo.Players[i];
        if(!bot->isAlive())
            continue;
        tmp = bot->GetPosition();
        _bot.coord.x = int(tmp.x);
        _bot.coord.y = int(tmp.y);
        _bot.speed = 1;
        _bot.name = bot->GetPlayerName();
        if((bot->bot())&&(bot->GetTeam() == team_name))
        {
            moveToTheVictim->bots.push_back(_bot);
            _bots.push_back(bot);
            //bot_name_direction.playerName = bot->GetPlayerName();
            //bot_name_direction.direction = direction_bot;
            //directions.push_back(bot_name_direction);
        }
                else
        {
            if(bot->GetTeam() != team_name)
            {
                moveToTheVictim->victims.push_back(_bot);

                //bot_name_direction.playerName = bot->GetPlayerName();
               // bot_name_direction.direction = direction_bot;
                //directions.push_back(bot_name_direction);

                //player_directions.push_back(direction_player);
            }
        }
    }

    TVector victim_position;
    for(int i = 0; i < (int)moveToTheVictim->bots.size(); i++)
    {
        sf::Vector2f speed = _bots[i]->GetSpeed();

        TVector real_speed;
        real_speed.x = int(speed.x);
        real_speed.y = int(speed.y);
        //---------------------------//
        //TSafeDirections safedirections = this->getSafeDirections(*(_bots[i]),real_speed);

        TSafeDirections safedirections;
        safedirections.Down = true;
        safedirections.Down_Left = true;
        safedirections.Down_Right = true;
        safedirections.Left = true;
        safedirections.None = true;
        safedirections.Right = true;
        safedirections.Up = true;
        safedirections.Up_Left = true;
        safedirections.Up_Right = true;

        _bots[i]->SetAdmissibleDirections(safedirections);
        //std::cout << safedirections.Up_Left << safedirections.Up << safedirections.Up_Right <<
        //        safedirections.Left << safedirections.None << safedirections.Right <<
        //        safedirections.Down_Left << safedirections.Down << safedirections.Down_Right << "\n";

        moveToTheVictim->moveToTheVictim(moveToTheVictim->bots[i],_bots[i],&victim_position,&direction_bot/*&bot_directions[i]*/);

        bot_name_direction.playerName = _bots[i]->GetPlayerName();
        std::cout << _bots[i]->GetPlayerName() << " position: " << (int)_bots[i]->GetPosition().x << "," << (int)_bots[i]->GetPosition().y << "\n";
        bot_name_direction.direction = direction_bot;
        directions.push_back(bot_name_direction);

        direction_bot.x = 0;
        direction_bot.y = 0;

        if(shoot_function)
        {
            BotShoot bot_shoot(this->server,moveToTheVictim->victims,moveToTheVictim->bots[i].coord);
            TVector shootCoord = bot_shoot.getVictimCoord();
            if( (shootCoord.x != 0)||(shootCoord.y != 0) )
            {
                this->server->AddHomingMissile(moveToTheVictim->bots[i].name,sf::Vector2i(shootCoord.x,shootCoord.y));
                this->server->AddBouncingBomb(moveToTheVictim->bots[i].name, sf::Vector2i(shootCoord.x,shootCoord.y));
            }
        }
    }
    moveToTheVictim->bots.clear();
    moveToTheVictim->victims.clear();
    _bots.clear();
}

//---------------------------------------------------------------------------------------//


void Game::CheckKey(){

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->window);
    this->mouse_object.x = mouse_pos.x;
    this->mouse_object.y = mouse_pos.y;

    sf::Vector2i direction_player(0,0);
    sf::Vector2i direction_bot(0,0);//it's new
    //std::vector< sf::Vector2i > player_directions;
    //std::vector< sf::Vector2i > bot_directions;//it's new

    sf::Keyboard key;    
    this->ShowScore = (key.isKeyPressed(sf::Keyboard::Tab));
    if (key.isKeyPressed(sf::Keyboard::A)) direction_player.x--;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_LEFT);
    if (key.isKeyPressed(sf::Keyboard::D)) direction_player.x++;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_RIGHT);
    if (key.isKeyPressed(sf::Keyboard::W)) direction_player.y--;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_UP);
    if (key.isKeyPressed(sf::Keyboard::S)) direction_player.y++;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_DOWN);

    std::vector< TPair_PlayerDirection > move_directions;//it's new
    rbw::Team team1, team2;
    for(int i = 0; i < (int)this->server->worldInfo.Players.size(); i++)
        if(this->server->worldInfo.Players[i]->GetPlayerName() == this->MyName)
        {
            team1 = this->server->worldInfo.Players[i]->GetTeam();
            break;
        }
    for(int i = 0; i < (int)this->server->worldInfo.Players.size(); i++)
        if(this->server->worldInfo.Players[i]->GetTeam() != team1)
        {
            team2 = this->server->worldInfo.Players[i]->GetTeam();
            break;
        }
    this->botyara(this->moveToTheVictim_first_team,move_directions,team1,true);
    this->botyara(this->moveToTheVictim_second_team,move_directions,team2,true);
    //-----------------------------------it's new------------------------------------//
//    rbw::Player * bot;
//    TPlayer _bot;
//    std::vector< rbw::Player* > _bots;
//    sf::Vector2f tmp;

//    for(int i = 0; i < (int)this->server->worldInfo.Players.size(); i++)
//    {
//        bot = this->server->worldInfo.Players[i];
//        if(!bot->isAlive())
//            continue;
//        tmp = bot->GetPosition();
//        _bot.coord.x = int(tmp.x);
//        _bot.coord.y = int(tmp.y);
//        _bot.speed = 1;
//        _bot.name = bot->GetPlayerName();
//        if(/*this->server->worldInfo.Players[i]*/bot->bot())
//        {
//            this->moveToTheVictim->bots.push_back(_bot);
//            _bots.push_back(bot);
//            bot_directions.push_back(direction_bot);
//        }
//                else
//        {
//            this->moveToTheVictim->victims.push_back(_bot);
//            player_directions.push_back(direction_player);
 //       }
 //   }

    //std::vector< rbw::Player* > _bots;
    //for(int i = 0; i < (int)this->server->worldInfo.Players.size(); i++)
    //    if(this->server->worldInfo.Players[i]->bot())
    //        _bots.push_back(this->server->worldInfo.Players[i]);

//    TVector victim_position;
//    for(int i = 0; i < (int)this->moveToTheVictim->bots.size(); i++)
//    {
//        sf::Vector2f speed = _bots[i]->GetSpeed();
//
//        TVector real_speed;
//        real_speed.x = int(speed.x);
//        real_speed.y = int(speed.y);
//        //---------------------------//
//        TSafeDirections directions = this->getSafeDirections(*(_bots[i]),real_speed);
 //       _bots[i]->SetAdmissibleDirections(directions);
 //       std::cout << directions.Up_Left << directions.Up << directions.Up_Right <<
 //               directions.Left << directions.None << directions.Right <<
 //               directions.Down_Left << directions.Down << directions.Down_Right << "\n";

//        /*if(*/this->moveToTheVictim->moveToTheVictim(this->moveToTheVictim->bots[i],_bots[i],&victim_position,&bot_directions[i]/*direction_bot*/);/*)*/
//        //    this->server->AddBouncingBomb(moveToTheVictim->bots[i].name,sf::Vector2i(victim_position.x,victim_position.y));
/*
        BotShoot bot_shoot(this->server,this->moveToTheVictim->victims,this->moveToTheVictim->bots[i].coord);
        TVector shootCoord = bot_shoot.getVictimCoord();
        if( (shootCoord.x != 0)||(shootCoord.y != 0) )
        {
            this->server->AddHomingMissile(this->moveToTheVictim->bots[i].name,sf::Vector2i(shootCoord.x,shootCoord.y));
            this->server->AddBouncingBomb(this->moveToTheVictim->bots[i].name, sf::Vector2i(shootCoord.x,shootCoord.y));
        }*/
//    }
//    this->moveToTheVictim->bots.clear();
//    this->moveToTheVictim->victims.clear();
//    _bots.clear();
    //-----------------------------------------------------------------//

    if (key.isKeyPressed(sf::Keyboard::Left)) direction_bot.x--;//this->server->AddMoveRequest("Bot", rbw::DIRECTION_LEFT);
    if (key.isKeyPressed(sf::Keyboard::Right)) direction_bot.x++;//this->server->AddMoveRequest("Bot", rbw::DIRECTION_RIGHT);
    if (key.isKeyPressed(sf::Keyboard::Up)) direction_bot.y--;//this->server->AddMoveRequest("Bot", rbw::DIRECTION_UP);
    if (key.isKeyPressed(sf::Keyboard::Down)) direction_bot.y++;//this->server->AddMoveRequest("Bot", rbw::DIRECTION_DOWN);

    while(!move_directions.empty())
    {
        this->server->AddMoveRequest(move_directions[0].playerName,move_directions[0].direction);
        for(int j = 0; j < (int)this->server->getWorldInfo()->Players.size(); j++)
            if((this->server->worldInfo.Players[j]->GetPlayerName() == move_directions[0].playerName)&&
                    (this->server->getWorldInfo()->Players[j]->isAlive()) )
            {
                this->server->worldInfo.Players[j]->lastDirection = move_directions[0].direction;
                break;
            }
        move_directions.erase(move_directions.begin());
    }

    for(int j = 0; j < (int)this->server->worldInfo.Players.size(); j++)
                    if( (!this->server->worldInfo.Players[j]->bot())&&(this->server->getWorldInfo()->Players[j]->isAlive()) )
            this->server->AddMoveRequest(this->server->worldInfo.Players[j]->GetPlayerName(), direction_player);

//    for(int i = 0; i < (int)this->server->worldInfo.Players.size(); i++)
//        if(this->server->worldInfo.Players[i]->bot())
//        {
//            //this->server->AddMoveRequest(this->server->worldInfo.Players[i]->GetPlayerName(), bot_directions[0]);
//            this->server->AddMoveRequest(move_directions[0].playerName,move_directions[0].direction);
//            this->server->worldInfo.Players[i]->lastDirection = move_directions[0].direction;
//            move_directions.erase(move_directions.begin());
//        }
//                else
//        {
//            this->server->AddMoveRequest(this->server->worldInfo.Players[i]->GetPlayerName(), direction_player);
//            //player_directions.erase(player_directions.begin());
//        }

    /*this->server->AddMoveRequest(this->MyName, direction_player);
    this->server->AddMoveRequest("Bot", direction_bot);*/

    if (key.isKeyPressed(sf::Keyboard::R)){        
        this->server->AddHomingMissile(this->MyName, sf::Vector2i(mouse_pos.x, mouse_pos.y));
    }
    //if (key.isKeyPressed(sf::Keyboard::Space)){
    //    this->server->AddHomingMissile(this->MyName, sf::Vector2i(mouse_pos.x, mouse_pos.y));
    //}
    //this->server->AddMoveRequest(this->MyName, std::make_pair(a,b));
}

void Game::RespondToEvent(sf::Event *event){
    if (event->type == sf::Event::MouseButtonPressed){
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->window);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)){
            // left mouse clicked
            std::cout << "right mousepressed "<<mouse_pos.x<<" "<<mouse_pos.y<<std::endl;
            this->server->AddBouncingBomb(this->MyName, sf::Vector2i(mouse_pos.x, mouse_pos.y));
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            // left mouse clicked
            std::cout << "left mousepressed "<<mouse_pos.x<<" "<<mouse_pos.y<<std::endl; //it's new
            if (!this->Switch_mouse)
                this->server->AddHomingMissile(this->MyName, sf::Vector2i(mouse_pos.x, mouse_pos.y));
            else
            {
                this->server->AddGrenade(this->MyName, sf::Vector2i(mouse_pos.x, mouse_pos.y));
                this->mouse_object.type = rbw::Graphic::MOUSE_POINTER_NORMAL;
                this->Switch_mouse = false;
            }
        }
    }
    if (event->type == sf::Event::KeyPressed){
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->window);
        if (event->key.code == sf::Keyboard::Space){
            this->Switch_mouse = !this->Switch_mouse;
            this->mouse_object.type = (this->Switch_mouse)? rbw::Graphic::MOUSE_POINTER_SWITCHED : rbw::Graphic::MOUSE_POINTER_NORMAL;
        }
        if (event->key.code == sf::Keyboard::M)
            this->server->RoundDraw();
        if (event->key.code == sf::Keyboard::Escape)
            this->window->close();
    }    
}

void Game::GenerateNextFrame(){

    //-----------------------------------it's new------------------------------------//
//    sf::Vector2i direction_bot(0,0);
//    Player * bot;
//    TPlayer _bot;
//    sf::Vector2f tmp;

//    for(int i = 0; i < (int)this->server->getWorldInfo()->Players.size(); i++)
//    {
//        bot = this->server->getWorldInfo()->Players[i];
//        tmp = bot->GetPosition();
//        _bot.coord.x = int(tmp.x);
//        _bot.coord.y = int(tmp.y);
//        _bot.speed = 1;
//        _bot.name = bot->GetPlayerName();
//        if(this->server->getWorldInfo()->Players[i]->bot())
//            moveToTheVictim->bots.push_back(_bot);
//                else
//            moveToTheVictim->victims.push_back(_bot);
//    }

//    std::vector< rbw::Player* > _bots;
//    for(int i = 0; i < (int)this->server->getWorldInfo()->Players.size(); i++)
//        if(this->server->getWorldInfo()->Players[i]->bot())
//            _bots.push_back(this->server->getWorldInfo()->Players[i]);

//    TVector victim_position;
//    for(int i = 0; i < (int)moveToTheVictim->bots.size(); i++){
//        if(moveToTheVictim->moveToTheVictim(moveToTheVictim->bots[i],_bots[i],&victim_position,&direction_bot))
//            this->server->AddBouncingBomb(moveToTheVictim->bots[i].name,sf::Vector2i(victim_position.x,victim_position.y));
 //       this->server->AddMoveRequest(moveToTheVictim->bots[i].name, direction_bot);
//    }
//    moveToTheVictim->bots.clear();
//    moveToTheVictim->victims.clear();
    //-----------------------------------------------------------------//


    float ElapsedTime = server->SimulateNextStep();    

    this->server->GetObjects(&this->Objects);    

    std::vector< rbw::PlayerExportInformation > pInfo = this->server->ExportPlayerInfo();


    this->level->Draw(*this->window);
    this->graphic->Render(this->Objects);

    /*
    for (int i=0; i<this->Objects.size(); i++){
        GraphicObject tmp = Objects[i];
        //std::cout<<tmp.Name<<" "<<tmp.type<<" "<<tmp.x<<" "<<tmp.y<<std::endl;
        rbw::DrawGraphicObject(this->window, &Objects[i]);
    }        
    */

    rbw::DrawGraphicObject(this->window, &this->mouse_object);
    if (this->ShowScore){
        this->graphic->ShowScoreTable(this->server->ExportPlayerInfo());
    }
    this->graphic->ShowEventList(this->server->ExportEvents());

    float fps = 1000.f / ElapsedTime;
    int FPS = fps;

    std::ostringstream ss;
    ss << FPS;
    std::string s(ss.str());

    sf::Font font;
    font.loadFromFile("Resources/fonts/UbuntuMono-R.ttf");
    sf::Text text;
    text.setFont(font);
    text.setString("FPS: " + s);
    text.setColor(sf::Color::White);

    //sf::Vector2u winSize = this->window->getSize();
    text.setPosition(sf::Vector2f(this->winSize.x - 150, 25 ));


    this->window->draw(text);    

    rbw::Team winningTeam;
    if (server->RoundEnded(&winningTeam))
        server->RoundDraw();

}
