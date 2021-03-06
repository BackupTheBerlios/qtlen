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
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qsettings.h>
#include <qpixmap.h>
#include <qiconset.h>
#include <qpoint.h>
#include <qcursor.h>
#include <qrect.h>

#include "qtlen.h"
#include "tlen.h"
#include "message_manager.h"
//#include "history_manager.h"
#include "utils.h"
#include "settingsdialog.h"
#include "roster_manager.h"
#include "roster_box.h"
#include "presence_manager.h"
#include "pubdir_manager.h"
#include "sound_manager.h"
#include "editdlg.h"
#include "trayicon.h"

QTlen::QTlen( QWidget *parent, const char *name )
	: QMainWindow( parent, name ),
	  v_tray( false ),
	  v_quit( false )
{
	QSettings settings;
	settings.setPath( "qtlen.berlios.de", "QTlen" );
	
	settings.beginGroup( "/window" );
	
	setGeometry( settings.readNumEntry( "/main/xpos", 50 ),
		     settings.readNumEntry( "/main/ypos", 50 ),
		     settings.readNumEntry( "/main/width", 225 ),
		     settings.readNumEntry( "/main/height", 450 ) );
	
	settings.resetGroup();
	
	PresenceManager::initModule();
	Tlen::initModule();
	MessageManager::initModule();
	RosterManager::initModule();
	
	PubdirManager::initModule();
	
	SoundManager::initModule();
	
	setCaption( "QTlen: "+tlen_manager->getJid() );
	setIcon( takePixmap("offline") );
	
	settings.beginGroup( "/general" );
	
	roster = new RosterBox( this );
	
	setCentralWidget( roster );
	
	addToolBars();
	
	if( settings.readBoolEntry( "/trayicon/activated" ) )
		activeTrayIcon();
	else
		v_quit = true;
	
	settings.resetGroup();
	
	connect( presence_manager, SIGNAL( statusChanged( PresenceManager::PresenceStatus, const QString& ) ),
			SLOT( statusChanged( PresenceManager::PresenceStatus, const QString& ) ) );
	
	connect( tlen_manager, SIGNAL( connecting() ), SLOT( connecting() ) );
}

void QTlen::activeTrayIcon()
{
	if( !v_tray )
	{
		trayPopup = new QPopupMenu( this );
		//trayPopup->insertItem( tr("Receive event"), this, SLOT( showHide() ), 0, 0 );
		//trayPopup->setItemVisible( 0, false );
		//trayPopup->insertSeparator();
		trayPopup->insertItem( tr("Hide"), this, SLOT( showHide() ), 0, 1 );
		trayPopup->insertItem( tr("Set status"), statusPopup );
		trayPopup->insertSeparator();
		trayPopup->insertItem( QIconSet( takePixmap("exit") ), tr("Exit program"), this, SLOT( quit() ) );
		
		tray = new TrayIcon( takePixmap("offline"), "QTlen<br>" + tr( "Offline" ), trayPopup, this );
		
		updateTrayIcon();
		
		tray->show();
		
		v_tray = true;
		
		v_quit = false;
		
		connect( tray, SIGNAL( clicked( const QPoint&, int) ),
			this, SLOT( trayClicked( const QPoint&, int) ) );
		connect( tray, SIGNAL( doubleClicked( const QPoint& ) ),
			this, SLOT( trayDoubleClicked( const QPoint& ) ) );
		connect( tray, SIGNAL( closed() ),
			this, SLOT( trayClosed() ) );
	}
}

void QTlen::disactiveTrayIcon()
{
	if( v_tray )
	{
		tray->gotCloseEvent();
		
		if( trayPopup )
			delete trayPopup;
		if( tray )
			delete tray;
		
		v_quit = true;
		
		v_tray = false;
	}
}

