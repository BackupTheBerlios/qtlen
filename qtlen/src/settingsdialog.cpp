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
#include <qlabel.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qtooltip.h>
#include <qsettings.h>
#include <qtextcodec.h>
#include <qvalidator.h>
#include <qcolor.h>
#include <qcolordialog.h>
#include <qfont.h>
#include <qfontdialog.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qdir.h>

#include "qtlen.h"
#include "settingsdialog.h"
#include "utils.h"
#include "message.h"
#include "message_manager.h"
#include "sound_manager.h"

SettingsDialog *settingsDialog = NULL;

void SettingsDialog::showDialog()
{
	if( settingsDialog )
	{
		if( settingsDialog->isActiveWindow() )
			settingsDialog->setActiveWindow();
		else
			settingsDialog->show();

	}
	else
	{
		settingsDialog = new SettingsDialog();
		settingsDialog->show();
	}
}

SettingsDialog::SettingsDialog( QWidget *parent, const char *name )
	: QTabDialog( parent, name, false )
{
	resize( 500, 350 );
	setCaption( tr("Settings") );
	setIcon( takePixmap("settings") );
	
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	connection = new QWidget( this );
	settings.beginGroup( "/connection" );
	
	QGridLayout *accLay = new QGridLayout( connection, 5, 1 );
	
	addTab( connection, tr("Connection") );
	
	QGroupBox *account = new QGroupBox( 2, Qt::Horizontal, tr("Account"), connection );
	
	(void)new QLabel( tr("Username:"), account );
	
	username = new QLineEdit( settings.readEntry( "/username" ), account );
	QToolTip::add( username, tr("Here you must enter only your Tlen.pl login <b>without</b> \"@tlen.pl\" suffix!") );
	
	(void)new QLabel( tr("Password:"), account );
	
	password = new QLineEdit( settings.readEntry( "/password" ), account );
	password->setEchoMode( QLineEdit::Password );
	
	(void)new QLabel( tr("Nick:"), account );
	
	nick = new QLineEdit( settings.readEntry( "/nick", tr("Me") ), account );
	QToolTip::add( nick, tr("This nick will be use in chat window") );
	
	accLay->addWidget( account, 0, 0 );
	
	acBox = new QGroupBox( 2, Qt::Horizontal, tr("Auto connect"), connection );
	acBox->setCheckable( true );
	acBox->setChecked( settings.readBoolEntry( "/autoConnect", false ) );
	
	(void)new QLabel( tr("Default status:"), acBox );
	
	defaultStatus = new QComboBox( false, acBox );
	defaultStatus->insertItem( takePixmap("online"), tr("Online") );
	defaultStatus->insertItem( takePixmap("chat"), tr("Free for Chat") );
	defaultStatus->insertItem( takePixmap("away"), tr("Away") );
	defaultStatus->insertItem( takePixmap("xa"), tr("Not Available") );
	defaultStatus->insertItem( takePixmap("dnd"), tr("Do not Disturb") );
	defaultStatus->insertItem( takePixmap("invisible"), tr("Invisible") );
	defaultStatus->setCurrentItem( settings.readNumEntry( "/defaultStatus" ) );
	
	(void)new QLabel( tr("Default description:"), acBox );
	
	defaultDescription = new QLineEdit( settings.readEntry( "/defaultDescription" ), acBox );
	
	accLay->addWidget( acBox, 1, 0 );
	
	autoReconnect = new QCheckBox( tr("Auto reconnect"), connection );
	autoReconnect->setChecked( settings.readBoolEntry( "/autoReconnect" ) );
	accLay->addWidget( autoReconnect, 2, 0 );
	
	settings.resetGroup();
	
	general = new QWidget( this );
	settings.beginGroup( "/general" );
	
	QGridLayout *genLay = new QGridLayout( general, 10, 2 );
	
	addTab( general, tr("General") );
	
	QLabel *label = new QLabel( tr("Language:"), general );
	genLay->addWidget( label, 0, 0 );
	
	language = new QComboBox( false, general );
	language->insertItem( tr("English") );
	language->insertItem( tr("Polish") );
	QString lang = settings.readEntry( "/language", QTextCodec::locale() );
	if( lang == "en" )
		language->setCurrentItem( 0 );
	else if( lang == "pl" )
		language->setCurrentItem( 1 );
	else
		language->setCurrentItem( 0 );
	
	genLay->addWidget( language, 0, 1 );
	
	email = new QCheckBox( tr("Notify about new mail"), general );
	email->setChecked( settings.readBoolEntry( "/email", true ) );
	genLay->addMultiCellWidget( email, 1, 1, 0, 1 );
	
	webmsg = new QCheckBox( tr("Notify about new web message"), general );
	webmsg->setChecked( settings.readBoolEntry( "/webmsg", true ) );
	genLay->addMultiCellWidget( webmsg, 2, 2, 0, 1 );
	
	newVersion = new QCheckBox( tr("Check for new version"), general );
	newVersion->setChecked( settings.readBoolEntry( "/newVersion", true ) );
	genLay->addMultiCellWidget( newVersion, 3, 3, 0, 1 );
	
	returnSend = new QCheckBox( tr("Key Return send message"), general );
	returnSend->setChecked( settings.readBoolEntry( "/returnSend", true ) );
	genLay->addMultiCellWidget( returnSend, 4, 4, 0, 1 );
	
	tray = new QGroupBox( 1, Qt::Horizontal, tr("System tray"), general );
	tray->setCheckable( true );
	tray->setChecked( settings.readBoolEntry( "/trayicon/activated" )  );
	
	hideOnStart = new QCheckBox( tr("Hide on start"), tray );
	hideOnStart->setChecked( settings.readBoolEntry( "/trayicon/hideOnStart" ) );
	
	windowMakerTray = new QCheckBox( tr("Use tray for Window Maker"), tray );
	windowMakerTray->setChecked( settings.readBoolEntry( "/trayicon/windowMakeTray" ) );
	
	genLay->addMultiCellWidget( tray, 5, 5, 0, 1 );
	
	settings.resetGroup();
	
	look = new QWidget( this );
	settings.beginGroup( "/look" );
	
	QGridLayout *lookLay = new QGridLayout( look, 6, 1 );
	
	addTab( look, tr("Look") );
	
	QGroupBox *colorsBox = new QGroupBox( 3, Qt::Horizontal, tr("Colors"), look );
	lookLay->addWidget( colorsBox, 0, 0 );
	
	(void)new QLabel( tr("Background color:"), colorsBox );
	bgColor = new QLineEdit( settings.readEntry( "/bgColor", "#ffffff" ), colorsBox );
	bgColor->setReadOnly( true );
	bgColor->setPaletteBackgroundColor( bgColor->text() );
	QPushButton *b = new QPushButton( tr("Select"), colorsBox );
	connect( b, SIGNAL( released() ), SLOT( selectBgColor() ) );
	
	(void)new QLabel( tr("My font color:"), colorsBox );
	myFontColor = new QLineEdit( settings.readEntry( "/myFontColor", "#ff0000" ), colorsBox );
	myFontColor->setReadOnly( true );
	myFontColor->setPaletteBackgroundColor( bgColor->text() );
	myFontColor->setPaletteForegroundColor( myFontColor->text() );
	b = new QPushButton( tr("Select"), colorsBox );
	connect( b, SIGNAL( released() ), SLOT( selectMyColor() ) );
	
	(void)new QLabel( tr("Contact font color:"), colorsBox );
	contactFontColor = new QLineEdit( settings.readEntry( "/contactFontColor", "#0000ff" ), colorsBox );
	contactFontColor->setReadOnly( true );
	contactFontColor->setPaletteBackgroundColor( bgColor->text() );
	contactFontColor->setPaletteForegroundColor( contactFontColor->text() );
	b = new QPushButton( tr("Select"), colorsBox );
	connect( b, SIGNAL( released() ), SLOT( selectContactColor() ) );
	
	(void)new QLabel( tr("Chat font color:"), colorsBox );
	chatFontColor = new QLineEdit( settings.readEntry( "/chatFontColor", "#000000" ), colorsBox );
	chatFontColor->setReadOnly( true );
	chatFontColor->setPaletteBackgroundColor( bgColor->text() );
	chatFontColor->setPaletteForegroundColor( chatFontColor->text() );
	b = new QPushButton( tr("Select"), colorsBox );
	connect( b, SIGNAL( released() ), SLOT( selectChatFontColor() ) );
	
	QGroupBox *fontsBox = new QGroupBox( 3, Qt::Horizontal, tr("Fonts"), look );
	lookLay->addWidget( fontsBox, 1, 0 );
	
	(void)new QLabel( tr("Chat:"), fontsBox );
	chatFont = new QLineEdit( settings.readEntry( "/chatFont", font().toString() ), fontsBox );
	chatFont->setReadOnly( true );
	QFont font;
	font.fromString( chatFont->text() );
	chatFont->setFont( font );
	b = new QPushButton( tr("Select"), fontsBox );
	connect( b, SIGNAL( released() ), SLOT( selectChatFont() ) );
	
	toolTip = new QCheckBox( tr("Show Tool Tips"), look );
	toolTip->setChecked( settings.readBoolEntry( "/showToolTip", true ) );
	lookLay->addWidget( toolTip, 2, 0 );
	
	descriptionUnderContact = new QCheckBox( tr("Description under contact"), look );
	descriptionUnderContact->setChecked( settings.readBoolEntry( "/descriptionUnderContact" ) );
	lookLay->addWidget( descriptionUnderContact, 3, 0 );
	
	QGroupBox *rosterBox = new QGroupBox( 3, Qt::Horizontal, tr("Roster"), look );
	lookLay->addWidget( rosterBox, 4, 0 );
	
	(void)new QLabel( tr("Roster background color:"), rosterBox );
	rosterBgColor = new QLineEdit( settings.readEntry( "/roster/background", "#eeeeee" ), rosterBox );
	rosterBgColor->setReadOnly( true );
	b = new QPushButton( tr("Select"), rosterBox );
	connect( b, SIGNAL( released() ), SLOT( selectRosterBgColor() ) );
	
	(void)new QLabel( tr("Roster foreground color:"), rosterBox );
	rosterFgColor = new QLineEdit( settings.readEntry( "/roster/foreground", "#000000" ), rosterBox );
	rosterFgColor->setReadOnly( true );
	b = new QPushButton( tr("Select"), rosterBox );
	connect( b, SIGNAL( released() ), SLOT( selectRosterFgColor() ) );
	
	settings.resetGroup();
	
	sounds = new QWidget( this );
	settings.beginGroup( "/sounds" );
	
	QGridLayout *sndLay = new QGridLayout( sounds, 8, 2 );
	
	addTab( sounds, tr("Sounds") );
	
	int size;
	
	label = new QLabel( tr( "Player:" ), sounds );
	sndLay->addWidget( label, 0, 0 );
	
	player = new QLineEdit( sounds );
#ifdef Q_OS_WIN
	player->setText( tr( "Microsoft Windows multimedia system" ) );
	player->setDisabled( true );
#else
	player->setText( settings.readEntry( "/player", "play" ) );
#endif
	sndLay->addWidget( player, 0, 1 );
	
	QGroupBox *sndBox = new QGroupBox( 4, Qt::Horizontal, tr("Sounds"), sounds );
	sndLay->addMultiCellWidget( sndBox, 1, 1, 0, 1 );
	
	sndMessage = new QCheckBox( tr( "Message:" ), sndBox );
	sndMessage->setChecked( settings.readBoolEntry( "/message/activated" ) );
	connect( sndMessage, SIGNAL( toggled( bool ) ), this, SLOT( sndMessageToggled( bool ) ) );
	
	sndEditMessage = new QLineEdit( settings.readEntry( "/message/fileName" ), sndBox );
	
	size = sndEditMessage->sizeHint().height();
	
	selectMessage = new QPushButton( sndBox );
	selectMessage->setPixmap( takePixmap( "fileopen" ) );
	selectMessage->setFixedSize( size, size );
	connect( selectMessage, SIGNAL( released() ), this, SLOT( selectMessageReleased() ) );
	
	playMessage = new QPushButton( sndBox );
	playMessage->setPixmap( takePixmap( "play" ) );
	playMessage->setFixedSize( size, size );
	connect( playMessage, SIGNAL( released() ), this, SLOT( playMessageReleased() ) );
	
	sndEditMessage->setEnabled( settings.readBoolEntry( "/message/activated" ) );
	selectMessage->setEnabled( settings.readBoolEntry( "/message/activated" ) );
	playMessage->setEnabled( settings.readBoolEntry( "/message/activated" ) );
	
	sndChatMessage = new QCheckBox( tr( "Chat message:" ), sndBox );
	sndChatMessage->setChecked( settings.readBoolEntry( "/chatMessage/activated" ) );
	connect( sndChatMessage, SIGNAL( toggled( bool ) ), this, SLOT( sndChatMessageToggled( bool ) ) );
	
	sndEditChatMessage = new QLineEdit( settings.readEntry( "/chatMessage/fileName" ), sndBox );
	
	selectChatMessage = new QPushButton( sndBox );
	selectChatMessage->setPixmap( takePixmap( "fileopen" ) );
	selectChatMessage->setFixedSize( size, size );
	connect( selectChatMessage, SIGNAL( released() ), this, SLOT( selectChatMessageReleased() ) );
	
	playChatMessage = new QPushButton( sndBox );
	playChatMessage->setPixmap( takePixmap( "play" ) );
	playChatMessage->setFixedSize( size, size );
	connect( playChatMessage, SIGNAL( released() ), this, SLOT( playChatMessageReleased() ) );
	
	sndEditChatMessage->setEnabled( settings.readBoolEntry( "/chatMessage/activated" ) );
	selectChatMessage->setEnabled( settings.readBoolEntry( "/chatMessage/activated" ) );
	playChatMessage->setEnabled( settings.readBoolEntry( "/chatMessage/activated" ) );
	
	sndErrorMessage = new QCheckBox( tr( "Error message:" ), sndBox );
	sndErrorMessage->setChecked( settings.readBoolEntry( "/errorMessage/activated" ) );
	connect( sndErrorMessage, SIGNAL( toggled( bool ) ), this, SLOT( sndErrorMessageToggled( bool ) ) );
	
	sndEditErrorMessage = new QLineEdit( settings.readEntry( "/errorMessage/fileName" ), sndBox );
	
	selectErrorMessage = new QPushButton( sndBox );
	selectErrorMessage->setPixmap( takePixmap( "fileopen" ) );
	selectErrorMessage->setFixedSize( size, size );
	connect( selectErrorMessage, SIGNAL( released() ), this, SLOT( selectErrorMessageReleased() ) );
	
	playErrorMessage = new QPushButton( sndBox );
	playErrorMessage->setPixmap( takePixmap( "play" ) );
	playErrorMessage->setFixedSize( size, size );
	connect( playErrorMessage, SIGNAL( released() ), this, SLOT( playErrorMessageReleased() ) );
	
	sndEditErrorMessage->setEnabled( settings.readBoolEntry( "/errorMessage/activated" ) );
	selectErrorMessage->setEnabled( settings.readBoolEntry( "/errorMessage/activated" ) );
	playErrorMessage->setEnabled( settings.readBoolEntry( "/errorMessage/activated" ) );
	
	sndNewsMessage = new QCheckBox( tr( "News message:" ), sndBox );
	sndNewsMessage->setChecked( settings.readBoolEntry( "/newsMessage/activated" ) );
	connect( sndNewsMessage, SIGNAL( toggled( bool ) ), this, SLOT( sndNewsMessageToggled( bool ) ) );
	
	sndEditNewsMessage = new QLineEdit( settings.readEntry( "/newsMessage/fileName" ), sndBox );
	
	selectNewsMessage = new QPushButton( sndBox );
	selectNewsMessage->setPixmap( takePixmap( "fileopen" ) );
	selectNewsMessage->setFixedSize( size, size );
	connect( selectNewsMessage, SIGNAL( released() ), this, SLOT( selectNewsMessageReleased() ) );
	
	playNewsMessage = new QPushButton( sndBox );
	playNewsMessage->setPixmap( takePixmap( "play" ) );
	playNewsMessage->setFixedSize( size, size );
	connect( playNewsMessage, SIGNAL( released() ), this, SLOT( playNewsMessageReleased() ) );
	
	sndEditNewsMessage->setEnabled( settings.readBoolEntry( "/newsMessage/activated" ) );
	selectNewsMessage->setEnabled( settings.readBoolEntry( "/newsMessage/activated" ) );
	playNewsMessage->setEnabled( settings.readBoolEntry( "/newsMessage/activated" ) );
	
	sndSoundAlert = new QCheckBox( tr( "Sound alert:" ), sndBox );
	sndSoundAlert->setChecked( settings.readBoolEntry( "/soundAlert/activated" ) );
	connect( sndSoundAlert, SIGNAL( toggled( bool ) ), this, SLOT( sndSoundAlertToggled( bool ) ) );
	
	sndEditSoundAlert = new QLineEdit( settings.readEntry( "/soundAlert/fileName" ), sndBox );
	
	selectSoundAlert = new QPushButton( sndBox );
	selectSoundAlert->setPixmap( takePixmap( "fileopen" ) );
	selectSoundAlert->setFixedSize( size, size );
	connect( selectSoundAlert, SIGNAL( released() ), this, SLOT( selectSoundAlertReleased() ) );
	
	playSoundAlert = new QPushButton( sndBox );
	playSoundAlert->setPixmap( takePixmap( "play" ) );
	playSoundAlert->setFixedSize( size, size );
	connect( playSoundAlert, SIGNAL( released() ), this, SLOT( playSoundAlertReleased() ) );
	
	sndEditSoundAlert->setEnabled( settings.readBoolEntry( "/soundAlert/activated" ) );
	selectSoundAlert->setEnabled( settings.readBoolEntry( "/soundAlert/activated" ) );
	playSoundAlert->setEnabled( settings.readBoolEntry( "/soundAlert/activated" ) );
	
	sndEmail = new QCheckBox( tr( "E-mail:" ), sndBox );
	sndEmail->setChecked( settings.readBoolEntry( "/email/activated" ) );
	connect( sndEmail, SIGNAL( toggled( bool ) ), this, SLOT( sndEmailToggled( bool ) ) );
	
	sndEditEmail = new QLineEdit( settings.readEntry( "/email/fileName" ), sndBox );
	
	selectEmail = new QPushButton( sndBox );
	selectEmail->setPixmap( takePixmap( "fileopen" ) );
	selectEmail->setFixedSize( size, size );
	connect( selectEmail, SIGNAL( released() ), this, SLOT( selectEmailReleased() ) );
	
	playEmail = new QPushButton( sndBox );
	playEmail->setPixmap( takePixmap( "play" ) );
	playEmail->setFixedSize( size, size );
	connect( playEmail, SIGNAL( released() ), this, SLOT( playEmailReleased() ) );
	
	sndEditEmail->setEnabled( settings.readBoolEntry( "/email/activated" ) );
	selectEmail->setEnabled( settings.readBoolEntry( "/email/activated" ) );
	playEmail->setEnabled( settings.readBoolEntry( "/email/activated" ) );
	
	sndWebMessage = new QCheckBox( tr( "Web message:" ), sndBox );
	sndWebMessage->setChecked( settings.readBoolEntry( "/webMessage/activated" ) );
	connect( sndWebMessage, SIGNAL( toggled( bool ) ), this, SLOT( sndWebMessageToggled( bool ) ) );
	
	sndEditWebMessage = new QLineEdit( settings.readEntry( "/webMessage/fileName" ), sndBox );
	
	selectWebMessage = new QPushButton( sndBox );
	selectWebMessage->setPixmap( takePixmap( "fileopen" ) );
	selectWebMessage->setFixedSize( size, size );
	connect( selectWebMessage, SIGNAL( released() ), this, SLOT( selectWebMessageReleased() ) );
	
	playWebMessage = new QPushButton( sndBox );
	playWebMessage->setPixmap( takePixmap( "play" ) );
	playWebMessage->setFixedSize( size, size );
	connect( playWebMessage, SIGNAL( released() ), this, SLOT( playWebMessageReleased() ) );
	
	sndEditWebMessage->setEnabled( settings.readBoolEntry( "/webMessage/activated" ) );
	selectWebMessage->setEnabled( settings.readBoolEntry( "/webMessage/activated" ) );
	playWebMessage->setEnabled( settings.readBoolEntry( "/webMessage/activated" ) );
	
	sndSubscribe = new QCheckBox( tr( "Subcribe request:" ), sndBox );
	sndSubscribe->setChecked( settings.readBoolEntry( "/subscribe/activated" ) );
	connect( sndSubscribe, SIGNAL( toggled( bool ) ), this, SLOT( sndSubscribeToggled( bool ) ) );
	
	sndEditSubscribe = new QLineEdit( settings.readEntry( "/subscribe/fileName" ), sndBox );
	
	selectSubscribe = new QPushButton( sndBox );
	selectSubscribe->setPixmap( takePixmap( "fileopen" ) );
	selectSubscribe->setFixedSize( size, size );
	connect( selectSubscribe, SIGNAL( released() ), this, SLOT( selectSubscribeReleased() ) );
	
	playSubscribe = new QPushButton( sndBox );
	playSubscribe->setPixmap( takePixmap( "play" ) );
	playSubscribe->setFixedSize( size, size );
	connect( playSubscribe, SIGNAL( released() ), this, SLOT( playSubscribeReleased() ) );
	
	sndEditSubscribe->setEnabled( settings.readBoolEntry( "/subscribe/activated" ) );
	selectSubscribe->setEnabled( settings.readBoolEntry( "/subscribe/activated" ) );
	playSubscribe->setEnabled( settings.readBoolEntry( "/subscribe/activated" ) );
	
	sndUnsubscribe = new QCheckBox( tr( "Unsubcribe request:" ), sndBox );
	sndUnsubscribe->setChecked( settings.readBoolEntry( "/unsubscribe/activated" ) );
	connect( sndUnsubscribe, SIGNAL( toggled( bool ) ), this, SLOT( sndUnsubscribeToggled( bool ) ) );
	
	sndEditUnsubscribe = new QLineEdit( settings.readEntry( "/unsubscribe/fileName" ), sndBox );
	
	selectUnsubscribe = new QPushButton( sndBox );
	selectUnsubscribe->setPixmap( takePixmap( "fileopen" ) );
	selectUnsubscribe->setFixedSize( size, size );
	connect( selectUnsubscribe, SIGNAL( released() ), this, SLOT( selectUnsubscribeReleased() ) );
	
	playUnsubscribe = new QPushButton( sndBox );
	playUnsubscribe->setPixmap( takePixmap( "play" ) );
	playUnsubscribe->setFixedSize( size, size );
	connect( playUnsubscribe, SIGNAL( released() ), this, SLOT( playUnsubscribeReleased() ) );
	
	sndEditUnsubscribe->setEnabled( settings.readBoolEntry( "/unsubscribe/activated" ) );
	selectUnsubscribe->setEnabled( settings.readBoolEntry( "/unsubscribe/activated" ) );
	playUnsubscribe->setEnabled( settings.readBoolEntry( "/unsubscribe/activated" ) );
	
	sndAvailableStatus = new QCheckBox( tr( "Available status:" ), sndBox );
	sndAvailableStatus->setChecked( settings.readBoolEntry( "/status/available/activated" ) );
	connect( sndAvailableStatus, SIGNAL( toggled( bool ) ), this, SLOT( sndAvailableStatusToggled( bool ) ) );
	
	sndEditAvailableStatus = new QLineEdit( settings.readEntry( "/status/available/fileName" ), sndBox );
	
	selectAvailableStatus = new QPushButton( sndBox );
	selectAvailableStatus->setPixmap( takePixmap( "fileopen" ) );
	selectAvailableStatus->setFixedSize( size, size );
	connect( selectAvailableStatus, SIGNAL( released() ), this, SLOT( selectAvailableStatusReleased() ) );
	
	playAvailableStatus = new QPushButton( sndBox );
	playAvailableStatus->setPixmap( takePixmap( "play" ) );
	playAvailableStatus->setFixedSize( size, size );
	connect( playAvailableStatus, SIGNAL( released() ), this, SLOT( playAvailableStatusReleased() ) );
	
	sndEditAvailableStatus->setEnabled( settings.readBoolEntry( "/status/available/activated" ) );
	selectAvailableStatus->setEnabled( settings.readBoolEntry( "/status/available/activated" ) );
	playAvailableStatus->setEnabled( settings.readBoolEntry( "/status/available/activated" ) );
	
	sndAwayStatus = new QCheckBox( tr( "Away status:" ), sndBox );
	sndAwayStatus->setChecked( settings.readBoolEntry( "/status/away/activated" ) );
	connect( sndAwayStatus, SIGNAL( toggled( bool ) ), this, SLOT( sndAwayStatusToggled( bool ) ) );
	
	sndEditAwayStatus = new QLineEdit( settings.readEntry( "/status/away/fileName" ), sndBox );
	
	selectAwayStatus = new QPushButton( sndBox );
	selectAwayStatus->setPixmap( takePixmap( "fileopen" ) );
	selectAwayStatus->setFixedSize( size, size );
	connect( selectAwayStatus, SIGNAL( released() ), this, SLOT( selectAwayStatusReleased() ) );
	
	playAwayStatus = new QPushButton( sndBox );
	playAwayStatus->setPixmap( takePixmap( "play" ) );
	playAwayStatus->setFixedSize( size, size );
	connect( playAwayStatus, SIGNAL( released() ), this, SLOT( playAwayStatusReleased() ) );
	
	sndEditAwayStatus->setEnabled( settings.readBoolEntry( "/status/away/activated" ) );
	selectAwayStatus->setEnabled( settings.readBoolEntry( "/status/away/activated" ) );
	playAwayStatus->setEnabled( settings.readBoolEntry( "/status/away/activated" ) );
	
	sndUnavailableStatus = new QCheckBox( tr( "Unavailable status:" ), sndBox );
	sndUnavailableStatus->setChecked( settings.readBoolEntry( "/status/unavailable/activated" ) );
	connect( sndUnavailableStatus, SIGNAL( toggled( bool ) ), this, SLOT( sndUnavailableStatusToggled( bool ) ) );
	
	sndEditUnavailableStatus = new QLineEdit( settings.readEntry( "/status/unavailable/fileName" ), sndBox );
	
	selectUnavailableStatus = new QPushButton( sndBox );
	selectUnavailableStatus->setPixmap( takePixmap( "fileopen" ) );
	selectUnavailableStatus->setFixedSize( size, size );
	connect( selectUnavailableStatus, SIGNAL( released() ), this, SLOT( selectUnavailableStatusReleased() ) );
	
	playUnavailableStatus = new QPushButton( sndBox );
	playUnavailableStatus->setPixmap( takePixmap( "play" ) );
	playUnavailableStatus->setFixedSize( size, size );
	connect( playUnavailableStatus, SIGNAL( released() ), this, SLOT( playUnavailableStatusReleased() ) );
	
	sndEditUnavailableStatus->setEnabled( settings.readBoolEntry( "/status/unavailable/activated" ) );
	selectUnavailableStatus->setEnabled( settings.readBoolEntry( "/status/unavailable/activated" ) );
	playUnavailableStatus->setEnabled( settings.readBoolEntry( "/status/unavailable/activated" ) );
	
	settings.resetGroup();
	
	proxy = new QWidget( this );
	settings.beginGroup( "/proxy" );
	
	QGridLayout *proLay = new QGridLayout( proxy, 3, 1 );
	
	addTab( proxy, tr("Proxy") );
	
	proxyBox = new QGroupBox( 2, Qt::Horizontal, tr("Proxy server"), proxy );
	proxyBox->setCheckable( true );
	proxyBox->setChecked( settings.readBoolEntry( "/useProxy" )  );
	
	(void)new QLabel( tr("IP address:"), proxyBox );
	proxyIp = new QLineEdit( settings.readEntry( "/ip", "0.0.0.0" ), proxyBox );
	
	(void)new QLabel( tr("Port:"), proxyBox );
	QString port;
	proxyPort = new QLineEdit( port.setNum(settings.readNumEntry( "/port", 0 )), proxyBox );
	QIntValidator v( proxyPort );
	proxyPort->setValidator( &v );
	
	(void)new QLabel( tr("Username:"), proxyBox );
	proxyUsername = new QLineEdit( settings.readEntry( "/username" ), proxyBox );
	
	(void)new QLabel( tr("Password:"), proxyBox );
	proxyPassword = new QLineEdit( settings.readEntry( "/password" ), proxyBox );
	proxyPassword->setEchoMode( QLineEdit::Password );
	
	proLay->addWidget( proxyBox, 0, 0 );
	
	settings.resetGroup();
	
	setOkButton( tr("OK") );
	setApplyButton( tr("Apply") );
	setCancelButton( tr("Cancel") );
	
	connect( this, SIGNAL(applyButtonPressed()), this, SLOT(apply()) );
	connect( this, SIGNAL(cancelButtonPressed()), this, SLOT(cancel()) );
}

