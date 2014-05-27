#include "ingame.h"
#include "QApplication"
#include <sstream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <movetothevictim.h>
#include <evadefromtherocket.h>

//=================Begin=Or=End=?========================================
InGame::InGame(QObject*, int Index):server(server),/*Text(""),*/maxIndex(-1),IndexOfGame(Index),
    TimeStep(0.5*1000),QThread(0),qre("(G|(?:BB)|(?:HM)|(?:AMR)):(\\-{0,1}[0-9]+) (\\-{0,1}[0-9]+)")

    //qre("(G|(?:BB)|(?:HM)|(?:AMR)):(([\\-0-9]+) ([\\-0-9]+)")
    //qre("(G|(?:BB)|(?:HM)|(?:AMR)):([0-9]+) ([0-9]+)")
    //,timer(this)([0-9]+) (0-9+)
{

    for(int i=0;i<=N-1;i++)
    {
        UserInfo[i]=NULL;
    }

    this->renderInfo.FPS = 60.0f;
    this->renderInfo.level.LoadFromFile("Resources/maps/street_new.tmx");

    this->renderInfo.world = new rbw::WorldSimulator;

    this->renderInfo.world->Init(&(this->renderInfo.level), this->renderInfo.FPS);

    this->renderInfo.moveToTheVictim = new MoveToTheVictim(this->renderInfo.world);
    this->renderInfo.moveToTheVictim_first_team = new MoveToTheVictim(this->renderInfo.world);  //it's new
    this->renderInfo.moveToTheVictim_second_team = new MoveToTheVictim(this->renderInfo.world);  //it's new

    this->renderInfo.botcount = 0;

    this->renderInfo.moveToTheVictim->walls = this->getWalls();    
    this->renderInfo.moveToTheVictim_first_team->walls = this->getWalls();// ------------it's new //
    this->renderInfo.moveToTheVictim_second_team->walls = this->getWalls();// ------------it's new //

    std::cout << "New game Initialization completed!" << std::endl;
}
InGame::~InGame()
{
    for(int i=0;i<=maxIndex;i++)
    {
        if(UserInfo[i]!=NULL)
        {
            delete UserInfo[i];
        }
    }
}
std::vector<TRectangle> InGame::getWalls()
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


void InGame::run()
{
    QString Text="";
    Text+="Start Game:";
    for(int i=0;i<=maxIndex;i++)
    {
        Text+=UserInfo[i]->UserName+" ";
    }
    sendToClient(Text);

    sf::Clock clock;
    bool timeToSimulate = true;
    clock.restart();

    while(!Timeout)
    {
        timeToSimulate = false;

        //Wait until 1/60th of a second has passed, then update everything.
        if (clock.getElapsedTime().asSeconds() >= 1.0f / this->renderInfo.FPS)
        {
            timeToSimulate = true; //We're ready to redraw everything
            clock.restart();
        }
        else //Sleep until next 1/60th of a second comes around
        {
            sf::Time sleepTime = sf::seconds((1.0f / this->renderInfo.FPS) - clock.getElapsedTime().asSeconds());
            sf::sleep(sleepTime);
        }

        //sf::sleep(sf::milliseconds(16));

        if (timeToSimulate)
            this->MoveThisWorld();

        QApplication::processEvents();
    };
    sigEndGame(IndexOfGame);
}
void InGame::addUser(QTcpSocket* Socket,QString UserName,int Index,int Team)
{
    UserInfo[++maxIndex]=new Infotmation(Socket,UserName,Index);
    if(!Team)
    {
        if(maxIndex%2==0)
        {
            this->renderInfo.world->AddPlayer(UserName.toStdString(),rbw::TEAM_BLACK, false);
        }
        else
        {
            this->renderInfo.world->AddPlayer(UserName.toStdString(),rbw::TEAM_WHITE, false);
        }
    }
    else
    {
        if(Team==1)
        {
            this->renderInfo.world->AddPlayer(UserName.toStdString(),rbw::TEAM_BLACK, false);
        }
        else
        {
            this->renderInfo.world->AddPlayer(UserName.toStdString(),rbw::TEAM_WHITE, false);
        }
    }
}
void InGame::addBot(int numberOfBotWhiteTeam,int numberOfBotBlackTeam)
{
    qDebug()<<"Number of bot for White Team:"<<numberOfBotWhiteTeam
          <<". Number of bot for Black Team:"<<numberOfBotBlackTeam<<".";
    for (int i=0; i<numberOfBotWhiteTeam; i++){
        this->renderInfo.world->AddPlayer(std::string("Bot")+std::to_string(i),rbw::TEAM_WHITE, true);
    }
    for (int i=0; i<numberOfBotBlackTeam; i++){
        this->renderInfo.world->AddPlayer(std::string("Bot")+std::to_string(i),rbw::TEAM_BLACK, true);
    }
    std::cout << "Bots added successfuly" << std::endl;
}

