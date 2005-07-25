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

#include <qobject.h>
#include <qtextstream.h>
#include <qdom.h>
#include <qsocket.h>
#include <qurl.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qregexp.h>
#include <iostream>
#include <qtimer.h>

#include "tlen.h"
#include "utils.h"
#include "auth.h"
#include "message_manager.h"
#include "presence_manager.h"
#include "roster_manager.h"
#include "pubdir_manager.h"
#include "settingsdialog.h"
#include "hub_manager.h"

Tlen *tlen_manager = NULL;

bool QTlenParser::startDocument()
{
	elementsCount = 0;
	
	return true;
}

bool QTlenParser::startElement( const QString&, const QString&, const QString &qName, const QXmlAttributes& )
{
	if( qName == "s" && elementsCount == 0 )
		stream = true;
	
	elementsCount++;
	
	return true;
}

bool QTlenParser::endElement( const QString&, const QString&, const QString& )
{
	elementsCount--;
	
	return true;
}

bool QTlenParser::endDocument()
{
	if( elementsCount == 0 || stream )
		return true;
	return false;
}

void Tlen::initModule()
{
	tlen_manager = new Tlen();
	
	QSettings settings;
	settings.setPath( "qtlen.berlios.de", "QTlen" );
	
	settings.beginGroup( "/connection" );
	
	tlen_manager->setUserPass( settings.readEntry("/username"), settings.readEntry("/password") );
	
	if( settings.readBoolEntry( "/autoConnect", false ) )
	{
		presence_manager->setStatus( (PresenceManager::PresenceStatus)settings.readNumEntry( "/defaultStatus" ),
					      settings.readEntry( "/defaultDescription" ) );
	}
	
	settings.resetGroup();
}

Tlen::Tlen() : QObject()
{
	state = Tlen::Disconnected;
	
	socket = new QSocket();
	
	pingtimer = new QTimer();
	
	connect( socket, SIGNAL( connected() ), SLOT( socketConnected() ) );
	connect( socket, SIGNAL( connectionClosed() ), SLOT( socketConnectionClosed() ) );
	connect( socket, SIGNAL( readyRead() ), SLOT( socketReadyRead() ) );
	connect( socket, SIGNAL( error( int ) ), SLOT( socketError( int ) ) );
	
	connect( pingtimer, SIGNAL( timeout() ), SLOT( sendPing() ) );
	
	connect( this, SIGNAL( connected() ), SLOT( tlenConnected() ) );
	connect( this, SIGNAL( disconnected() ), presence_manager, SLOT( disconnected() ) );
}

Tlen::~Tlen()
{
	if( socket )
		delete socket;
	
	if( pingtimer )
		delete pingtimer;
	if( errortimer )
		delete errortimer;
}

void Tlen::setUserPass( const QString &user, const QString &pass )
{
	username = user;
	password = pass;
}

void Tlen::setHost( const QString &name, int port )
{
	hostname = name;
	hostport = port;
}

QString Tlen::getLogin()
{
	return username;
}

bool Tlen::isConnected()
{
	qDebug("Tlen::isConnected()");

	switch(state)
	{
		case Tlen::ConnectingToHub:
			return false;
			break;
		case Tlen::Connecting:
			return true;
			break;
		case Tlen::Connected:
			return true;
			break;
		case Tlen::ErrorDisconnected:
			return false;
			break;
		case Tlen::Disconnected:
			return false;
			break;
	}

	return false;

}

QString Tlen::getJid()
{
	if( username.isEmpty() )
		return tr("No user");
	else
		return username+"@tlen.pl";
}

QString Tlen::getSid()
{
	return sid;
}

bool Tlen::writeXml( const QDomDocument& doc )
{
	if( !isConnected() )
		return false;
	
	QString data = doc.toString();
	
	std::cout << "Write on socket: " << data << std::endl;
	
	return socket->writeBlock( data.data(), data.length() ) == (int)data.length();
}

void Tlen::socketReadyRead()
{
	QCString buf;
	buf.resize( socket->bytesAvailable() );
	socket->readBlock( buf.data(), socket->bytesAvailable() );
	
	stream.append( buf.replace( "\0", "" ) );
	
	if( stream.isEmpty() )
		return;
	
	QString s = stream;
	
	s.prepend( "<s>" );
	s.append( "</s>" );
	
	QXmlSimpleReader reader;
	QXmlInputSource source;
	source.setData( s );
	
	QTlenParser handler;
	reader.setContentHandler( &handler );
	
	if( !reader.parse( &source, false ) && stream.left( 3 ) != "<s " )
	{
		std::cout << handler.errorString() << std::endl;
		return;	
	}
	
	std::cout << "Read from socket: " << stream << std::endl;
	
	stream.prepend( "<s>" );
	stream.append( "</s>" );
	
	QDomDocument doc;
	doc.setContent( stream );
	
	QDomNode root = doc.firstChild();
	
	if( root.hasChildNodes() )
	{
		for(QDomNode n = root.firstChild();
		    !n.isNull();
		    n = n.nextSibling())
		{
			event( n );
		}
	}
	
	stream = QString::null;
}

