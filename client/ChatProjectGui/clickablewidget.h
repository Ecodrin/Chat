#ifndef CLICKABLEWIDGET_H
#define CLICKABLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>

class ClickableWidget : public QLabel
{
public:
    ClickableWidget(const std::string & path, QWidget * parent);

protected:
    void mouseDoubleClickEvent(QMouseEvent * event) override;

private:
    std::string path;
};

#endif // CLICKABLEWIDGET_H
