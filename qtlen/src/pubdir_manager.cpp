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
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qiconset.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include <qfont.h>
#include <qcolor.h>
#include <qclipboard.h>

#include "tlen.h"
#include "pubdir_manager.h"
#include "editdlg.h"
#include "utils.h"

PubdirManager *pubdir_manager = NULL;

void PubdirManager::initModule()
{
	if( pubdir_manager == NULL )
		pubdir_manager = new PubdirManager();
}

PubdirManager::PubdirManager( QWidget *parent, const char *name )
	: QHBox( parent, name )
{
	resize( 750, 450 );
	setCaption( tr( "Searching..." ) );
	setIcon( takePixmap("find") );
	
	QVBox *leftBox = new QVBox( this );
	
	QGroupBox *searchBox = new QGroupBox( 2, Qt::Horizontal, leftBox );
	(void)new QLabel( tr( "First name:" ), searchBox );
	first = new QLineEdit( searchBox );
	
	(void)new QLabel( tr( "Last name:" ), searchBox );
	last = new QLineEdit( searchBox );
	
	(void)new QLabel( tr( "Nick:" ), searchBox );
	nick = new QLineEdit( searchBox );
	
	(void)new QLabel( tr( "Gender:" ), searchBox );
	gender = new QComboBox( searchBox );
	gender->insertItem( tr( "None" ), 0 );
	gender->insertItem( tr( "Male" ), 1 );
	gender->insertItem( tr( "Female" ), 2 );
	
	(void)new QLabel( tr( "City:" ), searchBox );
	city = new QLineEdit( searchBox );
	
	(void)new QLabel( tr( "E-Mail:" ), searchBox );
	email = new QLineEdit( searchBox );
	
	(void)new QLabel( tr( "Age from:" ), searchBox );
	min_age = new BirdaySpin( 0, 100, searchBox );
	connect( min_age, SIGNAL( valueChanged( int ) ), this, SLOT( ageValueChanged( int ) ) );
	
	(void)new QLabel( tr( "Age to:" ), searchBox );
	max_age = new BirdaySpin( min_age->value(), 100, searchBox );
	
	(void)new QLabel( tr( "School:" ), searchBox );
	school = new QLineEdit( searchBox );
	
	(void)new QLabel( tr( "Looking for:" ), searchBox );
	look_for = new QComboBox( searchBox );
	look_for->insertItem( tr( "None" ), 0 );
	look_for->insertItem( tr( "Talk" ), 1 );
	look_for->insertItem( tr( "Friend" ), 2 );
	look_for->insertItem( tr( "Flirt" ), 3 );
	look_for->insertItem( tr( "Love" ), 4 );
	
	(void)new QLabel( tr( "Job:" ), searchBox );
	job = new QComboBox( searchBox );
	job->insertItem( tr( "None" ), 0 );
	job->insertItem( tr( "Schoolboy" ), 1 );
	job->insertItem( tr( "Student" ), 2 );
	job->insertItem( tr( "Farmer" ), 3 );
	job->insertItem( tr( "Manager" ), 4 );
	job->insertItem( tr( "Expert/Free profession" ), 5 );
	job->insertItem( tr( "Clerk/Service/Administration" ), 6 );
	job->insertItem( tr( "Unemployed" ), 7 );
	job->insertItem( tr( "Pensioner" ), 8 );
	job->insertItem( tr( "Hostess" ), 9 );
	job->insertItem( tr( "Teacher" ), 10 );
	job->insertItem( tr( "Doctor" ), 11 );
	job->insertItem( tr( "Another" ), 12 );
	
	(void)new QLabel( tr( "Status:" ), searchBox );
	status = new QComboBox( searchBox );
	status->insertItem( tr( "All" ), 0 );
	status->insertItem( tr( "Only online" ), 1 );
	
	( new QLabel( searchBox ) )->setPixmap( takePixmap( "voice" ) );
	voice = new QCheckBox( tr( "Voice" ), searchBox );
	
	( new QLabel( searchBox ) )->setPixmap( takePixmap( "video" ) );
	video = new QCheckBox( tr( "Video" ), searchBox );
	
	searchBox = new QGroupBox( 2, Qt::Horizontal, leftBox );
	
	(void)new QLabel( tr( "Login:" ), searchBox );
	login = new QLineEdit( searchBox );
	
	(void)new QVBox( leftBox );
	
	QHBox *b = new QHBox( leftBox );
	
	b_startSearch = new QPushButton( QIconSet( takePixmap( "find" ) ), tr( "Search" ), b );
	b_startSearch->setFixedWidth( 120 );
	connect( b_startSearch, SIGNAL( released() ), this, SLOT( startSearch() ) );
	
	b_clearSearch = new QPushButton( QIconSet( takePixmap( "clear" ) ), tr( "Clear" ), b );
	b_clearSearch->setFixedWidth( 120 );
	connect( b_clearSearch, SIGNAL( released() ), this, SLOT( clearSearch() ) );
	
	(void)new QHBox( b );
	
	leftBox->setFixedWidth( leftBox->sizeHint().width() );
	
	QVBox *rightBox = new QVBox( this );
	
	list = new QListView( rightBox );
	list->setAllColumnsShowFocus( true );
	list->setSorting( 2 );
	list->addColumn( tr( "Voice" ), 18 );
	list->addColumn( tr( "Video" ), 18 );
	list->addColumn( tr( "ID" ), 150 );
	list->addColumn( tr( "Name" ), 150 );
	list->addColumn( tr( "Nick" ), 150 );
	list->addColumn( tr( "Age" ), 50 );
	list->addColumn( tr( "E-Mail" ), 150 );
	list->addColumn( tr( "City" ), 150 );
	list->addColumn( tr( "School" ), 120 );
	list->addColumn( tr( "Job" ), 150 );
	list->addColumn( tr( "Looking for" ), 150 );
	list->addColumn( tr( "Genger" ), 90 );
	
	menu = new QPopupMenu( this, "Popumenu for status" );
	menu->insertItem( tr( "Copy ID" ), 2 );
	menu->insertItem( tr( "Copy name" ), 3 );
	menu->insertItem( tr( "Copy nick" ), 4 );
	menu->insertItem( tr( "Copy age" ), 5 );
	menu->insertItem( tr( "Copy e-mail" ), 6 );
	menu->insertItem( tr( "Copy city" ), 7 );
	menu->insertItem( tr( "Copy school" ), 8 );
	menu->insertItem( tr( "Copy job" ), 9 );
	menu->insertItem( tr( "Copy looking for" ), 10 );
	menu->insertItem( tr( "Copy genger" ), 11 );
	
	connect( list, SIGNAL( contextMenuRequested( QListViewItem *, const QPoint &, int ) ),
		this, SLOT( listContextMenuRequested( QListViewItem *, const QPoint &, int ) ) );
	
	connect( menu, SIGNAL( activated( int ) ),
		this, SLOT( copyToClipboard( int ) ) );
	
	b = new QHBox( rightBox );
	
	(void)new QLabel( b );
	
	b_prev =  new QPushButton( QIconSet( takePixmap( "prev" ) ), QString::null, b );
	b_prev->setFixedSize( b_startSearch->sizeHint().height(), b_startSearch->sizeHint().height() );
	b_prev->setDisabled( true );
	connect( b_prev, SIGNAL( released() ), this, SLOT( prevPage() ) );
	
	b_next =  new QPushButton( QIconSet( takePixmap( "next" ) ), QString::null, b );
	b_next->setFixedSize( b_startSearch->sizeHint().height(), b_startSearch->sizeHint().height() );
	b_next->setDisabled( true );
	connect( b_next, SIGNAL( released() ), this, SLOT( nextPage() ) );
	
	(new QLabel( b ))->setFixedWidth( 10 );
	
	b_addContact = new QPushButton( QIconSet( takePixmap( "add" ) ), tr( "Add" ), b );
	b_addContact->setFixedWidth( 120 );
	connect( b_addContact, SIGNAL( released() ), this, SLOT( addContact() ) );
	
	b_closeSearch =  new QPushButton( QIconSet( takePixmap( "close" ) ), tr( "Close" ), b );
	b_closeSearch->setFixedWidth( 120 );
	connect( b_closeSearch, SIGNAL( released() ), this, SLOT( close() ) );
}

