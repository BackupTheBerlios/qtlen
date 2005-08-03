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

#include <qapplication.h>
#include <qmessagebox.h>

#include "tlen.h"
#include "roster_manager.h"
#include "utils.h"

PresenceManager *presence_manager = NULL;

void PresenceManager::initModule()
{
	presence_manager = new PresenceManager();
}

PresenceManager::PresenceManager()
	: QObject()
{
	status = PresenceManager::Unavailable;
	description = QString::null;
	
	connect( this, SIGNAL( subscribe( QString ) ),
			SLOT( subscribeContact( QString ) ) );
	connect( this, SIGNAL( subscribed( QString ) ),
			SLOT( subscribedContakt( QString ) ) );
	connect( this, SIGNAL( unsubscribe( QString ) ),
			SLOT( unsubscribeContakt( QString ) ) );
	connect( this, SIGNAL( unsubscribed( QString ) ),
			SLOT( unsubscribedContakt( QString ) ) );
}

void PresenceManager::receiveNode( QDomNode node )
{
	QDomElement element = node.toElement();
	
	if( node.nodeName() == "presence" )
	{
		QString jid = takeJid( element.attribute("from") );
		PresenceManager::PresenceStatus status;
		QString description;
		if( element.hasAttribute("type") )
		{
			if( element.attribute("type")=="unavailable" )
				status = PresenceManager::Unavailable;
			else if( element.attribute("type")=="subscribe" )
			{
				emit subscribe( jid );
				return;
			}
			else if( element.attribute("type")=="subscribed" )
			{
				emit subscribed( jid );
				return;
			}
			else if( element.attribute("type")=="unsubscribe" )
			{
				emit unsubscribe( jid );
				return;
			}
			else if( element.attribute("type")=="unsubscribed" )
			{
				emit unsubscribed( jid );
				return;
			}
		}
		
		for( QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling() )
		{
			if ( n.nodeName() == "show" )
			{
				QDomElement e = n.toElement();
				QString s = e.text();
				if(s=="available")
					status = PresenceManager::Available;
				else if(s=="chat")
					status = PresenceManager::Chat;
				else if(s=="away")
					status = PresenceManager::Away;
				else if(s=="xa")
					status = PresenceManager::ExtAway;
				else if(s=="dnd")
					status = PresenceManager::Dnd;
				else if(s=="unavailable")
					status = PresenceManager::Unavailable;
				else
					qDebug("Undefined status");
			}
			else if ( n.nodeName() == "status" )
			{
				QDomElement e = n.toElement();
				description = decode( e.text().latin1() );
			}
		}
		
		emit changeContactStatus( jid, status, description );
	}
}

void PresenceManager::setStatus( PresenceManager::PresenceStatus s, const QString &d )
{
	if( ( tlen_manager->state == Tlen::ErrorDisconnected ||
		tlen_manager->state == Tlen::Disconnected )
		&& s != PresenceManager::Unavailable )
	{
		status = s;
		description = d;
		qDebug("Tlen::setStatus() end");
		tlen_manager->connectToServer();
		return;
	}
	else if( tlen_manager->state == Tlen::Connecting )
	{
		status = s;
		description = d;
		qDebug("Tlen::setStatus() end");
		return;
	}
	
	
	QDomDocument doc;
	
	QDomElement presence = doc.createElement( "presence" );
	
	QString _s;
	
	if( s != PresenceManager::Invisible || s != PresenceManager::Unavailable )
	{
		_s = this->statusToShort( s );
	}
	else
	{
		presence.setAttribute( "type", this->statusToShort( s ) );
	}
	
	doc.appendChild( presence );
	
	QDomElement show = doc.createElement( "show" );
	if( !_s.isEmpty() )
	{
		QDomText text = doc.createTextNode( _s );
		show.appendChild( text );
	}
	presence.appendChild( show );
	
	QDomElement statusElement = doc.createElement( "status" );
	if( !d.isEmpty() )
	{
		QDomText text = doc.createTextNode( encode( d ) );
		statusElement.appendChild( text );
	}
	presence.appendChild( statusElement );
	
	if( tlen_manager->writeXml( doc ) )
	{
		status = s;
		description = d;
		
		emit statusChanged( status, description );
		
		if( status == PresenceManager::Unavailable )
			tlen_manager->disconnect();
	}
}

int PresenceManager::statusToInt( PresenceManager::PresenceStatus status )
{
	return (int)status;
}

QString PresenceManager::statusToString( PresenceManager::PresenceStatus status )
{
	switch( status )
	{
		case PresenceManager::Available:
			return tr( "Available" );
			break;
		case PresenceManager::Chat:
			return tr( "Free for Chat" );
			break;
		case PresenceManager::Away:
			return tr( "Away" );
			break;
		case PresenceManager::ExtAway:
			return tr( "Not Available" );
			break;
		case PresenceManager::Dnd:
			return tr( "Do not Disturb" );
			break;
		case PresenceManager::Invisible:
			return tr( "Invisible" );
			break;
		case PresenceManager::Unavailable:
			return tr( "Unavailable" );
			break;
	}
	
	return tr( "Unavailable" );
}

