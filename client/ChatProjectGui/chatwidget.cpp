#include "chatwidget.h"

ChatWidget::ChatWidget(const MsgData & msg_data, QWidget *parent )
    : QWidget(parent)
{
    QHBoxLayout * layout = new QHBoxLayout(this);
    QLabel * timew = new QLabel(QDateTime::fromSecsSinceEpoch(msg_data.timestamp).toString("dd.MM.yyyy hh:mm:ss"));
    layout->addWidget(timew);

    QLabel * whow = new QLabel(QString::fromStdString(msg_data.sender));
    layout->addWidget(whow);

    if(msg_data.is_file && (has_suffix(msg_data.data, ".png") || has_suffix(msg_data.data, ".jpg"))) {
        QLabel * imgw = new QLabel();
        QPixmap pixmap(QString::fromStdString(msg_data.data));
        if(!pixmap.isNull()) {
            imgw->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        layout->addWidget(imgw);
    } else {
        // TODO дописать если это файл -- сохранить как
        QLabel * textw = new QLabel(QString::fromStdString(msg_data.data));
        layout->addWidget(textw);
    }

    setLayout(layout);
}
