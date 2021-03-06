#include "server.h"

#include <QtWebSockets>
#include <QGraphicsScene>
#include <QtCore>
#include <QDebug>
#include <string>
#include <QTimer>

#include "player.h"
#include "player_teama.h"
#include "player_teamb.h"

#include <cstdio>
using namespace std;

extern QGraphicsScene *scene;

QT_USE_NAMESPACE

bool isFlagATaken=false;
bool isFlagBTaken=false;
int takersIDA;
int takersIDB;


static QString getIdentifier(QWebSocket *peer)
{
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}

Server::Server(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Chat Server"),
                                            QWebSocketServer::NonSecureMode,
                                            this))
{
    gameState = new GameState();
    flagA=new Flag(true);
    flagB=new Flag(false);


    if (m_pWebSocketServer->listen(QHostAddress("0.0.0.0"), port))
    {

        qDebug() << "Server's connecting URL : " << m_pWebSocketServer->serverUrl();
        QTextStream(stdout) << "Chat Server listening on port " << port << '\n';
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &Server::onNewConnection);
        qDebug()<<"passed server onnewconnnection";
    }
}

Server::~Server()
{

}

void Server::startGameLoop(){

    qDebug()<<"sending start message to all the clients...";
    for (QList<QWebSocket*>::iterator i = m_clients.begin(); i != m_clients.end(); i++)
    {
          (*i)->sendTextMessage(QString::fromStdString("start"));
    }
    qDebug()<<"sent start message to all the clients";

    QTimer * timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(sendGameStateToClients()));

    // start the timer
    timer->start(50);
}

void Server::onFlagDropped_A()
{
    // drop flagA make flag visible and send all clients text that flag is dropped and make flag visible
    std::string drop_message;
    isFlagATaken=false;
    drop_message="dropA:";
    scene->addItem(flagA);

    drop_message +=std::to_string(takersIDA);
    for (QList<QWebSocket*>::iterator i = m_clients.begin(); i != m_clients.end(); i++)
    {
          (*i)->sendTextMessage(QString::fromStdString(drop_message));
    }

}

void Server::onFlagDropped_B()
{
    // drop flagA make flag visible and send all clients text that flag is dropped and make flag visible
    std::string drop_message;
    isFlagBTaken=false;
    drop_message="dropB:";
    scene->addItem(flagB);

    drop_message+=std::to_string(takersIDB);
    for (QList<QWebSocket*>::iterator i = m_clients.begin(); i != m_clients.end(); i++)
    {
          (*i)->sendTextMessage(QString::fromStdString(drop_message));
    }

}

void Server::sendGameStateToClients(){
    QJsonDocument doc = gameState->getJsonDocFromGameState();
    QByteArray bytes = doc.toJson();

    //qDebug()<<"server sent json message at game init:";
    //qDebug()<<bytes;

    for (QList<QWebSocket*>::iterator i = m_clients.begin(); i != m_clients.end(); i++)
    {
          (*i)->sendBinaryMessage(bytes);
    }
}

void Server::onNewConnection()
{
    qDebug()<<"new connection with client...";

    auto pSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";
    pSocket->setParent(this);

    connect(pSocket, &QWebSocket::textMessageReceived,
            this, &Server::onTextMessageFromClient);

    connect(pSocket, &QWebSocket::binaryMessageReceived,
            this, &Server::onBinaryMessageFromClient);

    connect(pSocket, &QWebSocket::disconnected,
            this, &Server::socketDisconnected);

    m_clients << pSocket;

//********can use mutex for new connection sending id


    std::string init_messsage="init:";
    init_messsage+=std::to_string(playersConnected);
    pSocket->sendTextMessage(QString::fromStdString(init_messsage));
    qDebug()<<"init message sent to client #"<<playersConnected;
    this->player_connected.lock();
    playersConnected++;
    this->player_connected.unlock();

    scene->addItem(flagA);
    scene->addItem(flagB);

}

