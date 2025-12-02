#include "chatwidget.h"

ChatWidget::ChatWidget(const std::string & who, const std::string & text, const std::string & img_filename, int timestamp, QWidget *parent )
    : QWidget(parent), timestamp(timestamp)
{
    QHBoxLayout * layout = new QHBoxLayout(this);

    QLabel * imgw = new QLabel();
    QPixmap pixmap(QString::fromStdString(img_filename));
    if(!pixmap.isNull()) {
        imgw->setPixmap(pixmap.scaled(50, 50, Qt::KeepAspectRatio));
    }

    QLabel * timew = new QLabel(QDateTime::fromSecsSinceEpoch(timestamp).toString("dd.MM.yyyy hh:mm:ss"));
    QLabel * whow = new QLabel(QString::fromStdString(who));
    QLabel * textw = new QLabel(QString::fromStdString(text));

    if(!pixmap.isNull()) {
        layout->addWidget(imgw);
    } else {
        delete imgw;
    }
    layout->addWidget(timew);
    layout->addWidget(whow);
    layout->addWidget(textw);
    layout->addStretch();

    setLayout(layout);
}