//=================Helping=Functions============================
int InGame::takeMaxIndex()
{
    return maxIndex;
}
int InGame::takeIndexOfClient(int Index)
{
    return UserInfo[Index]->IndexOfClient;
}

QTcpSocket* InGame::TakeSocket(int Index)
{
    return UserInfo[Index]->qtcpSocket;
}
std::string goVector_to_string(std::vector< rbw::GraphicObject > * objects)
{
    std::stringstream buffer;
    buffer << objects->size() << std::endl;
    for (int i=0; i<objects->size(); i++){
        buffer << (objects->at(i)).x << " ";
        buffer << (objects->at(i)).y << " ";
        buffer << (objects->at(i)).velocity_x << " ";
        buffer << (objects->at(i)).velocity_y << " ";
        buffer << (int) (objects->at(i)).type << " ";
        buffer << (objects->at(i)).Name << " ";
        buffer << (int) (objects->at(i)).team << " ";
        buffer << (objects->at(i)).HealthPoint << " ";
        buffer << (objects->at(i)).zoom_coefficient << std::endl;
    }
    return buffer.str();
}
std::string peiVector_to_string(std::vector< rbw::PlayerExportInformation > * peInfo)
{
    std::stringstream buffer;
    buffer << peInfo->size() << std::endl;
    for (int i=0; i<peInfo->size(); i++){
        buffer << (peInfo->at(i)).PlayerName << " ";
        buffer << (int) (peInfo->at(i)).team << " ";
        buffer << (peInfo->at(i)).Kill << " ";
        buffer << (peInfo->at(i)).Death << " ";
        buffer << (peInfo->at(i)).DamageDealt << " ";
        buffer << (peInfo->at(i)).HomingMissilesLeft << " ";
        buffer << (peInfo->at(i)).BouncingBombsLeft << " ";
        buffer << (peInfo->at(i)).GrenadesLeft << " ";
        buffer << (peInfo->at(i)).isDead << std::endl;
    }
    return buffer.str();
}