void QTlen::updateTrayIcon()
{
	if( v_tray )
	{
		QSettings settings;
		settings.setPath( "qtlen.berlios.de", "QTlen" );
		
		settings.beginGroup( "/general" );
		
		tray->setIcon( takePixmap( statusToShort( presence_manager->getStatus() ) ) );
		
		tray->setToolTip( "QTlen<br>" + presence_manager->statusToString( presence_manager->getStatus() ) );
		
		if( presence_manager->getDescription() )
			tray->setToolTip( tray->toolTip() + "<br>" + plain2rich( presence_manager->getDescription() ) );
		
		tray->setWMDock( settings.readBoolEntry( "/trayicon/windowMakerTray" ) );
		tray->newTrayOwner();
		
		settings.resetGroup();
	}
}

QTlen::~QTlen()
{
	if( v_tray )
		disactiveTrayIcon();
	
	if( roster )
		delete roster;
	
	if( menuPopup )
		delete menuPopup;
	if( statusPopup )
		delete statusPopup;
	if( trayPopup )
		delete trayPopup;
	
	if( b_showAway )
		delete b_showAway;
	if( b_showOffline )
		delete b_showOffline;
	if( menuButton )
		delete menuButton;
	if( statusButton )
		delete statusButton;
	
	if( tlen_manager )
		delete tlen_manager;
	
	if( msg_manager )
		delete msg_manager;
	
}

void QTlen::keyPressEvent( QKeyEvent* ke )
{
	if( ke->key() == Qt::Key_Escape
#if defined( Q_WS_MAC )
	    || ( e->key() == Qt::Key_W && e->state() & Qt::ControlButton )
#endif
	  )
	{
		close();
	}
	
	QMainWindow::keyPressEvent( ke );
}

void QTlen::showEvent( QShowEvent* se )
{
	QMainWindow::showEvent( se );
	
	if( v_tray )
		trayPopup->changeItem( 1, tr( "Hide" ) );
}

void QTlen::hideEvent( QHideEvent* he )
{
	QMainWindow::hideEvent( he );
	
	if( v_tray )
		trayPopup->changeItem( 1, tr( "Show" ) );
}

void QTlen::closeEvent( QCloseEvent* ce )
{
	if( v_quit )
	{
		QSettings settings;
		settings.setPath( "qtlen.berlios.de", "QTlen" );
		
		settings.beginGroup( "/window" );
		
		if( isShown() )
		{
			settings.writeEntry( "/main/xpos", x() );
			settings.writeEntry( "/main/ypos", y() );
			settings.writeEntry( "/main/width", width() );
			settings.writeEntry( "/main/height", height() );
		}
		else
		{
			settings.writeEntry( "/main/xpos", mapToGlobal(QPoint(0,0)).x() );
			settings.writeEntry( "/main/ypos", mapToGlobal(QPoint(0,0)).y() );
			settings.writeEntry( "/main/width", frameGeometry().width() );
			settings.writeEntry( "/main/height", frameGeometry().height() );
		}
		
		settings.resetGroup();
		
		if(tlen_manager->isConnected())
			presence_manager->setStatus( PresenceManager::Unavailable, presence_manager->getDescription() );
		
		ce->accept();
	}
	else
	{
		ce->ignore();
		
		if( isHidden() )
			show();
		else
			hide();
	}
}

