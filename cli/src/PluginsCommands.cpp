/*
 * PluginsCommands.cpp - implementation of PluginsCommands class
 *
 * Copyright (c) 2019-2020 Tobias Junghans <tobydox@veyon.io>
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

#include "CommandLineIO.h"
#include "PluginsCommands.h"
#include "PluginManager.h"


PluginsCommands::PluginsCommands( QObject* parent ) :
	QObject( parent ),
	m_commands( {
		{ QStringLiteral("list"), tr( "List names of all installed plugins" ) },
		{ QStringLiteral("show"), tr( "Show table with details of all installed plugins" ) },
		} )
{
}



QStringList PluginsCommands::commands() const
{
	return m_commands.keys();
}



QString PluginsCommands::commandHelp( const QString& command ) const
{
	return m_commands.value( command );
}



CommandLinePluginInterface::RunResult PluginsCommands::handle_list( const QStringList& arguments )
{
	Q_UNUSED(arguments)

	for( auto plugin : VeyonCore::pluginManager().pluginInterfaces() )
	{
		print( plugin->name() );
	}

	return NoResult;
}



CommandLinePluginInterface::RunResult PluginsCommands::handle_show( const QStringList& arguments )
{
	Q_UNUSED(arguments)

	TableHeader tableHeader( { tr("Name"), tr("Description"), tr("Version"), tr("UID") } );
	TableRows tableRows;

	tableRows.reserve( VeyonCore::pluginManager().pluginInterfaces().count() );

	for( auto plugin : VeyonCore::pluginManager().pluginInterfaces() )
	{
		tableRows.append( {
			plugin->name(),
			plugin->description(),
			plugin->version().toString(),
			VeyonCore::formattedUuid( plugin->uid().toString() )
		} );
	}

	std::sort( tableRows.begin(), tableRows.end(), []( const TableRow& a, const TableRow& b ) {
		return a.first() < b.first();
	}) ;

	printTable( Table( tableHeader, tableRows ) );

	return NoResult;
}
