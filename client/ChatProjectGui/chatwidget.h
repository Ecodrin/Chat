#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QDateTime>
#include <QTextOption>
#include <Qt>
#include <QTextEdit>
#include <QtCore>

#include "workwithdata.h"
#include "utility"
#include "clickablewidget.h"

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
