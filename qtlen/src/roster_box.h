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

#ifndef CONTACTS_LIST_H
#define CONTACTS_LIST_H

#include <qlistview.h>
#include <qtooltip.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qpopupmenu.h>

#include "roster_manager.h"
#include "presence_manager.h"

class QSimpleRichText;


// Jest to edycja RichListItem produkcji Skaziego na moj± w³asn± potrzeba zaadaptowane do QTlena
class RosterBoxItem : public QListViewItem  
{
		int v_widthUsed;
		bool v_selected, v_active;
		QSimpleRichText* v_rt;
		
	protected:
		virtual void paintCell( QPainter * p, const QColorGroup & cg, int column, int width, int align );	
		virtual QString expandTemplate( int column = 0 );
		
	public:
		RosterBoxItem( QListView * parent );
		RosterBoxItem( QListViewItem * parent );
		
		RosterBoxItem( QString jid, QString type = "item", QListView * parent = 0 );
		RosterBoxItem( QString jid, QString type = "item", QListViewItem * parent = 0 );
		
		virtual void setup();
		
		virtual ~RosterBoxItem();
		
		int widthUsed();
};

class RosterBox : public QListView, QToolTip
{
		Q_OBJECT
	
	public:
		RosterBox( QWidget *parent = 0, const char *name = 0 );
		~RosterBox();
		
		bool isShowOffline() const { return v_showOffline; }
		bool isShowAway() const { return v_showAway; }
		
	public slots:
		void setShowOffline( bool );
		void setShowAway( bool );
		
	protected:
		void maybeTip( const QPoint &pos );
		
	private slots:
		void clicked( QListViewItem *item );
		void doubleClicked( QListViewItem *item, const QPoint &, int );
		void contextMenuRequested( QListViewItem * item, const QPoint & point, int col );
		
		void newMessage();
		void newChatMessage();
		void edit();
		void pubdir();
		void rename();
		void remove();
		
		void itemRenamed( QListViewItem *, int, const QString & );
		
		void refreshContext();
	
	private:
		void addGroup( QString name, bool open = true );
		QListViewItem* findGroup( QString name );
		bool isGroupOpen( QString name );
		
		void addContact( QString jid );
		
		QPopupMenu *menu;
		
		bool v_showOffline, v_showAway;
		
		QValueList< QListViewItem* > groupList;
		QMap< QString, bool > openGroups;
};

#endif
