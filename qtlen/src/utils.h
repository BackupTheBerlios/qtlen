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

#ifndef UTILS_H
#define UTILS_H

#include <qobject.h>
#include <qstring.h>
#include <qcstring.h>
#include <qpixmap.h>
#include <qtextcodec.h>

#include "presence_manager.h"

extern QPixmap takePixmap( const QString &name );
extern QString takeJid( QString jid );
extern QString takeLogin( QString jid );

extern QString statusToString( PresenceManager::PresenceStatus status );
extern QString statusToShort( PresenceManager::PresenceStatus status );

extern QCString encode( const QString &in );
extern QString decode( const QCString &in );

extern QString plain2rich( const QString &plain );
extern QString rich2plain( const QString &in );

#endif
