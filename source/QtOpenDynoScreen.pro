#-------------------------------------------------
#
# Project created by QtCreator 2014-01-03T13:20:43
#
#-------------------------------------------------

QT       += core gui xml xmlpatterns svg scripttools script
QT += designer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtOpenDynoScreen
TEMPLATE = app

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
SOURCES += main.cpp\
        mainwindow.cpp \
    widgetselection.cpp \
    invisible_widget.cpp \
    QScale/qscale.cpp \
    LedIndicator/qledindicator.cpp \
    scriptfunctions.cpp \
    PropertyBrowser/qtvariantproperty.cpp \
    PropertyBrowser/qttreepropertybrowser.cpp \
    PropertyBrowser/qtpropertymanager.cpp \
    PropertyBrowser/qtpropertybrowserutils.cpp \
    PropertyBrowser/qtpropertybrowser.cpp \
    PropertyBrowser/qtgroupboxpropertybrowser.cpp \
    PropertyBrowser/qteditorfactory.cpp \
    PropertyBrowser/qtbuttonpropertybrowser.cpp \
    AnalogWidgets/widgetwithbackground.cpp \
    AnalogWidgets/wallclock.cpp \
    AnalogWidgets/thermometer.cpp \
    AnalogWidgets/potentiometer.cpp \
    AnalogWidgets/manometer.cpp \
    AnalogWidgets/led.cpp \
    AnalogWidgets/functions.cpp \
    AnalogWidgets/counter.cpp \
    AnalogWidgets/bitmapbutton.cpp \
    AnalogWidgets/abstractmeter.cpp

HEADERS  += mainwindow.h \
    widgetselection.h \
    invisible_widget_p.h \
    QScale/qscale.h \
    LedIndicator/qledindicator.h \
    scriptfunctions.h \
    PropertyBrowser/qtvariantproperty.h \
    PropertyBrowser/qttreepropertybrowser.h \
    PropertyBrowser/qtpropertymanager.h \
    PropertyBrowser/qtpropertybrowserutils_p.h \
    PropertyBrowser/qtpropertybrowser.h \
    PropertyBrowser/qtgroupboxpropertybrowser.h \
    PropertyBrowser/qteditorfactory.h \
    PropertyBrowser/qtbuttonpropertybrowser.h \
    AnalogWidgets/widgetwithbackground.h \
    AnalogWidgets/wallclock.h \
    AnalogWidgets/thermometer.h \
    AnalogWidgets/potentiometer.h \
    AnalogWidgets/manometer.h \
    AnalogWidgets/led.h \
    AnalogWidgets/functions.h \
    AnalogWidgets/defines.h \
    AnalogWidgets/counter.h \
    AnalogWidgets/bitmapbutton.h \
    AnalogWidgets/abstractmeter.h \
    typeconverters.h \
    controlFactory.h

FORMS    += mainwindow.ui

RESOURCES += \
    Images.qrc \
    PropertyBrowser/qtpropertybrowser.qrc \
    AnalogWidgets/analogwidgets.qrc

LIBS += -L$$[QT_INSTALL_LIBS] -lqt5scintilla2
