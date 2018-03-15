/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2009 by Artem A. Senichev <artemsen@gmail.com>     *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#include "settings.h"
#include "buffer.h"

#define PW_SERIALIZE_MAPID		"MapID"					///< Map ID field
#define PW_SERIALIZE_MAPSTATE	"MapState"				///< Map state field
#define PW_SERIALIZE_MAPSIZE	"MapSize"				///< Map size field
#define PW_SERIALIZE_FILENAME	".pipewalker.save"		///< Filename to save state

CSettings::SETTINGS CSettings::Settings;


void CSettings::Load()
{
	//Set default values
	Settings.Id = 0;
	Settings.Size = MapSizeNormal;

	const string fileName = GetFileName();

	try {
		CIniFile ini;
		ini.Load(fileName.c_str());

		if (ini.ExistProperty(PW_SERIALIZE_MAPSTATE))
			Settings.State = ini.GetProperty(PW_SERIALIZE_MAPSTATE);

		if (ini.ExistProperty(PW_SERIALIZE_MAPID))
			Settings.Id = atoi(ini.GetProperty(PW_SERIALIZE_MAPID).c_str());

		if (ini.ExistProperty(PW_SERIALIZE_MAPSIZE))
			Settings.Size = static_cast<MapSize>(atoi(ini.GetProperty(PW_SERIALIZE_MAPSIZE).c_str()));
	}
	catch (CException& /*ex*/) {
	}
}


void CSettings::Save()
{
	const string fileName = GetFileName();

	try {
		CIniFile ini;

		ini.SetProperty(PW_SERIALIZE_MAPID, Settings.Id);
		ini.SetProperty(PW_SERIALIZE_MAPSTATE, Settings.State.c_str());
		ini.SetProperty(PW_SERIALIZE_MAPSIZE, Settings.Size);

		ini.Save(fileName.c_str());
	}
	catch (CException& /*ex*/) {
	}
}


string CSettings::GetFileName()
{
	string loadFileName = getenv(
#ifdef WIN32
		"USERPROFILE"
#else
		"HOME"
#endif //WIN32
		);

#ifdef WIN32
	loadFileName += '\\';
#else
	loadFileName += '/';
#endif //WIN32

	loadFileName += PW_SERIALIZE_FILENAME;
	return loadFileName;
}


void CIniFile::Load(const char* fileName)
{
	assert(fileName);

	CBuffer buf;
	buf.Load(fileName);

	while (!buf.EOB()) {
		string name;
		name.reserve(32);
		while (!buf.EOB()) {
			char c = 0;
			buf.Get(c);
			if (c == '\r')
				continue;
			if (c != '=')
				name += c;
			else
				break;
		}

		string val;
		val.reserve(32);
		while (!buf.EOB()) {
			char c = 0;
			buf.Get(c);
			if (c == '\r')
				continue;
			if (c != '\n')
				val += c;
			else
				break;
		}

		SetProperty(name.c_str(), val.c_str());
	}
}


void CIniFile::Save(const char* fileName) const
{
	assert(fileName);

	CBuffer buf;

	for (map<string, string>::const_iterator it = m_Ini.begin(); it != m_Ini.end(); ++it) {
		buf.Put(it->first);
		buf.PutData("=", 1);
		buf.Put(it->second);
		buf.PutData("\n", 1);
	}

	buf.Save(fileName);
}


void CIniFile::SetProperty(const char* name, const unsigned int value)
{
	char sz[16];
	sprintf(sz, "%i", value);
	SetProperty(name, sz);
}


void CIniFile::SetProperty(const char* name, const char* value)
{
	assert(name);
	assert(value);
	m_Ini.insert(make_pair(name, value));

}

string CIniFile::GetProperty(const char* name) const
{
	assert(name);

	map<string, string>::const_iterator it = m_Ini.find(name);
	return (it == m_Ini.end() ? string() : it->second);
}


bool CIniFile::ExistProperty(const char* name) const
{
	assert(name);
	return m_Ini.find(name) != m_Ini.end();
}