void SettingsDialog::apply()
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/connection" );
	
	settings.writeEntry( "/username", username->text() );
	settings.writeEntry( "/password", password->text() );
	settings.writeEntry( "/nick", nick->text() );
	
	if( username->edited() || password->edited() )
		tlen_manager->setUserPass( username->text(), password->text() );
	
	settings.writeEntry( "/autoConnect", acBox->isChecked() );
	settings.writeEntry( "/defaultStatus", defaultStatus->currentItem() );
	settings.writeEntry( "/defaultDescription", defaultDescription->text() );
	
	settings.writeEntry( "/autoReconnect", autoReconnect->isChecked() );
	
	settings.resetGroup();
	settings.beginGroup( "/general" );
	
	switch( language->currentItem() )
	{
		case 0:
			settings.writeEntry( "/language", "en" );
			break;
		case 1:
			settings.writeEntry( "/language", "pl" );
			break;
	}
	
	settings.writeEntry( "/email", email->isChecked() );
	settings.writeEntry( "/webmsg", webmsg->isChecked() );
	settings.writeEntry( "/newVersion", newVersion->isChecked() );
	settings.writeEntry( "/returnSend", returnSend->isChecked() );
	
	settings.writeEntry( "/trayicon/activated", tray->isChecked() );
	settings.writeEntry( "/trayicon/hideOnStart", hideOnStart->isChecked() );
	settings.writeEntry( "/trayicon/windowMakeTray", windowMakerTray->isChecked() );
	
	QTlen *tlen = (QTlen*)qApp->mainWidget();
	
	if( tray->isChecked() )
	{
		tlen->activeTrayIcon();
		tlen->updateTrayIcon();
	}
	else
	{
		tlen->disactiveTrayIcon();
	}
	
	settings.endGroup();
	settings.beginGroup( "/look" );
	
	settings.writeEntry( "/bgColor", bgColor->text() );
	settings.writeEntry( "/myFontColor", myFontColor->text() );
	settings.writeEntry( "/contactFontColor", contactFontColor->text() );
	settings.writeEntry( "/chatFontColor", chatFontColor->text() );
	
	settings.writeEntry( "/chatFont", chatFont->text() );
	
	settings.writeEntry( "/showToolTip", toolTip->isChecked() );
	settings.writeEntry( "/descriptionUnderContact", descriptionUnderContact->isChecked() );
	
	settings.writeEntry( "/roster/background", rosterBgColor->text() );
	settings.writeEntry( "/roster/foreground", rosterFgColor->text() );
	
	settings.resetGroup();
	settings.beginGroup( "/sounds" );
	
	settings.writeEntry( "/player", player->text() );
	
	settings.writeEntry( "/message/activated", sndMessage->isChecked() );
	settings.writeEntry( "/message/fileName", sndEditMessage->text() );
	
	settings.writeEntry( "/chatMessage/activated", sndChatMessage->isChecked() );
	settings.writeEntry( "/chatMessage/fileName", sndEditChatMessage->text() );
	
	settings.writeEntry( "/errorMessage/activated", sndErrorMessage->isChecked() );
	settings.writeEntry( "/errorMessage/fileName", sndEditErrorMessage->text() );
	
	settings.writeEntry( "/newsMessage/activated", sndNewsMessage->isChecked() );
	settings.writeEntry( "/newsMessage/fileName", sndEditNewsMessage->text() );
	
	settings.writeEntry( "/soundAlert/activated", sndSoundAlert->isChecked() );
	settings.writeEntry( "/soundAlert/fileName", sndEditSoundAlert->text() );
	
	settings.writeEntry( "/email/activated", sndEmail->isChecked() );
	settings.writeEntry( "/email/fileName", sndEditEmail->text() );
	
	settings.writeEntry( "/webMessage/activated", sndWebMessage->isChecked() );
	settings.writeEntry( "/webMessage/fileName", sndEditWebMessage->text() );
	
	settings.writeEntry( "/subscribe/activated", sndSubscribe->isChecked() );
	settings.writeEntry( "/subscribe/fileName", sndEditSubscribe->text() );
	
	settings.writeEntry( "/unsubscribe/activated", sndUnsubscribe->isChecked() );
	settings.writeEntry( "/unsubscribe/fileName", sndEditUnsubscribe->text() );
	
	settings.writeEntry( "/status/available/activated", sndAvailableStatus->isChecked() );
	settings.writeEntry( "/status/available/fileName", sndEditAvailableStatus->text() );
	
	settings.writeEntry( "/status/away/activated", sndAwayStatus->isChecked() );
	settings.writeEntry( "/status/away/fileName", sndEditAwayStatus->text() );
	
	settings.writeEntry( "/status/unavailable/activated", sndUnavailableStatus->isChecked() );
	settings.writeEntry( "/status/unavailable/fileName", sndEditUnavailableStatus->text() );
	
	sound_manager->update();
	
	settings.resetGroup();
	
	if( email->isChecked() )
		connect( msg_manager, SIGNAL( gotEmail( QString, const QString& ) ), msg_manager, SLOT( mailMessage( QString, const QString& ) ) );
	else
		disconnect( msg_manager, SIGNAL( gotEmail( QString, const QString & ) ), msg_manager, SLOT( mailMessage( QString, const QString & ) ) );
	
	//if( webmsg->isChecked() )
	//	connect( tlen_manager, SIGNAL(gotWebMessage( QString, QString, QString, const QString& )), msg_manager, SLOT(webMessage(QString, QString, QString, const QString& )) );
	//else
	//	disconnect( tlen_manager, SIGNAL(gotWebMessage( QString, QString, QString, const QString& )), msg_manager, SLOT(webMessage(QString, QString, QString, const QString& )) );
}

