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

#include <qlayout.h>
#include <qsettings.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qiconset.h>
#include <qcolor.h>
#include <qfont.h>
#include <qsplitter.h>
#include <qtextbrowser.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qdatetime.h>
#include <qtimer.h>

#include "chat.h"
#include "chat_viewer.h"
#include "tlen.h"
#include "utils.h"
#include "message_manager.h"
#include "roster_manager.h"

#if defined( Q_WS_WIN )
#include <windows.h>
#elif defined ( Q_WS_X11 )
#include<X11/Xutil.h>
#endif

ChatEdit::ChatEdit( bool a, QWidget *parent, const char *name )
	: QTextEdit( parent, name )
{
	setAutoSend( a );
}

ChatEdit::ChatEdit( QWidget *parent, const char *name )
	: QTextEdit( parent, name )
{
	setAutoSend( true );
}

ChatEdit::~ChatEdit()
{

}

void ChatEdit::keyPressEvent( QKeyEvent * e )
{
	if( autoSend && ( e->key() == Key_Return || e->key() == Key_Enter ) && e->state() != ShiftButton )
	{
		emit enterPressed();
	}
	else
	{
		emit keyPressed();
		QTextEdit::keyPressEvent( e );
	}
}

Chat::Chat( QString j, QWidget *parent, const char *name )
	: QMainWindow( parent, name )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/window/chat" );
	
	resize( settings.readNumEntry( "/width", 400 ),
			settings.readNumEntry( "/height", 450 ) );
	
	settings.resetGroup();
	
	setJid( takeJid( j ) );
	QString contactName = roster_manager->getContactName( jid );
	
	QString captionText = tr("Chat: ");
	captionText += contactName.isEmpty() ? jid : contactName;
	setCaption( captionText );
	
	setDockMenuEnabled( false );
	
	splitter = new QSplitter( Qt::Vertical, this );
	setCentralWidget( splitter );
	
	settings.beginGroup( "/look" );
	
	message = new ChatViewer( splitter );
	message->setWrapPolicy( QTextEdit::AtWordOrDocumentBoundary );
	message->setPaletteBackgroundColor( (QColor)settings.readEntry( "/bgColor", "#ffffff" ) );
	message->setPaletteForegroundColor( (QColor)settings.readEntry( "/chatFontColor", "#ffffff" ) );
	QFont font = this->font();
	font.fromString( settings.readEntry( "/chatFont" ) );
	message->setFont( font );
	message->setMinimumHeight( 100 );
	
	editBox = new QMainWindow( splitter, 0, 0 );
	editBox->setDockMenuEnabled( false );
	
	edit = new ChatEdit( editBox );	
	edit->setTextFormat( Qt::PlainText );
	connect( edit, SIGNAL( enterPressed() ), SLOT( returnPressed() ) );
	connect( edit, SIGNAL( keyPressed() ), SLOT( keyPressed() ) );
	edit->setPaletteBackgroundColor( (QColor)settings.readEntry( "/bgColor", "#ffffff" ) );
	edit->setPaletteForegroundColor( (QColor)settings.readEntry( "/chatFontColor", "#ffffff" ) );
	edit->setFont( font );
	
	splitter->setFocusProxy( edit );
	message->setFocusProxy( edit );
	
	settings.resetGroup();
	
	editBox->setCentralWidget( edit );
	
	settings.beginGroup( "/window/chat" );
	
	QValueList<int> sizes;
	QString s;
	
	for( int i = 0; i < settings.readNumEntry( "/size/count" ); i++ )
	{
		sizes.append( settings.readNumEntry( "/size/" + s.setNum( i ) ) );
	}
	
	settings.resetGroup();
	
	splitter->setSizes( sizes );
	
	QToolBar *tools = new QToolBar( editBox );
	tools->setMovingEnabled( false );
	editBox->addDockWindow( tools, tr("Typing-Notification Buttons"), Qt::DockTop, true );
	typing_notification = new QLabel( tools, "Typing notification" );
	typing_notification->setPixmap( takePixmap( "notify_off" ) );
	typing_notification->setMargin( 5 );
	(void)new QToolButton ( QIconSet( takePixmap( "sound_alert" ) ), tr("Send sound alert"), "", this, SLOT( sendSoundAlert() ), tools );
	
	tools = new QToolBar( editBox );
	tools->setMovingEnabled( false );
	editBox->addDockWindow( tools, tr("All main buttons"), Qt::DockBottom, true );
	
	QPushButton *send = new QPushButton( QIconSet(takePixmap("send")), tr("Send"), tools );
	send->setFixedWidth( 100 );
	connect( send, SIGNAL( released() ), SLOT( send() ) );
	
	settings.beginGroup( "/general" );
	
	autoSend = new QPushButton( QIconSet(takePixmap("enter")), QString::null, tools );
	autoSend->setFixedSize( send->sizeHint().height(), send->sizeHint().height() );
	autoSend->setToggleButton( true );
	autoSend->setOn( settings.readBoolEntry( "/returnSend", true ) );
	connect( autoSend, SIGNAL( released() ), SLOT( setAutoSend() ) );
	
	tools->addSeparator();
	
	QPushButton *clean = new QPushButton( QIconSet(takePixmap("clear")), QString::null, tools );
	clean->setFixedSize( send->sizeHint().height(), send->sizeHint().height() );
	connect( clean, SIGNAL( released() ), SLOT( clean() ) );
	
	setIcon( takePixmap("msg-chat") );
	
	QTimer *timer = new QTimer( this );
	connect( timer, SIGNAL( timeout() ), SLOT( checkTyping() ) );
	timer->start( 1000 );
}

