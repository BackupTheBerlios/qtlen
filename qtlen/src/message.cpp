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
#include <qhbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qtranslator.h>
#include <qmessagebox.h>
#include <qiconset.h>
#include <qsettings.h>

#include "message.h"
#include "qtlen.h"
#include "tlen.h"
#include "utils.h"
#include "message_manager.h"
#include "roster_manager.h"

Message::Message( QString j, MType t, QWidget *parent, const char *name )
	: QWidget( parent, name )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/window/message" );
	
	resize( settings.readNumEntry( "/width", 400 ),
			settings.readNumEntry( "/height", 450 ) );
	
	settings.resetGroup();
	
	setIcon( QPixmap("msg.png") );
	
	setJid( takeJid( j ) );
	setMType( t );
	
	QBoxLayout *mainLayout = new QVBoxLayout( this, 0, -1, "Main layout for message widget" );
	
	QBoxLayout *topLayout = new QHBoxLayout( this );
	mainLayout->addLayout( topLayout );
	
	QLabel *label = new QLabel( mtype == Message::New ? tr("To:") : tr("From:"), this );
	topLayout->addWidget( label );
	
	QString contactName = roster_manager->getContactName( jid );
	destination = new QLineEdit( contactName.isEmpty() ? jid : contactName+" <"+jid+">", this );
	topLayout->addWidget( destination );
	if( mtype == Message::Revice )
		destination->setReadOnly( true );
	
	QString captionText = tr("Message ");
	captionText += mtype == Message::New ? tr("to: ") : tr("from: ");
	captionText += contactName.isEmpty() ? jid : contactName;
	setCaption( captionText );

	topLayout = new QHBoxLayout( this );
	mainLayout->addLayout( topLayout );
	
	if( mtype == Message::New )
	{
		label = new QLabel( tr("Send as:"), this );
		topLayout->addWidget( label );

		sendAs = new QComboBox( this );
		sendAs->insertItem( takePixmap( "msg" ), tr("Normal") );
		sendAs->insertItem( takePixmap( "msg-chat" ), tr("Chat") );
		sendAs->setCurrentItem( 0 );
		topLayout->addWidget( sendAs );
	}
	else if( mtype == Message::Revice )
	{
		QBoxLayout *datetimeLayout = new QHBoxLayout( this );
		mainLayout->addLayout( datetimeLayout );

		label = new QLabel( tr("Time:"), this );
		datetimeLayout->addWidget( label );

		datetimeEdit = new QLineEdit( this );
		datetimeEdit->setReadOnly( true );

		datetimeLayout->addWidget( datetimeEdit );
	}

	message = new QTextEdit( this );
	message->setTextFormat( Qt::PlainText );
	mainLayout->addWidget( message );
	if( mtype == Message::Revice )
		message->setReadOnly( true );

	QBoxLayout *buttomLayout = new QHBoxLayout( this );
	mainLayout->addLayout( buttomLayout );

	QPushButton *exitButton = new QPushButton( QIconSet( takePixmap( "close" ) ), tr("Exit"), this );
	exitButton->setFixedWidth( 100 );
	buttomLayout->addWidget( exitButton );

	buttomLayout->addStretch();

	if(destination->text().isEmpty())
		destination->setFocus();
	else
		message->setFocus();

	QPushButton *sendButton = new QPushButton( QIconSet( takePixmap( "send" ) ), mtype == Message::New ? tr("Send") : tr("Reply"), this );
	sendButton->setFixedWidth( 100 );
	buttomLayout->addWidget( sendButton );

	if( t == Message::New )
	connect( sendButton, SIGNAL( released() ),
			SLOT( send() ));
	else
	connect( sendButton, SIGNAL( released() ),
			SLOT( reply() ));

	connect( destination, SIGNAL( textChanged( const QString & ) ),
			SLOT( changeCaption( const QString & ) ) );

	connect( exitButton, SIGNAL( released() ),
			SLOT(close()) );
}

void Message::setJid( QString j )
{
	jid = j;
}

QString Message::getJid()
{
	return jid;
}

void Message::setMType( MType t )
{
	mtype = t;
}

Message::MType Message::getMType()
{
	return mtype;
}

void Message::addMessage( const QString &msg, const QDateTime &dt )
{
	message->setText( msg );
	datetimeEdit->setText( dt.toString( "dd.MM.yyyy hh:mm:ss" ) );
}

void Message::addSystemMessage( const QString &msg, const QDateTime &dt )
{
	message->setText( "<font color=\"#ff0000\" size=\"5\">" + tr( "System message" ) + "</font>" + msg );
	datetimeEdit->setText( dt.toString( "dd.MM.yyyy hh:mm:ss" ) );
}

void Message::keyPressEvent( QKeyEvent * e )
{
	if( e->key() == Key_Escape )
	{
		close();
	}
	
	QWidget::keyPressEvent( e );
}

void Message::closeEvent( QCloseEvent* ce )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/window/message" );
	
	settings.writeEntry( "/width", width() );
	settings.writeEntry( "/height", height() );
	
	settings.resetGroup();
	
	ce->accept();
	
	//msg_manager->deleteChatDialog( getJid() );
}

void Message::send()
{
	if( !destination->text().isEmpty() && !message->text().isEmpty() )
	{
		setEnabled( false );
		MessageManager::MessageType type = MessageManager::NormalMessage;
		if( sendAs->currentItem() != 0 )
			type = MessageManager::ChatMessage;
		
		if( msg_manager->sendMessage( takeJid( jid ), message->text(), type ) )
			close();
		else
			QMessageBox::warning( this, "QTlen", tr("Error when sending message,\n maby you aren't connect") );
	}
	else
	{
		QMessageBox::information( this, "QTlen",
		tr("Sorry but this message can't be sended") );
	}
	
	setEnabled( true );
}

void Message::reply()
{
	msg_manager->newMessage( destination->text() );
}

void Message::changeCaption( const QString &text )
{
	int home = text.find( "<" );
	int end = text.find( ">", home+1 );

	if( home != -1 && end != -1 )
		jid = text.mid( home, end - home );
	else
		jid = text;

	setCaption( tr("Message to: ") + text );
}
