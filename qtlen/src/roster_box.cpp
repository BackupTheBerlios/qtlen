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

#include <qheader.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qtooltip.h>
#include <qsettings.h>
#include <qpoint.h>
#include <qrect.h>
#include <qcolor.h>
#include <qsimplerichtext.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qiconset.h>

#include "roster_box.h"
#include "message_manager.h"
#include "pubdir_manager.h"
#include "editdlg.h"
#include "utils.h"

RosterBoxItem::RosterBoxItem( QListView * parent )
	: QListViewItem( parent )
{
	v_rt = 0;
	v_active = v_selected = false;
}

RosterBoxItem::RosterBoxItem( QListViewItem * parent )
	: QListViewItem( parent )
{
	v_rt = 0;
	v_active = v_selected = false;
}

RosterBoxItem::RosterBoxItem( QString jid, QString type, QListView * parent )
	: QListViewItem( parent )
{
	v_rt = 0;
	v_active = v_selected = false;
	
	setText( 0, roster_manager->getContactName( jid ) );
	setText( 1, type );
	setText( 2, jid );
	
	if( type == "item" )
	{
		setRenameEnabled( 0, true );
		setPixmap( 0, takePixmap( statusToShort( roster_manager->getContactStatus( jid ) ) ) );
	}
	else
	{
		setPixmap( 0, takePixmap( "group_open" ) );
	}
}

RosterBoxItem::RosterBoxItem( QString jid, QString type, QListViewItem * parent )
	: QListViewItem( parent )
{
	v_rt = 0;
	v_active = v_selected = false;
	
	setText( 0, roster_manager->getContactName( jid ) );
	setText( 1, type );
	setText( 2, jid );
	
	if( type == "item" )
	{
		setRenameEnabled( 0, true );
		setPixmap( 0, takePixmap( statusToShort( roster_manager->getContactStatus( jid ) ) ) );
	}
	else
	{
		setPixmap( 0, takePixmap( "group_open" ) );
	}
}

RosterBoxItem::~RosterBoxItem()
{
	if(v_rt)
		delete v_rt;
}

void RosterBoxItem::setup()
{
	QListViewItem::setup();
	int h = height();

	QString txt = expandTemplate( 0 ); // single column !!!
	if( txt.isEmpty() ){
		if(v_rt)
			delete v_rt;
		v_rt = 0;
		return;
	}
    
	const QListView* lv = listView();
	const QPixmap* px = pixmap(0);
	int left =  lv->itemMargin() + ((px)?(px->width() + lv->itemMargin()):0);

	v_active = lv->isActiveWindow();
	v_selected = isSelected();

	if ( v_selected  ) {
		txt = QString("<font color=\"%1\">").arg(listView()->colorGroup().color( QColorGroup::HighlightedText ).name()) + txt + "</font>";
	}
	
	if(v_rt)
		delete v_rt;
	v_rt = new QSimpleRichText(txt, lv->font());
	
	v_rt->setWidth(lv->columnWidth(0) - left - depth() * lv->treeStepSize());

	v_widthUsed = v_rt->widthUsed() + left;

	h = QMAX( h, v_rt->height() );

    if ( h % 2 > 0 )
	h++;
    setHeight( h );
}

void RosterBoxItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align)
{
	if(!v_rt){
		QListViewItem::paintCell(p, cg, column, width, align);
		return;
	}
	
	p->save();
	
	QListView* lv = listView();
	
	if ( isSelected() != v_selected || lv->isActiveWindow() != v_active) 
		setup();
	
	int r = lv->itemMargin();
	
	const QBrush *paper;
	// setup (colors, sizes, ...)
	if ( isSelected() ) {
		paper = &cg.brush( QColorGroup::Highlight );
	}
	else{
		const QColorGroup::ColorRole crole = QPalette::backgroundRoleFromMode( lv->viewport()->backgroundMode() );
		paper = &cg.brush( crole );
	}
	
	const QPixmap * px = pixmap( column );
	QRect pxrect;
	int pxw = 0;
	int pxh = 0;
	if(px) {
		pxw = px->width();
		pxh = px->height();
		pxrect = QRect(r, (height() - pxh)/2, pxw, pxh);
		r += pxw + lv->itemMargin();
	}
	
	if(px)
		pxrect.moveTop( (height() - pxh)/2 );
	
	// start drawing
	QRect rtrect(r, (height() - v_rt->height())/2, v_widthUsed, v_rt->height());
	v_rt->draw(p, rtrect.left(), rtrect.top(), rtrect, cg, paper);
	
	QRegion clip(0, 0, width, height());
	clip -= rtrect;
	p->setClipRegion(clip, QPainter::CoordPainter);
	p->fillRect( 0, 0, width, height(), *paper );
	
	if(px)
		p->drawPixmap(pxrect, *px);
	
	p->restore();
}

