/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2010 by Artem A. Senichev <artemsen@gmail.com>     *
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

#pragma once

#include "common.h"

class CGame;

#ifdef _MSC_VER
#pragma warning(disable: 4512)	//assignment operator could not be generated
#endif // _MSC_VER


class CExplosion
{
public:
	/**
	 * Constructor
	 * \param game the game instance
	 * \param x an initial X explosion position
	 * \param y an initial Y explosion position
	 */
	CExplosion(CGame& game, const float x, const float y);

	/**
	 * Render explosion
	 */
	void Render();

private:
	/**
	 * Renew explosion
	 */
	void Renew();

private:
	//! Particles description
	struct PARTICLE {
		float	Life;		///< Life value
		float	FadeSpeed;	///< Fade speed
		float	PosX;		///< Position by X axis
		float	PosY;		///< Position by Y axis
		float	SpeedX;		///< Speed by X axis
		float	SpeedY;		///< Speed by Y axis
	};

	vector<PARTICLE>	_Particles;	///< Particle Array
	float				_X;			///< Initial X explosion position
	float				_Y;			///< Initial Y explosion position
	float				_Force;		///< Explosion force

	CGame&				_Game;		///< Game instance
};
