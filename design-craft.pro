QT += core gui widgets openglwidgets opengl sql

CONFIG += c++17

# Uncomment to disable deprecated APIs
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    src/controllers/main_controller/maincontroller.cpp \
    src/database/databasemanager.cpp \
    src/database/repository/furnite_item_repository/furnitureitemrepository.cpp \
    src/database/repository/furniture_category_repository/furniturecategoryrepository.cpp \
    src/main.cpp \
    src/widgets/create_furniture_widget/createfurnitureopenglwidget.cpp \
    src/widgets/create_furniture_widget/createfurniturewidget.cpp \
    src/widgets/openglwidget.cpp \
    src/widgets/wall_controller_widget/wallcontrollerwidget.cpp \
    src/windows/mainwindow.cpp \
    src/windows/startup_window/startupwindow.cpp

HEADERS += \
    src/controllers/main_controller/maincontroller.h \
    src/data_structures/Furniture.h \
    src/data_structures/enums/WallsEnum.h \
    src/database/databasemanager.h \
    src/database/repository/furnite_item_repository/furnitureitemrepository.h \
    src/database/repository/furniture_category_repository/furniturecategoryrepository.h \
    src/widgets/create_furniture_widget/createfurnitureopenglwidget.h \
    src/widgets/create_furniture_widget/createfurniturewidget.h \
    src/widgets/openglwidget.h \
    src/widgets/wall_controller_widget/wallcontrollerwidget.h \
    src/windows/mainwindow.h \
    src/windows/startup_window/startupwindow.h

FORMS += \
    src/windows/mainwindow.ui

LIBS += -lopengl32

# Deployment settings
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = resources/images/design_craft.ico

RESOURCES += \
    resources/app.qrc
