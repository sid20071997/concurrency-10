#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QDialog>
#include <QDebug>
#include "client.h"

namespace Ui {
class gamewindow;
}

class gamewindow : public QDialog
{
    Q_OBJECT

public:
    explicit gamewindow(Client *client,QWidget *parent = 0);
    ~gamewindow();
    bool team;
    Client *window_client;

private slots:


    void on_radioButton_toggled(bool checked);

    void on_radioButton_2_toggled(bool checked);

    void on_pushButton_clicked();

private:
    Ui::gamewindow *ui;
};

#endif // GAMEWINDOW_H
