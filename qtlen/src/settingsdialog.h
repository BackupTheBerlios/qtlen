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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qtabdialog.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qcombobox.h>

#include "tlen.h"

class QSpinBox;

class SettingsDialog : public QTabDialog
{
		Q_OBJECT
		
	public:
		static void showDialog();
		SettingsDialog( QWidget *parent=0, const char *name=0 );
		
	private:
		QWidget *connection, *general, *look, *sounds, *proxy;
		
		// Connection
		QLineEdit *username, *password, *nick;
		QCheckBox *autoReconnect;
		QGroupBox *acBox;
		QComboBox *defaultStatus;
		QLineEdit *defaultDescription;
		
		// General
		QComboBox *language;
		QCheckBox *email, *webmsg, *newVersion, *returnSend;
		QGroupBox *tray;
		QCheckBox *hideOnStart, *windowMakerTray;
		
		// Look
		QLineEdit *bgColor, *myFontColor, *contactFontColor, *chatFont, *chatFontColor;
		QCheckBox *toolTip, *descriptionUnderContact;
		QLineEdit *rosterBgColor, *rosterFgColor;
		
		// Sounds
		QLineEdit *player;
		QCheckBox *sndMessage, *sndChatMessage, *sndErrorMessage, *sndNewsMessage, *sndSoundAlert, *sndEmail, *sndWebMessage, *sndSubscribe, *sndUnsubscribe, *sndAvailableStatus, *sndAwayStatus, *sndUnavailableStatus;
		QLineEdit *sndEditMessage, *sndEditChatMessage, *sndEditErrorMessage, *sndEditNewsMessage, *sndEditSoundAlert, *sndEditEmail, *sndEditWebMessage, *sndEditSubscribe, *sndEditUnsubscribe, *sndEditAvailableStatus, *sndEditAwayStatus, *sndEditUnavailableStatus;
		QPushButton *selectMessage, *selectChatMessage, *selectErrorMessage, *selectNewsMessage, *selectSoundAlert, *selectEmail, *selectWebMessage, *selectSubscribe, *selectUnsubscribe, *selectAvailableStatus, *selectAwayStatus, *selectUnavailableStatus;
		QPushButton *playMessage, *playChatMessage, *playErrorMessage, *playNewsMessage, *playSoundAlert, *playEmail, *playWebMessage, *playSubscribe, *playUnsubscribe, *playAvailableStatus, *playAwayStatus, *playUnavailableStatus;
		
		// Proxy
		QGroupBox *proxyBox;
		QLineEdit *proxyIp, *proxyUsername, *proxyPassword;
		QSpinBox *proxyPort;
		
	private slots:
		void apply();
		void cancel();
		
		void selectBgColor();
		void selectMyColor();
		void selectContactColor();
		void selectChatFont();
		void selectChatFontColor();
		void selectRosterBgColor();
		void selectRosterFgColor();
		
		void sndMessageToggled( bool on );
		void sndChatMessageToggled( bool on );
		void sndErrorMessageToggled( bool on );
		void sndNewsMessageToggled( bool on );
		void sndSoundAlertToggled( bool on );
		void sndEmailToggled( bool on );
		void sndWebMessageToggled( bool on );
		void sndSubscribeToggled( bool on );
		void sndUnsubscribeToggled( bool on );
		void sndAvailableStatusToggled( bool on );
		void sndAwayStatusToggled( bool on );
		void sndUnavailableStatusToggled( bool on );
		
		void selectMessageReleased();
		void selectChatMessageReleased();
		void selectErrorMessageReleased();
		void selectNewsMessageReleased();
		void selectSoundAlertReleased();
		void selectEmailReleased();
		void selectWebMessageReleased();
		void selectSubscribeReleased();
		void selectUnsubscribeReleased();
		void selectAvailableStatusReleased();
		void selectAwayStatusReleased();
		void selectUnavailableStatusReleased();
		
		void playMessageReleased();
		void playChatMessageReleased();
		void playErrorMessageReleased();
		void playNewsMessageReleased();
		void playSoundAlertReleased();
		void playEmailReleased();
		void playWebMessageReleased();
		void playSubscribeReleased();
		void playUnsubscribeReleased();
		void playAvailableStatusReleased();
		void playAwayStatusReleased();
		void playUnavailableStatusReleased();
};

extern SettingsDialog *settings;

#endif
