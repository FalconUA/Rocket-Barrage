#include "game1.h"
Client::Client(int nPort, QWidget *pwgt) : QWidget(pwgt),
    strHost("localhost"),nPort(nPort)
                    , m_nNextBlockSize(0),qre("Start Game:"),
                    qpBomb("Resources/window/black-bomb-hi.png"),
                    qpMap2("Resources/window/old-world-map.jpg"),
                    qpRocket("Resources/window/Rocket.png"),
                    qpMap("Resources/window/old-world-map.jpg"),
                    qsLobbyBlackTeam("<span style=color:#000000><H1>BLACK Team</H1></span style>"),
                    qreUserName("[a-zA-z]{4,16}$"),
                    qsLobbyWhiteTeam("<span style=color:#FFFFFF><H1>WHITE Team</H1></span style>"),
                    qreInfo("([a-zA-z]{4,16}) ([0-9]+) ([0-9]+) ((?:Menu)|(?:SearchGame)|(?:In Game));"),
                    qreFile("\"(.+)\" ((?:[0-9]{2}\.[0-9]{2}\.[0-9]{3}\.[0-9]{3})|localhost)"),
                    qreLobbySearch("(None)|([[a-zA-z_ ]{3,30} \\[[0-9]{1,2}\\|[0-9]{1,2}\\])"),
                    qreinLobbyRefreshPlayer("\\[(W|B)\\]([a-zA-z_]{4,16} Win:[0-9]{1,} Lose:[0-9]{1,})"),
                    qreinLobby("(change|del|new|newKing) \\[(W|B)\\]([a-zA-z_]{4,16} Win:[0-9]{1,3} Lose:[0-9]{1,3})"),
                    qreinLobbyRefreshBot("Refresh Number of Bot:([0-9])([0-9])"),
                    qreinLobbyChat("chat ([a-zA-z]{4,16})([^\\^]+)$")
{
    m_pTcpSocket = new QTcpSocket(this);

    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(disconnected()),SLOT(slotDisconnect()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError)));
    connect(this,SIGNAL(sig_PauseGame(bool)),SLOT(slotPause(bool)));
    connect(qpbSingle,SIGNAL(clicked()),SLOT(slotSigleGame()));
    connect(qpbMulti,SIGNAL(clicked()),SLOT(slotMultiGame()));
    connect(qpbExit,SIGNAL(clicked()),SLOT(slotExit()));
    connect(qpbBack,SIGNAL(clicked()),SLOT(slotBack()));
    connect(qpbLobby,SIGNAL(clicked()),SLOT(slotLobby()));
    connect(qpbLobbySearch,SIGNAL(clicked()),SLOT(slotLobbySearch()));
    connect(qpbLobbyCreate,SIGNAL(clicked()),SLOT(slotLobbyCreate()));
    connect(qpbLobbySearchCreate,SIGNAL(clicked()),SLOT(slotLobbyCreate()));
    connect(qpbLobbySearchBack,SIGNAL(clicked()),SLOT(slotLobbyBack()));
    connect(qpbLobbyBack,SIGNAL(clicked()),SLOT(slotLobbyBack()));
    connect(qpbinLobbyBack,SIGNAL(clicked()),SLOT(slotLobbyBack()));
    connect(qpbLobbyUpdate,SIGNAL(clicked()),SLOT(slotLobbyUpdate()));
    connect(qpbinLobbyChange,SIGNAL(clicked()),SLOT(slotinLobbyChange()));
    connect(qpbinLobbyStart,SIGNAL(clicked()),SLOT(slotinLobbyStartGame()));
    connect(qpbinLobbyAddBotBlack,SIGNAL(clicked()),SLOT(slotinLobbyAddBotBlack()));
    connect(qpbinLobbyAddBotWhite,SIGNAL(clicked()),SLOT(slotinLobbyAddBotWhite()));
    connect(qpbinLobbyRemoveBotWhite,SIGNAL(clicked()),SLOT(slotinLobbyRemoveWhite()));
    connect(qpbinLobbyRemoveBotBlack,SIGNAL(clicked()),SLOT(slotinLobbyRemoveBlack()));
    connect(qpbinLobbySend,SIGNAL(clicked()),SLOT(slotinLobbySend()));
    connect(qleinLobbyChat,SIGNAL(editingFinished()),SLOT(slotinLobbySend()));
    //+++++++++++++++Single+or+Multi+Game++++++++++++++++++++++++++
    qboxTtBSingleorMultiGame=new QBoxLayout(QBoxLayout::TopToBottom);
    qboxTtBSingleorMultiGame->addWidget(qpbSingle);
    qboxTtBSingleorMultiGame->addWidget(qpbMulti);
    //++++++++++++++ListOfServer++++++++++++++++++++++
    QFile file("Resources/ListOfServer.txt");
    if(!file.exists())
    {
        qDebug()<<"We lost fill ListOfServer.txt :(";
        slotExit();
                  return;
    }
    file.open(QIODevice::ReadOnly);

    QString str;
    int Index=-1;
    while(!file.atEnd())
    {
        str=file.readLine();
        if(str.contains(qreFile))
        {
            qDebug()<<qreFile.cap(1)<<" "<<qreFile.cap(2);
            list_of_server<<"\""+qreFile.cap(1)+"\" "+qreFile.cap(2)+"";
            Index++;
        }
    }
    lwgListOfServer.addItems(list_of_server);
    connect(&lwgListOfServer,SIGNAL(itemClicked(QListWidgetItem*)),
            this,SLOT(slotTrytoConServer(QListWidgetItem*)));
    file.close();
    qwListOfServer=new MyWidget(qpMap);
    pal.setColor(QPalette::Base,QColor::fromRgb(0,0,0,0));
    lwgListOfServer.setPalette(pal);

    qboxTtBListofServer=new QBoxLayout(QBoxLayout::TopToBottom);
    qboxLtRListofServer=new QBoxLayout(QBoxLayout::LeftToRight);
    qboxTtBListofServer->addWidget(&lwgListOfServer,9);
    qboxTtBListofServer->addLayout(qboxLtRListofServer,1);
    qboxLtRListofServer->addStretch();
    qboxLtRListofServer->addWidget(qpbBack);
    qboxLtRListofServer->addStretch();
    qwListOfServer->setLayout(qboxTtBListofServer);
    for(int i=0;i<=Index;i++)
    {
        lwgListOfServer.item(i)->setForeground(Qt::white);
    }

    //+++++++++++++++++Menu+Search+Lobby+++++++++++++++++++
    qlMenu->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    qlSearch->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

    qboxToptoBottom=new QBoxLayout(QBoxLayout::TopToBottom);
    qboxTtBMenu=new     QBoxLayout(QBoxLayout::TopToBottom);
    qboxTtBSearch=new   QBoxLayout(QBoxLayout::TopToBottom);
    qboxTtBLobby=new    QBoxLayout(QBoxLayout::TopToBottom);

    qboxToptoBottom->addStretch(3);
    qboxToptoBottom->addLayout(qboxTtBMenu,1);
    qboxToptoBottom->addLayout(qboxTtBSearch,1);
    qboxToptoBottom->addLayout(qboxTtBLobby,1);
    qboxToptoBottom->addLayout(qboxTtBSingleorMultiGame,1);
    qboxToptoBottom->addStretch(3);

    qboxTtBMenu->  addWidget(qlMenu,1);
    qboxTtBMenu->  addWidget(qpbSearch,4);
    qboxTtBMenu->  addWidget(qpbLobby,4);
    qboxTtBLobby-> addStretch(4);
    qboxTtBLobby-> addWidget(qpbLobbySearch,4);
    qboxTtBLobby-> addWidget(qpbLobbyCreate,4);
    qboxTtBLobby-> addWidget(qpbLobbyBack,4);
    qboxTtBMenu->  addWidget(qpbInfo,4);
    qboxTtBMenu->  addWidget(qpbRename,4);
    qboxTtBMenu->  addWidget(qpbExit,4);
    qboxTtBSearch->addWidget(qlSearch,1);
    qboxTtBSearch->addWidget(qpbCancel,4);

    qboxLefttoRight=new QBoxLayout(QBoxLayout::LeftToRight);
    qboxLefttoRight->addStretch(1);
    qboxLefttoRight->addLayout(qboxToptoBottom,1);
    qboxLefttoRight->addStretch(2);
    setLayout(qboxLefttoRight);
    connect(qpbCancel,SIGNAL(clicked()),
            this,SLOT(slotCancel()));
    pal.setBrush(this->backgroundRole(),QBrush(qpBomb));
    setPalette(pal);
    setAutoFillBackground(1);
    setMask(qpBomb.mask());
    setFixedSize(qpBomb.width(),qpBomb.height());

    //++++++++++++Щось+:)+++++++++++++++
    pal.setBrush(QPalette::Base,QBrush(qpRocket));
    qmBox.setPalette(pal);
    qmBox.setAutoFillBackground(1);
    qmBox.setMask(qpRocket.mask());

    //++++++++++++++++LobbySearch++++++++++++
    qwLobbySearch=new MyWidget(qpMap2);
    pal.setColor(QPalette::Base,QColor::fromRgb(0,0,0,0));
    lwgLobbySearch.setPalette(pal);
    connect(&lwgLobbySearch,SIGNAL(itemClicked(QListWidgetItem*)),
            SLOT(slotTrytoConLobby(QListWidgetItem*)));

    qboxTtBLobbySearch=new QBoxLayout(QBoxLayout::TopToBottom);
    qboxLtRLobbySearch=new QBoxLayout(QBoxLayout::LeftToRight);
    qboxLtRLobbySearch->addWidget(qpbLobbySearchBack,1);
    qboxLtRLobbySearch->addStretch(4);
    qboxLtRLobbySearch->addWidget(qpbLobbySearchCreate,1);
    qboxLtRLobbySearch->addWidget(qpbLobbyUpdate,1);

    qboxTtBLobbySearch->addWidget(&lwgLobbySearch,9);
    qboxTtBLobbySearch->addLayout(qboxLtRLobbySearch,1);

    qwLobbySearch->setLayout(qboxTtBLobbySearch);

    //++++++++++++inLobby++++++++++++++
    pal.setColor(QPalette::Base,QColor::fromRgb(255,255,255,100));
    qteLobbyforBlack->setPalette(pal);
    pal.setColor(QPalette::Base,QColor::fromRgb(0,0,0,100));
    qteLobbyforWhite->setPalette(pal);
    qwinLobby=new  MyWidget(qpMap2);
    qboxTtBinLobby=new QBoxLayout(QBoxLayout::TopToBottom);
    qboxLtRinLobby1= new QBoxLayout(QBoxLayout::LeftToRight);
    qboxLtRinLobby2= new QBoxLayout(QBoxLayout::LeftToRight);
    qglInLobbyBot=new QGridLayout;
    qboxLtRinLobbyChat=new QBoxLayout(QBoxLayout::LeftToRight);

    qteLobbyforBlack->setReadOnly(1);
    qteLobbyforWhite->setReadOnly(1);
    qteLobbyforBlack->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    qteLobbyforWhite->setAlignment(Qt::AlignTop|Qt::AlignRight);

    qboxLtRinLobby1->addWidget(qteLobbyforBlack);
    qboxLtRinLobby1->addWidget(qteLobbyforWhite);

    qglInLobbyBot->setColumnStretch(0,4);
    qglInLobbyBot->addWidget(qpbinLobbyAddBotBlack,0,1,1,1);
    qglInLobbyBot->addWidget(qpbinLobbyAddBotWhite,0,2,1,1);
    qglInLobbyBot->addWidget(qpbinLobbyRemoveBotBlack,1,1,1,1);
    qglInLobbyBot->addWidget(qpbinLobbyRemoveBotWhite,1,2,1,1);
    qglInLobbyBot->setColumnStretch(3,4);
    qglInLobbyBot-> setHorizontalSpacing(500);

    qboxLtRinLobbyChat->addStretch(1);
    qboxLtRinLobbyChat->addWidget(qleinLobbyChat,7);
    qboxLtRinLobbyChat->addWidget(qpbinLobbySend,1);
    qteinLobbyChat->setReadOnly(1);

    qboxLtRinLobby2->addWidget(qpbinLobbyBack,1);
    qboxLtRinLobby2->addWidget(qpbinLobbyChange,1);
    qboxLtRinLobby2->addStretch(4);
    qboxLtRinLobby2->addWidget(qpbinLobbyStart,1);

    qboxTtBinLobby->addLayout(qboxLtRinLobby1,6);
    qboxTtBinLobby->addLayout(qglInLobbyBot,2);
    qboxTtBinLobby->addWidget(qteinLobbyChat,3);
    qboxTtBinLobby->addLayout(qboxLtRinLobbyChat,1);
    qboxTtBinLobby->addLayout(qboxLtRinLobby2,1);

    qwinLobby->setLayout(qboxTtBinLobby);
    show();
    ShowButton(3);
}