void SettingsDialog::cancel()
{
	QSettings settings;
	settings.setPath( "qtlen.sf.net", "QTlen" );
	
	settings.beginGroup( "/connection" );
	
	username->setText( settings.readEntry( "/username" ) );
	password->setText( settings.readEntry( "/password" ) );
	nick->setText( settings.readEntry( "/nick" , tr("Me") ) );
	
	acBox->setChecked( settings.readBoolEntry( "/autoConnect", false ) );
	defaultStatus->setCurrentItem( settings.readNumEntry( "/defaultStatus" ) );
	defaultDescription->setText( settings.readEntry( "/defaultDescription" ) );
	autoReconnect->setChecked( settings.readBoolEntry( "/autoReconnect" ) );
	
	settings.resetGroup();
	settings.beginGroup( "/general" );
	
	QString lang = settings.readEntry( "/language", QTextCodec::locale() );
	if( lang == "en" )
		language->setCurrentItem( 0 );
	else if( lang == "pl" )
		language->setCurrentItem( 1 );
	else
		language->setCurrentItem( 0 );
	
	email->setChecked( settings.readBoolEntry( "/email", true ) );
	webmsg->setChecked( settings.readBoolEntry( "/webmsg", true ) );
	newVersion->setChecked( settings.readBoolEntry( "/newVersion", true ) );
	returnSend->setChecked( settings.readBoolEntry( "/returnSend", true ) );
	
	tray->setChecked( settings.readBoolEntry( "/trayicon/activated" ) );
	hideOnStart->setChecked( settings.readBoolEntry( "/trayicon/hideOnStart" ) );
	windowMakerTray->setChecked( settings.readBoolEntry( "/trayicon/windowMakeTray" ) );
	
	settings.resetGroup();
	settings.beginGroup( "/look" );
	
	bgColor->setText( settings.readEntry( "/bgColor", "#ffffff" ) );
	bgColor->setPaletteBackgroundColor( bgColor->text() );
	chatFontColor->setPaletteBackgroundColor( bgColor->text() );
	myFontColor->setPaletteBackgroundColor( bgColor->text() );
	contactFontColor->setPaletteBackgroundColor( bgColor->text() );
	myFontColor->setText( settings.readEntry( "/myFontColor", "#ff0000" ) );
	myFontColor->setPaletteForegroundColor( myFontColor->text() );
	contactFontColor->setText( settings.readEntry( "/contactFontColor", "#0000ff" ) );
	contactFontColor->setPaletteForegroundColor( contactFontColor->text() );
	chatFontColor->setText( settings.readEntry( "/chatFontColor", "#000000" ) );
	chatFontColor->setPaletteForegroundColor( chatFontColor->text() );
	
	chatFont->setText( settings.readEntry( "/chatFont" ) );
	QFont font;
	font.fromString( chatFont->text() );
	chatFont->setFont( font );
	
	toolTip->setChecked( settings.readBoolEntry( "/showToolTip", true ) );
	descriptionUnderContact->setChecked( settings.readBoolEntry( "/descriptionUnderContact", false ) );
	
	rosterBgColor->setText( settings.readEntry( "/roster/background", "#eeeeee" ) );
	rosterFgColor->setText( settings.readEntry( "/roster/foreground", "#000000" ) );
	
	settings.resetGroup();
	settings.beginGroup( "/sounds" );
	
	player->setText( settings.readEntry( "/player" ) );
	
	sndMessage->setChecked( settings.readBoolEntry( "/message/activated" ) );
	sndChatMessage->setChecked( settings.readBoolEntry( "/chatMessage/activated" ) );
	sndErrorMessage->setChecked( settings.readBoolEntry( "/errorMessage/activated" ) );
	sndNewsMessage->setChecked( settings.readBoolEntry( "/newsMessage/activated" ) );
	sndSoundAlert->setChecked( settings.readBoolEntry( "/soundAlert/activated" ) );
	sndEmail->setChecked( settings.readBoolEntry( "/email/activated" ) );
	sndWebMessage->setChecked( settings.readBoolEntry( "/webMessage/activated" ) );
	sndSubscribe->setChecked( settings.readBoolEntry( "/subscribe/activated" ) );
	sndUnsubscribe->setChecked( settings.readBoolEntry( "/unsubscribe/activated" ) );
	sndAvailableStatus->setChecked( settings.readBoolEntry( "/status/available/activated" ) );
	sndAwayStatus->setChecked( settings.readBoolEntry( "/status/away/activated" ) );
	sndUnavailableStatus->setChecked( settings.readBoolEntry( "/status/unavailable/activated" ) );
	
	sndEditMessage->setText( settings.readEntry( "/message/fileName" ) );
	sndEditChatMessage->setText( settings.readEntry( "/chatMessage/fileName" ) );
	sndEditErrorMessage->setText( settings.readEntry( "/errorMessage/fileName" ) );
	sndEditNewsMessage->setText( settings.readEntry( "/newsMessage/fileName" ) );
	sndEditSoundAlert->setText( settings.readEntry( "/soundAlert/fileName" ) );
	sndEditEmail->setText( settings.readEntry( "/email/fileName" ) );
	sndEditWebMessage->setText( settings.readEntry( "/webMessage/fileName" ) );
	sndEditSubscribe->setText( settings.readEntry( "/subscribe/fileName" ) );
	sndEditUnsubscribe->setText( settings.readEntry( "/unsubscribe/fileName" ) );
	sndEditAvailableStatus->setText( settings.readEntry( "/status/available/fileName" ) );
	sndEditAwayStatus->setText( settings.readEntry( "/status/away/fileName" ) );
	sndEditUnavailableStatus->setText( settings.readEntry( "/status/unavailable/fileName" ) );
	
	settings.resetGroup();
	settings.beginGroup( "/proxy" );
	
	proxyBox->setChecked( settings.readBoolEntry( "/useProxy" ) );
	
	proxyIp->setText( settings.readEntry( "/ip", "0.0.0.0" ) );
	QString port;
	proxyPort->setText( port.setNum(settings.readNumEntry( "/port", 0 )) );
	proxyUsername->setText( settings.readEntry( "/username" ) );
	proxyPassword->setText( settings.readEntry( "/password" ) );
	
	settings.resetGroup();
}

