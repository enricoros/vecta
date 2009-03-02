VPATH += $$PWD
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += svg \
    xml
HEADERS += VecAnimator.h \
    VecAnimatorAttractor.h \
    VecAnimatorGravity.h \
    VecAnimatorText.h \
    VecAnimatorTwirl.h \
    VecAnimatorWind.h \
    VectaGenerator.h \
    VectaPoly.h \
    VectaTransition.h \
    VectaWidget.h
SOURCES += VecAnimatorTwirl.cpp \
    VecAnimatorGravity.cpp \
    VectaGenerator.cpp \
    VectaPoly.cpp \
    VectaTransition.cpp \
    VectaWidget.cpp
