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
#include <qsettings.h>
#include <qprocess.h>
#include <qsound.h>

#include "sound_manager.h"
#include "message_manager.h"
#include "presence_manager.h"

SoundManager *sound_manager = NULL;

void SoundManager::initModule()
{
	sound_manager = new SoundManager();
}

SoundManager::SoundManager()
	: QObject()
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	v_mute = settings.readBoolEntry( "/mute" );
	
	update();
	
	settings.resetGroup();
}

SoundManager::~SoundManager()
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	settings.writeEntry( "/mute", v_mute );
	
	settings.resetGroup();
}

void SoundManager::update()
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	if( settings.readBoolEntry( "/message/activated" ) )
		connect( msg_manager, SIGNAL( gotMessage( QString, const QString&, const QDateTime& ) ),
			this, SLOT( gotMessage( QString, const QString&, const QDateTime& ) ) );
	else
		disconnect( msg_manager, SIGNAL( gotMessage( QString, const QString&, const QDateTime& ) ),
			this, SLOT( gotMessage( QString, const QString&, const QDateTime& ) ) );
	
	if( settings.readBoolEntry( "/chatMessage/activated" ) )
		connect( msg_manager, SIGNAL( gotChatMessage( QString, const QString&, const QDateTime& ) ),
			this, SLOT( gotChatMessage( QString, const QString&, const QDateTime& ) ) );
	else
		disconnect( msg_manager, SIGNAL( gotChatMessage( QString, const QString&, const QDateTime& ) ),
			this, SLOT( gotChatMessage( QString, const QString&, const QDateTime& ) ) );
	
	if( settings.readBoolEntry( "/errorMessage/activated" ) )
		connect( msg_manager, SIGNAL( gotErrorMessage( QString, int, const QString&, const QString&, const QDateTime& ) ),
			this, SLOT( gotErrorMessage( QString, int, const QString&, const QString&, const QDateTime& ) ) );
	else
		disconnect( msg_manager, SIGNAL( gotErrorMessage( QString, int, const QString&, const QString&, const QDateTime& ) ),
			this, SLOT( gotErrorMessage( QString, int, const QString&, const QString&, const QDateTime& ) ) );
	
	if( settings.readBoolEntry( "/newsMessage/activated" ) )
		connect( msg_manager, SIGNAL( gotChatMessage( QString, const QString&, const QDateTime& ) ),
			this, SLOT( gotNewsMessage( const QString&, const QString&, const QDateTime&, const QString&, const QString& ) ) );
	else
		disconnect( msg_manager, SIGNAL( gotChatMessage( QString, const QString&, const QDateTime& ) ),
			this, SLOT( gotNewsMessage( const QString&, const QString&, const QDateTime&, const QString&, const QString& ) ) );
	
	if( settings.readBoolEntry( "/soundAlert/activated" ) )
		connect( msg_manager, SIGNAL( gotSoundAlert( QString ) ),
			this, SLOT( gotSoundAlert( QString ) ) );
	else
		disconnect( msg_manager, SIGNAL( gotSoundAlert( QString ) ),
			this, SLOT( gotSoundAlert( QString ) ) );
	
	if( settings.readBoolEntry( "/email/activated" ) )
		connect( msg_manager, SIGNAL( gotEmail( QString, const QString& ) ),
			this, SLOT( gotEmail( QString, const QString& ) ) );
	else
		disconnect( msg_manager, SIGNAL( gotEmail( QString, const QString& ) ),
			this, SLOT( gotEmail( QString, const QString& ) ) );
	
	if( settings.readBoolEntry( "/webMessage/activated" ) )
		connect( msg_manager, SIGNAL( gotWebMessage( QString, const QString&, const QString&, const QString& ) ),
			this, SLOT( gotWebMessage( QString, const QString&, const QString&, const QString& ) ) );
	else
		disconnect( msg_manager, SIGNAL( gotWebMessage( QString, const QString&, const QString&, const QString& ) ),
			this, SLOT( gotWebMessage( QString, const QString&, const QString&, const QString& ) ) );
	
	if( settings.readBoolEntry( "/subscribe/activated" ) )
		connect( presence_manager, SIGNAL( subscribe( QString ) ),
			this, SLOT( subscribe( QString ) ) );
	else
		disconnect( presence_manager, SIGNAL( subscribe( QString ) ),
			this, SLOT( subscribe( QString ) ) );
	
	if( settings.readBoolEntry( "/unsubscribe/activated" ) )
		connect( presence_manager, SIGNAL( unsubscribe( QString ) ),
			this, SLOT( unsubscribe( QString ) ) );
	else
		disconnect( presence_manager, SIGNAL( unsubscribe( QString ) ),
			this, SLOT( unsubscribe( QString ) ) );
	
	if( settings.readBoolEntry( "/status/available/activated" ) || settings.readBoolEntry( "/status/away/activated" ) || settings.readBoolEntry( "/status/unavailable/activated" ) )
		connect( presence_manager, SIGNAL( changeContactStatus( QString, PresenceManager::PresenceStatus, const QString& ) ),
			this, SLOT( changeContactStatus( QString, PresenceManager::PresenceStatus, const QString& ) ) );
	else
		disconnect( presence_manager, SIGNAL( changeContactStatus( QString, PresenceManager::PresenceStatus, const QString& ) ),
			this, SLOT( changeContactStatus( QString, PresenceManager::PresenceStatus, const QString& ) ) );
	
	settings.resetGroup();
}

