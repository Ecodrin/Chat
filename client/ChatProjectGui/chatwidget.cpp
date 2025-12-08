#include "chatwidget.h"

ChatWidget::ChatWidget(const MsgData & msg_data, QWidget *parent )
    : QWidget(parent)
{
    setMaximumWidth(parent->width() - 20);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    QVBoxLayout * main_layout = new QVBoxLayout(this);
    QHBoxLayout * layout = new QHBoxLayout();
    QLabel * timew = new QLabel(QDateTime::fromSecsSinceEpoch(msg_data.timestamp).toString("dd.MM.yyyy hh:mm:ss"));
    timew->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(timew);

    QLabel * whow = new QLabel(QString::fromStdString(msg_data.sender));
    whow->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(whow);
    main_layout->addLayout(layout);

    if(msg_data.is_file && (has_suffix(msg_data.data, ".png") || has_suffix(msg_data.data, ".jpg"))) {
        QLabel * imgw = new ClickableWidget(msg_data.data, this);
        QPixmap pixmap(QString::fromStdString(msg_data.data));
        if(!pixmap.isNull()) {
            imgw->setPixmap(pixmap.scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        main_layout->addWidget(imgw);
    } else {
        if(msg_data.is_file) {
            QLabel * filew = new ClickableWidget(msg_data.data, this);
            filew->setWordWrap(true);
            filew->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
            main_layout->addWidget(filew);
        } else {
            QTextEdit * textw = new QTextEdit(QString::fromStdString(msg_data.data));
            textw->setReadOnly(true);
            textw->setWordWrapMode(QTextOption::WrapAnywhere);
            textw->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            textw->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            textw->setFrameShape(QFrame::NoFrame);
            textw->setMaximumWidth(parent->width()-40);
            textw->document()->adjustSize();
            QSize size = textw->document()->size().toSize();
            textw->setFixedHeight(size.height() + 10);
            main_layout->addWidget(textw);
        }
    }

    setLayout(main_layout);
    adjustSize();
}
