#-------------------------------------------------
#
# Project created by QtCreator 2012-10-30T13:27:01
#
#-------------------------------------------------

QT       += core gui

TARGET = TradeGame
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    testagent.cpp \
    blackboard.cpp \
    tradingfloor.cpp \
    bidlist.cpp \
    tf/tfagent.cpp \
    gj/intercom.cpp \
    gj/genalgocomm.cpp \
    gj/bot123.cpp \
    to/tradeAgent.cpp

HEADERS  += mainwindow.h \
    types.h \
    testagent.h \
    agent.h \
    blackboard.h \
    tradingfloor.h \
    bidlist.h \
    tf/tfagent.h \
    gj/intercom.h \
    gj/genalgocomm.h \
    gj/bot123.h \
    to/tradeAgent.h

FORMS    += mainwindow.ui

#needed for newest cpp
CCFLAG +=-std=gnu++0x
QMAKE_CXXFLAGS +=-std=gnu++0x