QString RosterBoxItem::expandTemplate( int )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/look" );
	
	QString t = QString::null;
	
	if( settings.readBoolEntry( "/descriptionUnderContact", false ) )
	{
		t += QString( "<nobr>%1</nobr>" ).arg( text( 0 ) );
		if( !roster_manager->getContactDescription( text( 2 ) ).isEmpty() )
			t += QString( "<br><font size=\"-1\">%1</font>" ).arg( plain2rich( roster_manager->getContactDescription( text( 2 ) ) ) );
	}
	
	return t;
}

int RosterBoxItem::widthUsed()
{
	return v_widthUsed;
}

RosterBox::RosterBox( QWidget* parent, const char* name )
	: QListView( parent, name ), QToolTip( viewport() )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/look" );
	
	header()->hide();
	
	setResizeMode( QListView::AllColumns );
	addColumn( QString::null );
	setTreeStepSize( 5 );
	
	setPaletteBackgroundColor( (QColor)settings.readEntry( "/roster/background", "#eeeeee" ) );
	setPaletteForegroundColor( (QColor)settings.readEntry( "/roster/foreground", "#000000" ) );
	
	setSorting( -1 );
	
	connect( roster_manager, SIGNAL( refreshContext() ),
			this, SLOT( refreshContext() ) );
	
	connect( this, SIGNAL( clicked( QListViewItem * ) ),
			SLOT( clicked( QListViewItem * ) ) );
	connect( this, SIGNAL( doubleClicked( QListViewItem *, const QPoint &, int ) ),
			SLOT( doubleClicked( QListViewItem *, const QPoint &, int ) ) );
	connect( this, SIGNAL( contextMenuRequested( QListViewItem *, const QPoint &, int ) ),
			SLOT( contextMenuRequested( QListViewItem *, const QPoint &, int ) ) );
	
	connect( this, SIGNAL( itemRenamed(QListViewItem *, int, const QString &) ),
			SLOT( itemRenamed(QListViewItem *, int, const QString &) ) );
	
	menu = new QPopupMenu( this );
	menu->insertItem( QIconSet( takePixmap( "msg" ) ), tr( "New &message" ),  this, SLOT( newMessage() ), CTRL+Key_M );
	menu->insertItem( QIconSet( takePixmap( "msg-chat" ) ), tr( "New &chat" ),  this, SLOT( newChatMessage() ), CTRL+Key_C );
	menu->insertSeparator();
	menu->insertItem( QIconSet( takePixmap( "edit" ) ), tr( "Edit contact" ),  this, SLOT( edit() ) );
	menu->insertItem( QIconSet( takePixmap( "find" ) ), tr( "Check in pubdir" ),  this, SLOT( pubdir() ) );
	menu->insertSeparator();
	menu->insertItem( QIconSet( takePixmap( "rename" ) ), tr( "Rename contect" ),  this, SLOT( rename() ) );
	menu->insertItem( QIconSet( takePixmap( "delete" ) ), tr( "Remove contect" ),  this, SLOT( remove() ) );
	
	settings.endGroup();
	
	settings.beginGroup( "/roster" );
	
	setShowOffline( settings.readBoolEntry( "/showOffline", true ) );
	setShowAway( settings.readBoolEntry( "/showAway", true ) );
	
	settings.endGroup();
}

RosterBox::~RosterBox()
{
	QListView::clear();
}

void RosterBox::setShowOffline( bool v )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/roster" );
	
	v_showOffline = v;
	
	settings.writeEntry( "/showOffline", v );
	
	refreshContext();
	
	settings.endGroup();
}

void RosterBox::setShowAway( bool v )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/roster" );
	
	settings.beginGroup( "/roster" );
	
	v_showAway = v;
	
	settings.writeEntry( "/showAway", v );
	
	refreshContext();
	
	settings.endGroup();
}

void RosterBox::maybeTip( const QPoint &pos )
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/look" );
	
	if( !settings.readBoolEntry( "/showToolTip", true ) )
		return;
	
	QListViewItem* item = itemAt( pos );
	if( item )
	{
		QRect r(itemRect(item));
		QString s;
		
		if( item->text( 1 ) == "item" )
		{
			s += item->text( 2 );
			if( !roster_manager->getContactDescription( item->text( 2 ) ).isEmpty() )
			{
				s += "<br><hr>";
				s += plain2rich( roster_manager->getContactDescription( item->text( 2 ) ) );
			}
		}
		else if( item->text( 1 ) == "group" )
		{
			s += tr( "Group: " );
			s += item->text( 0 );
		}
		
		tip( r, s );
	}
	
	settings.endGroup();
}

