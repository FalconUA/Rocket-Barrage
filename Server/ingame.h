#ifndef INGAME_H
#define INGAME_H

#include <QWidget>
#include <QDebug>
#include "server.h"
#include <QThread>
#include <QRegExp>
#include "worldsimulator.h"
#include <movetothevictim.h>
#include <evadefromtherocket.h>

class rbw::WorldSimulator;
class QTcpSocket;
//enum StateInGame{Alive,Win,Lose,Surrender};
class Infotmation;
class QSendToClientEvent;
//class SendToClient;


typedef struct {
    float FPS;
    Level level;

    rbw::WorldSimulator * world;

    MoveToTheVictim * moveToTheVictim;  //it's new
    int botcount;

} RenderInformation;

const int N=20;
const double L=4;

class InGame : public QThread
{
    Q_OBJECT
private:
    void sendToClient(const QString& str,int Index=-1);
    void sendToClient(const std::vector< std::string >  stVetor,int Index=-1);
    void sendToClient(const std::vector< rbw::PlayerExportInformation >  peiVector,int Index=-1);
    void sendToClient(const std::vector< rbw::GraphicObject >  goVector,int Index=-1);
    void ReadyRead(int UserIndex, QString);
    QObject* server;

    RenderInformation renderInfo;
    std::vector<TRectangle> getWalls();

    QRegExp qre;
    int maxIndex;
    int Round = 0;
    const unsigned int IndexOfGame;
    const unsigned int TimeStep; 
    bool Timeout = 0;
public:
    InGame(QObject*,int Index);
    ~InGame();
    void run();
    void addUser(QTcpSocket* , QString , int , int Team=0);
    void addBot(int , int );
    void customEvent(QEvent* pe);
    int takeMaxIndex();
    int takeIndexOfClient(int);
    //StateInGame takeState(int Index);
    QTcpSocket* TakeSocket(int);

    Infotmation* UserInfo[N];

public slots:
private slots:
    float MoveThisWorld();
signals:
    void sigEndGame(int);
};
//======================================
class Infotmation:public QObject
{
public:
    Infotmation(QTcpSocket* Socket,QString UserName,int Index):qtcpSocket(Socket),UserName(UserName),IndexOfClient(Index)//,CenterOfTank(0,0)
    {}
    QTcpSocket* qtcpSocket;
    const QString UserName;
    const int IndexOfClient;
    bool IsSurrender;
};
//===========QSendToClientEvent============================
class QSendToClientEvent:public QEvent
{
public:
    enum {sendtoclienttype=User+1};
    QString Text;
    std::vector< std::string >  stVector;
    std::vector< rbw::PlayerExportInformation > peiVector;
    std::vector< rbw::GraphicObject > goVector;
    int Index;
    int forSwitch=-1;
    QSendToClientEvent(int Index):QEvent((Type)sendtoclienttype),
                       Index(Index)
    {}
};

//======================================

#endif // SERVER_H
