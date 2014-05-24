#include "ingame.h"
#include "QApplication"
#include <sstream>
#include <stdlib.h>
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

    level.LoadFromFile("Resources/maps/test_new.tmx");
    World.Init(&level,float(60));

    this->moveToTheVictim = new MoveToTheVictim;  //it's new

    std::vector< Object > walls = this->World.worldInfo.wallForPlayer;
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
    this->moveToTheVictim->walls = my_walls;

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
void InGame::run()
{
    QString Text="";
    Text+="Start Game:";
    for(int i=0;i<=maxIndex;i++)
    {
        Text+=UserInfo[i]->UserName+" ";
    }
    sendToClient(Text);
    MoveThisWorld();
    World.RoundDraw();    
    msleep(TimeStep);
    while(!Timeout)
    {
        msleep(2*TimeStep-MoveThisWorld());
        std::cout << "fucking moving" << std::endl;
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
            World.AddPlayer(UserName.toStdString(),rbw::TEAM_BLACK, false);
        }
        else
        {
            World.AddPlayer(UserName.toStdString(),rbw::TEAM_WHITE, false);
        }
    }
    else
    {
        if(Team==1)
        {
            World.AddPlayer(UserName.toStdString(),rbw::TEAM_BLACK, false);
        }
        else
        {
            World.AddPlayer(UserName.toStdString(),rbw::TEAM_WHITE, false);
        }
    }
}
void InGame::addBot(int numberOfBotWhiteTeam,int numberOfBotBlackTeam)
{
    qDebug()<<"Number of bot for White Team:"<<numberOfBotWhiteTeam
          <<". Number of bot for Black Team:"<<numberOfBotBlackTeam<<".";
    for (int i=0; i<numberOfBotWhiteTeam; i++){
        World.AddPlayer(std::string("Bot")+std::to_string(i),rbw::TEAM_WHITE, true);
    }
    for (int i=0; i<numberOfBotBlackTeam; i++){
        World.AddPlayer(std::string("Bot")+std::to_string(i),rbw::TEAM_BLACK, true);
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

//====================Move=this=world========================================
float InGame::MoveThisWorld()//QTimerEvent* event,TimerEvent,MoveThisWorld ((^.^)>)>)>)>>
{
    float ElapsedTime = World.SimulateNextStep();

    //-----------------------------------it's new------------------------------------//
    sf::Vector2i direction_bot(0,0);
    Player * bot;
    TPlayer _bot;
    sf::Vector2f tmp;

    for(int i = 0; i < (int)World.worldInfo.Players.size(); i++)
    {
        bot = World.worldInfo.Players[i];
        tmp = bot->GetPosition();
        _bot.coord.x = int(tmp.x);
        _bot.coord.y = int(tmp.y);
        _bot.speed = 1;
        _bot.name = bot->GetPlayerName();
        if(World.worldInfo.Players[i]->bot())
            moveToTheVictim->bots.push_back(_bot);
        else
            moveToTheVictim->victims.push_back(_bot);
    }

    std::vector< rbw::Player* > _bots;
    for(int i = 0; i < (int)World.worldInfo.Players.size(); i++)
        if(World.worldInfo.Players[i]->bot())
            _bots.push_back(World.worldInfo.Players[i]);

    TVector victim_position;
    for(int i = 0; i < (int)moveToTheVictim->bots.size(); i++)
        if(moveToTheVictim->moveToTheVictim(moveToTheVictim->bots[i],_bots[i],&victim_position,&direction_bot))
            World.AddBouncingBomb(moveToTheVictim->bots[i].name,sf::Vector2i(victim_position.x,victim_position.y));
    moveToTheVictim->bots.clear();
    moveToTheVictim->victims.clear();
    //-----------------------------------------------------------------//


    sendToClient(World.ExportEvents());
    sendToClient(World.ExportPlayerInfo());
    std::vector< rbw::GraphicObject >* objects=new std::vector< rbw::GraphicObject >;
    World.GetObjects(objects);
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
    if(World.RoundEnded(winningTeam))
    {
       Round++;qDebug()<<"Rounde:"<<Round-1<<" ended.("<<IndexOfGame<<")";
       sendToClient("Rounde:"+QString::number(Round-1)+" ended.");
       if(Round==10)
       {
           Timeout=1;
       }
       else
       {
           World.RoundDraw();
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
        World.AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),rbw::DIRECTION_NODIRECTION);
        return;
    }
    if(str=="U")
    {
        World.AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),rbw::DIRECTION_UP);
        return;
    }
    if(str=="D")
    {
        World.AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),rbw::DIRECTION_LEFT);
        return;
    }
    if(str=="L")
    {
        World.AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),rbw::DIRECTION_LEFT);
        return;
    }
    if(str=="R")
    {
        World.AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),rbw::DIRECTION_RIGHT);
        return;
    }*/
    if(str.contains(qre))
    {
        sf::Vector2i vector;
        vector.x=qre.cap(2).toInt();
        vector.y=qre.cap(3).toInt();
        if(qre.cap(1)=="AMR")
        {
            World.AddMoveRequest(UserInfo[UserIndex]->UserName.toStdString(),vector);
        }
        if(qre.cap(1)=="BB")
        {
            World.AddBouncingBomb(UserInfo[UserIndex]->UserName.toStdString(),vector);
            qDebug()<<"BB:"<<vector.x<<","<<vector.y;
            return;
        }
        if(qre.cap(1)=="HM")
        {
            World.AddHomingMissile(UserInfo[UserIndex]->UserName.toStdString(),vector);
            qDebug()<<"HM:"<<vector.x<<","<<vector.y;
            return;
        }
        if(qre.cap(1)=="G")
        {
            World.AddGrenade(UserInfo[UserIndex]->UserName.toStdString(),vector);
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