Client::~Client()
{

}
void Client::mousePressEvent(QMouseEvent* pe)
{
    mptPosition = pe->pos();
}
void Client::mouseMoveEvent(QMouseEvent* pe)
{
    move(pe->globalPos() - mptPosition+QPoint(-15,-30));
}

void Client::slotTrytoConServer(QListWidgetItem* qlwi)
{
    qwListOfServer->hide();
    QString str=qlwi->data(0).toString();
    str.contains(qreFile);
    qDebug()<<qreFile.cap(0)<<" "<<str;
    m_pTcpSocket->connectToHost(qreFile.cap(2), nPort);
}
void Client::slotTrytoConLobby(QListWidgetItem* qlwi)
{
    SendToServer("con "+qlwi->data(0).toString());
}

void Client::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(m_pTcpSocket->errorString())
                    );
    qDebug()<<strError;
    DestroyConnect();
    qwListOfServer->show();
    state=Registration;
}
void Client::slotDisconnect()
{
    sig_PauseGame(1);
}
void Client::slotConnected()
{
    qDebug()<<"Welcome to the League of DRAVENNN!!";
    bool b;
    QString UserName=QInputDialog::getText(0,"Rocket",
                                           "Put your UserName(>4,<16):",
                                           QLineEdit::Normal,
                                           "Your UserName.",
                                           &b);
    if(!b)
    {

        qmBox.information(this,"Information","Goodbuy :)");
        slotExit();
    }
    else
    {
        Register(UserName);
        show();
        ShowButton(1);
        sig_PauseGame(0);
    }
}
//========================Read=or=Send======================
void Client::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_4_2);
    while(1)
    {
        if (!m_nNextBlockSize)
        {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16))
            {
                break;
            }
            in >> m_nNextBlockSize;
        }
        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize)
        {
            break;
        }
        quint8 a;
        in >>a;
        switch (a)
        {
            case 0:
            {
                QString str;
                in>>str;
                qDebug()<<str;
                switch(state)
                {
                    case Registration:
                    {
                        if(str=="menu")
                        {
                            state=Menu;
                            CreateConnect();
                            break;
                        }
                        if(str=="rename")
                        {
                            bool b;
                            qmBox.information(this,"Information","Your UserName or incorrect or unavailable\n"
                                                                     "Please Rename yourself (^|^)>)>)> ");
                            QString UserName=QInputDialog::getText(0,"Rocket",
                                                                   "Put your UserName(>4,<16):",
                                                                   QLineEdit::Normal,
                                                                   "Your UserName.",
                                                                   &b);
                            if(!b)
                            {

                                qmBox.information(this,"Information","Goodbuy :)");
                                slotExit();
                            }
                            else
                            {
                                Register(UserName);
                                sig_PauseGame(0);
                            }
                            break;
                        }
                    }
                    case Menu:
                    {
                        if(str=="Start Search Game")
                        {
                            state=SearchGame;
                            break;
                        }
                        if(str.contains(qreInfo))
                        {
                            int pos=0;
                            QString Text="UserName  Win Lose Status\n";
                            while (pos >= 0)
                            {
                                pos = qreInfo.indexIn(str, pos);
                                if (pos >= 0)
                                {
                                   Text+=qreInfo.cap(1)+"  "+qreInfo.cap(2)+
                                           "  "+qreInfo.cap(3)+"  "+qreInfo.cap(4)+"\n";
                                   pos+=qreInfo.matchedLength();
                                }
                            }
                            QMessageBox::information(0,"Information",Text);
                            CreateConnect();
                            break;
                        }
                        if(str=="Start Lobby")
                        {
                            hide();
                            qwLobbySearch->hide();
                            qwinLobby->show();
                            qpbinLobbyStart->hide();
                            state=inLobby;
                        }
                    }
                    case SearchGame:
                    {
                        if(str.contains(qre))
                        {
                            str.replace("Start Game:","");
                            CreateGameField(str);
                            state=inGame;
                            hide();
                        }
                    }
                    case inLobbySearch:
                    {
                        if(str.contains(qreLobbySearch))
                        {
                            int pos=0;
                            lwgLobbySearch.clear();
                            list_of_lobby_search.clear();
                            if(qreLobbySearch.cap(1)=="None")
                            {
                                break;
                            }
                            while (pos >= 0)
                            {
                                pos = qreLobbySearch.indexIn(str, pos);
                                if (pos >= 0)
                                {
                                   list_of_lobby_search<<qreLobbySearch.cap(2);
                                   pos+=qreInfo.matchedLength();
                                }
                            }
                            lwgLobbySearch.addItems(list_of_lobby_search);
                            for(int i=0;i<lwgLobbySearch.count();i++)
                            {
                                lwgLobbySearch.item(i)->setForeground(Qt::white);
                            }

                            break;
                        }
                        if(str=="Start Lobby")
                        {
                            hide();
                            qwLobbySearch->hide();
                            qwinLobby->show();
                            qpbinLobbyStart->hide();
                            qpbinLobbyAddBotBlack->hide();
                            qpbinLobbyAddBotWhite->hide();
                            qpbinLobbyRemoveBotBlack->hide();
                            qpbinLobbyRemoveBotWhite->hide();
                            state=inLobby;
                        }

                        break;
                    }
                    case inLobby:
                    {
                        if(str.contains(qre))
                        {
                            str.replace("Start Game:","");
                            CreateGameField(str);
                            state=inGame;
                            qwinLobby->hide();
                        }
                        if(str=="add black bot")
                        {
                            numberOfBotBlackTeam++;
                            RefreshList();
                            break;
                        }
                        if(str=="add white bot")
                        {
                            numberOfBotWhiteTeam++;
                            RefreshList();
                            break;
                        }
                        if(str=="remove white bot")
                        {
                            numberOfBotWhiteTeam--;
                            RefreshList();
                            break;
                        }
                        if(str=="remove black bot")
                        {
                            numberOfBotBlackTeam--;
                            RefreshList();
                            break;
                        }
                        if(str=="You new King")
                        {
                            qpbinLobbyStart->show();
                            qpbinLobbyAddBotBlack->show();
                            qpbinLobbyAddBotWhite->show();
                            qpbinLobbyRemoveBotBlack->show();
                            qpbinLobbyRemoveBotWhite->show();
                            break;
                        }
                        if(str=="None")
                        {
                            qmBox.information(this,"Information","Team strength is not equal.\n"
                                                                 "Please add Bot, or wait for another player.");
                        }
                        if(str.contains(qreinLobby))//change|del|new
                        {
                            if(qreinLobby.cap(1)=="new")
                            {
                                if(qreinLobby.cap(2)=="B")
                                {
                                    qvstr_inLobbyBlackTeam<<qreinLobby.cap(3);
                                }
                                else
                                {
                                    qvstr_inLobbyWhiteTeam<<qreinLobby.cap(3);
                                }
                                RefreshList();
                                break;
                            }
                            if(qreinLobby.cap(1)=="del")
                            {
                                if(qreinLobby.cap(2)=="B")
                                {

                                    qvstr_inLobbyBlackTeam.remove(qvstr_inLobbyBlackTeam.indexOf(qreinLobby.cap(3)));
                                }
                                else
                                {

                                    qvstr_inLobbyWhiteTeam.remove(qvstr_inLobbyWhiteTeam.indexOf(qreinLobby.cap(3)));
                                }
                                RefreshList();
                            }
                            if(qreinLobby.cap(1)=="change")
                            {
                                int Index;
                                if(qreinLobby.cap(2)=="B")
                                {
                                    Index=qvstr_inLobbyWhiteTeam.indexOf(qreinLobby.cap(3));
                                    qvstr_inLobbyWhiteTeam.remove(Index);
                                    qvstr_inLobbyBlackTeam<<qreinLobby.cap(3);
                                }
                                else
                                {
                                    Index=qvstr_inLobbyBlackTeam.indexOf(qreinLobby.cap(3));
                                    qvstr_inLobbyBlackTeam.remove(Index);
                                    qvstr_inLobbyWhiteTeam<<qreinLobby.cap(3);
                                }
                                RefreshList();
                            }
                            if(qreinLobby.cap(1)=="newKing")
                            {
                                qsKingOfLobby=qreinLobby.cap(3);
                                RefreshList();
                            }
                            break;
                        }
                        if(str.contains(qreinLobbyRefreshPlayer))
                        {

                            qteLobbyforBlack->clear();
                            qteLobbyforWhite->clear();
                            qvstr_inLobbyWhiteTeam.clear();
                            qvstr_inLobbyBlackTeam.clear();
                            qteLobbyforBlack->append(qsLobbyBlackTeam);
                            qteLobbyforWhite->append(qsLobbyWhiteTeam);
                            int pos=0;
                            while (pos >= 0)
                            {
                                pos = qreinLobbyRefreshPlayer.indexIn(str, pos);
                                if (pos >= 0)
                                {
                                   if(qreinLobbyRefreshPlayer.cap(1)=="W")
                                   {
                                       qteLobbyforWhite->append("<span style=color:#FFFFFF><H2>"+qreinLobbyRefreshPlayer.cap(2)+"</H2></span style>");
                                       qvstr_inLobbyWhiteTeam<<(qreinLobbyRefreshPlayer.cap(2));
                                   }
                                   else
                                   {
                                       qteLobbyforBlack->append("<span style=color:#000000><H2>"+qreinLobbyRefreshPlayer.cap(2)+"</H2></span style>");
                                       qvstr_inLobbyBlackTeam<<(qreinLobbyRefreshPlayer.cap(2));
                                   }
                                   pos+=qreinLobbyRefreshPlayer.matchedLength();
                                }
                            }

                        }
                        if(str.contains(qreinLobbyRefreshBot))
                        {
                            numberOfBotBlackTeam=qreinLobbyRefreshBot.cap(1).toInt();
                            numberOfBotWhiteTeam=qreinLobbyRefreshBot.cap(2).toInt();
                            RefreshList();
                            break;
                        }
                        if(str.contains(qreinLobbyChat))
                        {
                            qteinLobbyChat->append("<H3><span style=color:#9370DB>"+qreinLobbyChat.cap(1)+"</span style>"+qreinLobbyChat.cap(2)+"</H3>");
                        }
                        break;
                    }
                    case inGame:
                    {
                        QSendToClientEvent* pe=new QSendToClientEvent();
                        pe->Text=str;
                        pe->forSwitch=0;
                        QApplication::postEvent(gamefield,pe);
                        break;
                    }
                }
                break;
            }//case 0 :)
            default:
            {
                QByteArray qba;
                in>>qba;
                QSendToClientEvent* pe=new QSendToClientEvent();
                pe->forSwitch=a;
                switch(a)
                {
                    case 1:
                    {
                        pe->stVector=(std::vector< std::string >)*qba.data();
                    }
                    case 2:
                    {
                        pe->peiVector=(std::vector< rbw::PlayerExportInformation >)*qba.data();
                    }
                    case 3:
                    {
                        pe->goVector=(std::vector< rbw::GraphicObject > )*qba.data();
                    }
                    if(gamefield!=NULL)
                    {
                        QApplication::postEvent(gamefield,pe);
                    }
                }
                break;
            }//defailt;
        }
        m_nNextBlockSize = 0;
    }
}

