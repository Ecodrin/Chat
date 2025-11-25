#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>

#include "grpc_client.hpp"
#include "newconversationdialog.h"

namespace Ui {
class ContactDialog;
}

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(const std::string & contact, GreeterClient * client, QWidget *parent = nullptr);
    ~ContactDialog();


    void on_AcceptContactButton_clicked();
    void on_DeleteContactButton_clicked();
    void on_DeclineContactButton_clicked();
    void on_NewChatButton_clicked();

private:
    std::string login;
    int status;
    GreeterClient * client;
    Ui::ContactDialog *ui;
};

#endif // CONTACTDIALOG_H
