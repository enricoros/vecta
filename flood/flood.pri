VPATH += $$PWD
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += svg \
    xml
HEADERS += FloodAnimator.h \
    FloodAnimatorAttractor.h \
    FloodAnimatorGravity.h \
    FloodAnimatorText.h \
    FloodAnimatorTwirl.h \
    FloodAnimatorWind.h \
    FloodGenerator.h \
    FloodPoly.h \
    FloodTransition.h \
    FloodWidget.h
SOURCES += FloodAnimatorTwirl.cpp \
    FloodAnimatorGravity.cpp \
    FloodGenerator.cpp \
    FloodPoly.cpp \
    FloodTransition.cpp \
    FloodWidget.cpp
