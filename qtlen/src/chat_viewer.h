/***************************************************************************
 *   Copyright (C) 2004-2005 by Zwierzak                                   *
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

#ifndef CHAT_VIEWER_H
#define CHAT_VIEWER_H

#include <qtextbrowser.h>
#include <qstring.h>
#include <qdatetime.h>

class ChatStyle;

class ChatViewer : public QTextBrowser
{
		Q_OBJECT
	
	public:
		ChatViewer( QWidget* parent = 0, const char* name = 0 );
		~ChatViewer();
		
		enum Type { Contact = 0, Mine = 1, System = 2 };
		
		void addMessage( QString nick, const QString &message, const QDateTime &dt, const ChatViewer::Type &type );
		
	private:
		ChatStyle *style;
};

#endif