// ----------------------------------------------------------------------
void Client::SendToServer(QString str)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);

    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
}

// ------------------------------------------------------------------
void Client::Register(QString UserName )
{
    SendToServer("rename");
    qlMenu->setText("<H1>"+UserName+"</H1>");
    SendToServer(UserName);
}
 void Client::ShowButton(int Index)
 {
     //qDebug()<<Index;
     switch(Index)
     {
         case 0://Search the Game
         {
             if(!qlMenu->isHidden())
             {
                 qlMenu->hide();
                 qpbSearch->hide();
                 qpbLobby->hide();
                 qpbInfo->hide();
                 qpbRename->hide();
                 qpbExit->hide();
             }
             if(qlSearch->isHidden())
             {
                 qlSearch->show();
                 qpbCancel->show();
             }
             if(!qpbLobbyCreate->isHidden())
             {
                 qpbLobbySearch->hide();
                 qpbLobbyCreate->hide();
                 qpbLobbyBack->hide();
             }if(!qpbSingle->isHidden())
             {
                 qpbSingle->hide();
                 qpbMulti->hide();
             }
             break;
         }
         case 1://Menu
         {
             if(qlMenu->isHidden())
             {
                 qlMenu->show();
                 qpbSearch->show();
                 qpbLobby->show();
                 qpbInfo->show();
                 qpbRename->show();
                 qpbExit->show();
             }
             if(!qlSearch->isHidden())
             {
                 qlSearch->hide();
                 qpbCancel->hide();
             }
             if(!qpbLobbyCreate->isHidden())
             {
                 qpbLobbySearch->hide();
                 qpbLobbyCreate->hide();
                 qpbLobbyBack->hide();
             }
             if(!qpbSingle->isHidden())
             {
                 qpbSingle->hide();
                 qpbMulti->hide();
             }
             break;
         }
         case 2://Lobby
         {
             if(!qlMenu->isHidden())
             {
                 qlMenu->hide();
                 qpbSearch->hide();
                 qpbLobby->hide();
                 qpbInfo->hide();
                 qpbRename->hide();
                 qpbExit->hide();
             }
             if(!qlSearch->isHidden())
             {
                 qlSearch->hide();
                 qpbCancel->hide();
             }
             if(qpbLobbyCreate->isHidden())
             {
                 qpbLobbySearch->show();
                 qpbLobbyCreate->show();
                 qpbLobbyBack->show();
             }
             if(!qpbSingle->isHidden())
             {
                 qpbSingle->hide();
                 qpbMulti->hide();
             }
             break;
         }
         case 3://Single or Multi Game
         {
             if(!qlMenu->isHidden())
             {
                 qlMenu->hide();
                 qpbSearch->hide();
                 qpbLobby->hide();
                 qpbInfo->hide();
                 qpbRename->hide();
                 qpbExit->hide();
             }
             if(!qlSearch->isHidden())
             {
                 qlSearch->hide();
                 qpbCancel->hide();
             }
             if(!qpbLobbyCreate->isHidden())
             {
                 qpbLobbySearch->hide();
                 qpbLobbyCreate->hide();
                 qpbLobbyBack->hide();
             }
             if(qpbSingle->isHidden())
             {
                 qpbSingle->show();
                 qpbMulti->show();
             }
             break;
         }
         case 4://inLobby for Singel Game;
         {
             qpbinLobbyChange->hide();
             qpbinLobbySend->hide();
             qteinLobbyChat->hide();
             qleinLobbyChat->hide();
             if(numberOfBotBlackTeam+qvstr_inLobbyBlackTeam.size()!=numberOfBotWhiteTeam+qvstr_inLobbyWhiteTeam.size())
             {
                 qpbinLobbyStart->setEnabled(0);
             }
             break;
         }
     }
 }
