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

#include "cell.h"
#include "synchro.h"


CCell::CCell()
{
	Reset();
}


CCell::~CCell()
{
}


void CCell::Reset()
{
	m_TubeType = TTNone;
	m_CellType = CTFree;
	m_Angle = 0.0f;
	m_State = false;
	m_Lock = false;
	m_ConnTop = false;
	m_ConnBottom = false;
	m_ConnLeft = false;
	m_ConnRight = false;
	m_Used = false;
}


string CCell::Save() const
{
	char state[32];
	sprintf(state, "%i,%i,%03i,%i,%i,%i,%i,%i", m_TubeType, m_CellType, static_cast<int>(m_Angle), m_Lock ? 1 : 0,
		m_ConnTop ? 1 : 0,
		m_ConnBottom  ? 1 : 0,
		m_ConnLeft ? 1 : 0,
		m_ConnRight ? 1 : 0);

	return string(state);
}


void CCell::Load(const string& state)
{
	m_TubeType = static_cast<TubeType>(atoi(state.c_str()));
	m_CellType = static_cast<CellType>(atoi(state.c_str() + 2));
	m_Angle = static_cast<float>(atof(state.c_str() + 4));
	m_Lock = atoi(state.c_str() + 8) != 0;
	m_ConnTop = atoi(state.c_str() + 10) != 0;
	m_ConnBottom = atoi(state.c_str() + 12) != 0;
	m_ConnLeft = atoi(state.c_str() + 14) != 0;
	m_ConnRight = atoi(state.c_str() + 16) != 0;
}


void CCell::AddTube(const ConnectionSide side)
{
	assert(CanAddTube());

	//Already connected?
	assert(!(m_ConnTop && side == CSTop));
	assert(!(m_ConnBottom && side == CSBottom));
	assert(!(m_ConnLeft && side == CSLeft));
	assert(!(m_ConnRight && side == CSRight));

	//Add connection
	switch (side) {
		case CSTop:		m_ConnTop = true;		break;
		case CSBottom:	m_ConnBottom = true;	break;
		case CSLeft:	m_ConnLeft = true;		break;
		case CSRight:	m_ConnRight = true;		break;
		default:
			assert(false && "Undefined connection side");
			break;
	}
	//Define cell type
	if (m_CellType == CTFree)
		m_CellType = CTTube;
		
	//Define tube type
	switch (GetTubeSideCount()) {
		case 1:	m_TubeType = TTHalf;	break;
		case 2:	m_TubeType = (m_ConnTop || m_ConnBottom) && (m_ConnLeft || m_ConnRight) ? TTCurved : TTStraight;	break;
		case 3:	m_TubeType = TTJoiner;	break;
		default:
			assert(false && "Incorrect connection count");
			break;
	}

	//Define angle from base direction (anticlockwise)
	m_Angle = 0.0f;
	switch (m_TubeType) {
		case TTHalf:
			if (m_ConnRight)									m_Angle = 90.0f * 3;
			else if (m_ConnBottom)								m_Angle = 90.0f * 2;
			else if (m_ConnLeft)								m_Angle = 90.0f * 1;
			break;
		case TTStraight:
			if (m_ConnRight /*|| m_ConnLeft*/)					m_Angle = 90.0f * 1;
			break;
		case TTCurved:
			if (m_ConnRight && m_ConnBottom)					m_Angle = 90.0f * 3;
			else if (m_ConnBottom && m_ConnLeft)				m_Angle = 90.0f * 2;
			else if (m_ConnLeft && m_ConnTop)					m_Angle = 90.0f * 1;
			break;
		case TTJoiner:
			if (m_ConnLeft && m_ConnRight && m_ConnBottom)		m_Angle = 90.0f * 3;
			else if (m_ConnTop && m_ConnBottom && m_ConnLeft)	m_Angle = 90.0f * 2;
			else if (m_ConnLeft && m_ConnRight && m_ConnTop)	m_Angle = 90.0f * 1;
			break;
		default:
			assert(false && "Unknown tube type");
			break;
	}
}


bool CCell::CanAddTube() const
{
	const unsigned char connectionCount = GetTubeSideCount();
	return 
		m_CellType == CTFree ||
		(m_CellType == CTTube && connectionCount < 3) ||
		((m_CellType == CTSender || m_CellType == CTReceiver) && connectionCount == 0);
}


unsigned char CCell::GetTubeSideCount() const
{
	unsigned char connectionCount = 0;
	if (m_ConnTop)		connectionCount++;
	if (m_ConnBottom)	connectionCount++;
	if (m_ConnLeft)		connectionCount++;
	if (m_ConnRight)	connectionCount++;
	return connectionCount;
}


void CCell::Rotate(const bool dir)
{
	if (IsLocked())
		return;

	if (IsRotationInProgress()) {
		m_Rotate.Twice = true;
		return;
	}

	m_Rotate.Direction = dir;
	m_Rotate.StartTime = CSynchro::GetTick();
	m_Rotate.Twice = false;
	m_Rotate.InitAngle = m_Angle;
	m_Rotate.NeedAngle = m_Rotate.InitAngle + (m_Rotate.Direction ? -90.0f : 90.0f);
}


bool CCell::ProcessRotation()
{
	if (!IsRotationInProgress())
		return false;

	float degree;
	if (CSynchro::GetPhase(m_Rotate.StartTime, 300, degree)) {
		degree *= 90.0f;
		m_Angle = (m_Rotate.Direction ? -degree : degree);
		m_Angle += m_Rotate.InitAngle;
		return false;
	}

	m_Angle = m_Rotate.NeedAngle;
	m_Rotate.StartTime = 0;

	//Calculate new connection sides
	bool newConnTop = false, newConnBottom = false, newConnLeft = false, newConnRight = false;
	if (m_Rotate.Direction) {
		newConnTop = m_ConnLeft;
		newConnBottom = m_ConnRight;
		newConnLeft = m_ConnBottom;
		newConnRight = m_ConnTop;
	}
	else {
		newConnTop = m_ConnRight;
		newConnBottom = m_ConnLeft;
		newConnLeft = m_ConnTop;
		newConnRight = m_ConnBottom;
	}
	m_ConnTop = newConnTop;
	m_ConnBottom = newConnBottom;
	m_ConnLeft = newConnLeft;
	m_ConnRight = newConnRight;


	if (m_Rotate.Twice) {
		Rotate(m_Rotate.Direction);
		return false;
	}

	return true;
}
