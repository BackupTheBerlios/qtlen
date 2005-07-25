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

#ifndef CHAT_H
#define CHAT_H

#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qmainwindow.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qdatetime.h>
#include <qevent.h>

class QSplitter;
class QLabel;
class ChatViewer;

class ChatEdit : public QTextEdit
{
		Q_OBJECT
		
	public:
		ChatEdit( bool a = true, QWidget *parent = 0, const char *name = 0 );
		ChatEdit( QWidget *parent = 0, const char *name = 0 );
		~ChatEdit();
		
		void setAutoSend( bool a ) { autoSend = a; }
		bool getAutoSend() const { return autoSend; }
		
	protected:
		void keyPressEvent( QKeyEvent * e );
		
	private:
		bool autoSend;
		
	private slots:
		
	signals:
		void enterPressed();
		void keyPressed();
};

class Chat : public QMainWindow
{
		Q_OBJECT

	public:
		Chat( QString j, QWidget *parent = 0, const char *name = 0 );
		
		QString getJid();
		void setJid( QString j );
		
		void addMessage( const QString &msg, const QDateTime &datetime );
		void addSystemMessage( const QString &msg, const QDateTime &datetime );
		
		void typingNotification( bool start_typing );

	protected:
		void keyPressEvent( QKeyEvent * e );
		void closeEvent( QCloseEvent* ce );

	private:
		QSplitter *splitter;
		ChatViewer *message;
		QMainWindow *editBox;
		ChatEdit *edit;
		QLabel *typing_notification;
		QPushButton *autoSend;
		QString jid;
		QDateTime end_typing;

	private slots:
		void send();
		void setAutoSend();
		
		void sendSoundAlert();
		
		void doFlash( bool );
		
		void clean();
		
		void returnPressed();
		void keyPressed();
		
		void checkTyping();

};

#endif
