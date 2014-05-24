#include "server.h"

Server::Server(int nPort, QWidget *parent)
    : QWidget(parent),m_nNextBlockSize(0),numPeople(2),
      qreUserName("[a-zA-z]{4,16}$"),qreCreateLobby("Create Lobby:([a-zA-z_ ]{3,30})$"),
      qreContoLobby("con ([a-zA-z_ ]{3,30}) \\["),
      qreinLobbyChat("chat ([^\\^]+)")
{
    qtcpServer=new QTcpServer(this);
    if(!qtcpServer->listen(QHostAddress::Any,nPort))
    {
        QMessageBox::critical(0,
                              "Server Error",
                              "Unable to start the server:"
                              +qtcpServer->errorString());
        qtcpServer->close();
        return;
    }
    connect(qtcpServer,SIGNAL(newConnection()),
            this,SLOT(slotNewConnection()));
    for(int i=0;i<=999;i++)
    {
        client[i]=NULL;
    }
    for(int i=0;i<=499;i++)
    {
        ingame[i]=NULL;
    }
    qDebug()<<this->thread()<<" "<<QApplication::instance()->thread();
}
Server::~Server()
{

}
//===========================Connect=or=Disconnect=Client================
void Server::slotNewConnection()
{
    int i=0;
    for(;i<=maxIndexClintInfo;i++)
    {
        if(client[i]==NULL)break;
    }
    if(i>maxIndexClintInfo)
    {
        maxIndexClintInfo++;
    }
    if(i>999)return;
    client[i]=new ClientsInfo(qtcpServer->nextPendingConnection(),i);
    connect(client[i],SIGNAL(sig_Disconnected()),
            this,SLOT(slotDisconnect()));
    connect(client[i],SIGNAL(sig_ReadyRead()),
            this,SLOT(slotReadClient()));
}
void Server::slotDisconnect()
{
    ClientsInfo* qtcpClient=(ClientsInfo*)sender();
    int Index=qtcpClient->TakeInfo("I");
    qDebug()<<Index;
    switch(client[Index]->state)
    {
       case SearchGame:
       {
           qlSearch.removeAt(qlSearch.indexOf(Index));
           break;
       }
       case inGame:
       {
           //sendToGame(qtcpClient->TakeInfo("I"),"SUR");
           break;
       }
       case inLobby:
       {
           client[Index]->lobby->delUsers(client[Index]);
       }
    }
    client[Index]->deleteLater();
    client[Index]=NULL;
    if(Index==maxIndexClintInfo)maxIndexClintInfo--;
}

