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
#include <qsettings.h>
#include <qerrormessage.h>
#include <qdom.h>

#include "tlen.h"
#include "message_manager.h"
#include "utils.h"

MessageManager *msg_manager = NULL;

void MessageManager::initModule()
{
	msg_manager = new MessageManager();
}

MessageManager::MessageManager()
	: QObject()
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/general" );
	
	connect( this, SIGNAL( gotMessage( QString, const QString&, const QDateTime& ) ),
			SLOT( receiveMessage( QString, const QString&, const QDateTime& ) ) );
	connect( this, SIGNAL( gotChatMessage( QString, const QString&, const QDateTime& ) ),
			SLOT( receiveChatMessage( QString, const QString&, const QDateTime& ) ) );
	connect( this, SIGNAL( gotErrorMessage( QString, int, const QString&, const QString&, const QDateTime& ) ),
			SLOT( errorMessage( QString, int, const QString&, const QString&, const QDateTime& ) ) );
		
	connect( this, SIGNAL( gotTypingNotification( QString, bool ) ),
			SLOT( receiveTypingNotification( QString, bool ) ) );
	connect( this, SIGNAL( gotSoundAlert( QString ) ),
			SLOT( receiveSoundAlert( QString ) ) );
	
	if( settings.readBoolEntry( "/email", true ) )
		connect( this, SIGNAL( gotEmail( QString, const QString& ) ),
				SLOT( mailMessage( QString, const QString& ) ) );
	//tymaczasowo do piasku puki jest brak klasy WebMessage
	//if( settings.readBoolEntry( "/webmsg", true ) )
	//	connect( tlen_manager, SIGNAL(gotWebMessage( const QString&, const QString&, const QString&, const QString& )),
	//			SLOT(webMessage( const QString&, const QString&, const QString&, const QString& )) );
	
	settings.endGroup();
}

MessageManager::~MessageManager()
{
	messageList.clear();
	
	chatList.clear();
}

void MessageManager::receiveNode( QDomNode node )
{
	qDebug( "MessageManager::receiveNode()" );
	QDomElement element = node.toElement();
	
	if( node.nodeName() == "message" )
	{		
		MessageType type = NormalMessage;
		
		//Normal Message
		QString jid, subject, text;
		
		if( element.hasAttribute("from") )
			jid = element.attribute("from");
		else if( element.hasAttribute("to") )
			jid = element.attribute("to");
		
		//Error Message
		int errornum;
		QString errorname;
		
		//News Message
		QString newsType, title, url;
		
		QDateTime datetime = QDateTime::currentDateTime( Qt::LocalTime );
		
		if( !node.hasChildNodes() )
			return;
		
		if( jid == "newspub" )
			type = NewsMessage;
		
		if( element.hasAttribute( "type" ) )
		{
			if( element.attribute( "type" ) == "chat" )
				type = ChatMessage;
			else if( element.attribute( "type" ) == "error" )
				type = ErrorMessage;
		}
		
		for( QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling() )
		{
			QDomElement e = n.toElement();
			
			if ( n.nodeName() == "subject" )
			{
				text = decode( e.attribute( "subject" ).latin1() );
			}
			else if ( n.nodeName() == "body" )
			{
				text = decode( e.text().latin1() );
			}
			else if( n.nodeName() == "no" )
				type = AdvertisementMessage;
			else if( n.nodeName() == "error" )
			{
				if( e.hasAttribute( "code" ) )
					errornum = e.attribute( "code" ).toInt();
				errorname = decode( e.text().latin1() );
			}
			else if( n.nodeName() == "x" )
			{
				if( n.hasChildNodes() )
				{
					QDomNode items = n.firstChild();
					
					for( QDomNode item = node.firstChild(); !item.isNull(); item = item.nextSibling() )
					{
						QDomElement itemElement = n.toElement();
						
						if( item.nodeName() == "title" )
							title = decode( itemElement.text().latin1() );
						else if( item.nodeName() == "date" )
						{
							QString date = decode( itemElement.text().latin1() );
							
							int year = date.mid( 0, 4 ).toInt();
							int month = date.mid( 5, 2 ).toInt();
							int day = date.mid( 8, 2 ).toInt();
							
							int hour = date.mid( 11, 2 ).toInt();
							int min = date.mid( 13, 2 ).toInt();
							int sec = date.mid( 16, 2 ).toInt();
							
							datetime.setDate( QDate( year, month, day ) );
							datetime.setTime( QTime( hour, min, sec ) );
						}							
						else if( item.nodeName() == "url" )
							url = decode( itemElement.text().latin1() );
						else if( item.nodeName() == "text" )
							text = decode( itemElement.text().latin1() );
					}
				}
				else
				{
					if( e.hasAttribute( "xmlns" ) && e.attribute( "xmlns" ) == "jabber:x:delay" )
					{
						QString stamp = e.attribute( "stamp" );
						
						int year = stamp.mid( 0, 4 ).toInt();
						int month = stamp.mid( 4, 2 ).toInt();
						int day = stamp.mid( 6, 2 ).toInt();
						
						int hour = stamp.mid( 9, 2 ).toInt();
						int min = stamp.mid( 12, 2 ).toInt();
						int sec = stamp.mid( 15, 2 ).toInt();
						
						datetime.setDate( QDate( year, month, day ) );
						datetime.setTime( QTime( hour, min, sec ) );
					}
				}
			}
		}
		
		if( text.isEmpty() )
			return;
		
		if( type == NormalMessage)
			emit gotMessage( jid, text, datetime );
		else if( type == ChatMessage )
			emit gotChatMessage( jid, text, datetime );
		else if( type == ErrorMessage )
			emit gotErrorMessage( jid, errornum, errorname, text, datetime );
		else if( type == NewsMessage )
			emit gotNewsMessage( newsType, title, datetime, url, text );

	}
	else if( node.nodeName() == "n" )
	{
		QString from = decode( element.attribute( "f" ).ascii() );
		QString subject = decode( element.attribute( "s" ).ascii() );
		emit gotEmail( from, subject );
	}
	else if( node.nodeName() == "w" && element.hasAttribute( "f" ) && element.hasAttribute( "e" ) && element.hasAttribute( "s" ) )
	{
		QString jid = element.attribute( "f" );
		QString email = element.attribute( "e" );
		QString www = element.attribute( "s" );
		QString msg = element.text();
		emit gotWebMessage( jid, email, www, msg );
	}
	else if( node.nodeName() == "m" && element.hasAttribute( "f" ) && element.hasAttribute( "tp" ) )
	{
		QString jid = element.attribute( "f" );
		QString type = element.attribute( "tp" );
		if( type == "a" )
			emit gotSoundAlert( jid );
		else if( type == "t" || type == "u" )
		{
			bool start_typing = true;
			if( type == "t" )
				start_typing = true;
			else if( type == "u" )
				start_typing = false;
			emit gotTypingNotification( jid, start_typing );
		}
	}
}

