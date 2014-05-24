#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QWidget>
#include <QDebug>
#include <QThread>
#include <QEvent>
#include <rbwincludes.h>
#include "worldsimulator.h"
#include <evadefromtherocket.h>
#include <movetothevictim.h>

typedef struct
{
    float FPS;
    sf::RenderWindow TWindow;
    sf::RenderWindow * window;
    Level level;
    rbw::GraphicEngine * graphic;

    bool Switch_Mouse;
    rbw::GraphicObject mouseObject;

    sf::Event event;

    rbw::WorldSimulator * world;
    bool isSinglePlayer;

    MoveToTheVictim * moveToTheVictim;  //it's new
    int botcount;
} RenderInfo;




class QSendToClientEvent;

class GameField : public QThread
{
    Q_OBJECT
private:
    void ReadyRead(QString);
    void SendToServer(QString text);                    
    void EndGame();

    bool MoveThisWorld();
    bool CheckKeyboard();
    bool CheckEvents();

    RenderInfo renderInfo;

    std::string MyName;

    void AddMoveRequest(sf::Vector2i vector);
    void AddGrenade(sf::Vector2i mousePosition);
    void AddBouncingBomb(sf::Vector2i mousePosition);
    void AddHomingMissile(sf::Vector2i mousePosition);

    std::vector< std::string >  stVector;
    std::vector< rbw::PlayerExportInformation > peiVector;
    std::vector< rbw::GraphicObject > goVector;
    QObject* client;
    float TimeStep;
    bool Timeout=0;
    bool pause=0;
    bool isSingleGame;
public:
    GameField(QObject *client, bool isSingleGame);
    ~GameField();

    void run();
    void customEvent(QEvent* pe);
    void AddUser(std::string);
    void addBot(int numberOfBotBlackTeam,int numberOfBotWhiteTeam);
public slots:
    void slotPause(bool);
signals:
    void sig_EndGame();
};

//----------------------------------------------------------------------
class QSendToClientEvent:public QEvent
{
public:
    enum {sendtoclienttype=User+1};
    QString Text;
    std::vector< std::string >  stVector;
    std::vector< rbw::PlayerExportInformation > peiVector;
    std::vector< rbw::GraphicObject > goVector;
    int forSwitch=-1;
    QSendToClientEvent():QEvent((Type)sendtoclienttype)
    {}
};

#endif // GAMEFIELD_H