void SettingsDialog::selectBgColor()
{
	bgColor->setText( QColorDialog::getColor( bgColor->text(), this ).name() );
	bgColor->setPaletteBackgroundColor( bgColor->text() );
	chatFontColor->setPaletteBackgroundColor( bgColor->text() );
	myFontColor->setPaletteBackgroundColor( bgColor->text() );
	contactFontColor->setPaletteBackgroundColor( bgColor->text() );
}

void SettingsDialog::selectMyColor()
{
	myFontColor->setText( QColorDialog::getColor( myFontColor->text(), this ).name() );
	myFontColor->setPaletteForegroundColor( myFontColor->text() );
}

void SettingsDialog::selectContactColor()
{
	contactFontColor->setText( QColorDialog::getColor( contactFontColor->text(), this ).name() );
	contactFontColor->setPaletteForegroundColor( contactFontColor->text() );
}

void SettingsDialog::selectChatFont()
{
	bool ok;
	QFont font;
	font.fromString( chatFont->text() );
	font = QFontDialog::getFont( &ok, font, this );

	if( ok )
		chatFont->setText( font.toString() );
	chatFont->setFont( font );
}

void SettingsDialog::selectChatFontColor()
{
	chatFontColor->setText( QColorDialog::getColor( chatFontColor->text(), this ).name() );
	chatFontColor->setPaletteForegroundColor( chatFontColor->text() );
}

