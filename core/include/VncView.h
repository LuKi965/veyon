/*
 * VncView.h - abstract base for all VNC views
 *
 * Copyright (c) 2006-2020 Tobias Junghans <tobydox@veyon.io>
 *
 * This file is part of Veyon - https://veyon.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#pragma once

#include <QEvent>
#include <QPixmap>

#include "KeyboardShortcutTrapper.h"
#include "VncConnection.h"

class QHoverEvent;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class KeyboardShortcutTrapper;
class VncConnection;

class VEYON_CORE_EXPORT VncView
{
public:
	enum Modes
	{
		RemoteControlMode,
		DemoMode,
		NumModes
	} ;
	using Mode = Modes;

	enum Shortcut
	{
		ShortcutCtrlAltDel,
		ShortcutCtrlEscape,
		ShortcutAltTab,
		ShortcutAltF4,
		ShortcutWinTab,
		ShortcutWin,
		ShortcutMenu,
		ShortcutAltCtrlF1,
		ShortcutCount
	} ;

	VncView( VncConnection* connection );
	virtual ~VncView();

	VncConnection* connection() const
	{
		return m_connection;
	}

	bool viewOnly() const
	{
		return m_viewOnly;
	}

	QSize scaledSize() const;
	QSize framebufferSize() const
	{
		return m_framebufferSize;
	}

	virtual void setViewOnly( bool viewOnly );
	void sendShortcut( Shortcut shortcut );

protected:
	template<class SubClass>
	void connectUpdateFunctions( SubClass* object )
	{
		QObject::connect( connection(), &VncConnection::imageUpdated, object,
						  [this](int x, int y, int w, int h) { updateImage( x, y, w, h ); } );
		QObject::connect( connection(), &VncConnection::framebufferSizeChanged, object,
						  [this]( int w, int h ) { updateFramebufferSize( w, h ); } );

		QObject::connect( connection(), &VncConnection::cursorPosChanged, object,
						  [this]( int x, int y ) { updateCursorPos( x, y ); } );
		QObject::connect( connection(), &VncConnection::cursorShapeUpdated, object,
						  [this]( const QPixmap& cursorShape, int xh, int yh ) { updateCursorShape( cursorShape, xh, yh ); } );
	}

	virtual void updateView( int x, int y, int w, int h ) = 0;
	virtual QSize viewSize() const = 0;
	virtual void setViewCursor( const QCursor& cursor ) = 0;

	virtual void updateCursorPos( int x, int y );
	virtual void updateCursorShape( const QPixmap& cursorShape, int xh, int yh );
	virtual void updateFramebufferSize( int w, int h );
	virtual void updateImage( int x, int y, int w, int h );

	void unpressModifiers();

	void handleShortcut( KeyboardShortcutTrapper::Shortcut shortcut );
	bool handleEvent( QEvent* handleEvent );

	virtual void hoverEventHandler( QHoverEvent* event );
	virtual void keyEventHandler( QKeyEvent* event );
	virtual void mouseEventHandler( QMouseEvent* event );
	virtual void wheelEventHandler( QWheelEvent* event );

	bool isScaledView() const;

	auto cursorShape() const
	{
		return m_cursorShape;
	}

	auto cursorPos() const
	{
		return m_cursorPos;
	}

	auto cursorHot() const
	{
		return m_cursorHot;
	}

	qreal scaleFactor() const;
	QPoint mapToFramebuffer( QPoint pos );
	QRect mapFromFramebuffer( QRect rect );

	void updateLocalCursor();

private:
	void updatePaintedCursor();
	void pressKey( unsigned int key );
	void unpressKey( unsigned int key );

	VncConnection* m_connection;
	QPixmap m_cursorShape;
	QPoint m_cursorPos;
	QPoint m_cursorHot;
	QSize m_framebufferSize;
	bool m_viewOnly;

	int m_buttonMask;
	QMap<unsigned int, bool> m_mods;

	KeyboardShortcutTrapper* m_keyboardShortcutTrapper;

} ;
