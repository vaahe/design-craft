#include "src/database/databasemanager.h"
#include "src/controllers/main_controller/maincontroller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DatabaseManager &dbManager = DatabaseManager::getInstance();
    bool isDBConnected = dbManager.connect("localhost", 5432, "design-craft", "postgres", "1234");
    if (!isDBConnected) {
        return -1;
    }

    MainController controller;
    controller.setWindowState(Qt::WindowMaximized);
    controller.show();

    return app.exec();
}
