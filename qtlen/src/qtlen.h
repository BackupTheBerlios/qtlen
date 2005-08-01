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

#ifndef QTLEN_H
#define QTLEN_H

#include <qwidget.h>
#include <qmainwindow.h>
#include <qstring.h>

#include "presence_manager.h"

class RosterBox;
class TrayIcon;
class QPopupMenu;
class QToolButton;

class QTlen : public QMainWindow
{
	Q_OBJECT

	public:
		QTlen( QWidget *parent=0, const char *name=0 );
		~QTlen();
		
		void activeTrayIcon();
		void disactiveTrayIcon();
		void updateTrayIcon();
		
	protected:
		void keyPressEvent( QKeyEvent* );
		
		void showEvent( QShowEvent* );
		void hideEvent( QHideEvent* );
		void closeEvent( QCloseEvent* );
		
	private:
		void addToolBars();
		
		RosterBox *roster;
		TrayIcon *tray;
		QPopupMenu *menuPopup, *statusPopup, *trayPopup;
		QToolButton *b_mute, *b_showAway, *b_showOffline, *menuButton, *statusButton;
		bool v_tray, v_quit;
		
	private slots:
		void showHide();
		void quit();
		
		void status( int id );
		void statusChanged( PresenceManager::PresenceStatus status, const QString &description );
		
		void trayClicked( const QPoint&, int);
		void trayDoubleClicked( const QPoint& );
		void trayClosed();
		
		void connecting();
		
		void settings();
		
		void showMenuPopup();
		void showStatusPopup();
		
		void addContact();
		void seekContact();
		
		void mute();
		
		void showAway();
		void showOffline();
		
		void showPersonalInformation();
		
		void newMessage();
		void newChatMessage();
};

#endif