//====================Move=this=world========================================
float InGame::MoveThisWorld()//QTimerEvent* event,TimerEvent,MoveThisWorld ((^.^)>)>)>)>>
{    
    std::cout << "Starting to move this fucking World ----" << std::endl;

    /*
    //-----------------------------------it's new------------------------------------//    
    std::vector< TPair_PlayerDirection > move_directions;//it's new
    rbw::Team team1 = rbw::TEAM_BLACK, team2 = rbw::TEAM_WHITE;

    this->botyara(this->renderInfo.moveToTheVictim_first_team,move_directions,team1,true);
    this->botyara(this->renderInfo.moveToTheVictim_second_team,move_directions,team2,true);

    std::cout << "THE EXCEPTION IS HERE >>> (1) <<<" << std::endl;

    // tut kakie-to zakomentirovannye kody

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

    /************************************************************ MY CODE !!!! */

    float ElapsedTime = this->renderInfo.world->SimulateNextStep();

    std::vector< rbw::GraphicObject >* objects=new std::vector< rbw::GraphicObject >;
    this->renderInfo.world->GetObjects(objects);

    sendToClient(this->renderInfo.world->ExportEvents());
    sendToClient(this->renderInfo.world->ExportPlayerInfo());
    sendToClient(*objects);


    std::cout << objects->size() << std::endl;
    delete objects;

    /****************************************************
     * IMPORTANT NOTE:
     *
     * Have to add some information about winning team
     * into the following cycle:
     ****************************************************/
    std::cout << "--- Finished moving this fucking world" << std::endl;

    rbw::Team * winningTeam;
    if(this->renderInfo.world->RoundEnded(winningTeam))
    {
       Round++;qDebug()<<"Rounde:"<<Round-1<<" ended.("<<IndexOfGame<<")";
       sendToClient("Rounde:"+QString::number(Round-1)+" ended.");
       if(Round==10)
       {
           Timeout=1;
       }
       else
       {
           this->renderInfo.world->RoundDraw();
       }
    }
    return ElapsedTime;
    //-------------------------------------------
}
//================Send=or=Read============================================
void InGame::sendToClient(const QString& str,int Index)
{
    if(Index==-1)
    {
        for(int i=0;i<=maxIndex;i++)
        {
            if(!UserInfo[i]->IsSurrender)
            {
                //qDebug()<<Round-1;
                QSendToClientEvent* pe=new QSendToClientEvent(UserInfo[i]->IndexOfClient);
                pe->Text=str;
                pe->forSwitch=0;
                QApplication::postEvent(server,pe);
            }
        }
    }
    else
    {
        if(!UserInfo[Index]->IsSurrender)
        {
            QSendToClientEvent* pe=new QSendToClientEvent(UserInfo[Index]->IndexOfClient);
            pe->Text=str;
            pe->forSwitch=0;
            QApplication::postEvent(server,pe);
        }
    }

}
void InGame::sendToClient(const std::vector< std::string >  stVetor,int Index)
{
    if(Index==-1)
    {
        for(int i=0;i<=maxIndex;i++)
        {
            if(!UserInfo[i]->IsSurrender)
            {
                QSendToClientEvent* pe=new QSendToClientEvent(UserInfo[i]->IndexOfClient);
                pe->stVector=stVetor;
                pe->forSwitch=1;
                QApplication::postEvent(server,pe);
            }
        }
    }
    else
    {
        if(!UserInfo[Index]->IsSurrender)
        {
            QSendToClientEvent* pe=new QSendToClientEvent(UserInfo[Index]->IndexOfClient);
            pe->stVector=stVetor;
            pe->forSwitch=1;
            QApplication::postEvent(server,pe);
        }
    }
}
void InGame::sendToClient(const std::vector< rbw::PlayerExportInformation >  peiVector,int Index)
{
    if(Index==-1)
    {
        for(int i=0;i<=maxIndex;i++)
        {
            if(!UserInfo[i]->IsSurrender)
            {
                QSendToClientEvent* pe=new QSendToClientEvent(UserInfo[i]->IndexOfClient);
                pe->peiVector=peiVector;
                pe->forSwitch=2;
                QApplication::postEvent(server,pe);
            }
        }
    }
    else
    {
        if(!UserInfo[Index]->IsSurrender)
        {
            QSendToClientEvent* pe=new QSendToClientEvent(UserInfo[Index]->IndexOfClient);
            pe->peiVector=peiVector;
            pe->forSwitch=2;
            QApplication::postEvent(server,pe);
        }
    }

}
void InGame::sendToClient(const std::vector< rbw::GraphicObject >  goVector,int Index)
{
    if(Index==-1)
    {
        for(int i=0;i<=maxIndex;i++)
        {
            if(!UserInfo[i]->IsSurrender)
            {
                QSendToClientEvent* pe=new QSendToClientEvent(UserInfo[i]->IndexOfClient);
                pe->goVector=goVector;
                pe->forSwitch=3;
                QApplication::postEvent(server,pe);
            }
        }
    }
    else
    {
        if(!UserInfo[Index]->IsSurrender)
        {
            QSendToClientEvent* pe=new QSendToClientEvent(UserInfo[Index]->IndexOfClient);
            pe->goVector=goVector;
            pe->forSwitch=3;
            QApplication::postEvent(server,pe);
        }
    }

}
void InGame::ReadyRead(int UserIndex, QString str)
{
    qDebug()<<"User["<<UserInfo[UserIndex]->UserName<<"] send to Server:"<<str;

    if(str=="SUR")
    {
        UserInfo[UserIndex]->IsSurrender=1;
        return;
    }
    /*if(str=="Non")
    {
        this->renderInfo.world->AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),rbw::DIRECTION_NODIRECTION);
        return;
    }
    if(str=="U")
    {
        this->renderInfo.world->AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),rbw::DIRECTION_UP);
        return;
    }
    if(str=="D")
    {
        this->renderInfo.world->AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),rbw::DIRECTION_LEFT);
        return;
    }
    if(str=="L")
    {
        this->renderInfo.world->AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),rbw::DIRECTION_LEFT);
        return;
    }
    if(str=="R")
    {
        this->renderInfo.world->AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),rbw::DIRECTION_RIGHT);
        return;
    }*/
    if(str.contains(qre))
    {
        sf::Vector2i vector;
        vector.x=qre.cap(2).toInt();
        vector.y=qre.cap(3).toInt();
        if(qre.cap(1)=="AMR")
        {
            this->renderInfo.world->AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),vector);
            qDebug() << "AMR real vector: " << vector.x << vector.y;
            return;
        }
        if(qre.cap(1)=="BB")
        {
            this->renderInfo.world->AddBouncingBomb(UserInfo[UserIndex]->UserName.toStdString(),vector);
            qDebug()<<"BB:"<<vector.x<<","<<vector.y;
            return;
        }
        if(qre.cap(1)=="HM")
        {
            this->renderInfo.world->AddHomingMissile(UserInfo[UserIndex]->UserName.toStdString(),vector);
            qDebug()<<"HM:"<<vector.x<<","<<vector.y;
            return;
        }
        if(qre.cap(1)=="G")
        {
            this->renderInfo.world->AddGrenade(UserInfo[UserIndex]->UserName.toStdString(),vector);
            qDebug()<<"G:"<<vector.x<<","<<vector.y;
            return;
        }
    }
}
void InGame::customEvent(QEvent* pe)
{
    if((int)(pe)->type()==QSendToClientEvent::sendtoclienttype)
    {
        QSendToClientEvent* qstce=(QSendToClientEvent*)pe;
        ReadyRead(qstce->Index,qstce->Text);
    }
    else qDebug()<<"Error!";
}