void Client::RefreshList()
{
    qteLobbyforBlack->clear();
    qteLobbyforWhite->clear();
    qteLobbyforWhite->append(qsLobbyWhiteTeam);
    qteLobbyforBlack->append(qsLobbyBlackTeam);
    QVector<QString>::iterator it=qvstr_inLobbyBlackTeam.begin();
    for(;it!=qvstr_inLobbyBlackTeam.end();++it)
    {
        if(*it==qsKingOfLobby)
        {
            qteLobbyforBlack->append("<span style=color:#000000><H2>♕ "+*it+"</H2></span style>");
        }
        else
        {
            qteLobbyforBlack->append("<span style=color:#000000><H2>"+*it+"</H2></span style>");
        }
        //♕
    }
    for(int i=0;i<numberOfBotBlackTeam;i++)
    {
        qteLobbyforBlack->append("<span style=color:#000000><H2>Bot</H2></span style>");
    }
    it=qvstr_inLobbyWhiteTeam.begin();
    for(;it!=qvstr_inLobbyWhiteTeam.end();++it)
    {
        if(*it==qsKingOfLobby)
        {
            qteLobbyforWhite->append("<span style=color:#FFFFFF><H2>♕ "+*it+"</H2></span style>");
        }
        else
        {
            qteLobbyforWhite->append("<span style=color:#FFFFFF><H2>"+*it+"</H2></span style>");
        }
    }
    for(int i=0;i<numberOfBotWhiteTeam;i++)
    {
        qteLobbyforWhite->append("<span style=color:#FFFFFF><H2>Bot</H2></span style>");
    }
}

