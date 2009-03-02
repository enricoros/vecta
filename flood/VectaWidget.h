/***************************************************************************
 * This file is part of the Vecta project                                  *
 *                                                                         *
 * Copyright (c) 2009 Enrico Ros                                           *
 *         2007-2009 Enrico Ros <enrico.ros@gmail.com>                     *
 *                                                                         *
 * Permission is hereby granted, free of charge, to any person             *
 * obtaining a copy of this software and associated documentation          *
 * files (the "Software"), to deal in the Software without                 *
 * restriction, including without limitation the rights to use,            *
 * copy, modify, merge, publish, distribute, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the               *
 * Software is furnished to do so, subject to the following                *
 * conditions:                                                             *
 *                                                                         *
 * The above copyright notice and this permission notice shall be          *
 * included in all copies or substantial portions of the Software.         *
 *                                                                         *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,         *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES         *
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                *
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT             *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,            *
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING            *
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR           *
 * OTHER DEALINGS IN THE SOFTWARE.                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef __VectaWidget_h__
#define __VectaWidget_h__

#include <QWidget>
#include <QColor>
#include <QTime>
#include "VectaPoly.h"
#include "VectaTransition.h"
class QTimer;

class VectaWidget : public QWidget {
    Q_OBJECT
    public:
        VectaWidget( QWidget * vampiredWidget );
        ~VectaWidget();

        // TODO: add anchoring capabilities and fixed-width/height/size stuff

        enum PaintingStyle { Color, ObjectColor, ObjectMonochrome, ScreenMonochrome };
        void setPaintingWireframe( bool enabled );
        void setPaintingStyle( PaintingStyle style );
        void setPaintingColor( const QColor & color );

        void appendTransition( VectaTransition * transition );
        void startTransitions();
        void clear();

    Q_SIGNALS:
        void transitionsEnded();

    protected:
        // ::QObject
        bool eventFilter( QObject * obj, QEvent * event );
        // ::Qwidget
        void paintEvent( QPaintEvent * event );

    private:
        // setup stuff
        QWidget * m_vampiredWidget;
        bool m_wireframe;
        PaintingStyle m_paintStyle;
        QColor m_color;

        // TEMP stuff
        VectaPolys m_initialLines;

        // animation
        VectaTransition * m_currentTransition;
        QList< VectaTransition * > m_nextTransitions;
        QTimer * m_timer;
        QTime m_timeMeasure;

    private Q_SLOTS:
        void slotAnimate();
        void slotTransitionStarted();
        void slotTransitionUpdated();
        void slotTransitionEnded();
};

#endif

