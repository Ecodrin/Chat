#include "clickablewidget.h"

ClickableWidget::ClickableWidget(const std::string & path, QWidget * parent)
    : path{path}, QLabel(parent) {
    setText(QString::fromStdString(path));
}

void ClickableWidget::mouseDoubleClickEvent(QMouseEvent * event) {
    if(event->button() == Qt::LeftButton) {
        QProcess process;
        process.start("xdg-open", QStringList() << QString::fromStdString(path));
        process.waitForFinished();
        int exit_code = process.exitCode();
        if(exit_code) {
            QString save_path = QFileDialog::getSaveFileName(
                this,
                tr("сохранить файл"),
                QString::fromStdString(path),
                tr("Все файлы (*)")
                );
            QFile::copy(QString::fromStdString(path), save_path);
        }
    }
    QLabel::mousePressEvent(event);
}
