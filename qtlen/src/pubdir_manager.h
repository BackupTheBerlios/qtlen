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

#ifndef PUBDIR_MANAGER_H
#define PUBDIR_MANAGER_H

#include <qwidget.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qdom.h>

class QLineEdit;
class QComboBox;
class QCheckBox;
class QListView;
class QPushButton;
class QPopupMenu;
class QListViewItem;

class BirdaySpin : public QSpinBox
{
		Q_OBJECT
	
	public:
		BirdaySpin( int minValue, int maxValue, QWidget * parent = 0, const char * name = 0 )
			: QSpinBox( minValue, maxValue, 1, parent, name )
		{
			setValue( minValue );
		}
		
		~BirdaySpin() {}
		
		QString mapValueToText( int value )
		{
			if ( value == minValue() ) // special case
				return tr( "None" );
			
			return QString( "%1" ).arg( value );
		}
		
		int mapTextToValue( bool *ok )
		{
			if ( text() == tr( "None" ) ) // special case
			{
				*ok = true;
				return minValue();
			}
			
			return text().toInt( ok );
		}
		
	private:
		bool birday;
};

class PubdirManager : public QHBox
{
		Q_OBJECT
		
	public:
		enum Type { SearchResult = 1, MyPubdir = 2, MyPubdirUpdated = 3 };
		
		static void initModule();
		PubdirManager( QWidget *parent = 0, const char *name = 0 );
		~PubdirManager();
		
		void showDialog();
		
		void showPersonalInformation();
		
		void receiveNode( QDomNode node );
		
		bool search();
		bool search( QString jid );
		
		bool getPersonalInfo();
		bool setPersonalInfo();
		
		class PersonalInformation;
		
	private:
		void setDisabledAll( bool _d = true );
		
		QLineEdit *first, *last, *nick, *city, *email, *school, *login;
		BirdaySpin *min_age, *max_age;
		QComboBox *gender, *look_for, *job, *status;
		QCheckBox *voice, *video;
		
		QListView *list;
		
		QPopupMenu *menu;
		
		QPushButton *b_startSearch, *b_clearSearch, *b_addContact, *b_closeSearch, *b_next, *b_prev;
		
		int v_page;
		
		PubdirManager::PersonalInformation *personalInfo;
		
	private slots:
		void ageValueChanged( int value );
		
		void listContextMenuRequested( QListViewItem * item, const QPoint & point, int col );
		
		void copyToClipboard( int id );
		
		void startSearch();
		void clearSearch();
		
		void nextPage();
		void prevPage();
		
		void addContact();
};

class PubdirManager::PersonalInformation : public QVBox
{
		Q_OBJECT
		
	public:
		PersonalInformation( QWidget *parent = 0, const char *name = 0 );
		~PersonalInformation();
		
	protected:
		void closeEvent( QCloseEvent* );
		
	private:
		void setDisabledAll( bool _d = true );
		
		bool d;
		
		QLineEdit *first, *last, *nick, *city, *email, *school;
		BirdaySpin *birthday;
		QComboBox *gender, *look_for, *job;
		QCheckBox *voice, *video, *visible;
		
		QPushButton *update, *refresh, *close;
		
		friend class PubdirManager;
		
	private slots:
		void updateData();
		void updatedData();
		void refreshData();
};

extern PubdirManager *pubdir_manager;

#endif