void QTlen::addToolBars()
{
	QToolBar *tools = new QToolBar ( this );
	tools->setMovingEnabled( false );
	addDockWindow( tools, tr("Tool Bar"), Qt::DockBottom, true );

	(void)new QToolButton( QIconSet( takePixmap( "add" ) ), tr( "Add new contact" ), QString::null, this, SLOT( addContact() ), tools );
	(void)new QToolButton( QIconSet( takePixmap( "find" ) ), tr( "Find friends" ), QString::null, this, SLOT( seekContact() ), tools );
	
	tools->addSeparator();
	
	b_mute = new QToolButton( QIconSet( takePixmap( "mute" ) ), tr( "Mute sounds" ), QString::null, this, SLOT( mute() ), tools );
	b_mute->setToggleButton( true );
	b_mute->setOn( sound_manager->isMute() );
	
	tools->addSeparator();
	
	b_showAway = new QToolButton( QIconSet( takePixmap( "away" ) ), tr( "Show away contacts" ), QString::null, this, SLOT( showAway() ), tools );
	b_showAway->setToggleButton( true );
	b_showAway->setOn( roster->isShowAway() );
	b_showOffline = new QToolButton( QIconSet( takePixmap( "offline" ) ), tr( "Show unavailable contacts" ), QString::null, this, SLOT( showOffline() ), tools );
	b_showOffline->setToggleButton( true );
	b_showOffline->setOn( roster->isShowOffline() );
	tools->addSeparator();
	(void)new QToolButton( QIconSet( takePixmap( "settings" ) ), tr( "Settings" ), QString::null, this, SLOT(settings()), tools );

	tools = new QToolBar ( this );
	tools->setMovingEnabled( false );
	addDockWindow( tools, tr("Menu Bar"), Qt::DockBottom, true );
	menuButton = new QToolButton( QIconSet( takePixmap( "menu" ) ), tr( "Menu" ), QString::null, this, SLOT(showMenuPopup()), tools );
	menuButton->setUsesBigPixmap( true );
	menuButton->setFixedSize( takePixmap( "menu" ).size() );

	tools = new QToolBar ( this );
	tools->setMovingEnabled( false );
	addDockWindow( tools, tr("Status Bar") );
	moveDockWindow( tools, Qt::DockBottom, false, 2, width() );
	statusButton = new QToolButton( QIconSet( takePixmap( "offline" ) ), tr( "Offline" ), QString::null, this, SLOT(showStatusPopup()), tools );

	menuPopup = new QPopupMenu( this, "Popumenu for QTlen button" );
	menuPopup->insertItem( QIconSet( takePixmap("add") ), tr( "Add new contact" ), this, SLOT( addContact() ) );
	menuPopup->insertItem( QIconSet(takePixmap("find")), tr("Find friends"), this, SLOT( seekContact() ) );
	menuPopup->insertSeparator();
	menuPopup->insertItem( QIconSet(takePixmap("info")), tr("Personal Information"), this, SLOT( showPersonalInformation() ) );
	menuPopup->insertSeparator();
	menuPopup->insertItem( QIconSet(takePixmap("msg")), tr("Message"), this, SLOT( newMessage() ) );
	menuPopup->insertItem( QIconSet(takePixmap("msg-chat")), tr("Chat"), this, SLOT( newChatMessage() ) );
	menuPopup->insertSeparator();
	menuPopup->insertItem( QIconSet(takePixmap("history")), tr("History"), 8/*, history_manager, SLOT( show() )*/ );
	menuPopup->insertItem( QIconSet(takePixmap("settings")), tr("Settings"), this, SLOT( settings() ) );
	menuPopup->insertSeparator();
	menuPopup->insertItem( QIconSet(takePixmap("close")), tr("Hide program"), this, SLOT( showHide() ) );
	menuPopup->insertItem( QIconSet(takePixmap("exit")), tr("Exit program"), this, SLOT( quit() ) );
	
	menuPopup->setItemEnabled( 8, false );
	
	statusPopup = new QPopupMenu( this, "Popumenu for status" );
	statusPopup->insertItem( QIconSet(takePixmap("online")), tr( "Available" ), 0 );
	statusPopup->insertItem( QIconSet(takePixmap("chat")), tr( "Free for Chat" ), 1 );
	statusPopup->insertItem( QIconSet(takePixmap("away")), tr( "Away" ), 2 );
	statusPopup->insertItem( QIconSet(takePixmap("xa")), tr( "Not Available" ), 3 );
	statusPopup->insertItem( QIconSet(takePixmap("dnd")), tr( "Do not Disturb" ), 4 );
	statusPopup->insertItem( QIconSet(takePixmap("invisible")), tr( "Invisible" ), 5 );
	statusPopup->insertItem( QIconSet(takePixmap("offline")), tr( "Unavailable" ), 6 );
	statusPopup->insertSeparator();
	statusPopup->insertItem( tr("Add description"), 7 );
	statusPopup->setCheckable( true );

	setDockMenuEnabled( false );

	connect( statusPopup, SIGNAL(activated(int)), this, SLOT(status(int)));
}

