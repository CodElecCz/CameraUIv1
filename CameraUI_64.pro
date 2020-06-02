#-------------------------------------------------
#
# Project created by QtCreator 2018-02-03T14:53:21
#
#-------------------------------------------------

QT       += core gui sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Camera
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += OPENCV
DEFINES += _TIMESPEC_DEFINED
DEFINES += _TIMEVAL_DEFINED

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:CONFIG(release, debug|release): LIBS += \
    -L$$PWD/../../Lib/Build64/release/ \
    -L"C:/Program Files/Basler/pylon 5/Development/lib/x64" \
    -lfvedio \
    -lclv
else:win32:CONFIG(debug, debug|release): LIBS += \
    -L$$PWD/../../Lib/Build64/debug/ \
    -L"C:/Program Files/Basler/pylon 5/Development/lib/x64" \
    -lfvedio \
    -lclv

INCLUDEPATH += $$PWD/../../Lib/
INCLUDEPATH += "C:/Program Files/Basler/pylon 5/Development/include"

DEPENDPATH += $$PWD/../../Lib/
DEPENDPATH += $$PWD/../../Lib/Build64/
DEPENDPATH += "C:/Program Files/Basler/pylon 5/Runtime/x64"

SOURCES += \
    main.cpp \
    MainWindowDevices.cpp \
    MainWindowTrace.cpp \
    MainWindowParam.cpp \
    MainWindowDio.cpp \
    MainWindow.cpp \
    DioView.cpp \
    LoadView.cpp \
    CameraViewSingle.cpp \
    CameraView.cpp \
    CameraParam.cpp \
    ReportView.cpp \
    MainWindowReport.cpp \
    StorageView.cpp \
    MainWindowStorage.cpp \
    BarcodeView.cpp \
    MainWindowBarcode.cpp

HEADERS += \
    DioView.h \
    LoadView.h \
    CameraViewSingle.h \
    CameraView.h \
    CameraParam.h \
    MainWindow.h \
    CameraGrabThread.h \
    LightWidget.h \
    ReportView.h \
    StorageView.h \
    BarcodeView.h

FORMS += \
    AboutDialog.ui \
    CameraParam.ui \
    CameraView.ui \
    CameraViewSingle.ui \
    DioView.ui \
    LoadView.ui \
    MainWindow.ui \
    ReportView.ui \
    StorageView.ui \
    BarcodeView.ui

RESOURCES += \
    Resources.qrc

RC_FILE += \
    Resource.rc