void Chat::keyPressEvent( QKeyEvent * e )
{
	if( e->key() == Key_Escape )
	{
		close();
	}
	
	QMainWindow::keyPressEvent( e );
}

void Chat::closeEvent( QCloseEvent* ce )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/window/chat" );
	
	settings.writeEntry( "/width", width() );
	settings.writeEntry( "/height", height() );
	
	QValueList<int> sizes = splitter->sizes();
	
	settings.writeEntry( "/size/count", (int)sizes.count() );
	
	for( int i = 0; i < (int)sizes.count(); i++ )
	{
		settings.writeEntry( "/size/" + QString::number( i ), sizes[ i ] );
	}
	
	settings.endGroup();
	
	ce->accept();
	
	msg_manager->deleteChatDialog( getJid() );
}

QString Chat::getJid()
{
	return jid;
}

void Chat::setJid( QString j )
{
	jid = j;
	setCaption( tr("Chat: ") + jid );
}

void Chat::addMessage( const QString &msg, const QDateTime &datetime )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/look" );
	
	QString nick = roster_manager->getContactName( jid );
	if( nick.isEmpty() )
		nick = jid;
	
	message->append( "<font color='"+settings.readEntry( "/contactFontColor", "#0000ff" )+"'>[" + datetime.toString( "hh:mm:ss" ) + "] &lt;" + nick + "&gt;</font> " + plain2rich( msg ) );
	
	typingNotification( false );
	
	doFlash( true );
	
	settings.endGroup();
}

void Chat::addSystemMessage( const QString &msg, const QDateTime &datetime )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/look" );
	
	QString nick = roster_manager->getContactName( jid );
	if( nick.isEmpty() )
		nick = jid;
	
	message->append( "<font color='"+settings.readEntry( "/contactFontColor", "#0000ff" )+"'>[" + datetime.toString( "hh:mm:ss" ) + "] &lt;" + tr("System Message") + "&gt;</font> " + plain2rich( msg ) );
	
	doFlash( true );
	
	settings.endGroup();
}