void Tlen::event(QDomNode node)
{
	QString nodeName = node.nodeName();
	QDomElement element = node.toElement();

	if( nodeName == "s" && element.hasAttribute("i") )
	{
		pingtimer->start( 60000 );
		sid = element.attribute("i");
		if(tlenLogin())
			state = Tlen::Connecting;
	}
	else if( nodeName == "iq" )
	{
		if( element.hasAttribute( "from" ) && element.attribute("from") == "tuba" )
			pubdir_manager->receiveNode( node );
		else if( element.hasAttribute( "type" ) && element.attribute("type") == "result" )
		{
			if( node.hasChildNodes() )
			{
				roster_manager->receiveNode( node );
				emit connected();
			}
			else if( state == Tlen::Connecting && element.attribute("id") == sid )
				roster_manager->getRoster();
		}
		else if( element.hasAttribute( "type" ) && element.attribute("type") == "set" )
			roster_manager->receiveNode( node );
		else if( element.hasAttribute( "type" ) && element.attribute("type") == "error" && state == Tlen::Connecting )
			QMessageBox::warning( NULL, "QTlen", tr("Connection failed!\nPlease check username and password") );
	}
	else if( nodeName == "message" || nodeName == "n" || nodeName == "w" ||  nodeName == "m" )
	{
		msg_manager->receiveNode( node );
	}
	else if(nodeName=="presence")
	{
		presence_manager->receiveNode( node );
	}
}

bool Tlen::tlenLogin()
{
	QCString data;
	
	data += "<iq type='set' id='";
	data += sid.latin1();
	data += "'><query xmlns='jabber:iq:auth'><username>";
	data += username.latin1();
	data += "</username><digest>";
	data += tlen_hash( password.ascii(), sid.ascii() );
	data += "</digest><resource>t</resource></query></iq>";
	
	bool ok;
	
	ok = ( socket->writeBlock( data.data(), data.length() ) == (int)data.length() );
	
	if(ok)
		std::cout << "Write on socket: " << data << std::endl;
	
	return ok;
}

void Tlen::beforeConnect()
{
	HubManager::initModule();
	connect( hub_manager, SIGNAL( finished() ), this, SLOT( connectToServer()() ) );
}

void Tlen::connectToServer()
{
	if( username.isEmpty() || password.isEmpty() )
	{
		QMessageBox::warning( NULL, "QTlen", tr("Please enter username and/or password!") );
		SettingsDialog::showDialog();
		return;
	}
	
	emit connecting();
	
	socket->connectToHost( hostname, hostport );
}

void Tlen::disconnect()
{
	if(!isConnected())
		return;
	
	pingtimer->stop();
	
	if(state == Tlen::Connected)
	{
		QCString data;
		
		data = "</s>";
		if( socket->writeBlock( data.data(), data.length() ) == (int)data.length() )
			std::cout << "Write on socket: " << data << std::endl;
	}
	
	socket->close();
	socketConnectionClosed();
}

void Tlen::sendPing()
{
	qDebug("Tlen::sendPing()");
	
	QString data = "  \t  ";
	
	socket->writeBlock( data.latin1(), data.length() );
}

void Tlen::tlenConnected()
{
	state = Tlen::Connected;
	presence_manager->setStatus( presence_manager->getStatus(), presence_manager->getDescription() );
}

void Tlen::socketConnected()
{
	emit connecting();
	
	QString data = "<s v=\"7\" t=\"05170402\">";
	
	state = Tlen::Connecting;
	
	if( socket->writeBlock( data.latin1(), data.length() ) == (int)data.length() )
		std::cout << "Write on socket: " << data << std::endl;
}

void Tlen::socketConnectionClosed()
{
	state = Tlen::Disconnected;
	emit disconnected();
}

void Tlen::socketError( int e )
{
	switch( e ) {
		case QSocket::ErrConnectionRefused:
			qWarning("Error: Connection refused");
			break;
		case QSocket::ErrHostNotFound:
			qWarning("Error: Host not found");
			break;
		case QSocket::ErrSocketRead:
			qWarning("Error: When reading from socket");
			return;
			break;
		default:
			qWarning("Error");
			break;
	}
	
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	state = Tlen::ErrorDisconnected;
	emit disconnected();
	
	settings.beginGroup( "/connection" );
	
	if( settings.readBoolEntry( "/autoReconnect", false ) )
		connectToServer();
	
	settings.endGroup();
}
