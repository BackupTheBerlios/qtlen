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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qdatetime.h>
#include <qcombobox.h>
#include <qstring.h>

class Message : public QWidget
{
		Q_OBJECT
		
	public:
		enum MType { New = 0, Revice = 1 };
		
		Message( QString j, MType t, QWidget *parent = 0, const char *name = 0 );
		void setJid( QString jid );
		QString getJid();
		void setMType( MType t );
		MType getMType();
		void addMessage( const QString &msg, const QDateTime &dt );
		void addSystemMessage( const QString &msg, const QDateTime &dt );
		
	protected:
		void keyPressEvent( QKeyEvent * e );
		void closeEvent( QCloseEvent* ce );
		
	private:
		QLineEdit *destination;
		QComboBox *sendAs;
		QLineEdit *datetimeEdit;
		QTextEdit *message;
		QString jid;
		MType mtype;
		
	private slots:
		void send();
		void reply();
		void changeCaption( const QString & );
};

#endif
