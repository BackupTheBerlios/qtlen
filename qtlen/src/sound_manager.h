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

#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <qobject.h>
#include <qstring.h>
#include <qdom.h>

#include "tlen.h"
#include "presence_manager.h"

class QDateTime;

class SoundManager : public QObject
{
		Q_OBJECT
	
	public:
		static void initModule();
		SoundManager();
		~SoundManager();
		
		void update();
		
		void play( const QString &fileName );
		
		void setMute( bool mute );
		bool isMute();
		
	private:
		bool v_mute;
		
	private slots:
		void gotMessage( QString, const QString&, const QDateTime& );
		void gotChatMessage( QString, const QString&, const QDateTime& );
		
		void gotErrorMessage( QString, int, const QString&, const QString&, const QDateTime& );
		
		void gotNewsMessage( const QString&, const QString&, const QDateTime&, const QString&, const QString& );
		
		void gotSoundAlert( QString );
		
		void gotEmail( QString, const QString& );
		void gotWebMessage( QString, const QString&, const QString&, const QString& );
		
		void subscribe( QString );
		void unsubscribe( QString );
		
		void changeContactStatus( QString, PresenceManager::PresenceStatus status, const QString& );
};

extern SoundManager *sound_manager;

#endif