//============================Read||Send=Client=====================================
void Server::slotReadClient()
{
    ClientsInfo* ciClient=(ClientsInfo*)sender();
    QTcpSocket* qtcpClient=ciClient->Take_Socket();
    QDataStream in(qtcpClient);
    in.setVersion(QDataStream::Qt_4_2);
    while(1)
    {
        if (!m_nNextBlockSize) {
            if (qtcpClient->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }
        //qDebug()<<m_nNextBlockSize;
        //qDebug()<<qtcpClient->bytesAvailable();
        if (qtcpClient->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QString str;
        in >> str;
        qDebug()<<ciClient->TakeInfo("I")<<"-"<<ciClient->UserName<<" say:"<<str;
        //----------------------State-------------------------------------
        switch(ciClient->state)
        {

            case Registration://+++++++++++++++++++++++++++++++++++++++++++
            {
                int i=0;
                if(str=="rename")break;
                if(!str.contains(qreUserName))
                {
                    sendToClient(qtcpClient,"rename");
                    break;
                }
                for(;i<=maxIndexClintInfo;i++)
                {
                    {
                        if(client[i]!=NULL)
                        {
                            if(str==client[i]->UserName)
                            {
                                sendToClient(qtcpClient,"rename");
                                break;
                            }
                        }
                    }
                }
                if(i>maxIndexClintInfo)
                {
                     ciClient->UserName=str;
                     sendToClient(qtcpClient,"menu");
                     ciClient->state=Menu;
                }
                break;
            }
            case Menu://+++++++++++++++++++++++++++++++++++++++++++++++++++
            {
                if(str=="search")
                {
                     if(qlSearch.isEmpty())
                     {
                        QTimer::singleShot(3*1000,this,SLOT(slotCreateGame()));
                     }
                     qlSearch<<ciClient->TakeInfo("I");
                     ciClient->state=SearchGame;
                     sendToClient(qtcpClient,"Start Search Game");
                }
                else
                    if(str=="info")
                    {
                         QString qsInformation;
                         for(int i=0;i<=maxIndexClintInfo;i++)
                         {
                            if((client[i]!=NULL)&&(client[i]->state!=Registration))
                            {
                                qsInformation+=client[i]->UserName+" "+
                                               QString::number(client[i]->TakeInfo("W"))+ " "+
                                               QString::number(client[i]->TakeInfo("L"))+ " ";
                                switch(client[i]->state)
                                {
                                    case Menu:
                                    {
                                        qsInformation+="Menu; ";
                                        break;
                                    }
                                    case SearchGame:
                                    {
                                        qsInformation+="SearchGame; ";
                                        break;
                                    }
                                    case inGame:
                                    {
                                        qsInformation+="In Game; ";
                                        break;
                                    }
                                }
                            }

                         }
                         sendToClient(qtcpClient,qsInformation);
                         qDebug()<<qsInformation;
                    }
                    else
                        if(str=="rename")
                        {
                             ciClient->UserName="";
                             ciClient->state=Registration;
                             break;
                        }
                    if(str=="list of lobby")
                    {
                        QString qsListofLobby="";
                        Lobby* l;
                        QVector<Lobby*>::iterator it=ListofLobby.begin();
                        for(;it!=ListofLobby.end();++it)
                        {
                            l=*it;
                            qsListofLobby+=l->LobbyName+l->lengthOfTeam();
                        }
                        if(ListofLobby.size()==0)
                        {
                            sendToClient(qtcpClient,"None");
                            break;
                        }
                        sendToClient(qtcpClient,qsListofLobby);
                        qDebug()<<qsListofLobby;
                        break;
                    }
                    if(str.contains(qreCreateLobby))
                    {
                        QString LobbyName=qreCreateLobby.cap(1);
                        Lobby* l;
                        QVector<Lobby*>::iterator it=ListofLobby.begin();
                        bool isRename=0;
                        for(;it!=ListofLobby.end();++it)
                        {
                            l=*it;
                            if(l->LobbyName==LobbyName)
                            {
                                sendToClient(qtcpClient,"rename");
                                isRename=true;
                                break;
                            }
                        }
                        if(isRename)break;
                        ciClient->lobby=new Lobby(ciClient,LobbyName,this);
                        ListofLobby<< ciClient->lobby;
                        ciClient->state=inLobby;
                        sendToClient(qtcpClient,"Start Lobby");

                        ciClient->lobby->RefreshList(ciClient);
                        ciClient->lobby->newKing();
                        connect(ciClient->lobby,SIGNAL(sig_delete()),SLOT(slotDeleteLobby()));
                        connect(ciClient->lobby,SIGNAL(sig_createTheGame()),SLOT(slotinLobbyCreateGame()));
                        qDebug()<<"Create new Lobby:"+LobbyName+"!!";
                        break;
                    }
                    if(str.contains(qreContoLobby))
                    {
                        Lobby* ci;
                        QString LobbyName=qreContoLobby.cap(1);
                        QVector<Lobby*>::iterator it=ListofLobby.begin();
                        for(;it!=ListofLobby.end();++it)
                        {
                            ci=*it;
                            if(ci->LobbyName==LobbyName)
                            {
                                ciClient->state=inLobby;
                                ciClient->lobby=ci;
                                sendToClient(qtcpClient,"Start Lobby");
                                ci->addUsers(ciClient);
                                ci->RefreshList(ciClient);
                                break;
                            }
                        }
                        qDebug()<<LobbyName;
                        if(it==ListofLobby.end())
                        {
                            sendToClient(qtcpClient,"error");
                        }
                    }
                break;
            }
            case SearchGame://+++++++++++++++++++++++++++++++++++++++++++++
            {
                if(str=="close")
                {
                    qlSearch.removeAt(qlSearch.indexOf(ciClient->TakeInfo("I")));
                    ciClient->state=Menu;
                }
                break;
            }
            case inLobby:
            {
                if(str=="close")
                {
                    ciClient->lobby->delUsers(ciClient);
                    ciClient->lobby=NULL;
                    ciClient->state=Menu;
                    break;
                }
                if(str=="change")
                {
                    ciClient->lobby->ChangeTeam(ciClient);
                    break;
                }
                if(str=="Start Game")
                {
                    if(ciClient->lobby->isKing(ciClient->UserName))
                    {
                        ciClient->lobby->StartGame();
                    }
                }
                if((ciClient->lobby!=NULL)&&(ciClient->lobby->isKing(ciClient->UserName)))
                {
                    ciClient->lobby->addBot(str);
                }
                if(str.contains(qreinLobbyChat))
                {
                    ciClient->lobby->SendMessagetoClient(":"+qreinLobbyChat.cap(1),ciClient);
                }
                break;
            }
            case inGame://+++++++++++++++++++++++++++++++++++++++++++++++++
            {

                if(str=="surrender")
                {
                    --(*ciClient);
                    ciClient->state=Menu;
                }
                qDebug()<<ciClient->TakeInfo("I");
                sendToGame(ciClient->TakeInfo("I"),str);
                break;
            }
        }   //----------------------------------------------------------------------

        m_nNextBlockSize = 0;
    }
}
void Server::sendToClient(QTcpSocket* qtcpClient,const QString& str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);

    out<< quint16(0)<<quint8(0)<<str;
    out.device()->seek(0);

    out<<quint16(arrBlock.size()-sizeof(quint16));
    qtcpClient->write(arrBlock);
}
void Server::sendToClient(QTcpSocket* qtcpClient, std::vector< rbw::PlayerExportInformation > vector)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);

    QByteArray qba;
    qba.fromRawData((const char *)&vector,sizeof(vector));
    out<< quint16(0)<<quint8(1)<<qba;
    out.device()->seek(0);

    out<<quint16(arrBlock.size()-sizeof(quint16));
    qtcpClient->write(arrBlock);
}
void Server::sendToClient(QTcpSocket* qtcpClient, std::vector< std::string > str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    QByteArray qba;
    qba.fromRawData((const char *)&str,sizeof(str));
    out<< quint16(0)<<quint8(2)<<qba;
    out.device()->seek(0);

    out<<quint16(arrBlock.size()-sizeof(quint16));
    qtcpClient->write(arrBlock);
}
void Server::sendToClient(QTcpSocket* qtcpClient, std::vector<rbw::GraphicObject> objects )
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);

    QByteArray qba;
    qba.fromRawData((const char *)&objects,sizeof(objects));
    out<< quint16(0)<<quint8(3)<<qba;
    out.device()->seek(0);

    out<<quint16(arrBlock.size()-sizeof(quint16));
    qtcpClient->write(arrBlock);
}
void Server::sendToGame(int Index,const QString& str)
{
    QSendToClientEvent* pe=new QSendToClientEvent(client[Index]->IndexInGame);
    pe->Text=str;
    QApplication::postEvent(ingame[client[Index]->Match],pe);
}

