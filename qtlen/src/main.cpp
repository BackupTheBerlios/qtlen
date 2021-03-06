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
#include <qtranslator.h>
#include <qsettings.h>
#include <qtextcodec.h>

#include "qtlen.h"

int main( int argc, char **argv )
{
	QApplication app( argc, argv );
	
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	settings.beginGroup( "/general" );
	
	QTranslator qtlen_tr( 0 );
	qtlen_tr.load( app.applicationDirPath() + "/../share/qtlen/translations/qtlen_" + settings.readEntry( "/language", QTextCodec::locale() ) , "." );
	app.installTranslator( &qtlen_tr );
	
	QTranslator qt_tr( 0 );
	qt_tr.load( app.applicationDirPath() + "/../share/qtlen/translations/qt_" + settings.readEntry( "/language", QTextCodec::locale() ) , "." );
	app.installTranslator( &qt_tr );
	
	QTlen *tlen = new QTlen( 0, "Main widget" );
	
	app.setMainWidget( tlen );
	
	if( !settings.readBoolEntry( "/trayicon/hideOnStart" ) )
		tlen->show();
	
	settings.endGroup();
	
	return app.exec();
}