//==============QPushButton=====================
void Client::CreateConnect()
{
    connect(qpbSearch,SIGNAL(clicked()),
            this,SLOT(slotSearch()));
    connect(qpbInfo,SIGNAL(clicked()),
            this,SLOT(slotTakeInformation()));
    connect(qpbRename,SIGNAL(clicked()),
            this,SLOT(slotRename()));
}

void Client::DestroyConnect()
{
    disconnect(qpbSearch,SIGNAL(clicked()),
               this,SLOT(slotSearch()));
    disconnect(qpbInfo,SIGNAL(clicked()),
               this,SLOT(slotTakeInformation()));
    disconnect(qpbRename,SIGNAL(clicked()),
               this,SLOT(slotRename()));
}
void Client::slotSigleGame()
{
    hide();
    isSingleGame=1;
    bool b=0;
    QString UserName="";

    while(!UserName.contains(qreUserName))
    {
        if(b)
        {
            qmBox.information(this,"Information","Your UserName or incorrect or unavailable\n"
                                                     "Please Rename yourself (^|^)>)>)> ");
        }
        UserName=QInputDialog::getText(this,"Rocket",
                                               "Put your UserName(>4,<16):",
                                               QLineEdit::Normal,
                                               "Your UserName.",
                                               &b);
        if(!b)
        {

            qmBox.information(this,"Information","Goodbuy :)");
            slotExit();
            return;
        }
    }
    qvstr_inLobbyBlackTeam.clear();
    qvstr_inLobbyWhiteTeam.clear();
    numberOfBotWhiteTeam=0;
    numberOfBotBlackTeam=0;
    qwinLobby->show();
    qvstr_inLobbyBlackTeam<<UserName;
    RefreshList();
    ShowButton(4);
}

