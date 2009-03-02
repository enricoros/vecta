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

#include "VectaGenerator.h"
#include "enricomath.h"
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QStringList>


VectaPoly VectaGenerator::fromQPainterPath( const QPainterPath & /*painterPath*/ )
{
    VectaPoly p;
    VectaPoly::Node n;
    n.point = Vector2( 10, 10 );
    n.control = Control2( 10, M_PI );
    p.addNode( n );
    qWarning( "VectaPoly::fromQPainterPath: Not Implemented" );
    return p;
}

/// SVG PARSE
static bool parseXY( const QString & string, double * x, double * y )
{
    QStringList vals = string.split( "," );
    if ( vals.count() != 2 ) {
        qWarning( "VectaPoly: recursiveParse (svg): parseXY: wrong arg number" );
        return false;
    }
    *x = vals[ 0 ].toDouble();
    *y = vals[ 1 ].toDouble();
    return true;
}

Control2 controlFromVect( const Vector2 & vect )
{
    Control2 c;
    c.setRo( vect.module() );
    c.setTheta( vect.angle() );
    return c;
}

static VectaPolys recursiveParse( QDomElement & element, const QString & elementId )
{
    VectaPolys lines;

    QDomNode subNode = element.firstChild();
    for ( ; !subNode.isNull(); subNode = subNode.nextSibling() ) {
        // skip non-elements
        QDomElement elem = subNode.toElement();
        if ( elem.isNull() )
            continue;

        QString tagName = elem.tagName();

        // g: descend
        if ( tagName == "g" ) {
            qWarning( "VectaPolys recursiveParse: transformed 'g' elements are not supported" );
            lines << recursiveParse( elem, elementId );
        }
        // path: parse
        else if ( tagName == "path" ) {
            // if specified, select only the element with the right id
            QString id = elem.attribute( "id" );
            if ( !elementId.isEmpty() && elementId != id )
                continue;

            // parse path tokens
            QString d = elem.attribute( "d" );
            QStringList pd = d.simplified().split( ' ' );
            if ( pd.size() < 2 )
                continue;

            // build the line
            VectaPoly line;
            while ( !pd.isEmpty() ) {
                VectaPoly::Node newNode;
                // NOTE: the second C overwrites the control of the previous C'ed point
                QString cmd = pd.takeFirst();
                if ( cmd == "M" )           // move
                    parseXY( pd.takeFirst(), &newNode.point.rx(), &newNode.point.ry() );
                else if ( cmd == "L" )      // line
                    parseXY( pd.takeFirst(), &newNode.point.rx(), &newNode.point.ry() );
                else if ( cmd == "C" ) {    // cubic
                    Vector2 c1, c2;
                    parseXY( pd.takeFirst(), &c1.rx(), &c1.ry() );
                    parseXY( pd.takeFirst(), &c2.rx(), &c2.ry() );
                    parseXY( pd.takeFirst(), &newNode.point.rx(), &newNode.point.ry() );
                    VectaPoly::Node & lastNode = line.edit().last();
                    lastNode.control = controlFromVect( c1 - lastNode.point );
                    newNode.control = controlFromVect( newNode.point - c2 );
                } else if ( cmd == "z" ) {
                    // close line if not closed
                    /*VectaPoly::Node & firstNode = line.edit().first();
                    VectaPoly::Node & lastNode = line.edit().last();
                    if ( firstNode.point != lastNode.point ) {
                        newNode.point = firstNode.point;
                        newNode.control = firstNode.control;
                        newNode.control.addTheta( M_PI );
                        line.addNode( newNode );
                    }*/

                    // append the line
                    lines.append( line );
                    line = VectaPoly();
                    continue;
                // suppress A element
                } else {
                    qWarning( "VectaPoly: recursiveParse (svg): unknown path element '%s'", qPrintable( cmd ) );
                    continue;
                }
                line.addNode( newNode );
            }
        }
        // text, rect: skip
        else if ( tagName == "text" || tagName == "rect" ) {
        }
        // others: warn
        else
            qWarning( "VectaPoly: recursiveParse (svg): unknown element '%s'", qPrintable( elem.tagName() ) );
    }

    return lines;
}

VectaPolys VectaGenerator::fromSvgPaths( const QString & fileName, const QString & elementId )
{
    VectaPolys polys;
    if ( !QFile::exists( fileName ) ) {
        qWarning( "VectaGenerator::fromSvgPaths: can't find '%s'", qPrintable( fileName ) );
        return polys;
    }

    // load svg
    QFile svgFile( fileName );
    svgFile.open( QIODevice::ReadOnly );
    QDomDocument doc;
    doc.setContent( &svgFile );
    svgFile.close();

    // recurse down into all 1st level elements
    QDomElement rootElement = doc.documentElement();
    QDomNode subNode = rootElement.firstChild();
    for ( ; !subNode.isNull(); subNode = subNode.nextSibling() ) {
        QDomElement elem = subNode.toElement();
        if ( !elem.isNull() && elem.tagName() == "g" )
            polys << recursiveParse( elem, elementId );
    }

    return polys;
}

VectaPolys VectaGenerator::starPolys( const VectaPolys & source, double mag )
{
    VectaPolys starred;
    VectaPolys::const_iterator it = source.begin(), end = source.end();
    for ( ; it != end; ++it ) {
        VectaPoly poly = *it;

        Vector2 polyCenter = poly.centerVector();

        VectaPoly::Nodes & nodes = poly.edit();
        VectaPoly::Nodes::iterator nIt = nodes.begin(), nEnd = nodes.end();
        for ( ; nIt != nEnd; ++nIt ) {
            VectaPoly::Node & node = *nIt;
            Vector2 v1 = node.point - polyCenter;
            node.point = polyCenter + v1 * (1.0 + mag * ((double)qrand() / RAND_MAX));
        }

        starred.append( poly );
    }
    return starred;
}

VectaPolys VectaGenerator::spreadPolys( const VectaPolys & source, const QRect & outGemetry )
{
    VectaPolys spread;

    // precalc params
    int rad = hypot( outGemetry.width(), outGemetry.height() );
    Vector2 center( outGemetry.center().x(), outGemetry.center().y() );

    VectaPolys::const_iterator it = source.begin(), end = source.end();
    for ( ; it != end; ++it ) {
        VectaPoly poly = *it;
        double hRad = rad + (qrand() % (rad/8));
        Control2 posPolar( hRad, 2 * M_PI * ((double)qrand() / RAND_MAX) );
        Vector2 posXY = posPolar.toVector2() + center;
        poly.setPos( posXY.x(), posXY.y() );
        spread.append( poly );
    }
    return spread;
}

VectaPolys VectaGenerator::heavyPolys( const VectaPolys & source, double G, double dT )
{
    VectaPolys polys;
    foreach ( const VectaPoly & poly, source ) {
        double acc = G * (1 + 2 * ((double)qrand() / RAND_MAX));
        double dY = acc * dT * dT * 0.5;

        // add dY to each node
        const VectaPoly::Nodes & nodes = poly.view();
        VectaPoly::Nodes::const_iterator it = nodes.begin(), end = nodes.end();
        VectaPoly newPoly;
        for ( ; it != end; ++it ) {
            VectaPoly::Node node = *it;
            node.point += Vector2( 0, dY );
            newPoly.addNode( node );
        }

        polys.append( newPoly );
    }
    return polys;
}
