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
#include <qdom.h>
#include <qsocket.h>
#include <qurl.h>
#include <qmessagebox.h>
#include <qsettings.h>

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

void Tlen::initModule()
{
	tlen_manager = new Tlen();

	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/connection" );

	tlen_manager->setUserPass( settings.readEntry("/username"), settings.readEntry("/password") );

	if( settings.readBoolEntry( "/autoConnect", false ) )
	{
		presence_manager->setStatus( (PresenceManager::PresenceStatus)settings.readNumEntry( "/defaultStatus" ),
						settings.readEntry( "/defaultDescription" ) );
	}
	
	HubManager::initModule();

	settings.endGroup();
}

Tlen::Tlen() : QObject()
{
	state = Tlen::Disconnected;
	socket = new QSocket();
	pingtimer = new QTimer();

	connect( socket, SIGNAL( connected() ), SLOT( socketConnected() ) );
	connect( this, SIGNAL( connected() ), SLOT( tlenConnected() ) );
	connect( socket, SIGNAL( connectionClosed() ), SLOT( socketConnectionClosed() ) );
	connect( socket, SIGNAL( readyRead() ), SLOT( socketReadyRead() ) );
	connect( socket, SIGNAL( error( int ) ), SLOT( socketError( int ) ) );
	connect( pingtimer, SIGNAL( timeout() ), SLOT( sendPing() ) );
	
	connect( this, SIGNAL( disconnected() ), presence_manager, SLOT( disconnected() ) );

	hostname = "s1.tlen.pl";
	hostport = 443;
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

void Tlen::connectToServer()
{
	if( username.isEmpty() || password.isEmpty() )
	{
		QMessageBox::warning( NULL, "QTlen", tr("Please enter username and/or password!") );
		SettingsDialog::showDialog();
		return;
	}
	
	//hub_manager->getServerInfo();
	
	emit connecting();
	
	socket->connectToHost(hostname,hostport);
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
			qDebug("Write on socket: "+data);
	}
	
	socket->close();
	socketConnectionClosed();
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
		qDebug("Send data:\n"+data);
	
	return ok;
}

bool Tlen::writeXml( const QDomDocument& doc )
{
	if( !isConnected() )
		return false;
	
	QString data = doc.toString();
	return socket->writeBlock( data.data(), data.length() ) == (int)data.length();
}

void Tlen::socketReadyRead()
{
	QCString s;
	s.resize( socket->bytesAvailable() );
	socket->readBlock( s.data(), socket->bytesAvailable() );
	s.insert( 0, "<s>" );
	s.append( "</s>" );
	
	QDomDocument doc;
	doc.setContent( s );
	QDomNode root = doc.firstChild();
	//qDebug( "Receive data:\n" + doc.toString() );
	if( root.hasChildNodes() )
	{
		for(QDomNode n = root.firstChild();
			!n.isNull();
			n = n.nextSibling())
		{
			event( n );
		}
	}
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

void Tlen::socketConnectionClosed()
{
	state = Tlen::Disconnected;
	emit disconnected();
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

void Tlen::setUserPass( const QString &user, const QString &pass)
{
	username = user;
	qDebug("\nUser was set on: "+user+"");
	password = pass;
	qDebug("Password was set on: <hidden>\n");
}

QString Tlen::getJid()
{
	if( username.isEmpty() )
		return tr("No user");
	else
		return username+"@tlen.pl";
}

void Tlen::socketConnected()
{
	emit connecting();
	
	QString data = "<s v='3'>";
	
	state = Tlen::Connecting;
	
	socket->writeBlock( data.latin1(), data.length() );
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

QString Tlen::getSid()
{
	return sid;
}
