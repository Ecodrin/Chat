#include "chatwidget.h"

ChatWidget::ChatWidget(const std::string & who, const std::string & text, const std::string & img_filename, int timestamp, QWidget *parent ) :
    QWidget{parent}, timestamp{timestamp}
{

    QHBoxLayout * layout = new QHBoxLayout(this);
    imgw = new QLabel();
    QPixmap pixmap(QString::fromStdString(img_filename));
    if(!pixmap.isNull()) {
        imgw->setPixmap(pixmap);
    }

    textw = new QLabel(QString::fromStdString(text));
    timew = new QLabel(QDateTime::fromSecsSinceEpoch(timestamp).toString("dd.MM.yyyy hh:mm:ss"));
    whow = new QLabel(QString::fromStdString(who));

    if(!pixmap.isNull()) {
        layout->addWidget(imgw);
    }
    layout->addWidget(timew);
    layout->addWidget(whow);
    layout->addWidget(textw);

}
