#ifndef NEWCONVERSATIONDIALOG_H
#define NEWCONVERSATIONDIALOG_H

#include <QDialog>

#include "grpc_client.hpp"

namespace Ui {
class NewConversationDialog;
}

class NewConversationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewConversationDialog(GreeterClient *client, QWidget *parent = nullptr);
    ~NewConversationDialog();

private:
    Ui::NewConversationDialog *ui;
    GreeterClient *client;
};

#endif // NEWCONVERSATIONDIALOG_H