//==========================Create|Destroy=Game=====================================
void Server::slotCreateGame()
{
    if(qlSearch.size()>=numPeople)//---------------CREATE-THE-WORLD----------------
    {
        int j=0;
        while(ingame[j]!=NULL)
        {
            if(++j>=500)
            {

                QTimer::singleShot(1*1000,this,SLOT(slotCreateGame()));
                return;
            }
        }
        ingame[j]=new InGame((QObject*)this,j);
        connect(ingame[j],SIGNAL(sigEndGame(int)),
                this,SLOT(slotDestroyGame(int)));
        qDebug()<<"Create Game for Users:";
        for(int i=0;i<=numPeople-1;i++)
        {
            int Index=qlSearch.takeFirst();
            qDebug()<<client[Index]->UserName;
            client[Index]->state=inGame;
            client[Index]->Match=j;
            client[Index]->IndexInGame=i;
            ingame[j]->addUser(client[Index]->Take_Socket(),client[Index]->UserName,Index);
        }
        ingame[j]->start();
    }//-------------------------------------------------------------------------------
    if(!qlSearch.isEmpty())
    {
        //qDebug()<<"I see you";
        QTimer::singleShot(1*1000,this,SLOT(slotCreateGame()));
    }
}
void Server::slotinLobbyCreateGame()
{
    Lobby* lobby=(Lobby*)sender();
    int j=0;
    while(ingame[j]!=NULL)
    {
        if((++j)>=500)
        {
            QTimer::singleShot(1*1000,this,SLOT(slotinLobbyCreateGame()));
            return;
        }
    }
    ingame[j]=new InGame((QObject*)this,j);
    connect(ingame[j],SIGNAL(sigEndGame(int)),
            this,SLOT(slotDestroyGame(int)));
    qDebug()<<"Create Game for Users:";

    QVector<ClientsInfo*> BlackTeam=lobby->take_vectorOfBlackTeam();
    QVector<ClientsInfo*> WhiteTeam=lobby->take_vectorOfWhiteTeam();
    ClientsInfo* ci;
    QVector<ClientsInfo*>::iterator it=BlackTeam.begin();
    int Index=-1;
    for(;it!=BlackTeam.end();++it)
    {
        ci=*it;
        qDebug()<<ci->UserName;
        ci->state=inGame;
        ci->Match=j;
        ci->IndexInGame=++Index;
        ingame[j]->addUser(ci->Take_Socket(),ci->UserName,ci->TakeInfo("I"),1);
    }
    it=WhiteTeam.begin();

    for(;it!=WhiteTeam.end();++it)
    {
        ci=*it;
        qDebug()<<ci->UserName;
        ci->state=inGame;
        ci->Match=j;
        ci->IndexInGame=++Index;
        ingame[j]->addUser(ci->Take_Socket(),ci->UserName,ci->TakeInfo("I"),2);
    }
    int botBlackTeam;
    int botWhiteTeam;
    lobby->RefreshNumberOfBot(botBlackTeam,botWhiteTeam);
    ingame[j]->addBot(botWhiteTeam,botBlackTeam);
    ingame[j]->start();
    lobby->sig_delete();
}