void Chat::typingNotification( bool start_typing )
{
	if( start_typing && end_typing == QDateTime() )
	{
		typing_notification->setPixmap( takePixmap( "notify_on" ) );
	}
	else
	{
		typing_notification->setPixmap( takePixmap( "notify_off" ) );
		end_typing = QDateTime();
	}
}

void Chat::send()
{
	if( !edit->text().isEmpty() )
	{
		if( msg_manager->sendMessage( jid, edit->text(), MessageManager::ChatMessage ) )
		{
			QSettings settings;
			settings.setPath( "qtlen.sf.net", "QTlen" );
			settings.beginGroup( "/connection" );
			
			QString nick = settings.readEntry( "/nick", tr("Me") );
			
			settings.endGroup();
			settings.beginGroup( "/look" );
			
			message->append( "<font color='"+ settings.readEntry( "/myFontColor", "#ff0000" ) +"'>[" + QDateTime::currentDateTime( Qt::LocalTime ).toString( "hh:mm:ss" ) + "] &lt;"+ nick +"&gt;</font> " + plain2rich( edit->text() ) );
			
			edit->clear();
			
			settings.endGroup();
			
			end_typing = QDateTime();
		}
		else
			QMessageBox::warning( this, "QTlen", tr("Error when sending message,\n maby you aren't connect") );
	}
	else
	{
		QMessageBox::information( this, "QTlen", tr("Sorry but this message can't be sended") );
	}
}

void Chat::setAutoSend()
{
	edit->setAutoSend( autoSend->state() == QButton::On );
}

void Chat::sendSoundAlert()
{
	if( msg_manager->sendSoundAlert( getJid() ) )
	{
		QString message = tr( "You send sound alert to this user" );
		addSystemMessage( message, QDateTime::currentDateTime( Qt::LocalTime ) );
	}
}

void Chat::clean()
{
	if( QMessageBox::information ( this, "QTlen", tr("Do you realy wont to delete messages in chat window?"), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
	{
		message->setText( QString::null );
	}
}

#if defined( Q_WS_WIN )
void Chat::doFlash( bool yes )
{
	if(yes) {
		flashAnimate(); // kick the first one immediately
	}
}
#elif defined( Q_WS_X11 )
void Chat::doFlash( bool yes )
{
	Display *xdisplay = qt_xdisplay();
	Window rootwin = qt_xrootwin();
	
	static Atom demandsAttention = XInternAtom( xdisplay, "_NET_WM_STATE_DEMANDS_ATTENTION", true );
	static Atom wmState = XInternAtom( xdisplay, "_NET_WM_STATE", true );
	
	XEvent e;
	e.xclient.type = ClientMessage;
	e.xclient.message_type = wmState;
	e.xclient.display = xdisplay;
	e.xclient.window = winId();
	e.xclient.format = 32;
	e.xclient.data.l[1] = demandsAttention;
	e.xclient.data.l[2] = 0l;
	e.xclient.data.l[3] = 0l;
	e.xclient.data.l[4] = 0l;
	
	if( yes )
	{
		e.xclient.data.l[0] = 1;
	}
	else
	{
		e.xclient.data.l[0] = 0;
	}
	XSendEvent(xdisplay, rootwin, False, (SubstructureRedirectMask | SubstructureNotifyMask), &e);
}
#else
void Chat::doFlash( bool )
{
}
#endif

void Chat::returnPressed()
{
	if( autoSend->state() == QButton::On )
		send();
}

void Chat::keyPressed()
{
	if( end_typing == QDateTime() )
		msg_manager->sendTypingNotification( getJid(), true );
	end_typing = QDateTime::currentDateTime( Qt::LocalTime ).addSecs( 30 );
}

void Chat::checkTyping()
{
	if( end_typing != QDateTime() && end_typing <= QDateTime::currentDateTime( Qt::LocalTime ) )
	{
		msg_manager->sendTypingNotification( getJid(), false );
		end_typing = QDateTime();
	}
}

//void Chat::link( const QString &link )
//{
//
//}
