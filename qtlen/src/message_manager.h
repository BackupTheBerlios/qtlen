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

#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qdom.h>
#include <qdatetime.h>

#include "message.h"
#include "chat.h"

class MessageManager : public QObject
{
		Q_OBJECT
		
	public:
		enum MessageType { NormalMessage = 0, ChatMessage = 1, ErrorMessage = 2, NewsMessage = 3, AdvertisementMessage = 4 };
		
		static void initModule();
		MessageManager();
		~MessageManager();
		
		void receiveNode( QDomNode node );
		
	public slots:
		bool sendMessage( QString j, const QString &msg, MessageType t );
		bool sendTypingNotification( QString jid, bool start_typing = true );
		bool sendSoundAlert( QString jid );
		
		int addMessageDialog( QString jid, Message::MType type );
		int findMessageDialog( QString jid, Message::MType type );
		void deleteMessageDialog( QString jid, Message::MType type );
		
		void newMessage( QString jid );
		void receiveMessage( QString jid, const QString &message, const QDateTime &datetime );
		
		int addChatDialog( QString jid );
		int findChatDialog( QString jid );
		void deleteChatDialog( QString jid );
		
		void newChatMessage( QString jid );
		void receiveChatMessage( QString jid, const QString &message, const QDateTime &datetime );
		
		void receiveTypingNotification( QString jid, bool start_typing = true );
		void receiveSoundAlert( QString jid );
		
		//void webMessage( const QString &from, const QString &email, const QString &www, const QString &message );
		
		void errorMessage( QString jid, int errornum, const QString &errorname, const QString &message, const QDateTime &datetime );
		
		void mailMessage( QString from, const QString &topic );
		
		//int findWebMessage( const QString &from );

	private:
		QValueList<Message*> messageList;
		
		QValueList<Chat*> chatList;
		
		//QValueList<Message*> webMessageList;
	
	signals:
		void gotMessage( QString jid, const QString &message, const QDateTime &datetime );
		void gotChatMessage( QString jid, const QString &message, const QDateTime &datetime );
		
		void messageSended( QString j, const QString &msg, MessageType t );
		
		void gotErrorMessage( QString jid, int errornum, const QString &errorname, const QString &message, const QDateTime &datetime );
		
		void gotNewsMessage( const QString &newsType, const QString &title, const QDateTime &dt, const QString &url, const QString &text );
		
		void gotTypingNotification( QString jid, bool start_typing );
		void gotSoundAlert( QString jid );
		
		void gotEmail( QString from, const QString &subject );
		void gotWebMessage( QString from, const QString &email, const QString &www, const QString &message );
};

extern MessageManager *msg_manager;

#endif
