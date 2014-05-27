#include "gamefield.h"
#include "QApplication"
#include <sstream>
#include <string>

GameField::GameField(QObject *client, bool isSingleGame):client(client),TimeStep(0.5*1000),isSingleGame(isSingleGame)
{
    this->renderInfo.isSinglePlayer = isSingleGame;
    this->renderInfo.FPS = 70.0f;

    std::cout << "debug window\n\n";

    this->renderInfo.level.LoadFromFile("Resources/maps/street_new.tmx");
    int h = this->renderInfo.level.GetHeight();
    int w = this->renderInfo.level.GetWidth();

    this->renderInfo.window = new sf::RenderWindow(sf::VideoMode(w + 200, h+20), "ROCKET BARRAGE", sf::Style::None);
    this->renderInfo.originalWinSize = this->renderInfo.window->getSize();

    std::cout << "window Created" << std::endl;
    this->renderInfo.window->setActive(false);

    this->renderInfo.window->setVerticalSyncEnabled(true); // call it once, after creating the window
    //window.setFramerateLimit(FPS); // call it once, after creating the window


    this->renderInfo.window->setMouseCursorVisible(false);

    this->renderInfo.Switch_Mouse = false;
    this->renderInfo.ShowScore = false;
    this->renderInfo.Spray_Fire = false;
    this->renderInfo.mouseObject.type = rbw::Graphic::MOUSE_POINTER_NORMAL;

    this->renderInfo.graphic = new rbw::GraphicEngine;
    this->renderInfo.graphic->initOutputWindow(this->renderInfo.window);
    this->renderInfo.graphic->initModels();
    this->renderInfo.graphic->initAnimations();

    if (this->renderInfo.isSinglePlayer){
        this->renderInfo.world = new rbw::WorldSimulator;
        this->renderInfo.world->Init(&(this->renderInfo.level), this->renderInfo.FPS);

        this->renderInfo.moveToTheVictim = new MoveToTheVictim(this->renderInfo.world);
        this->renderInfo.moveToTheVictim_first_team = new MoveToTheVictim(this->renderInfo.world);  //it's new
        this->renderInfo.moveToTheVictim_second_team = new MoveToTheVictim(this->renderInfo.world);  //it's new

        this->renderInfo.botcount = 0;

        this->renderInfo.moveToTheVictim->walls = this->getWalls();
        this->renderInfo.moveToTheVictim_first_team->walls = this->getWalls();// ------------it's new //
        this->renderInfo.moveToTheVictim_second_team->walls = this->getWalls();// ------------it's new //
    }
    this->renderInfo.simClock.restart();

    this->renderInfo.window->setSize(sf::Vector2u(sf::VideoMode::getDesktopMode().width,
                                                  sf::VideoMode::getDesktopMode().height));
    this->renderInfo.window->setPosition(sf::Vector2i(0,0));

    std::cout << "init completed!" << std::endl;
}
GameField::~GameField()
{

}

