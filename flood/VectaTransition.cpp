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

#include "VectaTransition.h"

VectaTransition::VectaTransition( const VectaPolys & startConfig, const VectaPolys & endConfig, double duration, TransitionShape shape, bool growing )
    : QObject()
    , m_startPolys( startConfig )
    , m_stopPolys( endConfig )
    , m_duration( duration )
    , m_polyTransition( shape )
    , m_polyGrowing( growing )
    , m_alphaTransition( One )
    , m_alphaGrowing( true )
    , m_running( false )
    , m_time( 0.0 )
    , m_alpha( 1.0 )
{
}

VectaTransition::~VectaTransition()
{
}

void VectaTransition::setDuration( double duration )
{
    m_duration = duration;
}

void VectaTransition::setTransition( TransitionShape shape, bool growing )
{
    m_polyTransition = shape;
    m_polyGrowing = growing;
}

void VectaTransition::setAlphaTransition( TransitionShape shape, bool growing )
{
    m_alphaTransition = shape;
    m_alphaGrowing = growing;
}

void VectaTransition::startAnimation()
{
    // sanity check
    if ( m_running ) {
        qWarning( "VectaTransition::startAnimation: issuing startAnimation twice" );
        return;
    }

    // start this up
    m_running = true;
    updatePolys( 0.0 );
    updateAlpha( 0.0 );
    emit started();
    emit updated();
}

void VectaTransition::stopAnimation()
{
    // sanity check
    if ( !m_running )
        return;

    // start this up
    m_running = false;
    updatePolys( 1.0 );
    updateAlpha( 1.0 );
    emit updated();
    emit ended();
}

bool VectaTransition::running() const
{
    return m_running;
}

void VectaTransition::updateAnimation( double dT )
{
    if ( !m_running )
        return;

    // update timing
    m_time += dT;
    if ( m_time > m_duration ) {
        stopAnimation();
        return;
    }

    // calc fading coeffs
    double phase = m_time / m_duration;
    if ( phase > 1.0 ) {
        stopAnimation();
        return;
    }

    // update stuff
    updatePolys( phase );
    updateAlpha( phase );
    emit updated();
}

const VectaPolys & VectaTransition::polys() const
{
    return m_currentPolys;
}

double VectaTransition::alpha() const
{
    return m_alpha;
}

void VectaTransition::updatePolys( double phase )
{
    // compute fade factor
    double fadeFactor = factor( phase, m_polyTransition, m_polyGrowing );

    // fast paths
    if ( fadeFactor <= 0.0 )
        m_currentPolys = m_startPolys;
    else if ( fadeFactor >= 1.0 )
        m_currentPolys = m_stopPolys;

    // perform a smooth transition between start and stop
    m_currentPolys.clear();
    VectaPolys::iterator itB = m_startPolys.begin(), endB = m_startPolys.end();
    VectaPolys::iterator itE = m_stopPolys.begin(), endE = m_stopPolys.end();
    while ( itB != endB && itE != endE ) {
        m_currentPolys.append( itB->fadedTo( *itE, fadeFactor ) );
        ++itB;
        ++itE;
    }
}

void VectaTransition::updateAlpha( double phase )
{
    m_alpha = factor( phase, m_alphaTransition, m_alphaGrowing );
}

double VectaTransition::factor( double phase, TransitionShape tShape, bool growing )
{
    double value = phase;
    switch ( tShape ) {

        // 0
        case VectaTransition::Zero:
            return 0.0;

        // 1
        case VectaTransition::One:
            return 1.0;

        // value already ok
        case VectaTransition::Linear:
            break;

        // x^2
        case VectaTransition::QuadIn:
            value *= value;
            break;

        // 1 - (1-x)^2
        case VectaTransition::QuadOut:
            value = 1.0 - (1.0 - value) * (1.0 - value);
            break;

        // mix of the 2 above
        case VectaTransition::QuadInOut:
            if ( value < 0.5 ) {
                value *= value * 2;
            } else {
                value = (value - 0.5) * 2;
                value = 1 - (1.0 - value) * (1.0 - value) / 2.0;
            }
            break;
    }

    // invert if not growing
    if ( !growing )
        return 1.0 - value;

    // normale case: return the computed value
    return value;
}
