#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <level.h>
#include <game.h>
#include <serveremulator.h>

const float FPS = 60.0f;

int main(){
    std::cout << "debug window\n\n";

    Level level;
    level.LoadFromFile("/home/hav4ik/Game-Development/Tanks/Tanks-local/Resources/test_new.tmx");
    int h = level.GetHeight();
    int w = level.GetWidth();

    sf::Keyboard key;
    sf::RenderWindow window(sf::VideoMode(w + 150, h), "Battle Tanks vodka edition");   
    //window.setMouseCursorVisible(false);

    window.setVerticalSyncEnabled(true); // call it once, after creating the window
    //window.setFramerateLimit(FPS); // call it once, after creating the window

    sf::Event event;

    rbw::WorldSimulator server;
    server.Init(&level, FPS);

    Game NewGame("FalconRT");
    NewGame.Init(&server, &window, &level);    

    sf::Clock clock;

    bool redraw = true;
    while (window.isOpen()){

        //Wait until 1/60th of a second has passed, then update everything.
        if (clock.getElapsedTime().asSeconds() >= 1.0f / FPS)
        {
            redraw = true; //We're ready to redraw everything
            clock.restart();
        }
        else //Sleep until next 1/60th of a second comes around
        {
            sf::Time sleepTime = sf::seconds((1.0f / FPS) - clock.getElapsedTime().asSeconds());
            sf::sleep(sleepTime);
        }

        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();
            NewGame.RespondToEvent(&event);
        }

        if (redraw){
        NewGame.CheckKey();

        window.clear();
        NewGame.GenerateNextFrame();        
        window.display();
        redraw = false;
        }
    }
    return 0;


}

/*
void check(Level * level, int * x, int * y){
    sf::Keyboard key;
    std::vector< Object > obj = level->GetObjects("wall");

    int circle_x = *x;
    int circle_y = *y;
    if (key.isKeyPressed(sf::Keyboard::W))
    {

        circle_y += -10;
        bool can_move = true;

        for (int i=0; i<obj.size(); i++){
            bool temp;

            int top = obj[i].rect.top;
            int left = obj[i].rect.left;
            int width = obj[i].rect.width;
            int height = obj[i].rect.height;
            int x0 = left;
            int y0 = top;
            int y1 = top + height;
            int x1 = left + width;

            temp = ( !((circle_y < y1) && (circle_y > y0) && (circle_x < x1) && (circle_x > x0)) );
            can_move = can_move && temp;            
        }
        if (!can_move) circle_y -= -10;
    }
    if (key.isKeyPressed(sf::Keyboard::S))
    {
        circle_y += 10;

        bool can_move = true;

        for (int i=0; i<obj.size(); i++){
            bool temp;

            int top = obj[i].rect.top;
            int left = obj[i].rect.left;
            int width = obj[i].rect.width;
            int height = obj[i].rect.height;
            int x0 = left;
            int y0 = top;
            int y1 = top + height;
            int x1 = left + width;            

            temp = ( !((circle_y < y1) && (circle_y > y0) && (circle_x < x1) && (circle_x > x0)) );
            can_move = can_move && temp;
        }
        if (!can_move) circle_y -= 10;
    }
    if (key.isKeyPressed(sf::Keyboard::A))
    {
        circle_x += -10;
        if (circle_x < 0) circle_x = 0;
        bool can_move = true;

        for (int i=0; i<obj.size(); i++){
            bool temp;

            int top = obj[i].rect.top;
            int left = obj[i].rect.left;
            int width = obj[i].rect.width;
            int height = obj[i].rect.height;
            int x0 = left;
            int y0 = top;
            int y1 = top + height;
            int x1 = left + width;          

            temp = ( !((circle_y < y1) && (circle_y > y0) && (circle_x < x1) && (circle_x > x0)) );
            can_move = can_move && temp;
        }
        if (!can_move) circle_x -= -10;
    }
    if (key.isKeyPressed(sf::Keyboard::D))
    {
        circle_x += 10;
        bool can_move = true;

        for (int i=0; i<obj.size(); i++){
            bool temp;

            int top = obj[i].rect.top;
            int left = obj[i].rect.left;
            int width = obj[i].rect.width;
            int height = obj[i].rect.height;
            int x0 = left;
            int y0 = top;
            int y1 = top + height;
            int x1 = left + width;

            temp = ( !((circle_y < y1) && (circle_y > y0) && (circle_x < x1) && (circle_x > x0)) );
              can_move = can_move && temp;
        }
        if (!can_move) circle_x -= 10;
    }
    *x = circle_x;
    *y = circle_y;
}


int main()
{
    cout << "------- debug console! ----------- \n";


    Level level;
    level.LoadFromFile("/home/hav4ik/Game-Development/Tanks/Tanks-local/Resources/test.tmx");

    sf::CircleShape circle(16);
    circle.setFillColor(sf::Color::Blue);

    int circle_x = 45.0, circle_y = 45.0;

    int h = level.GetHeight();
    int w = level.GetWidth();


    sf::Keyboard key;
    sf::RenderWindow window(sf::VideoMode(w, h), "Battle Tanks vodka edition");


    window.setVerticalSyncEnabled(true); // call it once, after creating the window
    window.setFramerateLimit(60); // call it once, after creating the window

    sf::Event event;

    while (window.isOpen()){
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();                        

        }
        check(&level, &circle_x, &circle_y);

        circle.setPosition(circle_x-16, circle_y-16);
        window.clear();
        level.Draw(window);
        window.draw(circle);
        window.display();
    }
    return 0;
}
*/