PubdirManager::~PubdirManager()
{
	list->clear();
}

void PubdirManager::showDialog()
{
	if( pubdir_manager->isActiveWindow() )
		pubdir_manager->setActiveWindow();
	else
		pubdir_manager->show();
}

void PubdirManager::showPersonalInformation()
{
	if( pubdir_manager->personalInfo == NULL )
		pubdir_manager->personalInfo = new PubdirManager::PersonalInformation();
	
	pubdir_manager->personalInfo->show();
}

void PubdirManager::receiveNode( QDomNode node )
{
	if( node.nodeName() == "iq" )
	{
		QDomElement element = node.toElement();
		
		Type t;
		
		if( element.attribute( "from" ) != "tuba" )
			return;
		
		if( element.attribute( "id" ) == "src" )
			t = SearchResult;
		else if( element.attribute( "id" ) == "tr" )
			t = MyPubdir;
		else if( element.attribute( "id" ) == "tw" )
			t = MyPubdirUpdated;
		
		QDomNode query = node.firstChild();
		
		if( t == MyPubdirUpdated )
		{
			personalInfo->updatedData();
		}
		else if( query.nodeName() == "query" )
		{
			QDomNodeList queryChilds = query.childNodes();
			
			int max;
			
			if( (int)queryChilds.count() >= 20 )
				max = 20;
			else
				max = (int)queryChilds.count();
			
			for ( int n = 0; n <= max; n++ )
			{
				QDomElement item = queryChilds.item( n ).toElement();
				
				if( item.nodeName() == "item" )
				{
					QDomNodeList itemChilds = item.childNodes();
					
					QString v_id, v_first, v_last, v_nick, v_email, v_city, v_school;
					int v_gender = 0, v_look_for = 0, v_age = 0, v_voice = 0, v_video = 0, v_job = 0, v_visible = 0;
					PresenceManager::PresenceStatus v_status = PresenceManager::Unavailable;
					
					v_id = item.attribute( "jid" );
					
					for ( int w = 0; w < (int)itemChilds.count(); w++ )
					{
						QDomElement i = itemChilds.item( w ).toElement();
						
						if( i.nodeName() == "i" )
						{
							v_id = decode( i.text().ascii() );
						}
						else if( i.nodeName() == "first" )
						{
							v_first = decode( i.text().ascii() );
						}
						else if( i.nodeName() == "last" )
						{
							v_last = decode( i.text().ascii() );
						}
						else if( i.nodeName() == "nick" )
						{
							v_nick = decode( i.text().ascii() );
						}
						else if( i.nodeName() == "email" )
						{
							v_email = decode( i.text().ascii() );
						}
						else if( i.nodeName() == "c" )
						{
							v_city = decode( i.text().ascii() );
						}
						else if( i.nodeName() == "e" )
						{
							v_school = decode( i.text().ascii() );
						}
						else if( i.nodeName() == "s" )
						{
							v_gender = i.text().toInt();
						}
						else if( i.nodeName() == "r" )
						{
							v_look_for = i.text().toInt();
						}
						else if( i.nodeName() == "b" )
						{
							v_age = i.text().toInt();
						}
						else if( i.nodeName() == "g" )
						{
							v_voice = i.text().toInt();
						}
						else if( i.nodeName() == "k" )
						{
							v_video = i.text().toInt();
						}
						else if( i.nodeName() == "j" )
						{
							v_job = i.text().toInt();
						}
						else if( i.nodeName() == "v" )
						{
							v_visible = i.text().toInt();
						}
						else if( i.nodeName() == "a" )
						{
							switch( i.text().toInt() )
							{
								case 2:
									v_status = PresenceManager::Available;
									break;
								case 3:
									v_status = PresenceManager::ExtAway;
									break;
								case 4:
									v_status = PresenceManager::Away;
									break;
								case 5:
									v_status = PresenceManager::Dnd;
									break;
								case 6:
									v_status = PresenceManager::Chat;
									break;
								default:
									v_status = PresenceManager::Unavailable;
									break;
							}
						}
					}
					
					if( t == MyPubdir )
					{
						personalInfo->first->setText( v_first );
						personalInfo->last->setText( v_last );
						personalInfo->nick->setText( v_nick );
						personalInfo->city->setText( v_city );
						personalInfo->email->setText( v_email );
						personalInfo->school->setText( v_school );
						
						personalInfo->birthday->setValue( v_age );
						
						personalInfo->gender->setCurrentItem( v_gender );
						personalInfo->look_for->setCurrentItem( v_look_for );
						personalInfo->job->setCurrentItem( v_job );
						
						personalInfo->voice->setChecked( v_voice );
						personalInfo->video->setChecked( v_video );
						personalInfo->visible->setChecked( v_visible );
					}
					else if( t == SearchResult )
					{
						QListViewItem *z = new QListViewItem( list );
						
						if( v_voice )
							z->setPixmap( 0, takePixmap( "voice" ) );
						if( v_video )
							z->setPixmap( 1, takePixmap( "video" ) );
						
						z->setPixmap( 2, takePixmap( statusToShort( v_status ) ) );
						z->setText( 2, v_id );
						z->setText( 3, v_first + " " + v_last );
						z->setText( 4, v_nick );
						z->setText( 5, QString::number( v_age ) );
						z->setText( 6, v_email );
						z->setText( 7, v_city );
						z->setText( 8, v_school );
						z->setText( 9, job->text( v_job ) );
						z->setText( 10, look_for->text( v_look_for ) );
						z->setText( 11, gender->text( v_gender ) );
					}
				}
				
				if( t == SearchResult )
				{
					setDisabledAll( false );
					
					if( queryChilds.count() >= 20 )
						b_next->setEnabled( true );
					else
						b_next->setEnabled( false );
						
					if( v_page )
						b_prev->setEnabled( true );
					else
						b_prev->setEnabled( false );
					
					if( queryChilds.count() <= 0 )
						QMessageBox::information( this, "QTlen", tr( "There is no users for this data" ) );
				}
			}
		}
	}
}

