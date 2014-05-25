#include "ingame.h"
#include "QApplication"
#include <sstream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <movetothevictim.h>
#include <evadefromtherocket.h>

//=================Begin=Or=End=?========================================
InGame::InGame(QObject *server,int Index):server(server),/*Text(""),*/maxIndex(-1),IndexOfGame(Index),
    TimeStep(0.5*1000),QThread(0),qre("(G|(?:BB)|(?:HM)|(?:AMR)):([0-9]+) ([0-9]+)")//,timer(this)([0-9]+) (0-9+)
{

    for(int i=0;i<=N-1;i++)
    {
        UserInfo[i]=NULL;
    }

    this->renderInfo.FPS = 80.0f;
    this->renderInfo.level.LoadFromFile("Resources/maps/desert.tmx");

    this->renderInfo.world = new rbw::WorldSimulator;
    this->renderInfo.world->Init(&(this->renderInfo.level), this->renderInfo.FPS);

    this->renderInfo.moveToTheVictim = new MoveToTheVictim;
    this->renderInfo.botcount = 0;

    this->renderInfo.moveToTheVictim->walls = this->getWalls();    
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
    std::vector< Object > walls = this->renderInfo.world->getWorldInfo()->wallForPlayer;
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

    this->renderInfo.world->RoundDraw();    
    while(!Timeout)
    {
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
    //-----------------------------------it's new------------------------------------//
    sf::Vector2i direction_bot(0,0);
    Player * bot;
    TPlayer _bot;
    sf::Vector2f tmp;

    for(int i = 0; i < (int)this->renderInfo.world->getWorldInfo()->Players.size(); i++)
    {
        bot = this->renderInfo.world->getWorldInfo()->Players[i];
        tmp = bot->GetPosition();
        _bot.coord.x = int(tmp.x);
        _bot.coord.y = int(tmp.y);
        _bot.speed = 1;
        _bot.name = bot->GetPlayerName();
        if(this->renderInfo.world->getWorldInfo()->Players[i]->bot())
            this->renderInfo.moveToTheVictim->bots.push_back(_bot);
        else
            this->renderInfo.moveToTheVictim->victims.push_back(_bot);
    }

    std::vector< rbw::Player* > _bots;
    for(int i = 0; i < (int)this->renderInfo.world->getWorldInfo()->Players.size(); i++)
        if(this->renderInfo.world->getWorldInfo()->Players[i]->bot())
            _bots.push_back(this->renderInfo.world->getWorldInfo()->Players[i]);

    TVector victim_position;
    for(int i = 0; i < (int)this->renderInfo.moveToTheVictim->bots.size(); i++){
        if(this->renderInfo.moveToTheVictim->moveToTheVictim(this->renderInfo.moveToTheVictim->bots[i],_bots[i],&victim_position,&direction_bot))
            this->renderInfo.world->AddBouncingBomb(this->renderInfo.moveToTheVictim->bots[i].name,sf::Vector2i(victim_position.x,victim_position.y));
        this->renderInfo.world->AddMoveRequest(this->renderInfo.moveToTheVictim->bots[i].name, direction_bot);
    }
    this->renderInfo.moveToTheVictim->bots.clear();
    this->renderInfo.moveToTheVictim->victims.clear();
    //-----------------------------------------------------------------//

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