QString PresenceManager::statusToShort( PresenceManager::PresenceStatus status )
{
	switch( status )
	{
		case PresenceManager::Available:
			return "available";
			break;
		case PresenceManager::Chat:
			return "chat";
			break;
		case PresenceManager::Away:
			return "away";
			break;
		case PresenceManager::ExtAway:
			return "xa";
			break;
		case PresenceManager::Dnd:
			return "dnd";
			break;
		case PresenceManager::Invisible:
			return "invisible";
			break;
		case PresenceManager::Unavailable:
			return "unavailable";
			break;
	}
	
	return "offline";
}

PresenceManager::PresenceStatus PresenceManager::getStatus()
{
	return status;
}

const QString& PresenceManager::getDescription()
{
	return description;
}

void PresenceManager::subscribeContact( QString jid ) //Kto¶ wysy³a pro¶bê o subskrybcjê
{
	int index = roster_manager->findContact( jid );
	
	if( roster_manager->roster[ index ].subscription == "both" || roster_manager->roster[ index ].subscription == "to" )
	{
		subscribedMe( jid );
		return;
	}
	
	QMessageBox *msg = new QMessageBox( "QTlen", "<b>" + jid + "</b>\n" + tr( "This contact want to subscribe you.\nDo you want to add this to your roster?" ), QMessageBox::NoIcon, QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape, QMessageBox::NoButton, qApp->mainWidget() );
	
	msg->setIconPixmap( takePixmap( "messagebox_question" ) );
	//msg->show();
	
	if( msg->exec() == QMessageBox::Yes )
	{
		roster_manager->addContact( jid );
		
		if( index == -1 || roster_manager->roster[ index ].subscription == "none"
				|| roster_manager->roster[ index ].subscription == "to" )
		{
			subscribeMe( jid );
		}
		subscribedMe( jid );
	}
	else
	{
		unsubscribedMe( jid );
	}
}

void PresenceManager::subscribedContakt( QString jid ) //Kto¶ zgadza siê na subskrybcjê
{
	int n = roster_manager->findContact( jid );
	
	if( n != -1 )
	{
		if( roster_manager->roster[n].subscription == "none" )
			roster_manager->roster[n].subscription = "to";
		else if( roster_manager->roster[n].subscription == "from" )
			roster_manager->roster[n].subscription = "both";
	}
}

void PresenceManager::unsubscribeContakt( QString jid ) //Kto¶ wysy³a pro¶bê o desubskrybcjê
{
	int a = QMessageBox::question( qApp->mainWidget(), "QTlen", roster_manager->getContactName( jid ) + tr( " want do remove you from his roster.\nWhat do you want to do?" ), tr( "Remove subscription" ), tr( "Remove from roster" ), tr( "Do nothing" ) );
	if( a == 0 )
	{
		unsubscribedMe( jid );
	}
	else if( a == 1 )
	{
		unsubscribedMe( jid );
		if( roster_manager->isExist( jid ) )
			roster_manager->removeContact( jid );
	}
}

void PresenceManager::unsubscribedContakt( QString ) //Kto¶ zgadza siê na desubskrybcjê
{
	//Nie op³aca siê go usunaæ bo juz ktos mi o tym mowil
	//if( roster_manager->isExist( jid ) )
	//	roster_manager->removeContact( jid );
}

void PresenceManager::subscribeMe( QString jid ) //Ja wysy³a pro¶bê o subskrybcjê
{
	QDomDocument doc;
	
	QDomElement presence = doc.createElement( "presence" );
	presence.setAttribute( "to", jid );
	presence.setAttribute( "type", "subscribe" );
	doc.appendChild( presence );
	
	tlen_manager->writeXml( doc );
}

void PresenceManager::subscribedMe( QString jid ) //Ja zgadza siê na subskrybcjê
{
	QDomDocument doc;
	
	QDomElement presence = doc.createElement( "presence" );
	presence.setAttribute( "to", jid );
	presence.setAttribute( "type", "subscribed" );
	doc.appendChild( presence );
	
	tlen_manager->writeXml( doc );
}

void PresenceManager::unsubscribeMe( QString jid ) //Ja wysy³a pro¶bê o desubskrybcjê
{
	QDomDocument doc;
	
	QDomElement presence = doc.createElement( "presence" );
	presence.setAttribute( "to", jid );
	presence.setAttribute( "type", "unsubscribe" );
	doc.appendChild( presence );
	
	tlen_manager->writeXml( doc );
}

void PresenceManager::unsubscribedMe( QString jid ) //Ja zgadza siê na desubskrybcjê
{
	QDomDocument doc;
	
	QDomElement presence = doc.createElement( "presence" );
	presence.setAttribute( "to", jid );
	presence.setAttribute( "type", "unsubscribed" );
	doc.appendChild( presence );
	
	tlen_manager->writeXml( doc );
}

void PresenceManager::disconnected()
{
	emit statusChanged( Unavailable, description );
}