void Client::slotMultiGame()
{
    qwListOfServer->show();
    hide();
    isSingleGame=0;
}

void Client::slotSearch()
{
    SendToServer("search");
    ShowButton(0);
}

void Client::slotTakeInformation()
{
    SendToServer("info");
    DestroyConnect();
}

void Client::slotRename()
{
    SendToServer("rename");
    state=Registration;
    DestroyConnect();
    bool b;
    QString UserName=QInputDialog::getText(this,"Rocket",
                                           "Put your UserName(>4,<16):",
                                           QLineEdit::Normal,
                                           "Your UserName.",
                                           &b);
    if(!b)
    {

        qmBox.information(this,"Information","Goodbuy :)");
        slotExit();
    }
    else
    {
        Register(UserName);
        sig_PauseGame(0);
    }
}

void Client::slotExit()
{
    hide();
    m_pTcpSocket->close();
    QApplication::exit();
}
void Client::slotCancel()
{
    SendToServer("close");
    state=Menu;
    ShowButton(1);
}
void Client::slotBack()
{
    qwListOfServer->hide();
    show();
    ShowButton(3);
}
void Client::slotLobby()
{
    ShowButton(2);
}
void Client::slotLobbySearch()
{
    hide();
    SendToServer("list of lobby");
    state=inLobbySearch;
    qwLobbySearch->show();
    qpbLobbyBack->show();
}