void SettingsDialog::selectRosterBgColor()
{
	rosterBgColor->setText( QColorDialog::getColor( rosterBgColor->text(), this ).name() );
}

void SettingsDialog::selectRosterFgColor()
{
	rosterFgColor->setText( QColorDialog::getColor( rosterFgColor->text(), this ).name() );
}

void SettingsDialog::sndMessageToggled( bool on )
{
	sndEditMessage->setEnabled( on );
	selectMessage->setEnabled( on );
	playMessage->setEnabled( on );
}

void SettingsDialog::sndChatMessageToggled( bool on )
{
	sndEditChatMessage->setEnabled( on );
	selectChatMessage->setEnabled( on );
	playChatMessage->setEnabled( on );
}

void SettingsDialog::sndErrorMessageToggled( bool on )
{
	sndEditErrorMessage->setEnabled( on );
	selectErrorMessage->setEnabled( on );
	playErrorMessage->setEnabled( on );
}

void SettingsDialog::sndNewsMessageToggled( bool on )
{
	sndEditNewsMessage->setEnabled( on );
	selectNewsMessage->setEnabled( on );
	playNewsMessage->setEnabled( on );
}

void SettingsDialog::sndSoundAlertToggled( bool on )
{
	sndEditSoundAlert->setEnabled( on );
	selectSoundAlert->setEnabled( on );
	playSoundAlert->setEnabled( on );
}

