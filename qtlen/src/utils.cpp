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

#include <qstring.h>
#include <qcstring.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qtextcodec.h>
#include <qurl.h>
#include <qregexp.h>
#include <stdlib.h>

#include "qtlen.h"
#include "utils.h"
#include "tlen.h"

// trzeba to jeszcze przerobic aby wchodzil do scierzki ../share/qtlen/icons,
// wszystko trzeba liczyc od sciezki gdze lerzy program
QPixmap takePixmap( const QString &name )
{
	return QPixmap( qApp->applicationDirPath() + "/../share/qtlen/icons/" + name + ".png" );
};

QString takeJid( QString jid )
{
	qDebug( "takeJid()" );
	
	int pos = jid.find("/");
	
	if( pos > 0 )
		jid = jid.left( pos - 1 );
	
	if( jid.right( 8 ) != "@tlen.pl" )
	{
		pos = jid.find("@");
		if( pos > 0 )
			jid = jid.left( pos );
		jid += "@tlen.pl";
	}
	
	return jid;
}

QString takeLogin( QString jid )
{
	qDebug( "takeLogin()" );
	
	int pos = jid.find("@");
	
	if( pos > 0 )
		jid = jid.left( pos );
	
	return jid;
}

QString statusToString( PresenceManager::PresenceStatus status )
{
	switch( status )
	{
		case PresenceManager::Available:
			return QT_TRANSLATE_NOOP( "QTlen", "Available" );
			break;
		case PresenceManager::Chat:
			return QT_TRANSLATE_NOOP( "QTlen", "Free for Chat" );
			break;
		case PresenceManager::Away:
			return QT_TRANSLATE_NOOP( "QTlen", "Away" );
			break;
		case PresenceManager::ExtAway:
			return QT_TRANSLATE_NOOP( "QTlen", "Not Available" );
			break;
		case PresenceManager::Dnd:
			return QT_TRANSLATE_NOOP( "QTlen", "Do not Disturb" );
			break;
		case PresenceManager::Invisible:
			return QT_TRANSLATE_NOOP( "QTlen", "Invisible" );
			break;
		case PresenceManager::Unavailable:
			return QT_TRANSLATE_NOOP( "QTlen", "Unavailable" );
			break;
	}
	
	return QT_TRANSLATE_NOOP( "QTlen", "Unavailable" );
}

QString statusToShort( PresenceManager::PresenceStatus status )
{
	switch( status )
	{
		case PresenceManager::Available:
			return "online";
			break;
		case PresenceManager::Chat:
			return "chat";
			break;
		case PresenceManager::Away:
			return "away";
			break;
		case PresenceManager::ExtAway:
			return "xa";
			break;
		case PresenceManager::Dnd:
			return "dnd";
			break;
		case PresenceManager::Invisible:
			return "invisible";
			break;
		case PresenceManager::Unavailable:
			return "offline";
			break;
	}
	
	return "offline";
}

// w³asny encode który nie tylko zmienia tekst na "nieczytelny" ale tak¿e
// zmienia strony kodowe tekstu
QCString encode( const QString &in )
{
	QTextCodec *codec = QTextCodec::codecForName("ISO8859-2");
	QCString o = codec->fromUnicode( in );

	QCString out;

	QRegExp ex("[^A-Za-z0-9\\.\\-\\_]");

	for(uint i = 0; i < o.length(); ++i)
	{
		if( o.mid(i,1) == " " )
			out += "+";
		else if( ex.exactMatch( o.mid(i,1) ) )
		{
			//QCString d = o.mid(i,1);
			//d.sprintf( "%%%02x", o.mid(i,1).data());
			//out += d;
			uchar inCh = (uchar)o[ i ];

			out += "%";

			ushort c = inCh / 16;
			c += c > 9 ? 'A' - 10 : '0';
			out += c;

			c = inCh % 16;
			c += c > 9 ? 'A' - 10 : '0';
			out += c;
		}
		else
			out += o.mid(i,1);
	}

	return out;
}

QString decode( const QCString &in )
{
	QCString o;
	QString out;

	QRegExp ex("\\%[0-9A-Fa-f][0-9A-Fa-f]");

	for(uint i = 0; i < in.length(); ++i)
	{
		if( in.mid(i,1) == "+" )
			o += " ";
		else if( ex.exactMatch(in.mid(i,3)) )
		{
			bool ok;
			int code = QString(in.mid(i+1,2)).toInt( &ok, 16 );

			if(ok)
				o += (char)code;
			i+=2;
		}
		else
			o += in.mid(i,1);
	}

	QTextCodec *codec = QTextCodec::codecForName("ISO8859-2");
	out = codec->toUnicode( o );

	return out;
}

QString plain2rich(const QString &plain)
{
	QString rich;
	int col = 0;

	for(int i = 0; i < (int)plain.length(); ++i) {
		if(plain[i] == '\n') {
			rich += "<br>";
			col = 0;
		}
		else if(plain[i] == '\t') {
			rich += QChar::nbsp;
			while(col % 4) {
				rich += QChar::nbsp;
				++col;
			}
		}
		else if(plain[i].isSpace()) {
			if(i > 0 && plain[i-1] == ' ')
				rich += QChar::nbsp;
			else
				rich += ' ';
		}
		else if(plain[i] == '<')
			rich += "&lt;";
		else if(plain[i] == '>')
			rich += "&gt;";
		else if(plain[i] == '\"')
			rich += "&quot;";
		else if(plain[i] == '\'')
			rich += "&apos;";
		else if(plain[i] == '&')
			rich += "&amp;";
		else
			rich += plain[i];
		++col;
	}

	return rich;
}

QString rich2plain(const QString &in)
{
	QString out;

	for(int i = 0; i < (int)in.length(); ++i) {
		// tag?
		if(in[i] == '<') {
			// find end of tag
			++i;
			int n = in.find('>', i);
			if(n == -1)
				break;
			QString str = in.mid(i, (n-i));
			i = n;

			QString tagName;
			n = str.find(' ');
			if(n != -1)
				tagName = str.mid(0, n);
			else
				tagName = str;

			if(tagName == "br")
				out += '\n';
		}
		// entity?
		else if(in[i] == '&') {
			// find a semicolon
			int n = in.find(';', i);
			if(n == -1)
				break;
			QString type = in.mid(i, (n-i));

			if(type == "amp")
				out += '&';
			else if(type == "lt")
				out += '<';
			else if(type == "gt")
				out += '>';
			else if(type == "quot")
				out += '\"';
			else if(type == "apos")
				out += '\'';
			else
			{
				out += '&';
				break;
			}

			i = n+1; // should be n+1, but we'll let the loop increment do it
		}
		else if(in[i].isSpace()) {
			if(in[i] == QChar::nbsp)
				out += ' ';
			else if(in[i] != '\n') {
				if(i == 0)
					out += ' ';
				else {
					QChar last = out.at(out.length()-1);
					bool ok = TRUE;
					if(last.isSpace() && last != '\n')
						ok = FALSE;
					if(ok)
						out += ' ';
				}
			}
		}
		else {
			out += in[i];
		}
	}

	return out;
}