std::vector<TRectangle> GameField::getWalls()
{
    std::vector< Object > walls = this->renderInfo.world->getWorldInfo()->wallForBots;
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

std::vector< rbw::GraphicObject > goVector_from_string(std::string charArray)
{
    std::vector< rbw::GraphicObject > result;
    std::stringstream buffer(charArray);
    int count;
    buffer >> count;
    for (int i=0; i<count; i++){
        rbw::GraphicObject newGraphicObject;

        int tmp;
        buffer >> newGraphicObject.x;
        buffer >> newGraphicObject.y;
        buffer >> newGraphicObject.velocity_x;
        buffer >> newGraphicObject.velocity_y;
        buffer >> tmp;
            newGraphicObject.type = (rbw::Graphic::GraphicObjectType) tmp;
        buffer >> newGraphicObject.Name;
        buffer >> tmp;
            newGraphicObject.team = (rbw::Team) tmp;
        buffer >> newGraphicObject.HealthPoint;
        buffer >> newGraphicObject.zoom_coefficient;

        result.push_back(newGraphicObject);
    }
    return result;
}
std::vector< rbw::PlayerExportInformation > peiVector_from_string(std::string charArray)
{
    std::vector< rbw::PlayerExportInformation > result;
    std::stringstream buffer(charArray);
    int count;
    buffer >> count;
    for (int i=0; i<count; i++){
        rbw::PlayerExportInformation newPei;

        int tmp;
        buffer >> newPei.PlayerName;
        buffer >> tmp;
            newPei.team = (rbw::Team) tmp;
        buffer >> newPei.Kill;
        buffer >> newPei.Death;
        buffer >> newPei.DamageDealt;
        buffer >> newPei.HomingMissilesLeft;
        buffer >> newPei.BouncingBombsLeft;
        buffer >> newPei.GrenadesLeft;
        buffer >> newPei.isDead;

        result.push_back(newPei);
    }
    return result;
}

void GameField::run()
{        
    sf::Clock clock;

    bool redraw = true;
    while (this->renderInfo.window->isOpen()){

        //Wait until 1/60th of a second has passed, then update everything.
        if (clock.getElapsedTime().asSeconds() >= 1.0f / this->renderInfo.FPS)
        {
            redraw = true; //We're ready to redraw everything
            clock.restart();
        }
        else //Sleep until next 1/60th of a second comes around
        {
            sf::Time sleepTime = sf::seconds((1.0f / this->renderInfo.FPS) - clock.getElapsedTime().asSeconds());
            sf::sleep(sleepTime);
        }

        while (this->renderInfo.window->pollEvent(this->renderInfo.event)){
            if (this->renderInfo.event.type == sf::Event::Closed)
                this->renderInfo.window->close();
            this->CheckEvents();
        }

        if (redraw){
            this->CheckKeyboard();
            this->renderInfo.window->clear();
            this->MoveThisWorld();
            this->renderInfo.window->display();
            redraw = false;
        }
        QApplication::processEvents();
    }


    sig_EndGame();//конец игри
}
void GameField::slotPause(bool b)
{
    pause=b;
}
bool GameField::MoveThisWorld()
{    
    std::cout << "start moving" << std::endl;
    if (this->renderInfo.isSinglePlayer == false){
        float ElapsedTime = this->renderInfo.simClock.restart().asMilliseconds();
        this->renderInfo.level.Draw(*(this->renderInfo.window));
        this->renderInfo.graphic->Render(this->goVector);
        std::cout << "goVector length: " << this->goVector.size() << std::endl;
        rbw::DrawGraphicObject(this->renderInfo.window, &(this->renderInfo.mouseObject));
        if (this->renderInfo.ShowScore){
            this->renderInfo.graphic->ShowScoreTable(this->peiVector);
        }
        this->renderInfo.graphic->ShowEventList(this->stVector);

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

        text.setPosition(sf::Vector2f(this->renderInfo.originalWinSize.x - 150, 25 ));


        this->renderInfo.window->draw(text);
    }
    else
    {

        //-----------------------------------it's new------------------------------------//
        sf::Vector2i direction_bot(0,0);
        Player * bot;
        TPlayer _bot;
        sf::Vector2f tmp;

        std::vector< TPair_PlayerDirection > move_directions;//it's new
        rbw::Team team1, team2;
        team1 = rbw::TEAM_BLACK;
        team2 = rbw::TEAM_WHITE;
        this->botyara(this->renderInfo.moveToTheVictim_first_team,move_directions,team1,true);
        this->botyara(this->renderInfo.moveToTheVictim_second_team,move_directions,team2,true);
        //-----------------------------------it's new------------------------------------//

        while(!move_directions.empty())
        {
            this->renderInfo.world->AddMoveRequest(move_directions[0].playerName,move_directions[0].direction);
            for(int j = 0; j < (int)this->renderInfo.world->getWorldInfo()->Players.size(); j++)
                if((this->renderInfo.world->worldInfo.Players[j]->GetPlayerName() == move_directions[0].playerName)&&
                        (this->renderInfo.world->getWorldInfo()->Players[j]->isAlive()) )
                {
                    this->renderInfo.world->worldInfo.Players[j]->lastDirection = move_directions[0].direction;
                    break;
                }
            move_directions.erase(move_directions.begin());
        }



        //-----------------------------------------------------------------//

        float ElapsedTime = this->renderInfo.world->SimulateNextStep();
        this->renderInfo.world->GetObjects(&this->goVector);

        std::vector< rbw::PlayerExportInformation > pInfo = this->renderInfo.world->ExportPlayerInfo();

        this->renderInfo.level.Draw(*(this->renderInfo.window));
        this->renderInfo.graphic->Render(this->goVector);
        rbw::DrawGraphicObject(this->renderInfo.window, &this->renderInfo.mouseObject, this->renderInfo.Spray_Fire);

        if (this->renderInfo.ShowScore){
            this->renderInfo.graphic->ShowScoreTable(pInfo);
        }
        this->renderInfo.graphic->ShowEventList(this->renderInfo.world->ExportEvents());

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

        text.setPosition(sf::Vector2f(this->renderInfo.originalWinSize.x - 150, 25 ));


        this->renderInfo.window->draw(text);


        rbw::Team winningTeam;
        if (this->renderInfo.world->RoundEnded(&winningTeam))
            this->renderInfo.world->RoundDraw();
    }
    return true;
}
bool GameField::CheckEvents()
{    
    if (this->renderInfo.event.type == sf::Event::MouseButtonPressed){        
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->renderInfo.window);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)){
            // left mouse clicked
            std::cout << "right mousepressed "<<mouse_pos.x<<" "<<mouse_pos.y<<std::endl;
            this->AddBouncingBomb(sf::Vector2i(mouse_pos.x, mouse_pos.y));
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            // left mouse clicked
            std::cout << "right mousepressed "<<mouse_pos.x<<" "<<mouse_pos.y<<std::endl;
            if (!this->renderInfo.Switch_Mouse)
                this->AddHomingMissile(sf::Vector2i(mouse_pos.x, mouse_pos.y));
            else
            {
                this->AddGrenade(sf::Vector2i(mouse_pos.x, mouse_pos.y));
                this->renderInfo.mouseObject.type = rbw::Graphic::MOUSE_POINTER_NORMAL;
                this->renderInfo.Switch_Mouse = false;
            }
        }
    }

    if (this->renderInfo.event.type == sf::Event::KeyPressed){
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->renderInfo.window);
        if (this->renderInfo.event.key.code == sf::Keyboard::Space){
            this->renderInfo.Switch_Mouse = !this->renderInfo.Switch_Mouse;
            this->renderInfo.mouseObject.type = (this->renderInfo.Switch_Mouse)? rbw::Graphic::MOUSE_POINTER_SWITCHED : rbw::Graphic::MOUSE_POINTER_NORMAL;
        }
        if (this->renderInfo.event.key.code == sf::Keyboard::R){
            this->renderInfo.Spray_Fire = !this->renderInfo.Spray_Fire;
        }
        if (this->renderInfo.event.key.code == sf::Keyboard::Escape){
            this->renderInfo.window->close();
        }
    }

}
bool GameField::CheckKeyboard()
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->renderInfo.window);
    this->renderInfo.mouseObject.x = mouse_pos.x;
    this->renderInfo.mouseObject.y = mouse_pos.y;

    sf::Vector2i direction_player(0,0);

    sf::Keyboard key;
    sf::Mouse mouse;
    sf::Mouse::Button button;
    if ((this->renderInfo.Spray_Fire)&&(mouse.isButtonPressed(sf::Mouse::Left))){
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->renderInfo.window);
        this->AddHomingMissile(sf::Vector2i(mouse_pos.x, mouse_pos.y));
    }
    if ((this->renderInfo.Spray_Fire)&&(mouse.isButtonPressed(sf::Mouse::Right))){
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->renderInfo.window);
        this->AddBouncingBomb(sf::Vector2i(mouse_pos.x, mouse_pos.y));
    }
    if (key.isKeyPressed(sf::Keyboard::A)) direction_player.x--;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_LEFT);
    if (key.isKeyPressed(sf::Keyboard::D)) direction_player.x++;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_RIGHT);
    if (key.isKeyPressed(sf::Keyboard::W)) direction_player.y--;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_UP);
    if (key.isKeyPressed(sf::Keyboard::S)) direction_player.y++;//this->server->AddMoveRequest(this->MyName, rbw::DIRECTION_DOWN);
    this->renderInfo.ShowScore = (key.isKeyPressed(sf::Keyboard::Tab));

    this->AddMoveRequest(direction_player);
}

