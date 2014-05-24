#ifndef SERVER_H
#define SERVER_H
#include <QtNetwork>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QIODevice>
#include <QRegExp>
#include <QApplication>
#include "ingame.h"
#include "worldsimulator.h"
#include <QVector>
#include <QList>
class QTcpServer;
class QTcpSocket;
class ClientsInfo;
class InGame;
class QSendToClientEvent;
class Lobby;
//struct rbw::PlayerExportInformation;
class Server : public QWidget
{
    Q_OBJECT
private:
    void sendToGame(int Index,const QString& str);
    InGame *ingame[500];
    QTcpServer* qtcpServer;
    ClientsInfo* client[1000];
    int maxIndexClintInfo=-1;
    quint16 m_nNextBlockSize;
    QList<unsigned int> qlSearch;
    QVector<Lobby*> ListofLobby;
    const unsigned int numPeople;
    QRegExp qreUserName;
    QRegExp qreCreateLobby;
    QRegExp qreContoLobby;
    QRegExp qreinLobbyChat;
public:
    Server(int nPort,QWidget *parent = 0);
    void customEvent(QEvent* pe);
    ~Server();
public slots:
    void slotCreateGame();
    void slotinLobbyCreateGame();
    virtual void  slotNewConnection();
    void slotDisconnect();
    void slotReadClient();
    void sendToClient(QTcpSocket*,const QString&);
    void sendToClient(QTcpSocket*, std::vector< rbw::PlayerExportInformation >);
    void sendToClient(QTcpSocket*, std::vector< std::string >);
    void sendToClient(QTcpSocket*, std::vector< rbw::GraphicObject >);
    void slotDestroyGame(int Index);
    void slotDeleteLobby();
//    void slotSendToClient(int);
signals:
};
//=====================================
enum State{Registration,Menu,SearchGame,inLobby,inGame};
class ClientsInfo:public QObject
{
    Q_OBJECT
private:
    QTcpSocket* qtcpSocket;
    int Win=0;
    int Lose=0;
    const int Index;
public:
    ClientsInfo(QTcpSocket* Socket,int i):Index(i)
    {
        qDebug()<<"Hello New Client["<<Index<<"].Welcome to HELL!";
        qtcpSocket=Socket;
        connect(Socket,SIGNAL(readyRead()),
                this,SLOT(slotReadyRead()));
        connect(Socket,SIGNAL(disconnected()),
                this,SLOT(slotDisconnected()));
    }
    QTcpSocket* Take_Socket()
    {
        return qtcpSocket;
    }
    ~ClientsInfo()
    {
    }
    int TakeInfo(const QString);
    void operator ++(int)
    {
        Win++;
    }
    void operator --(int)
    {
        Lose++;
    }
    void operator ++()
    {
        Win++;
    }
    void operator --()
    {
        Lose++;
    }
    QString UserName="";
    int Match=-1;
    int IndexInGame=-1;
    Lobby* lobby=NULL;
    enum State state=Registration;
signals:
    void sig_ReadyRead();
    void sig_Disconnected();
private slots:
    void slotReadyRead()
    {
        sig_ReadyRead();
    }
    void slotDisconnected()
    {
        sig_Disconnected();
    }
};
//============Lobby===============
class Lobby:public QObject
{
    Q_OBJECT
private:
    void SendNameOfKing(ClientsInfo* client);
    QVector<ClientsInfo*> BlackTeam;
    QVector<ClientsInfo*> WhiteTeam;
    QObject* server;
    QString KingOfLobby;
    ClientsInfo* ciKingOfLobby=NULL;
    int numberOfBotBlackTeam=0;
    int numberOfBotWhiteTeam=0;
    bool WhiteKing=0;
public:
    Lobby(ClientsInfo* ciCreator,QString LobbyName,QObject* server):LobbyName(LobbyName),server(server),
        QObject(0)
    {
        BlackTeam<<ciCreator;
    }
    ~Lobby();
    void SendtoClient(QString);
    void SendMessagetoClient(QString Text, ClientsInfo *client);
    void addUsers(ClientsInfo* client);
    void addBot(QString);
    void RefreshNumberOfBot(ClientsInfo* client);
    void RefreshNumberOfBot(int& NumberOfBotBlackTeam,int& NumberOfBotWhiteTeam);
    void delUsers(ClientsInfo* client);
    void ChangeTeam(ClientsInfo* client);
    void RefreshList(ClientsInfo* client);
    void newKing();
    bool isKing(QString);
    void StartGame();
    QVector<ClientsInfo*> take_vectorOfWhiteTeam();
    QVector<ClientsInfo*> take_vectorOfBlackTeam();
    QString lengthOfTeam();
    QString LobbyName;
    QString pass;

signals:
    void sig_createTheGame();
    void sig_delete();
};

#endif // SERVER_H
