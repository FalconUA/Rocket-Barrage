#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QWidget>
#include <QDebug>
#include <QThread>
#include <QEvent>
#include <rbwincludes.h>
#include "worldsimulator.h"
#include <evadefromtherocket.h> //it's new
#include <movetothevictim.h>    //it's new
#include <botshoot.h>   //it's new


typedef struct
{
    float FPS;
    sf::RenderWindow TWindow;
    sf::RenderWindow * window;
    Level level;
    rbw::GraphicEngine * graphic;

    bool Switch_Mouse;
    bool Spray_Fire;
    bool ShowScore;
    rbw::GraphicObject mouseObject;    
    sf::Clock simClock;

    sf::Event event;

    rbw::WorldSimulator * world;
    bool isSinglePlayer;

    MoveToTheVictim * moveToTheVictim,/*temporary*/
        * moveToTheVictim_first_team, * moveToTheVictim_second_team;  //it's new

    sf::Vector2u originalWinSize;
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
    std::vector<TRectangle> getWalls(); //it's new
    void botyara(MoveToTheVictim * moveToTheVictim, std::vector<TPair_PlayerDirection> &directions, Team team_name, bool shoot_function);   //it's new
    TSafeDirections getSafeDirections(rbw::Player botyara, TVector real_speed);  //it's new


    std::string MyName;

    std::vector< rbw::GraphicObject > goVector_from_string(std::string charArray);
    std::vector< rbw::PlayerExportInformation > peiVector_from_string(std::string charArray);

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