void Server::slotDestroyGame(int Index)
{
    int IndexOfClient;qDebug()<<"DESTROY THIS WORLD("<<Index<<")";
    disconnect(ingame[Index],SIGNAL(sigEndGame(int)),
               this,SLOT(slotDestroyGame(int)));
    for(int i=0;i<=ingame[Index]->takeMaxIndex();i++)
    {
        IndexOfClient=ingame[Index]->takeIndexOfClient(i);
        if(client[IndexOfClient]!=NULL)
        {
            /*switch(ingame[Index]->takeState(i))
            {
                case Lose:
                {
                    (*client[IndexOfClient])--;
                    break;
                }
                case Win:
                {
                    (*client[IndexOfClient])++;
                    break;
                }
            }*/
            client[IndexOfClient]->state=Menu;
        }
    }
    ingame[Index]->deleteLater();
    ingame[Index]=NULL;
}

//==========================================================================
void Server::customEvent(QEvent* pe)
{
    if((int)(pe)->type()==QSendToClientEvent::sendtoclienttype)
    {
        QSendToClientEvent* qstce=(QSendToClientEvent*)pe;
        if(client[qstce->Index]==NULL)return;
        switch(qstce->forSwitch)
        {
            case 0:
            {
                sendToClient(client[qstce->Index]->Take_Socket(),qstce->Text);
                //qDebug()<<"Hello Text";
                break;
            }
            case 1:
            {
                sendToClient(client[qstce->Index]->Take_Socket(),qstce->stVector);
                //qDebug()<<"Hello stVector";
                break;
            }
            case 2:
            {
                sendToClient(client[qstce->Index]->Take_Socket(),qstce->peiVector);
                //qDebug()<<"Hello peiVector";
                break;
            }
            case 3:
            {
                sendToClient(client[qstce->Index]->Take_Socket(),qstce->goVector);
                //qDebug()<<"Hello goVector";
                break;
            }
        }
    }
    else qDebug()<<"Error!";
}
//==========================================================================
//============ClientInfo===========================
int ClientsInfo::TakeInfo(const QString str)
{
    if(str=="I")
    {
        return Index;
    }
    if(str=="W")
    {
        return Win;
    }
    if(str=="L")
    {
        return Lose;
    }
    return -1;
}
//=============Lobby========================================
Lobby::~Lobby()
{
    ClientsInfo* ci;
    QVector<ClientsInfo*>::iterator it=BlackTeam.begin();
    for(;it!=BlackTeam.end();++it)
    {
        ci=*it;
        ci->lobby=NULL;
    }
    it=WhiteTeam.begin();
    for(;it!=WhiteTeam.end();++it)
    {
        ci=*it;
        ci->lobby=NULL;
    }
}