void GameField::EndGame()
{
    Timeout=1;
}

void GameField::AddUser(std::string str)
{    
    this->MyName = str;
    if (this->renderInfo.isSinglePlayer)
        this->renderInfo.world->AddPlayer(str, rbw::TEAM_BLACK, false);
    else
        qDebug()<<QString::fromStdString(str);
}
void GameField::addBot(int numberOfBotBlackTeam,int numberOfBotWhiteTeam)
{
    qDebug()<<"Number of bot for White Team:"<<numberOfBotWhiteTeam
           <<". Number of bot for Black Team:"<<numberOfBotBlackTeam<<".";        
    if (this->renderInfo.isSinglePlayer){
        for (int i=0; i<numberOfBotWhiteTeam; i++){
            int ind = this->renderInfo.botcount++;
            this->renderInfo.world->AddPlayer(std::string("Bot")+std::to_string(ind),rbw::TEAM_WHITE, true);
        }
        for (int i=0; i<numberOfBotBlackTeam; i++){
            int ind = this->renderInfo.botcount++;
            this->renderInfo.world->AddPlayer(std::string("Bot")+std::to_string(ind),rbw::TEAM_BLACK, true);
        }
    }
}
void GameField::customEvent(QEvent* pe)
{
    if((int)(pe)->type()==QSendToClientEvent::sendtoclienttype)
    {
        QSendToClientEvent* qstce=(QSendToClientEvent*)pe;
        switch(qstce->forSwitch)
        {
            case 0:
            {
                //qDebug()<<qstce->Text;
                break;
            }
            case 1:
            {
                stVector=qstce->stVector;
                //qDebug()<<"Hello stVector";
                break;
            }
            case 2:
            {
                peiVector=qstce->peiVector;
                //qDebug()<<"Hello peiVector";
                break;
            }
            case 3:
            {
                goVector=qstce->goVector;
                //qDebug()<<"Hello goVector";
                break;
            }
        }
    }
    else qDebug()<<"Error!";
}
//==================Read=or=Send============
void GameField::ReadyRead(QString text)
{
    qDebug()<<text;
    if(0)
    {
        Timeout=1;// end of game
    }
}
void GameField::SendToServer(QString text)
{
    if(1)
    {
        QSendToClientEvent* pe=new QSendToClientEvent();
        pe->Text=text;
        pe->forSwitch=0;
        QApplication::postEvent(client,pe);
    }
}
//=================add=Something=New=======================
void GameField::AddMoveRequest(sf::Vector2i vector)
{
    if (this->renderInfo.isSinglePlayer){
        this->renderInfo.world->AddMoveRequest(this->MyName, vector);
    }
    else
        SendToServer("AMR:"+QString::number(vector.x)+" "+QString::number(vector.y));
}