void SettingsDialog::sndEmailToggled( bool on )
{
	sndEditEmail->setEnabled( on );
	selectEmail->setEnabled( on );
	playEmail->setEnabled( on );
}

void SettingsDialog::sndWebMessageToggled( bool on )
{
	sndEditWebMessage->setEnabled( on );
	selectWebMessage->setEnabled( on );
	playWebMessage->setEnabled( on );
}

void SettingsDialog::sndSubscribeToggled( bool on )
{
	sndEditSubscribe->setEnabled( on );
	selectSubscribe->setEnabled( on );
	playSubscribe->setEnabled( on );
}

void SettingsDialog::sndUnsubscribeToggled( bool on )
{
	sndEditUnsubscribe->setEnabled( on );
	selectUnsubscribe->setEnabled( on );
	playUnsubscribe->setEnabled( on );
}

void SettingsDialog::sndAvailableStatusToggled( bool on )
{
	sndEditAvailableStatus->setEnabled( on );
	selectAvailableStatus->setEnabled( on );
	playAvailableStatus->setEnabled( on );
}

void SettingsDialog::sndAwayStatusToggled( bool on )
{
	sndEditAwayStatus->setEnabled( on );
	selectAwayStatus->setEnabled( on );
	playAwayStatus->setEnabled( on );
}

