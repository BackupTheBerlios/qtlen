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

#include <qmessagebox.h>
#include <qnamespace.h>

#include "tlen.h"
#include "roster_manager.h"
#include "presence_manager.h"
#include "utils.h"

RosterManager *roster_manager = NULL;

void RosterManager::initModule()
{
	if( roster_manager == NULL )
		roster_manager = new RosterManager();
}

RosterManager::RosterManager()
	: QObject()
{
	connect( tlen_manager, SIGNAL( disconnected() ),
		this, SLOT( disconnected() ) );
	connect( presence_manager, SIGNAL( changeContactStatus( QString, PresenceManager::PresenceStatus, const QString & ) ),
		this, SLOT( changeContactStatus( QString, PresenceManager::PresenceStatus, const QString & ) ) );
}

void RosterManager::receiveNode( QDomNode node )
{
	QDomNodeList nodeList;
	QDomElement element = node.toElement();
	
	if( node.hasChildNodes() )
	{
		nodeList = node.childNodes();
	}
	
	if( node.nodeName() == "iq" )
	{
		if( element.attribute( "type" ) == "result" )
			for( int index = 0; index < (int)roster.count(); index++ )
				roster[ index ].subscription = "remove";
		
		for ( int n = 0; n < (int)nodeList.count(); n++ )
			receiveNode( nodeList.item( n ) );
		
		if( element.attribute( "type" ) == "result" )
			for( int index = (int)roster.count(); index >= 0 ; index-- )
				if( roster[ index ].subscription == "remove" )
					roster.remove( roster.at( index ) );
		
		qBubbleSort( roster.begin(), roster.end() );
		
		emit refreshContext();
	}	
	else if( node.nodeName() == "query" )
	{
		for ( int n = 0; n < (int)nodeList.count(); n++ )
			receiveNode( nodeList.item( n ) );
	}
	else if( node.nodeName() == "item" )
	{
		QString jid = element.attribute("jid");
		QString subscription = QString::null;
		if( element.hasAttribute( "subscription" ) )
			subscription = element.attribute( "subscription" );
		QString group = QString::null;
		if( node.hasChildNodes() )
		{
			for ( int n = 0; n < (int)nodeList.count(); n++ )
			{
				QDomNode groupNode = nodeList.item( n );
				QDomElement groupElement = groupNode.toElement();
				if( groupNode.nodeName() == "group" )
				{
					group = decode( groupElement.text().latin1() );
				}
			}
		}
		
		int index = findContact( jid );
		
		if( subscription == "remove" )
		{
			roster.remove( roster.at( index ) );
		}
		else
		{
			if(index==-1)
			{
				Contact c;
				c.jid = jid;
				if(element.hasAttribute("name"))
					c.name = decode(element.attribute("name").latin1());
				else
					c.name = jid;
				c.group = group;
				c.subscription = subscription;
				c.status = PresenceManager::Unavailable;
				c.description = QString::null;
				roster.append( c );
			}
			else
			{
				if(element.hasAttribute("name"))
					roster[index].name = decode(element.attribute("name").latin1());
				else
					roster[index].name = jid;
				roster[index].group = group;
				roster[index].subscription = subscription;
			}
		}
	}
}

void RosterManager::getRoster()
{
	QDomDocument doc;
	
	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "type", "get" );
	iq.setAttribute( "id", "GetRoster" );
	doc.appendChild( iq );
	
	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:roster" );
	iq.appendChild( query );
	
	tlen_manager->writeXml( doc );
}

QString RosterManager::getContactName( QString jid )
{
	int index = findContact( jid );
	
	if( index != -1 )
		return roster[ index ].name;
	else
		return QString::null;
}

QString RosterManager::getContactGroup( QString jid )
{
	int index = findContact( jid );
	
	if( index != -1 )
		return roster[ index ].group;
	else
		return QString::null;
}

PresenceManager::PresenceStatus RosterManager::getContactStatus( QString jid )
{
	int index = findContact( jid );
	
	if( index != -1 )
		return roster[ index ].status;
	else
		return PresenceManager::Unavailable;
}

QString RosterManager::getContactDescription( QString jid )
{
	int index = findContact( jid );
	
	if( index != -1 )
		return roster[ index ].description;
	else
		return QString::null;
}

bool RosterManager::addContact( QString j, const QString &n, const QString &g )
{
	QDomDocument doc;
	
	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "type", "set" );
	iq.setAttribute( "id", tlen_manager->getSid() );
	doc.appendChild( iq );
	
	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:roster" );
	iq.appendChild( query );
	
	QDomElement item = doc.createElement( "item" );
	item.setAttribute( "jid", j );
	if( !n.isEmpty() )
		item.setAttribute( "name", encode( n ) );
	query.appendChild( item );
	
	if( !g.isEmpty() )
	{
		QDomElement group = doc.createElement( "group" );
		item.appendChild( group );
		
		QDomText text = doc.createTextNode( encode( g ) );
		group.appendChild( text );
	}
	
	return tlen_manager->writeXml( doc );
}

bool RosterManager::addContact( const Contact &c )
{
	return addContact( c.jid, c.name, c.group );
}

bool RosterManager::editContact( QString j, const QString &n, const QString &g )
{
	if( isExist( j ) )
		return addContact( j, n, g );
	return false;
}

bool RosterManager::editContact( const Contact &c )
{
	if( isExist( c.jid ) )
		return addContact( c.jid, c.name, c.group );
	return false;
}

bool RosterManager::removeContact( QString j )
{
	presence_manager->unsubscribeMe( j );
	presence_manager->unsubscribedMe( j );
	
	QDomDocument doc;
	
	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "type", "set" );
	iq.setAttribute( "id", tlen_manager->getSid() );
	doc.appendChild( iq );
	
	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:roster" );
	iq.appendChild( query );
	
	QDomElement item = doc.createElement( "item" );
	item.setAttribute( "jid", j );
	item.setAttribute( "subscription", "remove" );
	query.appendChild( item );
	
	return tlen_manager->writeXml( doc );
}

void RosterManager::changeContactStatus( QString jid, PresenceManager::PresenceStatus status, const QString &description )
{
	int index = findContact( jid );
	if( index != -1 )
	{
		roster[ index ].status = status;
		roster[ index ].description = description;
	}
	
	emit refreshContext();
}

void RosterManager::disconnected()
{
	for( int index = 0; index < (int)roster.count(); index++ )
		roster[ index ].status = PresenceManager::Unavailable;
	emit refreshContext();
}

bool RosterManager::isExist( QString jid )
{
	int n = findContact( jid );
	if( n == -1 )
		return false;
	return true;
}

int RosterManager::findContact( QString jid )
{
	for ( int n = 0; n < (int)roster.count(); n++ )
		if( roster[n].jid == jid )
			return n;
	return -1;
}

