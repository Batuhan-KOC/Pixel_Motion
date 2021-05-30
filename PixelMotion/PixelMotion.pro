QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32:RC_ICONS += pixelmotion.ico


CONFIG += c++17
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:\Libraries\opencvBuild64\install
DEPENDPATH += C:\Libraries\opencvBuild64\install
INCLUDEPATH += C:\Libraries\opencvBuild64\install\include
DEPENDPATH += C:\Libraries\opencvBuild64\install\include


LIBS += C:\Libraries\opencvBuild64\install\x64\mingw\bin\libopencv_core451.dll
LIBS += C:\Libraries\opencvBuild64\install\x64\mingw\bin\libopencv_highgui451.dll
LIBS += C:\Libraries\opencvBuild64\install\x64\mingw\bin\libopencv_imgcodecs451.dll
LIBS += C:\Libraries\opencvBuild64\install\x64\mingw\bin\libopencv_imgproc451.dll
LIBS += C:\Libraries\opencvBuild64\install\x64\mingw\bin\libopencv_calib3d451.dll
LIBS += C:\Libraries\opencvBuild64\install\x64\mingw\bin\libopencv_video451.dll
LIBS += C:\Libraries\opencvBuild64\install\x64\mingw\bin\libopencv_videoio451.dll

SOURCES += \
    Quantizers/mediancutquantizer.cpp \
    Quantizers/octreequantizer.cpp \
    gaussianblur.cpp \
    imageprocessor.cpp \
    main.cpp \
    mainwindow.cpp \
    tunemanager.cpp \
    videoprocessor.cpp

HEADERS += \
    Quantizers.h \
    Quantizers/mediancutquantizer.h \
    Quantizers/octreequantizer.h \
    gaussianblur.h \
    imageprocessor.h \
    mainwindow.h \
    tunemanager.h \
    videoprocessor.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc



