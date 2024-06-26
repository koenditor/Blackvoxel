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
 * ZGame_Events.h
 *
 *  Created on: 24 juin 2011
 *      Author: laurent
 */

#pragma once 

#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <stddef.h>

#include "ZEventManager.h"
#include "SDL2/SDL_keyboard.h"
#include "z/ZTypes.h"

class ZGame;

class ZGame_Events : public ZEventConsumer
{
  protected:
    ZEventManager * EventManager;
    ZGame         * GameEnv;
    const Uint8   * Keyboard_Matrix;
    bool          * Mouse_Matrix;
    bool          EnableMouseEvents;
    double        LastFrameTime;
  public:

    ZGame_Events()
    {
      ULong i;
      Keyboard_Matrix = SDL_GetKeyboardState(NULL);
      Mouse_Matrix    = new bool[128];
      for (i=0;i<128;i++)  Mouse_Matrix[i] = false;
      EnableMouseEvents = true;
      EventManager = nullptr;
      GameEnv = nullptr;
      LastFrameTime = 16.0;
    }

    ~ZGame_Events()
    {
      // if (Keyboard_Matrix) delete [] Keyboard_Matrix;
    }

    virtual Bool KeyDown( SDL_Scancode KeySym );
    virtual Bool KeyUp( SDL_Scancode KeySym );
    virtual Bool MouseMove (Short Relative_x, Short Relative_y, UShort Absolute_x, UShort Absolute_y);
    virtual Bool MouseButtonClick  (UShort nButton, Short Absolute_x, Short Absolute_y);
    virtual Bool MouseButtonRelease(UShort nButton, Short Absolute_x, Short Absolute_y);

    virtual void SetEventManager(ZEventManager * EventManager) { this->EventManager = EventManager; }
    virtual void SetGameEnv(ZGame * GameEnv) {this->GameEnv = GameEnv; }
    virtual void SetLastFrameTime(double LastFrameTime) { this->LastFrameTime = LastFrameTime; }
    virtual void Process_StillEvents();

    virtual void SetEnableMouseEvents()  {EnableMouseEvents = true; }
    virtual void SetDisableMouseEvents() {EnableMouseEvents = false;}

    // bool Is_KeyPressed(UShort KeyCode, bool Reset)
    // {
      // bool IsPressed;

      // IsPressed = Keyboard_Matrix[KeyCode] ? true : false;
      // if (Reset) Keyboard_Matrix[KeyCode] = 0;
      // return IsPressed;
    // }
    bool Is_MouseButtonPressed(UShort ButtonCode, bool Reset)
     {
       bool IsPressed;

       IsPressed = (Mouse_Matrix[ButtonCode] ? true : false);
       if (Reset) Mouse_Matrix[ButtonCode] = 0;
       return IsPressed;
     }

};
