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

#ifndef TLEN_H
#define TLEN_H

#include <qstring.h>
#include <qcstring.h>
#include <qobject.h>
#include <qtimer.h>
#include <qdom.h>

class QSocket;

class Tlen : public QObject
{
		Q_OBJECT

	public:
		enum ConnectionState { ConnectingToHub = 0,Connecting = 1, Connected = 2, ErrorDisconnected = 3, Disconnected = 4 };
		
		static void initModule();
		Tlen();
		~Tlen();
		
		void connectToServer();
		void disconnect();
		bool tlenLogin();
		
		bool writeXml( const QDomDocument& doc );
		
		void setUserPass( const QString &username, const QString &password );
		
		bool isConnected();
		QString getJid();
		QString getSid();

	private:
		QString username, password, sid;
		
		ConnectionState state;
		
		QSocket *socket;
		QString hostname;
		Q_UINT16 hostport;
		
		QTimer *pingtimer, *errortimer;
		
		void event( QDomNode node );
		
		friend class PresenceManager;

	private slots:
		void sendPing();
		void tlenConnected();
		void socketConnected();
		void socketConnectionClosed();
		void socketReadyRead();
		void socketError( int e );
		
	signals:
		void connecting();
		void connected();
		void disconnected();
		
		void updateUserList();
};

extern Tlen* tlen_manager;

#endif
