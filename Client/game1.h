#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QBoxLayout>
#include <QtNetwork>
#include <QTcpSocket>
#include <QtGui>
#include <QString>
#include <QVector>
#include <QDebug>
#include <QRegExp>
#include <QEvent>
#include <QApplication>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QLabel>
#include <QTextEdit>
#include <QPixmap>
#include <QPushButton>
#include "gamefield.h"
enum State{Registration,Menu,SearchGame,inLobbySearch,inLobby,inGame};
class GameField;
class MyWidget;
struct UsersinLobby;

class Client : public QWidget
{
    Q_OBJECT
private:
    void SendToServer(QString str);
    void Register(QString UserName) ;
    void CreateGameField(QString UsersName);
    void CreateConnect();
    void DestroyConnect();
    void ShowButton(int);
    void RefreshList();
    virtual void mousePressEvent(QMouseEvent* pe);
    virtual void mouseMoveEvent(QMouseEvent* pe);
    void customEvent(QEvent* pe);

    //struct UsersinLobby teamLabel;
    GameField* gamefield=NULL;
    QStringList list_of_server;
    QStringList list_of_lobby_search;
    QListWidget lwgListOfServer;
    QListWidget lwgLobbySearch;
    QString strHost;
    MyWidget* qwListOfServer;
    MyWidget* qwLobbySearch;
    MyWidget* qwinLobby;
    QBoxLayout* qboxTtBSingleorMultiGame;
    QBoxLayout* qboxToptoBottom;
    QBoxLayout* qboxTtBMenu;
    QBoxLayout* qboxTtBSearch;
    QBoxLayout* qboxTtBLobby;
    QBoxLayout* qboxLefttoRight;
    QBoxLayout* qboxTtBListofServer;
    QBoxLayout* qboxLtRListofServer;
    QBoxLayout* qboxTtBLobbySearch;
    QBoxLayout* qboxLtRLobbySearch;
    QBoxLayout* qboxLtRinLobby1;
    QBoxLayout* qboxLtRinLobby2;
    QBoxLayout* qboxTtBinLobby;
    QGridLayout* qglInLobbyBot;
    QBoxLayout* qboxLtRinLobbyChat;
    QString qsLobbyBlackTeam;
    QString qsLobbyWhiteTeam;
    QString qsKingOfLobby;
    QVector<QString> qvstr_inLobbyBlackTeam;
    QVector<QString> qvstr_inLobbyWhiteTeam;
    QTextEdit* qteLobbyforBlack=new QTextEdit;
    QTextEdit* qteLobbyforWhite=new QTextEdit;
    QTextEdit* qteinLobbyChat=new QTextEdit;
    QLineEdit* qleinLobbyChat=new QLineEdit;
    QLabel* qlMenu=new QLabel;
    QPushButton* qpbSingle=new QPushButton("Single Game");
    QPushButton* qpbMulti=new QPushButton("Connect to Server");
    QPushButton* qpbSearch=new QPushButton("Start Search");
    QPushButton* qpbInfo=new QPushButton("Information");
    QPushButton* qpbExit=new QPushButton("Exit");
    QPushButton* qpbRename=new QPushButton("Rename");
    QLabel* qlSearch=new QLabel("<H1>Wait for oponents.</H1>");
    QPushButton* qpbCancel=new QPushButton("Cancel");
    QPushButton* qpbBack=new QPushButton("Back");
    QPushButton* qpbLobby=new QPushButton("Lobby");
    QPushButton* qpbLobbySearch=new QPushButton("Search lobby");
    QPushButton* qpbLobbyCreate=new QPushButton("Create");
    QPushButton* qpbLobbySearchCreate=new QPushButton("Create");
    QPushButton* qpbLobbySearchBack=new QPushButton("Back");
    QPushButton* qpbLobbyBack=new QPushButton("Back");
    QPushButton* qpbinLobbyBack=new QPushButton("Back");
    QPushButton* qpbLobbyUpdate=new QPushButton("Refresh list");
    QPushButton* qpbinLobbyChange=new QPushButton("Change Team");
    QPushButton* qpbinLobbyStart=new QPushButton("Start Game");
    QPushButton* qpbinLobbyAddBotBlack=new QPushButton("Add");
    QPushButton* qpbinLobbyAddBotWhite=new QPushButton("Add");
    QPushButton* qpbinLobbyRemoveBotWhite=new QPushButton("Remove");
    QPushButton* qpbinLobbyRemoveBotBlack=new QPushButton("Remove");
    QPushButton* qpbinLobbySend=new QPushButton("Send");
    QPixmap qpBomb;
    QPixmap qpMap;
    QPixmap qpMap2;
    QPixmap qpRocket;
    QPalette pal;
    QMessageBox qmBox;
    QTcpSocket* m_pTcpSocket;
    quint16     m_nNextBlockSize;
    QRegExp qreUserName;
    QRegExp qre;
    QRegExp qreInfo;
    QRegExp qreFile;
    QRegExp qreLobbySearch;
    QRegExp qreinLobbyRefreshPlayer;
    QRegExp qreinLobby;
    QRegExp qreinLobbyRefreshBot;
    QRegExp qreinLobbyChat;
    QPoint mptPosition;
    int nPort;
    int numberOfBotBlackTeam=0;
    int numberOfBotWhiteTeam=0;
    State state=Registration;
    bool pause;
    bool isSingleGame;
    bool Balance=1;
public:
    Client(/*const QString& strHost,*/ int nPort, QWidget *pwgt = 0);
    ~Client();
private slots:
    void slotReadyRead   (                            );
    void slotError       (QAbstractSocket::SocketError);
    void slotConnected   (                            );
    void slotDisconnect();
    void DestroyGameField();
    void slotPause(bool);
    void slotTrytoConServer(QListWidgetItem*);
    void slotTrytoConLobby(QListWidgetItem*);
    void slotSigleGame();
    void slotMultiGame();
    void slotSearch();
    void slotTakeInformation();
    void slotRename();
    void slotExit();
    void slotCancel();
    void slotBack();
    void slotLobby();
    void slotLobbySearch();
    void slotLobbyCreate();
    void slotLobbyBack();
    void slotLobbyUpdate();
    void slotinLobbyChange();
    void slotinLobbyStartGame();
    void slotinLobbyAddBotBlack();
    void slotinLobbyAddBotWhite();
    void slotinLobbyRemoveWhite();
    void slotinLobbyRemoveBlack();
    void slotinLobbySend();

signals:
    void sig_PauseGame(bool);
};
//================================================
class MyWidget:public QWidget
{
public:
    MyWidget(QPixmap pixmap, QWidget *qw=0);
    virtual void mousePressEvent(QMouseEvent* pe);
    virtual void mouseMoveEvent(QMouseEvent* pe);
private:
    QPoint mptPosition;
};
//===========================================
struct UsersinLobby
{
    QVector<QString> BlackTeam;
    QVector<QString> WhiteTeam;
};

#endif // WIDGET_H
