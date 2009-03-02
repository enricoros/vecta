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

#ifndef __VectaTransition_h__
#define __VectaTransition_h__

#include <QObject>
#include "VectaPoly.h"

class VectaTransition : public QObject {
    Q_OBJECT
    public:
        // public types
        enum TransitionShape { Zero, One, Linear, QuadIn, QuadOut, QuadInOut };

        // ctor
        VectaTransition( const VectaPolys & startConfig, const VectaPolys & endConfig, double duration = 1.0, TransitionShape shape = Linear, bool growing = true );
        ~VectaTransition();

        // animation setup
        void setDuration( double duration );
        void setTransition( TransitionShape shape, bool growing );
        void setAlphaTransition( TransitionShape shape, bool growing );

        // animation control
        void startAnimation();
        void stopAnimation();
        bool running() const;

        // called by VectaWidget
        void updateAnimation( double dT );
        const VectaPolys & polys() const;
        double alpha() const;

    Q_SIGNALS:
        void started();
        void updated();
        void ended();

    private:
        void updatePolys( double phase );
        void updateAlpha( double phase );
        double factor( double phase, TransitionShape tShape, bool growing );

        // startup config
        VectaPolys m_startPolys;
        VectaPolys m_stopPolys;
        VectaPolys m_currentPolys;
        double m_duration;
        TransitionShape m_polyTransition;
        bool m_polyGrowing;
        TransitionShape m_alphaTransition;
        bool m_alphaGrowing;

        // animation stuff
        bool m_running;
        double m_time;
        double m_alpha;
};

#endif
