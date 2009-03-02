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
#include "VectaGenerator.h"
#include "VectaTransition.h"
#include "VectaWidget.h"

int main( int argc, char ** args )
{
    QApplication app( argc, args );

    VectaPolys svgPolys = VectaGenerator::fromSvgPaths( "main-example.svg" );
    VectaPolys apartPolys = VectaGenerator::spreadPolys( svgPolys, QRect( 0, 0, 640, 480 ) );
    VectaPolys fallenPolys = VectaGenerator::heavyPolys( svgPolys, 100, 1.0 );
    VectaPolys s1p = VectaGenerator::starPolys( svgPolys, 0.2 );
    VectaPolys s2p = VectaGenerator::starPolys( s1p, 1.2 );
    VectaPolys s3p = VectaGenerator::starPolys( s2p, 1.2 );
    VectaPolys s4p = VectaGenerator::starPolys( svgPolys, 0.1 );

    VectaWidget vecta( 0 );
    vecta.resize( 1024, 768 );
    vecta.show();

    VectaTransition * t1 = new VectaTransition( fallenPolys, svgPolys );
    t1->setTransition( VectaTransition::QuadInOut, true );
    t1->setDuration( 2.0 );

    VectaTransition * t2 = new VectaTransition( svgPolys, apartPolys );
    t2->setTransition( VectaTransition::QuadIn, true );
    t2->setDuration( 2.0 );

    vecta.appendTransition( t1 );
    vecta.appendTransition( t2 );
    vecta.appendTransition( new VectaTransition( apartPolys, svgPolys ) );
    vecta.appendTransition( new VectaTransition( svgPolys, s1p ) );
    vecta.appendTransition( new VectaTransition( s1p, s2p ) );
    vecta.appendTransition( new VectaTransition( s2p, s3p ) );
    vecta.appendTransition( new VectaTransition( s3p, s4p ) );
    vecta.appendTransition( new VectaTransition( s4p, svgPolys ) );
    vecta.startTransitions();

    return app.exec();
}