void SettingsDialog::sndUnavailableStatusToggled( bool on )
{
	sndEditUnavailableStatus->setEnabled( on );
	selectUnavailableStatus->setEnabled( on );
	playUnavailableStatus->setEnabled( on );
}

void SettingsDialog::selectMessageReleased()
{
	sndEditMessage->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectChatMessageReleased()
{
	sndEditChatMessage->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectErrorMessageReleased()
{
	sndEditErrorMessage->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectNewsMessageReleased()
{
	sndEditNewsMessage->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectSoundAlertReleased()
{
	sndEditSoundAlert->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectEmailReleased()
{
	sndEditEmail->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectWebMessageReleased()
{
	sndEditWebMessage->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectSubscribeReleased()
{
	sndEditSubscribe->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectUnsubscribeReleased()
{
	sndEditUnsubscribe->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectAvailableStatusReleased()
{
	sndEditAvailableStatus->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectAwayStatusReleased()
{
	sndEditAwayStatus->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::selectUnavailableStatusReleased()
{
	sndEditUnavailableStatus->setText( QFileDialog::getOpenFileName( QDir::home().path(), tr( "Sounds (*.wav)" ), this, "open file dialog", tr( "Choose a file to open" ) ) );
}

void SettingsDialog::playMessageReleased()
{
	sound_manager->play( sndEditMessage->text() );
}

void SettingsDialog::playChatMessageReleased()
{
	sound_manager->play( sndEditChatMessage->text() );
}

void SettingsDialog::playErrorMessageReleased()
{
	sound_manager->play( sndEditErrorMessage->text() );
}

void SettingsDialog::playNewsMessageReleased()
{
	sound_manager->play( sndEditNewsMessage->text() );
}

void SettingsDialog::playSoundAlertReleased()
{
	sound_manager->play( sndEditSoundAlert->text() );
}

void SettingsDialog::playEmailReleased()
{
	sound_manager->play( sndEditEmail->text() );
}

void SettingsDialog::playWebMessageReleased()
{
	sound_manager->play( sndEditWebMessage->text() );
}

void SettingsDialog::playSubscribeReleased()
{
	sound_manager->play( sndEditSubscribe->text() );
}

void SettingsDialog::playUnsubscribeReleased()
{
	sound_manager->play( sndEditUnsubscribe->text() );
}

void SettingsDialog::playAvailableStatusReleased()
{
	sound_manager->play( sndEditAvailableStatus->text() );
}

void SettingsDialog::playAwayStatusReleased()
{
	sound_manager->play( sndEditAwayStatus->text() );
}

void SettingsDialog::playUnavailableStatusReleased()
{
	sound_manager->play( sndEditUnavailableStatus->text() );
}