void GameField::AddGrenade(sf::Vector2i mousePosition)
{
    if (this->renderInfo.isSinglePlayer){
        this->renderInfo.world->AddGrenade(this->MyName, mousePosition);
    }
    else
        SendToServer("G:"+QString::number(mousePosition.x)+" "+QString::number(mousePosition.y));
}

void GameField::AddBouncingBomb(sf::Vector2i mousePosition)
{
    if (this->renderInfo.isSinglePlayer){
        this->renderInfo.world->AddBouncingBomb(this->MyName, mousePosition);
    }
    else
        SendToServer("BB:"+QString::number(mousePosition.x)+" "+QString::number(mousePosition.y));
}

void GameField::AddHomingMissile(sf::Vector2i mousePosition)
{
    if (this->renderInfo.isSinglePlayer){
        this->renderInfo.world->AddHomingMissile(this->MyName, mousePosition);
    }
    else
        SendToServer("HM:"+QString::number(mousePosition.x)+" "+QString::number(mousePosition.y));
}




//---------------------------------------it's new-------------------------------------//
TSafeDirections GameField::getSafeDirections(rbw::Player botyara, TVector real_speed)
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
void GameField::botyara(MoveToTheVictim * moveToTheVictim,
                   std::vector< TPair_PlayerDirection > &directions,
                   rbw::Team team_name, bool shoot_function)
{
    sf::Vector2i direction_bot(0,0);//it's new
    TPair_PlayerDirection bot_name_direction;

    rbw::Player * bot;
    TPlayer _bot;
    std::vector< rbw::Player* > _bots;
    sf::Vector2f tmp;

    for(int i = 0; i < (int)this->renderInfo.world->worldInfo.Players.size(); i++)
    {
        bot = this->renderInfo.world->worldInfo.Players[i];
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
            BotShoot bot_shoot(this->renderInfo.world, moveToTheVictim->victims,moveToTheVictim->bots[i].coord);
            TVector shootCoord = bot_shoot.getVictimCoord();
            if( (shootCoord.x != 0)||(shootCoord.y != 0) )
            {
                this->renderInfo.world->AddHomingMissile(moveToTheVictim->bots[i].name,sf::Vector2i(shootCoord.x,shootCoord.y));
                this->renderInfo.world->AddBouncingBomb(moveToTheVictim->bots[i].name, sf::Vector2i(shootCoord.x,shootCoord.y));
            }
        }
    }
    moveToTheVictim->bots.clear();
    moveToTheVictim->victims.clear();
    _bots.clear();
}

//---------------------------------------------------------------------------------------//
