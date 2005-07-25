/***************************************************************************
 *   Copyright (C) 2004-2005 by Zwierzak                                   *
 *   zwierzak2003@gmail.com                                                *
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
#include <qdom.h>
#include <qxml.h>

class QSocket;
class QTimer;

class QTlenParser : public QXmlDefaultHandler
{
	public:
		bool startDocument();
		bool startElement( const QString&, const QString&, const QString&, const QXmlAttributes& );
		bool endElement( const QString&, const QString&, const QString& );
		bool endDocument();
		
		bool skippedEntity( const QString& ) { return true; };
		bool characters( const QString& ) { return true; };
		
		bool startCDATA() { return true; };
		bool startDTD( const QString&, const QString&, const QString& ) { return true; };
		bool startEntity( const QString& ) { return true; };
		bool startPrefixMapping(const QString&, const QString& ) { return true; };
		
		bool endCDATA() { return true; };
		bool endDTD() { return true; };
		bool endEntity( const QString& ) { return true; };
		bool endPrefixMapping( const QString& ) { return true; };
		
	private:
		int elementsCount;
		bool stream;
};

class Paser
{
	public:
		Paser() {}
		
		void receiveNode( QDomNode* ) {}
};

class Tlen : public QObject
{
	Q_OBJECT
		
	public:
		enum ConnectionState { ConnectingToHub = 0,Connecting = 1, Connected = 2, ErrorDisconnected = 3, Disconnected = 4 };
		
		static void initModule();
		Tlen();
		~Tlen();
		
		bool writeXml( const QDomDocument& doc );
		
		void setUserPass( const QString &username, const QString &password );
		void setHost( const QString &name, int port );
		
		bool isConnected();
		
		QString getLogin();
		QString getJid();
		QString getSid();

	private:
		QString username, password, sid;
		
		ConnectionState state;
		
		QSocket *socket;
		QString hostname;
		Q_UINT16 hostport;
		
		QString stream;
		
		QTimer *pingtimer, *errortimer;
		
		void event( QDomNode node );
		bool tlenLogin();
		
		friend class PresenceManager;

	private slots:
		void beforeConnect();
		void connectToServer();
		void disconnect();
		
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
};

extern Tlen* tlen_manager;

#endif
