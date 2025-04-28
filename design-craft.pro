QT += core gui widgets openglwidgets opengl sql

CONFIG += c++17

# Uncomment to disable deprecated APIs
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    src/controllers/main_controller/maincontroller.cpp \
    src/core/cutting_optimizer/cuttingoptimizer.cpp \
    src/core/furniture_generator/ClosetGenerator.cpp \
    src/data_structures/door/door.cpp \
    src/data_structures/models/furniture_model/furnituremodel.cpp \
    src/database/databasemanager.cpp \
    src/database/repository/furnite_item_repository/furnitureitemrepository.cpp \
    src/database/repository/furniture_category_repository/furniturecategoryrepository.cpp \
    src/main.cpp \
    src/widgets/create_furniture_widget/createfurnitureopenglwidget.cpp \
    src/widgets/create_furniture_widget/createfurniturewidget.cpp \
    src/widgets/cutting_preview_widget/cuttingpreviewwidget.cpp \
    src/widgets/material_optimizer_widget/materialoptimizerwidget.cpp \
    src/widgets/room_viewer_widget/roomviewer.cpp \
    src/windows/mainwindow.cpp \
    src/windows/startup_window/startupwindow.cpp

HEADERS += \
    src/controllers/main_controller/maincontroller.h \
    src/core/cutting_optimizer/cuttingoptimizer.h \
    src/core/furniture_generator/ClosetGenerator.h \
    src/data_structures/Furniture.h \
    src/data_structures/door/door.h \
    src/data_structures/enums/FurniturePartsEnum.h \
    src/data_structures/enums/WallsEnum.h \
    src/data_structures/models/furniture_model/furnituremodel.h \
    src/data_structures/structs/FurnitureMaterial.h \
    src/data_structures/structs/furniture_part/furniturepart.h \
    src/data_structures/structs/furniture_spec/furniturespec.h \
    src/data_structures/structs/part_types/part_types.h \
    src/database/databasemanager.h \
    src/database/interfaces/furniture_category/furniture_category.h \
    src/database/interfaces/furniture_item/furniture_item.h \
    src/database/repository/furnite_item_repository/furnitureitemrepository.h \
    src/database/repository/furniture_category_repository/furniturecategoryrepository.h \
    src/utils/laminate_utils.h \
    src/widgets/create_furniture_widget/createfurnitureopenglwidget.h \
    src/widgets/create_furniture_widget/createfurniturewidget.h \
    src/widgets/cutting_preview_widget/cuttingpreviewwidget.h \
    src/widgets/material_optimizer_widget/materialoptimizerwidget.h \
    src/widgets/room_viewer_widget/roomviewer.h \
    src/windows/mainwindow.h \
    src/windows/startup_window/startupwindow.h

FORMS += \
    src/windows/mainwindow.ui

INCLUDEPATH += C:\\Users\\User\\Documents\\vcpkg\\installed\\x64-windows\\include

LIBS += -lopengl32
# Deployment settings
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = resources/images/design_craft.ico

RESOURCES += \
    resources/app.qrc
