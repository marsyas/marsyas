/*
 * Copyright ( C ) 2007 Arnold Krille <arnold@arnoldarts.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or ( at your option ) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <iostream>
#include "qoscserver.h"

#include <QtCore/QRegExp>
#include <QtNetwork/QUdpSocket>

QOscServer::QOscServer( quint16 port, QObject* p )
	: QOscBase( p )
{
	socket()->bind( QHostAddress::Any, port );
	connect( socket(), SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
}
QOscServer::QOscServer( QHostAddress address, quint16 port, QObject* p )
	: QOscBase( p )
{
	socket()->bind( address, port );
	connect( socket(), SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
}

QOscServer::~QOscServer() {
}

void QOscServer::registerPathObject( PathObject* p ) {
	paths.push_back( p );
}
void QOscServer::unregisterPathObject( PathObject* p ) {
	paths.removeAll( p );
}

#define BUFFERSIZE 255

void QOscServer::readyRead() {
	while ( socket()->hasPendingDatagrams() ) {
		QByteArray packet( BUFFERSIZE, char( 0 ) );
		int size = socket()->readDatagram( packet.data(), BUFFERSIZE );

		QString path;
		QString args;
		QVariant arguments;

		int i=0;
		if ( packet[ i ] == '/' ) {

			for ( ; i<size && packet[ i ] != char( 0 ); ++i )
				path += packet[ i ];

			while ( packet[ i ] != ',' ) ++i;
			++i;
			while ( packet[ i ] != char( 0 ) )
				args += packet[ i++ ];

			if ( ! args.isEmpty() ) {
				QList<QVariant> list;

				foreach( QChar type, args ) {
					while ( i%4 != 0 ) ++i;

					QByteArray tmp = packet.right( packet.size()-i );
					QVariant value;
					if ( type == 's' ) {
						QString s = toString( tmp );
						value = s;
						i += s.size();
					}
					if ( type == 'i' ) {
						value = toInt32( tmp );
						i+=4;
					}
					if ( type == 'f' ) {
						value = toFloat( tmp );
						i+=4;
					}
					//qDebug() << " got" << value;

					if ( args.size() > 1 )
						list.append( value );
					else
						arguments = value;
				}

				if ( args.size() > 1 )
					arguments = list;
			}
		}

		QMap<QString,QString> replacements;
		replacements[ "!" ] = "^";
		replacements[ "{" ] = "(";
		replacements[ "}" ] = ")";
		replacements[ "," ] = "|";
		replacements[ "*" ] = ".*";
		replacements[ "?" ] = ".";

		foreach( QString rep, replacements.keys() )
			path.replace( rep, replacements[ rep ] );

		// patch for propagating data to the Marsyas network [ML]
		emit data(path, arguments);

		QRegExp exp( path );
		foreach( PathObject* obj, paths ) {
			if ( exp.exactMatch( obj->_path ) )
				obj->signalData( arguments );
		}
	}
}

