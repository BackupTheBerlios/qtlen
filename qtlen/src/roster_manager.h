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
 
#ifndef ROSTER_MANAGER_H
#define ROSTER_MANAGER_H

#include <qobject.h>
#include <qstring.h>
#include <qdom.h>
#include <qvaluelist.h>

#include "roster_box.h"
#include "presence_manager.h"

class Contact
{
	public:
		Contact() {}
		Contact( QString j, QString n, QString g, QString s )
				: jid(j), name(n), group(g), subscription(s),
				status( PresenceManager::Unavailable ), description( QString::null ) {}
		Contact(const Contact& other)
		{
			jid = other.jid;
			name = other.name;
			group = other.group;
			subscription = other.subscription;
			status = other.status;
			description = other.description;
		}

		Contact& operator=(const Contact& other) {
			if (this == &other) return *this;
			jid = other.jid;
			name = other.name;
			group = other.group;
			subscription = other.subscription;
			status = other.status;
			description = other.description;
			return *this;
		}
		
		bool operator<(const Contact& other) {
			if( QString::localeAwareCompare( name, other.name ) > 0 )
				return true;
			else
				return false;
			
		}
		
		~Contact() {}
		
		QString jid;
		QString name;
		QString group;
		QString subscription;
		PresenceManager::PresenceStatus status;
		QString description;
};

class RosterManager : public QObject
{
		Q_OBJECT
	
	public:
		static void initModule();
		RosterManager();
		
		void receiveNode( QDomNode node );
		void getRoster();
		
		QString getContactName( QString jid );
		QString getContactGroup( QString jid );
		PresenceManager::PresenceStatus getContactStatus( QString jid );
		QString getContactDescription( QString jid );
		
	public slots:
		bool addContact( QString j, const QString &n = QString::null, const QString &g = QString::null );
		bool addContact( const Contact &c );
		
		bool editContact( QString j, const QString &n = QString::null, const QString &g = QString::null );
		bool editContact( const Contact &c );
		
		bool removeContact( QString j );
		
	private slots:
		void changeContactStatus( QString jid, PresenceManager::PresenceStatus status, const QString &description );
		void disconnected();
	
	private:
		bool isExist( QString jid );
		
		int findContact( QString jid );
		
		QValueList<Contact> roster;
		
		friend class PresenceManager;
		friend class RosterBox;
	
	signals:
		void refreshContext();
};

extern RosterManager *roster_manager;

#endif
