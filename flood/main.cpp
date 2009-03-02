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

#include <QApplication>
#include <QWidget>
#include "FloodGenerator.h"
#include "FloodTransition.h"
#include "FloodWidget.h"

int main( int argc, char ** args )
{
    QApplication app( argc, args );

    FloodPolys svgPolys = FloodGenerator::fromSvgPaths( "main-example.svg" );
    FloodPolys apartPolys = FloodGenerator::spreadPolys( svgPolys, QRect( 0, 0, 640, 480 ) );
    FloodPolys fallenPolys = FloodGenerator::heavyPolys( svgPolys, 100, 1.0 );
    FloodPolys s1p = FloodGenerator::starPolys( svgPolys, 0.2 );
    FloodPolys s2p = FloodGenerator::starPolys( s1p, 1.2 );
    FloodPolys s3p = FloodGenerator::starPolys( s2p, 1.2 );
    FloodPolys s4p = FloodGenerator::starPolys( svgPolys, 0.1 );

    FloodWidget vecta( 0 );
    vecta.resize( 1024, 768 );
    vecta.show();

    FloodTransition * t1 = new FloodTransition( fallenPolys, svgPolys );
    t1->setTransition( FloodTransition::QuadInOut, true );
    t1->setDuration( 2.0 );

    FloodTransition * t2 = new FloodTransition( svgPolys, apartPolys );
    t2->setTransition( FloodTransition::QuadIn, true );
    t2->setDuration( 2.0 );

    vecta.appendTransition( t1 );
    vecta.appendTransition( t2 );
    vecta.appendTransition( new FloodTransition( apartPolys, svgPolys ) );
    vecta.appendTransition( new FloodTransition( svgPolys, s1p ) );
    vecta.appendTransition( new FloodTransition( s1p, s2p ) );
    vecta.appendTransition( new FloodTransition( s2p, s3p ) );
    vecta.appendTransition( new FloodTransition( s3p, s4p ) );
    vecta.appendTransition( new FloodTransition( s4p, svgPolys ) );
    vecta.startTransitions();

    return app.exec();
}
