#include "game.h"
#include <QTimer>
#include <QGraphicsTextItem>
#include <QFont>
#include <QBrush>
#include <QDebug>
#include <QImage>
#include <QUrl>
#include <QGraphicsRectItem>
#include "gamestate.h"
#include "inputhandler.h"
#include "client.h"
#include "wall.h"

Game::Game(Client * client,QWidget *parent){
    //qDebug()<<"setting up client...";

    // create the scene
    scene = new QGraphicsScene();


    scene->setSceneRect(0,0,1280,720); // make the scene 800x600 instead of infinity by infinity (default)

    qDebug()<<"reached here";

    qDebug()<<"crossed";
    InputHandler *inputHandler = new InputHandler(client,NULL);
    scene->setBackgroundBrush(QBrush(QImage(":/images/bg_spaceship.jpg")));

    // madhur
    //For the out walls (will be use in all the gameplay windows)

    QGraphicsRectItem *wall1= new QGraphicsRectItem();
    QGraphicsRectItem *wall2= new QGraphicsRectItem();
    QGraphicsRectItem *wall3= new QGraphicsRectItem();
    QGraphicsRectItem *wall4= new QGraphicsRectItem();
    QGraphicsRectItem *wall = new QGraphicsRectItem();

    scene->addItem((wall1));
    scene->addItem((wall2));
    scene->addItem((wall3));
    scene->addItem((wall4));

    wall1->setRect(0,0,50,720);
    wall2->setRect(0,0,1280,50);
    wall3->setRect(0,0,50,720);
    wall4->setRect(0,0,1280,50);

    wall1->setPos(0,0);
    wall2->setPos(0,0);
    wall3->setPos(1230,0);
    wall4->setPos(0,720-50);

    //  vertical bars

        QGraphicsRectItem *v1 = new QGraphicsRectItem();
        QGraphicsRectItem *v2 = new QGraphicsRectItem();
        scene->addItem(v1);
        scene->addItem(v2);
        v1->setRect(400,50,25,200);
        v2->setRect(1280-400-25,720-50-200,25,200);

        QGraphicsRectItem *v3 = new QGraphicsRectItem();
        QGraphicsRectItem *v4 = new QGraphicsRectItem();
        scene->addItem(v3);
        scene->addItem(v4);
        v3->setRect(475,50+300,25,200);
        v4->setRect(1280-475-25,720-50-200-300,25,200);

        QGraphicsRectItem *v5 = new QGraphicsRectItem();
        scene->addItem(v5);
        v5->setRect(1280/2-12,720/2-200,25,400);

        QGraphicsRectItem *v6 = new QGraphicsRectItem();
        QGraphicsRectItem *v7 = new QGraphicsRectItem();
        scene->addItem(v6);
        scene->addItem(v7);
        v6->setRect(1280-250-25,50,25,150);
        v7->setRect(250,720-50-150,25,150);

        //  Horizontal bars

        QGraphicsRectItem *h1 = new QGraphicsRectItem();
        QGraphicsRectItem *h2 = new QGraphicsRectItem();
        scene->addItem(h1);
        scene->addItem(h2);
        h1->setRect(225-25,50+350,300,25);
        h2->setRect(1280-225-300+25,720-350-50-25,300,25);

        QGraphicsRectItem *h3 = new QGraphicsRectItem();
        QGraphicsRectItem *h4 = new QGraphicsRectItem();
        scene->addItem(h3);
        scene->addItem(h4);
        h3->setRect(50,275,150,25);
        h4->setRect(1280-50-150,720-275-25,150,25);
    //
    // make the newly created scene the scene to visualize (since Game is a QGraphicsView Widget,
    // it can be used to visualize scenes)
    inputHandler->setScene(scene);
    inputHandler->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    inputHandler->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    inputHandler->setFixedSize(1280,720);

    inputHandler->setGameState(client->gameState);
    inputHandler->showFullScreen();

    inputHandler->show();
}

