#include "registrationwindow.h"
#include "mainwindow.h"

#include "grpc_client.hpp"

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{

    std::string server_address("0.0.0.0:50051");
    GreeterClient greeter(grpc::CreateChannel(
        server_address, grpc::InsecureChannelCredentials()));

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ChatProject_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }


    RegistrationWindow window(&greeter);
    window.show();
    QObject::connect(qApp, &QCoreApplication::destroyed, [&window, &greeter]() {
        qDebug("11111");
        // сохранение бд
    });
    return a.exec();
}