void Client::slotLobbyCreate()
{
    bool b;
    QString LobbyName=QInputDialog::getText(this,"Rocket",
                                           "Put name of lobby.",
                                           QLineEdit::Normal,
                                           "",
                                           &b);
    if(b)
    {
        SendToServer("Create Lobby:"+LobbyName);
        qwLobbySearch->hide();
        qwinLobby->show();
    }


}
void Client::slotLobbyBack()
{
    show();
    if(isSingleGame)
    {
        ShowButton(3);
        qwinLobby->hide();
    }
    else
    {
        ShowButton(1);
        qwLobbySearch->hide();
        if(state==inLobby)
        {
            qwinLobby->hide();
            SendToServer("close");
        }
        state=Menu;
    }
}
void Client::slotLobbyUpdate()
{
    SendToServer("list of lobby");
}
void Client::slotinLobbyChange()
{
    SendToServer("change");
}
void Client::slotinLobbyStartGame()
{
    if(isSingleGame)
    {
        CreateGameField(qvstr_inLobbyBlackTeam.first());
    }
    else SendToServer("Start Game");
}
void Client::slotinLobbyAddBotBlack()
{
    if(numberOfBotBlackTeam<5)
    {
        qpbinLobbyRemoveBotBlack->setEnabled(1);
        if(isSingleGame)
        {
            numberOfBotBlackTeam++;
            if(numberOfBotBlackTeam+1==numberOfBotWhiteTeam)
            {
                qpbinLobbyStart->setEnabled(1);
            }
            else qpbinLobbyStart->setEnabled(0);
        }
        else
        SendToServer("add black bot");
    }
    else
    {
        qpbinLobbyAddBotBlack->setEnabled(0);
    }
    RefreshList();
}
void Client::slotinLobbyAddBotWhite()
{
    if(numberOfBotWhiteTeam<5)
    {
        qpbinLobbyRemoveBotWhite->setEnabled(1);
        if(isSingleGame)
        {
            numberOfBotWhiteTeam++;
            if(numberOfBotBlackTeam+1==numberOfBotWhiteTeam)
            {
                qpbinLobbyStart->setEnabled(1);
            }
            else qpbinLobbyStart->setEnabled(0);
        }
        else
        SendToServer("add white bot");
    }
    else
    {
        qpbinLobbyAddBotWhite->setEnabled(0);
    }
    RefreshList();
}
void Client::slotinLobbyRemoveWhite()
{
    if(numberOfBotWhiteTeam>0)
    {
        qpbinLobbyAddBotWhite->setEnabled(1);
        if(isSingleGame)
        {
            numberOfBotWhiteTeam--;
            if(numberOfBotBlackTeam+1==numberOfBotWhiteTeam)
            {
                qpbinLobbyStart->setEnabled(1);
            }
            else qpbinLobbyStart->setEnabled(0);
        }
        else
        SendToServer("remove white bot");
    }
    else
    {
        qpbinLobbyRemoveBotWhite->setEnabled(0);
    }
    RefreshList();
}
void Client::slotinLobbyRemoveBlack()
{
    if(numberOfBotBlackTeam>0)
    {
        qpbinLobbyAddBotBlack->setEnabled(1);
        if(isSingleGame)
        {
            numberOfBotBlackTeam--;
            if(numberOfBotBlackTeam+1==numberOfBotWhiteTeam)
            {
                qpbinLobbyStart->setEnabled(1);
            }
            else qpbinLobbyStart->setEnabled(0);
        }
        else
        SendToServer("remove black bot");
    }
    else
    {
        qpbinLobbyRemoveBotBlack->setEnabled(0);
    }
    RefreshList();
}
void Client::slotinLobbySend()
{
    if(qleinLobbyChat->text()!="")
    {
        SendToServer("chat "+qleinLobbyChat->text());
        qleinLobbyChat->setText("");
    }
}
//=================Create|Destroy=Game========================
void Client::CreateGameField(QString UsersName)
{
    gamefield=new GameField(this,isSingleGame);
    connect(gamefield,SIGNAL(sig_EndGame()),
            this,SLOT(DestroyGameField()));
    connect(this,SIGNAL(sig_PauseGame(bool)),
            gamefield,SLOT(slotPause(bool)));
    QStringList stlist=UsersName.split(" ");
    while(stlist.size()>0)
    {
        gamefield->AddUser(stlist.takeFirst().toStdString());
    }
    gamefield->addBot(numberOfBotBlackTeam,numberOfBotWhiteTeam);
    gamefield->start();
}
void Client::DestroyGameField()
{
    qDebug()<<"DESTROY THIS WORLD!!!";
    gamefield->deleteLater();
    gamefield=NULL;
    show();
    ShowButton(1);
}

//=============================================================
void Client::customEvent(QEvent* pe)
{
    if(!pause)
    {
        if((int)(pe)->type()==QSendToClientEvent::sendtoclienttype)
        {
            QSendToClientEvent* qstce=(QSendToClientEvent*)pe;
            SendToServer(qstce->Text);
        }
        else qDebug()<<"Error!";
    }
}
void Client::slotPause(bool b)
{
    pause=b;
}
//=================MyWidget=======================
MyWidget::MyWidget(QPixmap pixmap,QWidget* qw):QWidget(qw)
{
    QPalette pal;
    pal.setBrush(this->backgroundRole(),QBrush(pixmap));
    setPalette(pal);
    setAutoFillBackground(1);
    setMask(pixmap.createHeuristicMask());
    setFixedSize(pixmap.width(),pixmap.height());
}
void MyWidget::mousePressEvent(QMouseEvent* pe)
{
    mptPosition = pe->pos();
}

void MyWidget::mouseMoveEvent(QMouseEvent* pe)
{
    move(pe->globalPos() - mptPosition+QPoint(-15,-30));
}