void RosterBox::clicked ( QListViewItem * )
{
	
}

void RosterBox::doubleClicked( QListViewItem *item, const QPoint &, int )
{	
	if( !item )
		return;
	
	if( item->text( 1 ) == "group" )
	{
		if( !item->isOpen() )
			item->setPixmap( 0, takePixmap( "group_open" ) );
		else
			item->setPixmap( 0, takePixmap( "group_closed" ) );
		openGroups[  item->text( 0 ) ] = !item->isOpen();
	}
	else if( item->text( 1 ) == "item" )
	{
		msg_manager->newChatMessage( item->text( 2 ) );
	}
}

void RosterBox::contextMenuRequested( QListViewItem * item, const QPoint & point, int )
{
	if( item && item->text( 1 ) == "item" )
		menu->popup( point );
}

void RosterBox::newMessage()
{
	msg_manager->newMessage( currentItem()->text( 2 ) );
}

void RosterBox::newChatMessage()
{
	msg_manager->newChatMessage( currentItem()->text( 2 ) );
}

void RosterBox::edit()
{
	EditDlg *edit = new EditDlg( currentItem()->text( 2 ) );
	edit->show();
}

void RosterBox::pubdir()
{
	pubdir_manager->showDialog();
	pubdir_manager->search( currentItem()->text( 2 ) );
}

void RosterBox::rename()
{
	currentItem()->startRename( 0 );
}

void RosterBox::remove()
{
	roster_manager->removeContact( currentItem()->text( 2 ) );
}

void RosterBox::itemRenamed( QListViewItem *item, int, const QString &text )
{
	int index = roster_manager->findContact( item->text( 2 ) );
	
	if( index != -1 )
		roster_manager->editContact( roster_manager->roster[ index ].jid, text, roster_manager->roster[ index ].group );
}

void RosterBox::refreshContext()
{
	QString name;
	if( currentItem() )
		name = currentItem()->text( 2 );
	
	QListView::clear();
	groupList.clear();
	
	if( v_showOffline )
	{
		for ( int n = 0; n < (int)roster_manager->roster.count(); ++n )
		{
			if( roster_manager->roster[ n ].status == PresenceManager::Invisible || roster_manager->roster[ n ].status == PresenceManager::Unavailable )
				addContact( roster_manager->roster[ n ].jid );
		}
	}
	
	if( v_showAway )
	{
		for ( int n = 0; n < (int)roster_manager->roster.count(); ++n )
		{
			if( roster_manager->roster[ n ].status == PresenceManager::Away || roster_manager->roster[ n ].status == PresenceManager::ExtAway || roster_manager->roster[ n ].status == PresenceManager::Dnd )
				addContact( roster_manager->roster[ n ].jid );
		}
	}
	
	for ( int n = 0; n < (int)roster_manager->roster.count(); ++n )
	{
		if( roster_manager->roster[ n ].status == PresenceManager::Available || roster_manager->roster[ n ].status == PresenceManager::Chat )
			addContact( roster_manager->roster[ n ].jid );
	}
	
	if( !name.isEmpty() )
		setCurrentItem( QListView::findItem( name, 2 ) );
}

void RosterBox::addGroup( QString name, bool open )
{
	QListViewItem *group = new QListViewItem( this, name, "group", name );
	
	if( open )
		group->setPixmap( 0, takePixmap( "group_open" ) );
	else
		group->setPixmap( 0, takePixmap( "group_closed" ) );
	openGroups[ name ] = open;
	group->setOpen( open );
	
	groupList.append( group );
}

QListViewItem* RosterBox::findGroup( QString name )
{
	for ( int n = 0; n < (int)groupList.count(); n++ )
		if( groupList[n]->text( 0 ) == name )
			return groupList[n];
	return 0;
}

bool RosterBox::isGroupOpen( QString name )
{
	QMap< QString, bool >::Iterator it;
	for ( it = openGroups.begin(); it != openGroups.end(); ++it )
		if( it.key() == name )
			return it.data();
	return true;
}

void RosterBox::addContact( QString jid )
{
	QString group = roster_manager->getContactGroup( jid );
	
	if( group.isEmpty() )
		group = tr( "General" );
	
	QListViewItem *groupItem = findGroup( group );
	
	if( !groupItem )
	{
		addGroup( group, isGroupOpen( group ) );
		groupItem = findGroup( group );
	}
	
	(void)new RosterBoxItem( jid, "item", groupItem );
}
