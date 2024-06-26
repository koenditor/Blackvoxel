/*
 * This file is part of Blackvoxel.
 *
 * Copyright 2010-2014 Laurent Thiebaut & Olivia Merle
 *
 * Blackvoxel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Blackvoxel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * ZScreen_Loading.cpp
 *
 *  Created on: 30 mai 2011
 *      Author: laurent
 */

#include "ZScreen_Loading.h"

#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include "GL/gl.h"
#include "ZGame.h"
#include "ZGui_FontFrame.h"
#include "ZTileSets.h"


ULong ZScreen_Loading::ProcessScreen(ZGame * GameEnv)
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); SDL_GL_SwapWindow(GameEnv->screen);
  GameEnv->GuiManager.RemoveAllFrames();

  ZFrame_FontFrame Frame_Loading;
    ZVector2f Loading_Size;
    Frame_Loading.SetDisplayText((char *)"LOADING...");
    Frame_Loading.SetStyle(GameEnv->TileSetStyles->GetStyle(1));
    Frame_Loading.GetTextDisplaySize(&Loading_Size);
    Frame_Loading.SetPosition(GameEnv->ScreenResolution.x / 2.0 - Loading_Size.x / 2.0, GameEnv->ScreenResolution.y / 2.0 - Loading_Size.y / 2.0 );
    Frame_Loading.SetSize(Loading_Size.x+1.0,Loading_Size.y);
    Frame_Loading.SetZPosition(49.0f);

    Frame_Loading.TextureNum = 3;

    //TitleBackground.AddFrame(&Frame_PlayGame);
    GameEnv->GuiManager.AddFrame(&Frame_Loading);
  GameEnv->GuiManager.Render();
  SDL_GL_SwapWindow(GameEnv->screen);
  SDL_Delay(10);
  GameEnv->GuiManager.RemoveAllFrames();

  return(0);
}
