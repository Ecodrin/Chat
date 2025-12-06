#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QDateTime>

#include "workwithdata.h"
#include "utility"

class ChatWidget : public QWidget
{
    Q_OBJECT
public:
    ChatWidget(const MsgData & msg_data, QWidget *parent = nullptr);

private:
    QLabel * imgw;
    QLabel * textw;
    QLabel * whow;
    QLabel *timew;
    int timestamp;
};

#endif // CHATWIDGET_H