TSafeDirections InGame::getSafeDirections(rbw::Player botyara, TVector real_speed)
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

void InGame::botyara(MoveToTheVictim * moveToTheVictim,
                   std::vector< TPair_PlayerDirection > &directions,
                   rbw::Team team_name, bool shoot_function)
{
    std::cout << "    # Botyara begin" << std::endl;
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

    std::cout << "    # Botyara found safe directions" << std::endl;
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
            BotShoot bot_shoot(this->renderInfo.world,moveToTheVictim->victims,moveToTheVictim->bots[i].coord);            
            TVector shootCoord = bot_shoot.getVictimCoord();            
            if( (shootCoord.x != 0)||(shootCoord.y != 0) )
            {                
                std::cout << "trying to add a HM ... (" << this->renderInfo.world << ") " << this->renderInfo.moveToTheVictim->bots[i].name << std::endl;
                this->renderInfo.world->AddHomingMissile(this->renderInfo.moveToTheVictim->bots[i].name,sf::Vector2i(shootCoord.x,shootCoord.y));                                
                std::cout << "... HM added" << std::endl;
                this->renderInfo.world->AddBouncingBomb(this->renderInfo.moveToTheVictim->bots[i].name, sf::Vector2i(shootCoord.x,shootCoord.y));
            }
        }        
    }
    moveToTheVictim->bots.clear();
    moveToTheVictim->victims.clear();
    _bots.clear();
    std::cout << "    # Botyara finished" << std::endl;
}
//---------------------------------------------------------------------------------------//