void SoundManager::play( const QString &fileName )
{
	qDebug( "Play: " + fileName );
	if( !v_mute )
	{
#ifdef Q_OS_WIN
		QSound::play( fileName );
#else
		QSettings settings;
		settings.setPath( "qtlen.sf.net", "QTlen" );
		
		settings.beginGroup( "/sounds" );
		
		QProcess *proc = new QProcess( this );
		
		proc->setWorkingDirectory( qApp->applicationDirPath() );
		
		proc->addArgument( settings.readEntry( "/player", "play" ) );
		proc->addArgument( fileName );
		
		proc->start();
		
		settings.resetGroup();
#endif
	}
}

void SoundManager::setMute( bool mute )
{
	v_mute = mute;
}

bool SoundManager::isMute()
{
	return v_mute;
}

void SoundManager::gotMessage( QString, const QString&, const QDateTime& )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	play( settings.readEntry( "/message/fileName" ) );
	
	settings.resetGroup();
}

void SoundManager::gotChatMessage( QString, const QString&, const QDateTime& )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	play( settings.readEntry( "/chatMessage/fileName" ) );
	
	settings.resetGroup();
}

void SoundManager::gotErrorMessage( QString, int, const QString&, const QString&, const QDateTime& )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	play( settings.readEntry( "/errorMessage/fileName" ) );
	
	settings.resetGroup();
}

void SoundManager::gotNewsMessage( const QString&, const QString&, const QDateTime&, const QString&, const QString& )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	play( settings.readEntry( "/newsMessage/fileName" ) );
	
	settings.resetGroup();
}

void SoundManager::gotSoundAlert( QString )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	play( settings.readEntry( "/soundAlert/fileName" ) );
	
	settings.resetGroup();
}

void SoundManager::gotEmail( QString, const QString& )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	play( settings.readEntry( "/email/fileName" ) );
	
	settings.resetGroup();
}

void SoundManager::gotWebMessage( QString, const QString&, const QString&, const QString& )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	play( settings.readEntry( "/webMessage/fileName" ) );
	
	settings.resetGroup();
}

void SoundManager::subscribe( QString )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	play( settings.readEntry( "/subscribe/fileName" ) );
	
	settings.resetGroup();
}

void SoundManager::unsubscribe( QString )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	play( settings.readEntry( "/unsubscribe/fileName" ) );
	
	settings.resetGroup();
}

void SoundManager::changeContactStatus( QString, PresenceManager::PresenceStatus status, const QString& )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );

	settings.beginGroup( "/sounds" );
	
	if( status == PresenceManager::Unavailable && settings.readBoolEntry( "/status/unavailable/activated" ) )
	{
		play( settings.readEntry( "/status/unavailable/fileName" ) );
	}
	
	if( status == PresenceManager::Available && settings.readBoolEntry( "/status/available/activated" ) )
	{
		play( settings.readEntry( "/status/available/fileName" ) );
	}
	
	if( status != PresenceManager::Unavailable && status != PresenceManager::Available &&  settings.readBoolEntry( "/status/away/activated" ) )
	{
		play( settings.readEntry( "/status/away/fileName" ) );
	}
	
	settings.resetGroup();
}