bool MessageManager::sendMessage( QString j, const QString &msg, MessageType t )
{
	if( !tlen_manager->isConnected() )
		return false;
	
	QDomDocument doc;
	
	QDomElement message = doc.createElement( "message" );
	message.setAttribute( "to", j );
	QString type = "normal";
	if( t == MessageManager::ChatMessage )
		type = "chat";
	message.setAttribute( "type", type );
	doc.appendChild( message );
	
	QDomElement body = doc.createElement( "body" );
	message.appendChild( body );
	
	if( !msg.isEmpty() )
	{
		QDomText text = doc.createTextNode( encode( msg ) );
		body.appendChild( text );
	}
	
	if( tlen_manager->writeXml( doc ) )
	{
		emit messageSended( j, msg, t );
		return true;
	}
	
	return false;
}

bool MessageManager::sendTypingNotification( QString jid, bool start_typing )
{
	if( !tlen_manager->isConnected() )
		return false;
	
	QDomDocument doc;
	
	QDomElement typing_notification = doc.createElement( "m" );
	typing_notification.setAttribute( "to", jid );
	
	typing_notification.setAttribute( "tp", start_typing ? "t" : "u" );
	doc.appendChild( typing_notification );
	
	return tlen_manager->writeXml( doc );
}

bool MessageManager::sendSoundAlert( QString jid )
{
	if( !tlen_manager->isConnected() )
		return false;
	
	QDomDocument doc;
	
	QDomElement sound_alert = doc.createElement( "m" );
	sound_alert.setAttribute( "to", jid );
	sound_alert.setAttribute( "tp", "a" );
	doc.appendChild( sound_alert );
	
	return tlen_manager->writeXml( doc );	
}

int MessageManager::addMessageDialog( QString jid, Message::MType type )
{
	qDebug( "MessageManager::addMessageDialog()" );
	messageList += new Message( jid, type );
	
	return findMessageDialog( jid, type );
}

int MessageManager::findMessageDialog( QString jid, Message::MType type )
{
	qDebug( "MessageManager::findMessageDialog()" );
	
	for ( int n = 0; n < (int)messageList.count(); n++ )
	{
		qDebug( "dupa" );
		if( messageList[n]->getMType() == type && messageList[n]->getJid() == jid )
			return n;
	}
	
	return -1;
}

