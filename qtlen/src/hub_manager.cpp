/***************************************************************************
 *   Copyright (C) 2004 by Zwierzak                                        *
 *   zwierzak@programista.org                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <qsocket.h>
#include <qdom.h>
#include <qregexp.h>
#include <qdatetime.h>
#include <qsettings.h>

#include "hub_manager.h"

HubManager *hub_manager = NULL;

void HubManager::initModule()
{
	if( !hub_manager )
		hub_manager = new HubManager();
}

HubManager::HubManager()
{
	QSettings settings;
	settings.setPath( "qtlen.berlios.de", "QTlen" );

	settings.beginGroup( "/proxy" );
	
	socket = new QSocket( this );
	connect( socket, SIGNAL( connected() ), this, SLOT( connected() ) );
	connect( socket, SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
	connect( socket, SIGNAL( error( int ) ), this, SLOT( error( int ) ) );

	if( settings.readBoolEntry( "/useProxy" ) )
		socket->connectToHost( settings.readEntry( "/ip", "127.0.0.1" ), settings.readNumEntry( "/port", 80 ) );
	else
		socket->connectToHost( "idi.tlen.pl", 80 );
	
	settings.resetGroup();
}

void HubManager::connected()
{
	QSettings settings;
	settings.setPath( "qtlen.berlios.de", "QTlen" );

	settings.beginGroup( "/proxy" );
	
	QString query = "GET ";
	
	if ( settings.readBoolEntry( "/useProxy" ) )
		query += "http://idi.tlen.pl";
	
	query += QString( "/4starters.php?u=%1&v=10" ).arg( tlen_manager->getLogin() );
	
	query += " HTTP/1.1\r\n";
	query += "Host: idi.tlen.pl\r\n";
	query += "\r\n";
	
	socket->writeBlock( query.latin1(), query.length() );
	
	settings.resetGroup();
}

void HubManager::readyRead()
{
	QCString s;
	
	s.resize( socket->bytesAvailable() + 1 );
	
	socket->readBlock( s.data(), socket->bytesAvailable() );
	
	s = s.right( s.find( "\r\n\r\n" ) + 4 );
	
	QDomDocument doc;
	
	QString hostname = "s1.tlen.pl";
	int hostport = 443;
	
	doc.setContent( s );
	
	QDomElement root = doc.firstChild().toElement();
	
	if( root.tagName() == "t" )
	{
		if( root.hasAttribute( "s" ) )
			hostname = root.attribute( "s", "s1.tlen.pl" );
		if( root.hasAttribute( "p" ) )
			hostport = root.attribute( "p", "443" ).toInt();
	}
	tlen_manager->setHost( hostname, hostport );
	
	socket->close();
	
	emit finished();
}

void HubManager::error( int )
{
	tlen_manager->setHost( "s1.tlen.pl", 443 );
	
	socket->close();
	
	emit finished();
}
