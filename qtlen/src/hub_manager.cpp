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

#include <qdom.h>

#include "hub_manager.h"

HubManager *hub_manager = NULL;

void HubManager::initModule()
{
	hub_manager = new HubManager();
}

HubManager::HubManager()
	: QObject()
{
	socket = new QSocket();
	
	connect( socket, SIGNAL( connected() ), SLOT( connected() ) );
	connect( socket, SIGNAL( readyRead() ), SLOT( readyRead() ) );
	connect( socket, SIGNAL( error( int ) ), SLOT( error( int ) ) );
}

HubManager::~ HubManager()
{
	
}

void HubManager::getServerInfo()
{
	socket->connectToHost( "idi.tlen.pl", 80 );
}

void HubManager::connected()
{
	QString data;
	data += "GET /4starters.php?u=identyfikator&v=10 HTTP/1.0\n\r";
	data += "Host: idi.tlen.pl";
	
	socket->writeBlock( data.ascii(), data.length() );
}

void HubManager::readyRead()
{
	QCString s;
	s.resize( socket->bytesAvailable() );
	socket->readBlock( s.data(), socket->bytesAvailable() );
	
	qDebug( s );
	
	/*QDomDocument doc;
	doc.setContent( s );
	QDomNode root = doc.firstChild();
	QDomElement element = root.toElement();
	
	if( root.nodeName() == "t" )
	{
		QString server = "s1.tlen.pl", ip;
		Q_UINT16 port = 443;
		
		if( element.hasAttribute( "s" ) )
			server = element.attribute( "s" );
		
		if( element.hasAttribute( "p" ) )
			port = element.attribute( "p" ).toInt();
		
		if( element.hasAttribute( "i" ) )
			ip = element.attribute( "i" );
		
		emit serverInfo( server, port, ip );
	}
	else
		emit serverInfo( "s1.tlen.pl", 443, "" );*/
	
	socket->close();
}

void HubManager::error( int )
{
	emit serverInfo( "s1.tlen.pl", 443, "" );
}