void MessageManager::deleteMessageDialog( QString jid, Message::MType type )
{
	QValueList<Message*>::iterator it;
	
	for ( it = messageList.begin(); it != messageList.end(); ++it )
	{
		if( (*it)->getMType() == type && (*it)->getJid() == jid )
		{
			messageList.remove( it );
			
			delete (*it);
			
			return;
		}
	}
}

void MessageManager::newMessage( QString jid )
{
	int pos;
	
	if( jid.isEmpty() )
		return;
	
	pos = findMessageDialog( jid, Message::New );
	
	if( pos == -1 )
		pos = addMessageDialog( jid, Message::New );
	
	messageList[ pos ]->show();
}

void MessageManager::receiveMessage( QString jid, const QString &message, const QDateTime &datetime )
{
	int pos = findMessageDialog( jid, Message::Revice );
	if( pos == -1 )
		pos = addMessageDialog( jid, Message::Revice );
	
	messageList[ pos ]->addMessage( message, datetime );
	messageList[ pos ]->show();
}

int MessageManager::addChatDialog( QString jid )
{
	chatList += new Chat( jid );
	
	return findChatDialog( jid );
}

int MessageManager::findChatDialog( QString jid )
{
	for ( int n = 0; n < (int)chatList.count(); n++ )
		if( chatList[n]->getJid() == jid )
			return n;
	return -1;
}

void MessageManager::deleteChatDialog( QString jid )
{
	QValueList< Chat* >::iterator it;
	
	for ( it = chatList.begin(); it != chatList.end(); ++it )
	{
		if( (*it)->getJid() == jid )
		{
			chatList.remove( it );
			
			delete (*it);
			
			return;
		}
	}
}

void MessageManager::newChatMessage( QString jid )
{
	if( jid.isEmpty() || jid == "@tlen.pl" )
		return;
	
	int pos = findChatDialog( jid );
	
	if( pos == -1 )
		pos = addChatDialog( jid );
	
	chatList[ pos ]->show();
}

void MessageManager::receiveChatMessage( QString jid, const QString &message, const QDateTime &datetime )
{
	int pos = findChatDialog( jid );
	
	if( pos == -1 )
	{
		pos = addChatDialog( jid );
	}
	
	if( chatList[ pos ]->isHidden() )
		chatList[ pos ]->show();
	
	chatList[ pos ]->addMessage( message, datetime );
}

void MessageManager::receiveTypingNotification( QString jid, bool start_typing )
{
	int pos = findChatDialog( jid );
	
	if( pos != -1 )
	{
		chatList[ pos ]->typingNotification( start_typing );
	}
}

void MessageManager::receiveSoundAlert( QString jid )
{
	int pos = findChatDialog( jid );
	
	if( pos == -1 )
	{
		pos = addChatDialog( jid );
	}
	
	if( chatList[ pos ]->isHidden() )
		chatList[ pos ]->show();
	
	QString message = tr( "This user send sound alert to you" );
	
	chatList[ pos ]->addSystemMessage( message, QDateTime::currentDateTime( Qt::LocalTime ) );
}

//void MessageManager::webMessage( const QString &from, const QString &email, const QString &www, const QString &message )
//{
//    Message *msg = new Message( from, email, www, message );
//    msg->show();
//}

void MessageManager::errorMessage( QString jid, int errornum, const QString &errorname, const QString &message, const QDateTime &datetime )
{
//	QString text;
//	
//	text += QString::number( errornum );
//	text += " - ";
//	text += errorname;
//	
//	text += "<hr><br>";
//	
//	text += message;
//	
//	int pos = findMessageDialog( jid, Message::Revice );
//	if( pos == -1 )
//		pos = addMessageDialog( jid, Message::Revice );
//	
//	messageList[ pos ]->addSystemMessage( text, datetime );
//	messageList[ pos ]->show();
}

void MessageManager::mailMessage( QString from, const QString &topic )
{
	QMessageBox *msg = new QMessageBox( tr( "New e-mail" ),
			"<nobr>" + tr( "<b>From:</b> " ) + plain2rich( from ) + "</nobr><br>" +
			"<nobr>" + tr( "<b>Topic:</b> " ) + plain2rich( topic ) + "</nobr>",
			QMessageBox::NoIcon, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, 0, 0, false );
	
	msg->setIconPixmap( takePixmap( "mail" ) );
	msg->show();
}

//int MessageManager::findWebMessage( const QString &from )
//{
//	for ( int n = 0; n < webMessageList.count(); ++n )
//		if( webMessageList[n]->getJid() == jid )
//			return n;
//	return -1;
//}