void Server::onBinaryMessageFromClient(QByteArray message){
    // should be done in parallel
    QJsonDocument item_doc = QJsonDocument::fromJson(message);
    QJsonObject item_object = item_doc.object();
    int id = item_object["id"].toInt();

    QString key = item_object["key"].toString();

    if(key=="LEFT"){
        ///use mutex for flag
        ///below: returns whether flag taken or not
        bool flagTaken=gameState->players.at(id)->moveLeft();
        if(flagTaken==true){
            gameState->players.at(id)->setPixmap(QPixmap(":images/player.png"));
            std::string take_messsage;
            if(gameState->players.at(id)->team) {
                takersIDB=id;
                isFlagBTaken=true;
                take_messsage="takerB:";
                scene->removeItem(flagB);
            }
            else{
                isFlagATaken=true;
                takersIDA=id;
                take_messsage="takerA:";
                scene->removeItem(flagA);
            }

            take_messsage+=std::to_string(id);
            for (QList<QWebSocket*>::iterator i = m_clients.begin(); i != m_clients.end(); i++)
            {
                  (*i)->sendTextMessage(QString::fromStdString(take_messsage));
            }

        }
    }
    if(key=="RIGHT"){
        bool flagTaken = gameState->players.at(id)->moveRight();
        if(flagTaken==true){
            gameState->players.at(id)->setPixmap(QPixmap(":images/player.png"));
            std::string take_messsage;
            if(gameState->players.at(id)->team) {
                takersIDB=id;
                isFlagBTaken=true;
                take_messsage="takerB:";
                scene->removeItem(flagB);
            }
            else{
                isFlagATaken=true;
                takersIDA=id;
                take_messsage="takerA:";
                scene->removeItem(flagA);
            }

            take_messsage+=std::to_string(id);
            for (QList<QWebSocket*>::iterator i = m_clients.begin(); i != m_clients.end(); i++)
            {
                  (*i)->sendTextMessage(QString::fromStdString(take_messsage));
            }

        }
    }
    if(key=="UP"){
        bool flagTaken = gameState->players.at(id)->moveUp();
        if(flagTaken==true){
            gameState->players.at(id)->setPixmap(QPixmap(":images/player.png"));
            std::string take_messsage;
            if(gameState->players.at(id)->team) {
                takersIDB=id;
                isFlagBTaken=true;
                take_messsage="takerB:";
                scene->removeItem(flagB);
            }
            else{
                isFlagATaken=true;
                takersIDA=id;
                take_messsage="takerA:";
                scene->removeItem(flagA);
            }

            take_messsage+=std::to_string(id);
            for (QList<QWebSocket*>::iterator i = m_clients.begin(); i != m_clients.end(); i++)
            {
                  (*i)->sendTextMessage(QString::fromStdString(take_messsage));
            }

        }
    }
    if(key=="DOWN"){
        bool flagTaken = gameState->players.at(id)->moveDown();
        if(flagTaken==true){
            gameState->players.at(id)->setPixmap(QPixmap(":images/player.png"));
            std::string take_messsage;
            if(gameState->players.at(id)->team) {
                takersIDB=id;
                isFlagBTaken=true;
                take_messsage="takerB:";
                scene->removeItem(flagB);
            }
            else{
                isFlagATaken=true;
                takersIDA=id;
                take_messsage="takerA:";
                scene->removeItem(flagA);
            }

            take_messsage+=std::to_string(id);
            for (QList<QWebSocket*>::iterator i = m_clients.begin(); i != m_clients.end(); i++)
            {
                  (*i)->sendTextMessage(QString::fromStdString(take_messsage));
            }

        }
    }
    if(key=="W"||key=="A"||key=="S"||key=="D"){
        bullet *new_bullet = gameState->createBullet(key,gameState->players.at(id)->team,gameState->players.at(id)->pos().x()+25,gameState->players.at(id)->pos().y()+25);
        scene->addItem(new_bullet);
    }

    QJsonDocument doc = gameState->getJsonDocFromGameState();
    QByteArray bytes = doc.toJson();

    for (QList<QWebSocket*>::iterator i = m_clients.begin(); i != m_clients.end(); i++)
    {
          (*i)->sendBinaryMessage(bytes);
    }


}

void Server::onTextMessageFromClient(const QString &message)
{
    qDebug()<<"message recieved for ready";

        //qDebug()<<"init message recieved: "<<message;

        // make new player for client in server
        player *new_player;
        if(message.startsWith("readyA:")){
            int m_id = message.mid(7,message.size()-7).toInt();
            new_player = new player(m_id,true);
            new_player->setPos(start_a); // TODO generalize to always be in the middle bottom of screen
        }
       if(message.startsWith("readyB:")){
           int m_id = message.mid(7,message.size()-7).toInt();
            new_player = new player(m_id,false);
            new_player->setPos(start_b); // TODO generalize to always be in the middle bottom of screen
        }// make the player focusable and set it to be the current focus

        scene->addItem(new_player);

        gameState->players.push_back(new_player);


        QString m_id = message.mid(6,message.size()-6);
        this->mutex_ready.lock();
        playersReady++;
        this->mutex_ready.unlock();

}

void Server::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
    if (pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
