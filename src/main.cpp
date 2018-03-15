/************************************************************************** 
 *  PipeWalker - simple puzzle game                                       * 
 *  Copyright (C) 2007-2008 by Artem A. Senichev <artemsen@gmail.com>     * 
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

#include "common.h"
#include "opengl.h"


#ifdef PW_SYSTEM_WINNT
/****************************************************************/
/*                  MS Windows main routine                     */
/****************************************************************/
int APIENTRY WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	#ifndef NDEBUG
		//Create console for debug session
		AllocConsole();
		freopen("CONOUT$", "wb", stdout);
		freopen("CONOUT$", "wb", stderr);
	#endif
#else
/****************************************************************/
/*                      ANSI C main routine                     */
/****************************************************************/
int main(int /*argc*/, char** /*argv*/)
{
#endif
	
	return COpenGL::Run();
}
