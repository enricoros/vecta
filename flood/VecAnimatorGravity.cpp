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

#include "VecAnimatorGravity.h"

VecAnimatorGravity::VecAnimatorGravity( double g )
    : m_gConst( g )
{
}

void VecAnimatorGravity::step( int id, VectaPoly & VectaPoly, double dT )
{
    if ( !m_states.contains( id ) )
        m_states[ id ] = QPair< double, double >( 0, m_gConst * (1 + (double)qrand() / RAND_MAX) );

    // phy update
    double acc = m_states[ id ].second;
    double speed = m_states[ id ].first + acc * dT;
    m_states[ id ].first = speed;

    QList<VectaPoly::Node> & nodes = VectaPoly.edit();
    QList<VectaPoly::Node>::iterator it = nodes.begin(), end = nodes.end();
    for ( ; it != end; ++it )
        it->point += Vector2( 0, speed * dT );
}

