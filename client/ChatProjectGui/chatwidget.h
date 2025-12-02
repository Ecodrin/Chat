#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QDateTime>

class ChatWidget : public QWidget
{
    Q_OBJECT
public:
    ChatWidget(const std::string & who, const std::string & text, const std::string & img_filename, int timestamp, QWidget *parent = nullptr);

private:
    QLabel * imgw;
    QLabel * textw;
    QLabel * whow;
    QLabel *timew;
    int timestamp;
};

#endif // CHATWIDGET_H
