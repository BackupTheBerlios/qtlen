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

#include <qlayout.h>
#include <qhbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qfont.h>

#include "editdlg.h"
#include "roster_manager.h"
#include "pubdir_manager.h"
#include "utils.h"

EditDlg::EditDlg( QString jid, bool add, QWidget *parent, const char *_name )
	: QWidget( parent, _name, Qt::WDestructiveClose )
{
	resize( 380, sizeHint().height() );
	setCaption( add ? tr( "Add contact" ) : tr( "Edit contact" ) );
	setIcon( takePixmap( add ? "add" : "edit" ) );
	
	QGridLayout *layout = new QGridLayout( this, 3, 2 );
	
	QLabel *label = new QLabel( this );
	label->setPixmap( takePixmap( add ? "add_big" : "edit_big" ) );
	layout->addWidget( label, 0, 0 );
	
	QFont font;
	font.setPointSize( 14 );
	font.setBold( true );
	
	label = new QLabel( add ? tr( "Add contact" ) : tr( "Edit contact" ), this );
	label->setFont( font );
	label->setAlignment( AlignVCenter | AlignRight );
	layout->addWidget( label, 0, 1 );
	
	QGroupBox *box = new QGroupBox( 2, Qt::Horizontal, this );
	layout->addMultiCellWidget( box, 1, 1, 0, 1 );
	
	(void)new QLabel( tr( "Name:" ), box );
	name = new QLineEdit( add ? "" : roster_manager->getContactName( jid ), box );
	
	(void)new QLabel( tr( "Login:" ), box );
	login = new QLineEdit( jid, box );
	if( !add )
		login->setReadOnly( true );
	
	(void)new QLabel( tr( "Group:" ), box );
	group = new QLineEdit( add ? "" : roster_manager->getContactGroup( jid ), box );
	
	if( add )
	{
		(void)new QLabel( "", box );
		request_subscribe = new QCheckBox( tr( "Request subscribe" ), box );
		request_subscribe->setChecked( true );
	}
	
	QHBox *b = new QHBox( this );
	layout->addMultiCellWidget( b, 2, 2, 0, 1 );
	
	QPushButton *button = new QPushButton( takePixmap( add ? "add" : "edit" ), add ? tr( "Add" ) : tr( "Edit" ), b );
	button->setFixedWidth( 100 );
	if( add )
		connect( button, SIGNAL( released() ), this, SLOT( add() ) );
	else
		connect( button, SIGNAL( released() ), this, SLOT( edit() ) );
	
	button = new QPushButton( takePixmap( "find" ), add ? tr( "Search" ) : tr( "Check" ), b );
	button->setFixedWidth( 100 );
	if( add )
		connect( button, SIGNAL( released() ), this, SLOT( search() ) );
	else
		connect( button, SIGNAL( released() ), this, SLOT( check() ) );
	
	(void)new QHBox( b );
	
	button = new QPushButton( takePixmap( "close" ), tr( "Close" ), b );
	button->setFixedWidth( 100 );
	connect( button, SIGNAL( released() ), this, SLOT( close() ) );
}

EditDlg::~EditDlg()
{
	
}

void EditDlg::add()
{
	if( login->text().isEmpty() )
	{
		QMessageBox::warning( this, "QTlen", tr( "Please, fill in empty login box before you send form" ) );
		return;
	}
	
	if( roster_manager->addContact( takeJid( login->text() ), name->text(), group->text() ) )
	{
		if( request_subscribe->isChecked() )
			presence_manager->subscribeMe( takeJid( login->text() ) );
		close();
	}
	else
		QMessageBox::warning( this, "QTlen", tr( "Error when sending request,\n maby you aren't connect" ) );
}

void EditDlg::edit()
{
	if( roster_manager->editContact( takeJid( login->text() ), name->text(), group->text() ) )
		close();
	else
		QMessageBox::warning( this, "QTlen", tr( "Error when sending request,\n maby you aren't connect" ) );
}

void EditDlg::search()
{
	pubdir_manager->showDialog();
	close();
}

void EditDlg::check()
{
	pubdir_manager->search( login->text() );
	pubdir_manager->showDialog();
}