void Lobby::addUsers(ClientsInfo* client)
{
    QString str=client->UserName+" Win:"+QString::number(client->TakeInfo("W"))+
                " Lose:"+QString::number(client->TakeInfo("L"));
    if(BlackTeam.size()+numberOfBotBlackTeam <= WhiteTeam.size()+numberOfBotWhiteTeam)
    {
        SendtoClient("new [B]"+str);
        BlackTeam<<client;
    }
    else
    {
        SendtoClient("new [W]"+str);
        WhiteTeam<<client;
    }
    SendMessagetoClient(" <span style=color:#006400>Welcome</span style>!",client);
}
void Lobby::delUsers(ClientsInfo* client)
{
    int Index;
    Index=BlackTeam.indexOf(client);
    if(Index!=-1)
    {
        BlackTeam.remove(Index);
        SendtoClient("del [B]"+client->UserName+" Win:"+QString::number(client->TakeInfo("W"))+
                     " Lose:"+QString::number(client->TakeInfo("L")));
    }
    else
    {
        WhiteTeam.remove(WhiteTeam.indexOf(client));
        SendtoClient("del [W]"+client->UserName+" Win:"+QString::number(client->TakeInfo("W"))+
                     " Lose:"+QString::number(client->TakeInfo("L")));
    }
    client->lobby=NULL;
    if(WhiteTeam.size()+BlackTeam.size()==0)
    {
        sig_delete();
        return;
    }
    SendMessagetoClient(" <span style=color:#696969>came out of the lobby</span style>.",client);
    if(client->UserName==KingOfLobby)
    {
        newKing();
    }
}
void Lobby::addBot(QString str)
{
    if(str=="add black bot")
    {
        if(numberOfBotBlackTeam<5)
        {
            numberOfBotBlackTeam++;
            SendtoClient(str);
            SendMessagetoClient(" <span style=color:#008B8B>add black bot</span style>.",ciKingOfLobby);
        }
    }
    else
        if(str=="add white bot")
        {
            if(numberOfBotWhiteTeam<5)
            {
                numberOfBotWhiteTeam++;
                SendtoClient(str);
                SendMessagetoClient(" <span style=color:#008B8B>add white bot</span style>.",ciKingOfLobby);
            }
        }
        else
            if(str=="remove white bot")
            {
                if(numberOfBotWhiteTeam>0)
                {
                    numberOfBotWhiteTeam--;
                    SendtoClient(str);
                    SendMessagetoClient(" <span style=color:#008B8B>remove white bot</span style>.",ciKingOfLobby);
                }
            }
            else
                if(str=="remove black bot")
                {
                    if(numberOfBotBlackTeam>0)
                    {
                        numberOfBotBlackTeam--;
                        SendtoClient(str);
                        SendMessagetoClient(" <span style=color:#008B8B>remove black bot</span style>.",ciKingOfLobby);
                    }
                }
}
void Lobby::RefreshNumberOfBot(ClientsInfo* client)
{
    QSendToClientEvent* pe=new QSendToClientEvent(client->TakeInfo("I"));
    pe->Text="Refresh Number of Bot:"+QString::number(numberOfBotBlackTeam)+
                                      QString::number(numberOfBotWhiteTeam);
    pe->forSwitch=0;
    QApplication::postEvent(server,pe);
}
void Lobby::RefreshNumberOfBot(int& NumberOfBotBlackTeam,int& NumberOfBotWhiteTeam)
{
    NumberOfBotBlackTeam=numberOfBotBlackTeam;
    NumberOfBotWhiteTeam=numberOfBotWhiteTeam;
}
void Server::slotDeleteLobby()
{
    Lobby* lobby=(Lobby*)sender();
    disconnect(lobby,SIGNAL(sig_delete()),this,SLOT(slotDeleteLobby()));
    ListofLobby.remove(ListofLobby.indexOf(lobby));
    qDebug()<<"Destroy Lobby;";
    delete lobby;
}

