#include "game.h"
#include <iostream>
#include <sstream>

using namespace rbw;

Game::Game(std::string PlayerName){
    this->MyName = PlayerName;    
    moveToTheVictim = new MoveToTheVictim;  //it's new
}

Game::Game(WorldSimulator * server, std::string PlayerName, sf::RenderWindow *window, Level * level){
    this->MyName = PlayerName;
    this->Init(server, window, level);
}

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

    moveToTheVictim->walls = this->getWalls();// ------------it's new //

    this->server->AddPlayer(this->MyName, rbw::TEAM_BLACK,false);
    //this->server->AddPlayer("Bot0", rbw::TEAM_WHITE,false);
    //this->server->AddPlayer("Bot1", rbw::TEAM_WHITE,false);

    this->winSize = this->window->getSize();
    return true;
}

//---------------------------------------it's new-----------------------------------//
std::vector<TRectangle> Game::getWalls()
{
    std::vector< Object > walls = this->server->getWorldInfo()->wallForPlayer;
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
//----------------------------------------------------------------------------------//


void Game::CheckKey(){

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->window);
    this->mouse_object.x = mouse_pos.x;
    this->mouse_object.y = mouse_pos.y;

    sf::Vector2i direction_player(0,0);
    sf::Vector2i direction_bot(0,0);

    sf::Keyboard key;    
    this->ShowScore = (key.isKeyPressed(sf::Keyboard::Tab));
    if (key.isKeyPressed(sf::Keyboard::A)) direction_player.x--;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_LEFT);
    if (key.isKeyPressed(sf::Keyboard::D)) direction_player.x++;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_RIGHT);
    if (key.isKeyPressed(sf::Keyboard::W)) direction_player.y--;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_UP);
    if (key.isKeyPressed(sf::Keyboard::S)) direction_player.y++;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_DOWN);

    if (key.isKeyPressed(sf::Keyboard::Left)) direction_bot.x--;//this->server->AddMoveRequest("Bot", rbw::DIRECTION_LEFT);
    if (key.isKeyPressed(sf::Keyboard::Right)) direction_bot.x++;//this->server->AddMoveRequest("Bot", rbw::DIRECTION_RIGHT);
    if (key.isKeyPressed(sf::Keyboard::Up)) direction_bot.y--;//this->server->AddMoveRequest("Bot", rbw::DIRECTION_UP);
    if (key.isKeyPressed(sf::Keyboard::Down)) direction_bot.y++;//this->server->AddMoveRequest("Bot", rbw::DIRECTION_DOWN);

    this->server->AddMoveRequest(this->MyName, direction_player);
    this->server->AddMoveRequest("Bot0", direction_bot);

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
            std::cout << "right mousepressed "<<mouse_pos.x<<" "<<mouse_pos.y<<std::endl;
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
    sf::Vector2i direction_bot(0,0);
    Player * bot;
    TPlayer _bot;
    sf::Vector2f tmp;

    for(int i = 0; i < (int)this->server->getWorldInfo()->Players.size(); i++)
    {
        bot = this->server->getWorldInfo()->Players[i];
        tmp = bot->GetPosition();
        _bot.coord.x = int(tmp.x);
        _bot.coord.y = int(tmp.y);
        _bot.speed = 1;
        _bot.name = bot->GetPlayerName();
        if(this->server->getWorldInfo()->Players[i]->bot())
            moveToTheVictim->bots.push_back(_bot);
                else
            moveToTheVictim->victims.push_back(_bot);
    }

    std::vector< rbw::Player* > _bots;
    for(int i = 0; i < (int)this->server->getWorldInfo()->Players.size(); i++)
        if(this->server->getWorldInfo()->Players[i]->bot())
            _bots.push_back(this->server->getWorldInfo()->Players[i]);

    TVector victim_position;
    for(int i = 0; i < (int)moveToTheVictim->bots.size(); i++){
        if(moveToTheVictim->moveToTheVictim(moveToTheVictim->bots[i],_bots[i],&victim_position,&direction_bot))
            this->server->AddBouncingBomb(moveToTheVictim->bots[i].name,sf::Vector2i(victim_position.x,victim_position.y));
        this->server->AddMoveRequest(moveToTheVictim->bots[i].name, direction_bot);
    }
    moveToTheVictim->bots.clear();
    moveToTheVictim->victims.clear();
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
