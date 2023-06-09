TEMPLATE = app
TARGET = TupiTube
QT += widgets xml network svg

# Enables debugging option
CONFIG += console
DEFINES += TUP_DEBUG

HEADERS += tupmainwindow.h \
           tupglobal.h \
           tupcanvas.h \
           tupgraphicsscene.h \
           tupserializer.h \
           tupnethandler.h \
           tuppathitem.h \
           tuprectitem.h \
           tupellipseitem.h \
           tupsvg2qt.h \
           tupgraphicalgorithm.h \
           tuppenpreviewcanvas.h \
           tupthicknessdialog.h \
           tupopacitypanel.h \
           tupopacitydialog.h \
           tupframe.h \
           tuphelpdialog.h \
           tupslider.h \
           tupfactoryhandler.h \
           tupitemfactory.h \
           tupbrushespanel.h \
           tupcolorhistory.h \
           tupcolorpreviewcanvas.h \
           tupcolorpicker.h \
           tupcurrentcolor.h \
           tupcolorbuttonpanel.h \
           tupsettings.h \
           tupbutton.h \
           tupbrushdialog.h \
           tupcolordialog.h \
           tupcolortab.h \
           tupcheckbox.h \
           tuplabel.h \
           tuptoolsflatmenu.h \
           tuptoolstopmenu.h \
           tupfileflatmenu.h \
           tupfiletopmenu.h \
           tupsplash.h \
           tuppostdialog.h \
           tupguiparams.h \
           tupcolorbutton.h \
           tupproject.h \
           tupgallerydialog.h \
           tupspace.h \
           tupdownloader.h \
           tupdownloadmanager.h \
           tuppermissions.h \
           tupframesmenu.h \
           tuptabletfilemenu.h \
           tupexitdialog.h \
           tuppenpropertiesmenu.h \
           tupdrawtoolsmenu.h

SOURCES += tupmainwindow.cpp \
           tupcanvas.cpp \
           tupgraphicsscene.cpp \
           tupserializer.cpp \
           tupnethandler.cpp \
           tuppathitem.cpp \
           tuprectitem.cpp \
           tupellipseitem.cpp \
           tupsvg2qt.cpp \
           tupgraphicalgorithm.cpp \
           tuppenpreviewcanvas.cpp \
           tupthicknessdialog.cpp \
           tupopacitypanel.cpp \
           tupopacitydialog.cpp \
           tupframe.cpp \
           tupslider.cpp \
           tupfactoryhandler.cpp \
           tupitemfactory.cpp \
           tupbrushespanel.cpp \
           tupcolorhistory.cpp \
           tupcolorpreviewcanvas.cpp \
           tupcolorpicker.cpp \
           tupcurrentcolor.cpp \
           tupcolorbuttonpanel.cpp \
           tupsettings.cpp \
           tupbutton.cpp \
           tupbrushdialog.cpp \
           tupcolordialog.cpp \
           tupcolortab.cpp \
           tupcheckbox.cpp \
           tuplabel.cpp \
           tuptoolsflatmenu.cpp \
           tuptoolstopmenu.cpp \
           tupfileflatmenu.cpp \
           tupfiletopmenu.cpp \
           tupsplash.cpp \
           tuppostdialog.cpp \
           tupguiparams.cpp \
           tupcolorbutton.cpp \
           tupproject.cpp \
           tupgallerydialog.cpp \
           tuphelpdialog.cpp \
           tupspace.cpp \
           tupdownloader.cpp \
           tupdownloadmanager.cpp \
           tuppermissions.cpp \
           tupframesmenu.cpp \
           tuptabletfilemenu.cpp \
           tupexitdialog.cpp \
           tuppenpropertiesmenu.cpp \
           tupdrawtoolsmenu.cpp \
           main.cpp

win32 {
    include($$PWD/quazip.win.pri)
    RESOURCES += resources_pc.qrc

    HEADERS += tuppackagehandler.h
    SOURCES += tuppackagehandler.cpp
}

android {
    QT += quick core-private
    RESOURCES += resources_android.qrc

    DISTFILES += \
       android/AndroidManifest.xml \
       android/gradle/wrapper/gradle-wrapper.jar \
       android/gradlew \
       android/res/values/libs.xml \
       android/build.gradle \
       android/gradle/wrapper/gradle-wrapper.properties \
       android/gradlew.bat

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../android
    OTHER_FILES += $$PWD/../android/src/com/maefloresta/QShareUtils.java \
                   $$PWD/../android/src/com/maefloresta/QShowPermissionRationale.java

    HEADERS += tuplogdialog.h
    SOURCES += tuplogdialog.cpp

    include($$PWD/openssl.pri)
}

linux:!android {
    RESOURCES += resources_pc.qrc

    INCLUDEPATH += /usr/local/quazip6/include
    LIBS += -L/usr/local/quazip6/lib -lz -lquazip1-qt6

    HEADERS += tuppackagehandler.h
    SOURCES += tuppackagehandler.cpp
}