void Lobby::ChangeTeam(ClientsInfo* client)
{
    int Index;
    Index=BlackTeam.indexOf(client);
    if(Index!=-1)
    {
        BlackTeam.remove(Index);
        WhiteTeam<<client;
        SendtoClient("change [W]"+client->UserName+" Win:"+QString::number(client->TakeInfo("W"))+
                     " Lose:"+QString::number(client->TakeInfo("L")));
    }
    else
    {
        WhiteTeam.remove(WhiteTeam.indexOf(client));
        BlackTeam<<client;
        SendtoClient("change [B]"+client->UserName+" Win:"+QString::number(client->TakeInfo("W"))+
                     " Lose:"+QString::number(client->TakeInfo("L")));
    }
}
void Lobby::RefreshList(ClientsInfo* client)
{
    QString Text;
    ClientsInfo* ci;
    QVector<ClientsInfo*>::iterator it=BlackTeam.begin();
    for(;it!=BlackTeam.end();++it)
    {
        ci=*it;
        Text+="[B]"+ci->UserName+" Win:"+QString::number(client->TakeInfo("W"))+
                " Lose:"+QString::number(client->TakeInfo("L"))+" ";
    }
    it=WhiteTeam.begin();
    for(;it!=WhiteTeam.end();++it)
    {
        ci=*it;
        Text+="[W]"+ci->UserName+" Win:"+QString::number(client->TakeInfo("W"))+
                " Lose:"+QString::number(client->TakeInfo("L"))+" ";\
    }
    QSendToClientEvent* pe=new QSendToClientEvent(client->TakeInfo("I"));
    pe->Text=Text;
    pe->forSwitch=0;
    QApplication::postEvent(server,pe);
    if(client->UserName!=KingOfLobby)
    {
        SendNameOfKing(client);
    }
    RefreshNumberOfBot(client);
}
void Lobby::SendNameOfKing(ClientsInfo* client)
{
    QString str="newKing [B]"+KingOfLobby+" Win:"+QString::number(client->TakeInfo("W"))+
            " Lose:"+QString::number(client->TakeInfo("L"));
    QSendToClientEvent* pe=new QSendToClientEvent(client->TakeInfo("I"));
    pe->Text=str;
    pe->forSwitch=0;
    QApplication::postEvent(server,pe);
}
void Lobby::newKing()
{
    ClientsInfo* client;
    if(BlackTeam.size()*!WhiteKing+!WhiteTeam.size())
    {
        client=BlackTeam.first();
        KingOfLobby=client->UserName;
        SendtoClient("newKing [B]"+KingOfLobby+" Win:"+QString::number(client->TakeInfo("W"))+
                     " Lose:"+QString::number(client->TakeInfo("L")));
    }
    else
    {
        client=WhiteTeam.first();
        KingOfLobby=client->UserName;
        SendtoClient("newKing [W]"+KingOfLobby+" Win:"+QString::number(client->TakeInfo("W"))+
                     " Lose:"+QString::number(client->TakeInfo("L")));
    }
    QSendToClientEvent* pe=new QSendToClientEvent(client->TakeInfo("I"));
    ciKingOfLobby=client;
    pe->Text="You new King";
    pe->forSwitch=0;
    QApplication::postEvent(server,pe);
    SendMessagetoClient(" <span style=color:#CD950C>new King of Lobby</span style>.",client);
}
bool Lobby::isKing(QString UserName)
{
    return UserName==KingOfLobby;
}
void Lobby::StartGame()
{
    if(WhiteTeam.size()+numberOfBotWhiteTeam==BlackTeam.size()+numberOfBotBlackTeam)
    {
        sig_createTheGame();
    }
    else
    {
        SendtoClient("None");
    }
}
QVector<ClientsInfo*> Lobby::take_vectorOfWhiteTeam()
{
    return WhiteTeam;
}
QVector<ClientsInfo*> Lobby::take_vectorOfBlackTeam()
{
    return BlackTeam;
}
QString Lobby::lengthOfTeam()
{
    return (" ["+
            QString::number(BlackTeam.size())+"|"+
            QString::number(WhiteTeam.size())+"] ");
}

void Lobby::SendtoClient(QString str)
{
    ClientsInfo* ci;
    QVector<ClientsInfo*>::iterator it=BlackTeam.begin();
    for(;it!=BlackTeam.end();++it)
    {
        ci=*it;
        QSendToClientEvent* pe=new QSendToClientEvent(ci->TakeInfo("I"));
        pe->Text=str;
        pe->forSwitch=0;
        QApplication::postEvent(server,pe);
    }
    it=WhiteTeam.begin();
    for(;it!=WhiteTeam.end();++it)
    {
        ci=*it;
        QSendToClientEvent* pe=new QSendToClientEvent(ci->TakeInfo("I"));
        pe->Text=str;
        pe->forSwitch=0;
        QApplication::postEvent(server,pe);
    }
}
void  Lobby::SendMessagetoClient(QString Text,ClientsInfo* client)
{

    ClientsInfo* ci;
    QVector<ClientsInfo*>::iterator it=BlackTeam.begin();
    for(;it!=BlackTeam.end();++it)
    {
        ci=*it;
        QSendToClientEvent* pe=new QSendToClientEvent(ci->TakeInfo("I"));
        pe->Text="chat "+client->UserName+Text;
        pe->forSwitch=0;
        QApplication::postEvent(server,pe);
    }
    it=WhiteTeam.begin();
    for(;it!=WhiteTeam.end();++it)
    {
        ci=*it;
        QSendToClientEvent* pe=new QSendToClientEvent(ci->TakeInfo("I"));
        pe->Text="chat "+client->UserName+Text;
        pe->forSwitch=0;
        QApplication::postEvent(server,pe);
    }
    //qDebug()<<"chat "+ci->UserName+Text;
}