bool PubdirManager::search()
{
	if( !tlen_manager->isConnected() )
		return false;
	
	QDomDocument doc;
	
	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "type", "get" );
	iq.setAttribute( "id", "src" );
	iq.setAttribute( "to", "tuba" );
	doc.appendChild( iq );
	
	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:search" );
	iq.appendChild( query );
	
	QDomElement item;
	QDomText text;
	
	if( !login->text().isEmpty() )
	{
		item = doc.createElement( "i" );
		text = doc.createTextNode( encode( login->text() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( !first->text().isEmpty() )
	{
		item = doc.createElement( "first" );
		text = doc.createTextNode( encode( first->text() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( !last->text().isEmpty() )
	{
		item = doc.createElement( "last" );
		text = doc.createTextNode( encode( last->text() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( !nick->text().isEmpty() )
	{
		item = doc.createElement( "nick" );
		text = doc.createTextNode( encode( nick->text() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( !email->text().isEmpty() )
	{
		item = doc.createElement( "email" );
		text = doc.createTextNode( encode( email->text() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( !city->text().isEmpty() )
	{
		item = doc.createElement( "c" );
		text = doc.createTextNode( encode( city->text() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( !school->text().isEmpty() )
	{
		item = doc.createElement( "e" );
		text = doc.createTextNode( encode( school->text() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( gender->currentItem() )
	{
		item = doc.createElement( "s" );
		text = doc.createTextNode( QString::number( gender->currentItem() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( min_age->value() > min_age->minValue() )
	{
		item = doc.createElement( "d" );
		text = doc.createTextNode( QString::number( min_age->value() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( max_age->value() > max_age->minValue() )
	{
		item = doc.createElement( "u" );
		text = doc.createTextNode( QString::number( max_age->value() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( job->currentItem() )
	{
		item = doc.createElement( "j" );
		text = doc.createTextNode( QString::number( job->currentItem() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( look_for->currentItem() )
	{
		item = doc.createElement( "r" );
		text = doc.createTextNode( QString::number( look_for->currentItem() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( look_for->currentItem() )
	{
		item = doc.createElement( "a" );
		text = doc.createTextNode( QString::number( 2 ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( voice->isChecked() )
	{
		item = doc.createElement( "g" );
		text = doc.createTextNode( QString::number( (int)voice->isChecked() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( video->isChecked() )
	{
		item = doc.createElement( "k" );
		text = doc.createTextNode( QString::number( (int)video->isChecked() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	if( v_page )
	{
		item = doc.createElement( "f" );
		text = doc.createTextNode( QString::number( v_page * 20 ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	return tlen_manager->writeXml( doc );
}

bool PubdirManager::search( QString jid )
{
	clearSearch();
	
	login->setText( takeLogin( jid ) );
	
	return search();
}

bool PubdirManager::getPersonalInfo()
{
	if( !tlen_manager->isConnected() )
		return false;
	
	QDomDocument doc;
	
	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "type", "get" );
	iq.setAttribute( "id", "tr" );
	iq.setAttribute( "to", "tuba" );
	doc.appendChild( iq );
	
	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:register" );
	iq.appendChild( query );
	
	return tlen_manager->writeXml( doc );
}

bool PubdirManager::setPersonalInfo()
{
	if( !tlen_manager->isConnected() )
		return false;
	
	QDomDocument doc;
	
	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "type", "set" );
	iq.setAttribute( "id", "tw" );
	iq.setAttribute( "to", "tuba" );
	doc.appendChild( iq );
	
	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:register" );
	iq.appendChild( query );
	
	QDomElement item = doc.createElement( "first" );
	QDomText text = doc.createTextNode( encode( personalInfo->first->text() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	item = doc.createElement( "last" );
	text = doc.createTextNode( encode( personalInfo->last->text() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	item = doc.createElement( "nick" );
	text = doc.createTextNode( encode( personalInfo->nick->text() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	item = doc.createElement( "email" );
	text = doc.createTextNode( encode( personalInfo->email->text() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	item = doc.createElement( "c" );
	text = doc.createTextNode( encode( personalInfo->city->text() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	item = doc.createElement( "e" );
	text = doc.createTextNode( encode( personalInfo->school->text() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	item = doc.createElement( "s" );
	text = doc.createTextNode( QString::number( personalInfo->gender->currentItem() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	item = doc.createElement( "v" );
	text = doc.createTextNode( QString::number( (int)personalInfo->visible->isChecked() ) );
	item.appendChild( text );
	query.appendChild( item );

	if( personalInfo->birthday->value() > personalInfo->birthday->minValue() )
	{
		item = doc.createElement( "b" );
		text = doc.createTextNode( QString::number( personalInfo->birthday->value() ) );
		item.appendChild( text );
		query.appendChild( item );
	}
	
	item = doc.createElement( "j" );
	text = doc.createTextNode( QString::number( personalInfo->job->currentItem() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	item = doc.createElement( "r" );
	text = doc.createTextNode( QString::number( personalInfo->look_for->currentItem() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	item = doc.createElement( "g" );
	text = doc.createTextNode( QString::number( (int)personalInfo->voice->isChecked() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	item = doc.createElement( "k" );
	text = doc.createTextNode( QString::number( (int)personalInfo->video->isChecked() ) );
	item.appendChild( text );
	query.appendChild( item );
	
	return tlen_manager->writeXml( doc );
}

void PubdirManager::setDisabledAll( bool _d )
{
	first->setDisabled( _d );
	last->setDisabled( _d );
	nick->setDisabled( _d );
	city->setDisabled( _d );
	email->setDisabled( _d );
	school->setDisabled( _d );
	login->setDisabled( _d );
	
	min_age->setDisabled( _d );
	max_age->setDisabled( _d );
	gender->setDisabled( _d );
	look_for->setDisabled( _d );
	job->setDisabled( _d );
	status->setDisabled( _d );
	voice->setDisabled( _d );
	video->setDisabled( _d );
	
	b_startSearch->setDisabled( _d );
	b_clearSearch->setDisabled( _d );
	b_addContact->setDisabled( _d );
	b_closeSearch->setDisabled( _d );
	
	if( _d )
	{
		b_next->setDisabled( _d );
		b_prev->setDisabled( _d );
	}
}

void PubdirManager::ageValueChanged( int value )
{
	if( max_age->value() == max_age->minValue() )
	{
		max_age->setMinValue( value );
		max_age->setValue( value );
	}
	else
		max_age->setMinValue( value );
}

void PubdirManager::listContextMenuRequested( QListViewItem *item, const QPoint & point, int )
{
	if( item )
		menu->popup( point );
}

void PubdirManager::copyToClipboard( int id )
{
	QClipboard *cb = qApp->clipboard();
	
	cb->setText( list->currentItem()->text( id ), QClipboard::Clipboard );
}

void PubdirManager::startSearch()
{
	setDisabledAll( true );
	
	v_page = 0;
	
	list->clear();
	
	if( !search() )
		QMessageBox::warning( this, "QTlen", tr( "Error when sending request,\n maby you aren't connect" ) );
}

void PubdirManager::clearSearch()
{
	first->clear();
	last->clear();
	nick->clear();
	city->clear();
	email->clear();
	school->clear();
	login->clear();
	
	min_age->setValue( 0 );
	max_age->setMinValue( 0 );
	max_age->setValue( 0 );
	
	gender->setCurrentItem( 0 );
	look_for->setCurrentItem( 0 );
	job->setCurrentItem( 0 );
	status->setCurrentItem( 0 );
	
	voice->setChecked( false );
	video->setChecked( false );
	
	list->clear();
	
	b_next->setDisabled( true );
	b_prev->setDisabled( true );
	
	v_page = 0;
}

void PubdirManager::nextPage()
{
	list->clear();
	
	b_next->setDisabled( true );
	b_prev->setDisabled( true );
	
	v_page++;
	
	search();
}

void PubdirManager::prevPage()
{
	list->clear();
	
	b_next->setDisabled( true );
	b_prev->setDisabled( true );
	
	if( v_page > 0 )
		v_page--;
	
	search();
}

void PubdirManager::addContact()
{
	if( list->currentItem() )
	{
		EditDlg *dlg = new EditDlg( takeJid( list->currentItem()->text( 2 ) ), true );
		dlg->show();
	}
}

PubdirManager::PersonalInformation::PersonalInformation( QWidget *parent, const char *name )
	: QVBox( parent, name )
{
	resize( 380, sizeHint().height() );
	setCaption( tr( "Personal Information" ) );
	setIcon( takePixmap("info") );
	
	d = false;
	
	QHBox *box = new QHBox( this );
	//box->setPaletteBackgroundColor( (QColor)"#6da1ff" );
	
	QLabel *label = new QLabel( box );
	label->setPixmap( takePixmap( "info_big" ) );
	
	QFont font;
	font.setPointSize( 14 );
	font.setBold( true );
	
	label = new QLabel( tr( "Personal Information" ), box );
	label->setFont( font );
	label->setAlignment( AlignVCenter | AlignRight );
	
	QGroupBox *personalInfo = new QGroupBox( 2, Qt::Horizontal, tr( "Personal Information" ), this );
	
	(void)new QLabel( tr( "Firstname:" ), personalInfo );
	first = new QLineEdit( personalInfo );
	
	(void)new QLabel( tr( "Lastname:" ), personalInfo );
	last = new QLineEdit( personalInfo );
	
	(void)new QLabel( tr( "Nick:" ), personalInfo );
	nick = new QLineEdit( personalInfo );
	
	(void)new QLabel( tr( "Birthday:" ), personalInfo );
	birthday = new BirdaySpin( QDate::currentDate().year() - 100, QDate::currentDate().year(), personalInfo );
	
	(void)new QLabel( tr( "Gender:" ), personalInfo );
	gender = new QComboBox( personalInfo );
	gender->insertItem( tr( "None" ), 0 );
	gender->insertItem( tr( "Male" ), 1 );
	gender->insertItem( tr( "Female" ), 2 );
	
	(void)new QLabel( tr( "City:" ), personalInfo );
	city = new QLineEdit( personalInfo );
	
	(void)new QLabel( tr( "E-Mail:" ), personalInfo );
	email = new QLineEdit( personalInfo );
	
	(void)new QLabel( tr( "Looking for:" ), personalInfo );
	look_for = new QComboBox( personalInfo );
	look_for->insertItem( tr( "None" ), 0 );
	look_for->insertItem( tr( "Talk" ), 1 );
	look_for->insertItem( tr( "Friend" ), 2 );
	look_for->insertItem( tr( "Flirt" ), 3 );
	look_for->insertItem( tr( "Love" ), 4 );
	
	(void)new QLabel( tr( "School:" ), personalInfo );
	school = new QLineEdit( personalInfo );
	
	(void)new QLabel( tr( "Job:" ), personalInfo );
	job = new QComboBox( personalInfo );
	job->insertItem( tr( "None" ), 0 );
	job->insertItem( tr( "Schoolboy" ), 1 );
	job->insertItem( tr( "Student" ), 2 );
	job->insertItem( tr( "Farmer" ), 3 );
	job->insertItem( tr( "Manager" ), 4 );
	job->insertItem( tr( "Expert/Free profession" ), 5 );
	job->insertItem( tr( "Clerk/Service/Administration" ), 6 );
	job->insertItem( tr( "Unemployed" ), 7 );
	job->insertItem( tr( "Pensioner" ), 8 );
	job->insertItem( tr( "Hostess" ), 9 );
	job->insertItem( tr( "Teacher" ), 10 );
	job->insertItem( tr( "Doctor" ), 11 );
	job->insertItem( tr( "Another" ), 12 );
	
	( new QLabel( personalInfo ) )->setPixmap( takePixmap( "voice" ) );
	voice = new QCheckBox( tr( "Voice" ), personalInfo );
	
	( new QLabel( personalInfo ) )->setPixmap( takePixmap( "video" ) );
	video = new QCheckBox( tr( "Video" ), personalInfo );
	
	(void)new QLabel( personalInfo );
	visible = new QCheckBox( tr( "Visible" ), personalInfo );
	
	box = new QHBox( this );
	
	update = new QPushButton( takePixmap( "update" ), tr( "Update" ), box );
	update->setFixedWidth( 120 );
	connect( update, SIGNAL( released() ), this, SLOT( updateData() ) );
	
	refresh = new QPushButton( takePixmap( "refresh" ), tr( "Refresh" ), box );
	refresh->setFixedWidth( 120 );
	connect( refresh, SIGNAL( released() ), this, SLOT( refreshData() ) );
	
	(void)new QHBox( box );
	
	close = new QPushButton( takePixmap( "close" ), tr( "Close" ), box );
	close->setFixedWidth( 120 );
	connect( close, SIGNAL( released() ), this, SLOT( close() ) );
}

PubdirManager::PersonalInformation::~PersonalInformation()
{
	
}

void PubdirManager::PersonalInformation::closeEvent( QCloseEvent* ce )
{
	if( d )
		ce->ignore();
	else
		ce->accept();
}

void PubdirManager::PersonalInformation::setDisabledAll( bool _d )
{
	first->setDisabled( _d );
	last->setDisabled( _d );
	nick->setDisabled( _d );
	city->setDisabled( _d );
	email->setDisabled( _d );
	school->setDisabled( _d );
	
	birthday->setDisabled( _d );
	
	gender->setDisabled( _d );
	look_for->setDisabled( _d );
	job->setDisabled( _d );
	
	voice->setDisabled( _d );
	video->setDisabled( _d );
	visible->setDisabled( _d );
	
	update->setDisabled( _d );
	refresh->setDisabled( _d );
	close->setDisabled( _d );
	
	d = _d;
}

void PubdirManager::PersonalInformation::updateData()
{
	if( !pubdir_manager->setPersonalInfo() )
		QMessageBox::warning( this, "QTlen", tr( "Error when sending request,\n maby you aren't connect" ) );
	else
		setDisabledAll( true );
}

void PubdirManager::PersonalInformation::updatedData()
{
	QMessageBox::information( this, "QTlen", tr( "Updating data finished succes!" ) );
	setDisabledAll( false );
}

void PubdirManager::PersonalInformation::refreshData()
{
	if( !pubdir_manager->getPersonalInfo() )
		QMessageBox::warning( this, "QTlen", tr( "Error when sending request,\n maby you aren't connect" ) );
}
