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

#ifndef PRESANCE_MANAGER_H
#define PRESANCE_MANAGER_H

#include <qobject.h>
#include <qstring.h>
#include <qdom.h>

#include "tlen.h"

class PresenceManager : public QObject
{
		Q_OBJECT
	
	public:
		enum PresenceStatus { Available = 0, Chat = 1, Away = 2, ExtAway = 3,
					Dnd = 4, Invisible = 5, Unavailable = 6 };
		
		static void initModule();
		PresenceManager();
		
		void receiveNode( QDomNode node );
		
		void setStatus( PresenceManager::PresenceStatus s, const QString &d = QString::null );
		
		int statusToInt( PresenceManager::PresenceStatus status );
		QString statusToString( PresenceManager::PresenceStatus status );
		QString statusToShort( PresenceManager::PresenceStatus status );
		
		PresenceManager::PresenceStatus getStatus();
		const QString& getDescription();
		
	public slots:
		void subscribeContact( QString jid ); //Kto¶ wysy³a pro¶bê o subskrybcjê
		void subscribedContakt( QString jid ); //Kto¶ zgadza siê na subskrybcjê
		void unsubscribeContakt( QString jid ); //Kto¶ wysy³a pro¶bê o desubskrybcjê
		void unsubscribedContakt( QString jid ); //Kto¶ zgadza siê na desubskrybcjê
		
		void subscribeMe( QString jid ); //Ja wysy³a pro¶bê o subskrybcjê
		void subscribedMe( QString jid ); //Ja zgadza siê na subskrybcjê
		void unsubscribeMe( QString jid ); //Ja wysy³a pro¶bê o desubskrybcjê
		void unsubscribedMe( QString jid ); //Ja zgadza siê na desubskrybcjê
		
		void disconnected();
		
	private:
		PresenceStatus status;
		QString description;
		
	signals:
		void subscribe( QString jid );
		void subscribed( QString jid );
		void unsubscribe( QString jid );
		void unsubscribed( QString jid );
		
		void statusChanged( PresenceManager::PresenceStatus status, const QString &description );
		
		void changeContactStatus( QString jid, PresenceManager::PresenceStatus status, const QString &description );
};

extern PresenceManager *presence_manager;

#endif