void QTlen::showHide()
{
	if( tray )
	{
		if( !isMinimized() && isActiveWindow() )
		{
			hide();
		}
		else
		{
			if( isShown() && !isMinimized() )
				setActiveWindow();
			else
				showNormal();
		}
	}
	else
	{
		close();
	}
}

void QTlen::quit()
{
	v_quit = true;
	close();
}

void QTlen::status(int id)
{
	PresenceManager::PresenceStatus status = presence_manager->getStatus();
	QString description = presence_manager->getDescription();
	
	if( id < 7 )
	{
		status = (PresenceManager::PresenceStatus)id;
	}
	else
	{
		bool ok;
		QString desc = QInputDialog::getText(
				"QTlen", tr("Status:"), QLineEdit::Normal,
		presence_manager->getDescription(), &ok, this );
		if(ok)
			description = desc;
		else
			return;
	}
	
	presence_manager->setStatus(status, description);
}

void QTlen::statusChanged( PresenceManager::PresenceStatus status, const QString &description )
{
	QSettings settings;
	settings.setPath( "qtlen.berlios.de", "QTlen" );
	
	settings.beginGroup( "/general" );
	
	if( v_tray )
	{
		tray->setIcon( takePixmap( statusToShort( status ) ) );
		tray->setToolTip( "QTlen<br>" + presence_manager->statusToString( status ) );
	}
	
	statusButton->setIconSet( QIconSet( takePixmap( statusToShort( status ) ) ) );
	statusButton->setTextLabel( presence_manager->statusToString( status ) );
	
	setIcon( takePixmap( statusToShort( status ) ) );
	
	if( !description.isEmpty() )
	{
		statusPopup->setItemChecked(7,true);
		statusButton->setTextLabel( statusButton->textLabel() + "<br>" + plain2rich( description ) );
		if( v_tray )
			tray->setToolTip( tray->toolTip() + "<br>" + plain2rich( description ) );
	}
	else
	{
		statusPopup->setItemChecked(7,false);
	}
	
	settings.resetGroup();
}

void QTlen::trayClicked( const QPoint&, int button )
{
	if( button == Qt::LeftButton )
	{
		showHide();
	}
}

void QTlen::trayDoubleClicked( const QPoint& )
{
	
}

void QTlen::trayClosed()
{
	v_tray = false;
}

void QTlen::connecting()
{
	statusButton->setTextLabel( tr( "Connecting..." ) );
}

void QTlen::settings()
{
	SettingsDialog::showDialog();
}

void QTlen::showMenuPopup()
{
	menuPopup->exec( QCursor::pos() );
}

void QTlen::showStatusPopup()
{
	statusPopup->exec( QCursor::pos() );
}

void QTlen::addContact()
{
	EditDlg *dlg = new EditDlg( QString::null, true );
	dlg->show();
}

void QTlen::seekContact()
{
	pubdir_manager->showDialog();
}

void QTlen::mute()
{
	sound_manager->setMute( !sound_manager->isMute() );
	b_mute->setOn( sound_manager->isMute() );
}

void QTlen::showAway()
{
	roster->setShowAway( !roster->isShowAway() );
	b_showAway->setOn( roster->isShowAway() );
}

void QTlen::showOffline()
{
	roster->setShowOffline( !roster->isShowOffline() );
	b_showOffline->setOn( roster->isShowOffline() );
}

void QTlen::showPersonalInformation()
{
	pubdir_manager->showPersonalInformation();
}

void QTlen::newMessage()
{
	bool ok;
	QString jid = QInputDialog::getText(
			"QTlen", tr("Enter Tlen Identifier:"), QLineEdit::Normal,
			QString::null, &ok, this );
	if(ok)
		msg_manager->newMessage( takeJid( jid ) );
}

void QTlen::newChatMessage()
{
	bool ok;
	QString jid = QInputDialog::getText(
			"QTlen", tr("Enter Tlen Identifier:"), QLineEdit::Normal,
			QString::null, &ok, this );
	if(ok)
		msg_manager->newChatMessage( takeJid( jid ) );
}
