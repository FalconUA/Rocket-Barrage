#include "game.h"
#include <iostream>
#include <sstream>

using namespace rbw;

Game::Game(std::string PlayerName){
    this->MyName = PlayerName;    
}

Game::Game(WorldSimulator * server, std::string PlayerName, sf::RenderWindow *window, Level * level){
    this->MyName = PlayerName;
    this->Init(server, window, level);
}

bool Game::Init(WorldSimulator * server, sf::RenderWindow *window, Level * level){
    this->Switch_mouse = false;
    this->mouse_object.type = rbw::Graphic::MOUSE_POINTER_NORMAL;
    window->setMouseCursorVisible(false);
    this->level = level;
    this->server = server;
    this->window = window;

    this->server->AddPlayer(this->MyName, rbw::TEAM_BLACK);
    this->server->AddPlayer("Bot", rbw::TEAM_WHITE);
    return true;
}

void Game::CheckKey(){

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->window);
    this->mouse_object.x = mouse_pos.x;
    this->mouse_object.y = mouse_pos.y;

    sf::Keyboard key;    
    if (key.isKeyPressed(sf::Keyboard::A)) this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_LEFT);
    if (key.isKeyPressed(sf::Keyboard::D)) this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_RIGHT);
    if (key.isKeyPressed(sf::Keyboard::W)) this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_UP);
    if (key.isKeyPressed(sf::Keyboard::S)) this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_DOWN);

    if (key.isKeyPressed(sf::Keyboard::Left)) this->server->AddMoveRequest("Bot", rbw::DIRECTION_LEFT);
    if (key.isKeyPressed(sf::Keyboard::Right)) this->server->AddMoveRequest("Bot", rbw::DIRECTION_RIGHT);
    if (key.isKeyPressed(sf::Keyboard::Up)) this->server->AddMoveRequest("Bot", rbw::DIRECTION_UP);
    if (key.isKeyPressed(sf::Keyboard::Down)) this->server->AddMoveRequest("Bot", rbw::DIRECTION_DOWN);

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
    }    
}

void Game::GenerateNextFrame(){
    float ElapsedTime = server->SimulateNextStep();    
    this->server->GetObjects(&this->Objects);
    this->level->Draw(*this->window);
    for (int i=0; i<this->Objects.size(); i++){
        GraphicObject tmp = Objects[i];
        //std::cout<<tmp.Name<<" "<<tmp.type<<" "<<tmp.x<<" "<<tmp.y<<std::endl;
        DrawGraphicObject(this->window, &Objects[i]);
    }    
    DrawGraphicObject(this->window, &this->mouse_object);


    float fps = 1000.f / ElapsedTime;
    int FPS = fps;

    std::ostringstream ss;
    ss << FPS;
    std::string s(ss.str());

    sf::Font font;
    font.loadFromFile("Resources/UbuntuMono-R.ttf");
    sf::Text text;
    text.setFont(font);
    text.setString(s);
    text.setColor(sf::Color::White);
    sf::Vector2u winSize = this->window->getSize();

    text.setPosition(sf::Vector2f( winSize.x - 100, 25 ));

    this->window->draw(text);

    if (server->RoundEnded())
        server->RoundDraw();

}
