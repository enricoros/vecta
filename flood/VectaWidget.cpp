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

#include "VectaWidget.h"
#include "VecAnimatorTwirl.h"
#include "VecAnimatorGravity.h"

#include <QPainter>
#include <QTimer>
#include <QEvent>
#include <QResizeEvent>

VectaWidget::VectaWidget( QWidget * vampiredWidget )
    : QWidget( vampiredWidget )
    , m_vampiredWidget( vampiredWidget )
    , m_wireframe( false )
    , m_paintStyle( ObjectColor )
    , m_currentTransition( 0 )
    , m_timer( 0 )
{
    // hook on vampiredWidget's resize events
    if ( m_vampiredWidget ) {
        m_vampiredWidget->installEventFilter( this );
        resize( m_vampiredWidget->size() );
        show();
    }

    // ignore mouse events, only use this widget for 'overlay-VectaWidget'
    setAttribute( Qt::WA_TransparentForMouseEvents );

    // start the refresh timer
    m_timer = new QTimer( this );
    connect( m_timer, SIGNAL(timeout()), this, SLOT(slotAnimate()) );
}

VectaWidget::~VectaWidget()
{
    clear();
}

void VectaWidget::setPaintingWireframe( bool enabled )
{
    m_wireframe = enabled;
}

void VectaWidget::setPaintingStyle( PaintingStyle style )
{
    m_paintStyle = style;
}

void VectaWidget::setPaintingColor( const QColor & color )
{
    m_color = color;
}

void VectaWidget::appendTransition( VectaTransition * transition )
{
    // enqueue it
    m_nextTransitions.append( transition );

    // wire it up
    connect( transition, SIGNAL(started()), this, SLOT(slotTransitionStarted()) );
    connect( transition, SIGNAL(updated()), this, SLOT(slotTransitionUpdated()) );
    connect( transition, SIGNAL(ended()), this, SLOT(slotTransitionEnded()) );
}

void VectaWidget::startTransitions()
{
    // select the next transition if we have no current
    if ( !m_currentTransition ) {
        if ( m_nextTransitions.isEmpty() )
            return;
        m_currentTransition = m_nextTransitions.takeFirst();
    }

    // start the transition if not already started
    if ( !m_currentTransition->running() )
        m_currentTransition->startAnimation();

    // start the update timer if stopped
    if ( !m_timer->isActive() ) {
        m_timer->start( 20 );
        m_timeMeasure.start();
    }
}

void VectaWidget::clear()
{
    // dispose transitions
    delete m_currentTransition;
    m_currentTransition = 0;
    qDeleteAll( m_nextTransitions );
    m_nextTransitions.clear();

    // stop timer and refresh screen
    m_timer->stop();
    update();
}

bool VectaWidget::eventFilter( QObject * obj, QEvent * event )
{
    // resize to cover the whole vampiredWidget area
    if ( obj == m_vampiredWidget && event->type() == QEvent::Resize ) {
        resize( static_cast< QResizeEvent * >( event )->size() );
        // TODO perform other size-related changes
    }
    // normal event processing
    return QObject::eventFilter( obj, event );
}

void VectaWidget::paintEvent( QPaintEvent * /*event*/ )
{
    if ( !m_currentTransition )
        return;

    QPainter p( this );
    p.setRenderHint( QPainter::Antialiasing, true );
    double alpha = m_currentTransition->alpha();
    if ( alpha < 0.99 )
        p.setOpacity( alpha );

    // create common gradient
    QLinearGradient lg( 0, 0, 1, 1 );
    if ( m_paintStyle == ScreenMonochrome ) {
        lg.setFinalStop( width() / 8, height() );
        lg.setColorAt( 0.0, m_color.darker( 160 ) );
        lg.setColorAt( 0.55, m_color.lighter( 160 ) );
        lg.setColorAt( 1.0, m_color.darker( 160 ) );
    } else {
        lg.setCoordinateMode( QGradient::ObjectBoundingMode );
        lg.setColorAt( 0.0, Qt::white );
        lg.setColorAt( 1.0, Qt::black );
    }

    // draw each poly
    int i = 0;
    const VectaPolys & polys = m_currentTransition->polys();
    foreach ( const VectaPoly & poly, polys ) {

        // adapt to painting style
        switch ( m_paintStyle ) {
            case Color:
                break;
            case ObjectColor:
                lg.setColorAt( 0.5, QColor::fromHsv( (i * 30) % 360, 255, 255 ) );
                break;
            case ObjectMonochrome:
                lg.setColorAt( 0.5, QColor::fromHsv( (i * 30) % 360, 0, (i * 1000) % 255 ) );
                break;
            case ScreenMonochrome:
                break;
        }
        i++;

        // draw wireframe or filled
        if ( m_wireframe ) {
            if ( m_paintStyle == Color )
                p.strokePath( poly.toPainterPath(), QPen( m_color, 2 ) );
            else
                p.strokePath( poly.toPainterPath(), QPen( lg, 2 ) );
        } else {
            if ( m_paintStyle == Color )
                p.fillPath( poly.toPainterPath(), m_color );
            else
                p.fillPath( poly.toPainterPath(), lg );
        }
    }
}

void VectaWidget::slotAnimate()
{
    if ( !m_currentTransition )
        return;

    // find out the timegap since the last call
    double dT = (double)m_timeMeasure.restart() / 1000.0;
    if ( dT > 0.2 )
        dT = 0.2;

    // animate current Transition
    m_currentTransition->updateAnimation( dT );
}

void VectaWidget::slotTransitionStarted()
{
    // nothing to do here..
}

void VectaWidget::slotTransitionUpdated()
{
    update();
}

void VectaWidget::slotTransitionEnded()
{
    // find out the Transition
    VectaTransition * transition = static_cast< VectaTransition * >( sender() );
    transition->deleteLater();

    // sanity check
    if ( transition != m_currentTransition ) {
        qWarning( "VectaWidget::slotTransitionEnded: some transition ended, but not the current one!" );
        return;
    }

    // END if no more transitions...
    if ( m_nextTransitions.isEmpty() ) {
        m_currentTransition = 0;
        m_timer->stop();
        emit transitionsEnded();
        return;
    }

    // else start the next one
    m_currentTransition = m_nextTransitions.takeFirst();
    m_currentTransition->startAnimation();
}
