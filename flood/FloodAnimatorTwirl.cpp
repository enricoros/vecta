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

#include "FloodAnimatorTwirl.h"
#include "enricomath.h"

FloodAnimatorTwirl::FloodAnimatorTwirl( double cx, double cy, double rads )
    : m_center( cx, cy )
    , m_rads( rads )
{
}

void FloodAnimatorTwirl::step( FloodPoly & FloodPoly )
{
    QList<FloodPoly::Node> & nodes = FloodPoly.edit();
    QList<FloodPoly::Node>::iterator it = nodes.begin(), end = nodes.end();
    for ( ; it != end; ++it ) {
        Vector2 rad = it->point - m_center;
        // rotate the rad vector
        double mod = rad.module();
        if ( mod > 0.0 ) {
            Control2 c( mod * 0.999, rad.angle() );
            double thDiff = m_rads / mod;
            c.addTheta( thDiff );
            rad = c.toVector2();
            it->point = rad + m_center;
            it->control.addTheta( thDiff );
        }
    }
}

void FloodAnimatorTwirl::setPos( double x, double y )
{
    m_center = Vector2( x, y );
}

void FloodAnimatorTwirl::setSpeed( double rads )
{
    m_rads = rads;
}

