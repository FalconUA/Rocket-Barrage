#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <level.h>
#include <game.h>
#include <stdio.h>

const float FPS = 70.0f;

int main(int argc, char * argv[]){
    //char basePath[255] = "";
    //_fullpath(basePath, argv[0], sizeof(basePath));

    std::cout << "debug window\n\n";

    Level level;
    level.LoadFromFile("Resources/desert.tmx");
    int h = level.GetHeight();
    int w = level.GetWidth();

    sf::RenderWindow window(sf::VideoMode(w + 200, h), "ROCKET BARRAGE");
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


