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
 * ZEventDispatch.h
 *
 *  Created on: 8 avr. 2011
 *      Author: laurent
 */

#pragma once 

#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_keyboard.h>

#include "z/ZTypes.h"
#include "z/ZObject.h"
#include "z/ZObject_Lists.h"


class ZEventTable
{


};


class ZEventConsumer : public ZObject
{
  public:
    virtual Bool KeyDown( SDL_Scancode KeySym )  = 0;
    virtual Bool KeyUp( SDL_Scancode KeySym )    = 0;
    virtual Bool MouseMove ( Short Relative_x, Short Relative_y, UShort Absolute_x, UShort Absolute_y) = 0;
    virtual Bool MouseButtonClick  (UShort nButton, Short Absolute_x, Short Absolute_y) = 0;
    virtual Bool MouseButtonRelease(UShort nButton, Short Absolute_x, Short Absolute_y) = 0;

};

class ZEventManager
{
  protected:
    ZList ConsumerList;
  public:
    // Actual state

    const Uint8* Keyboard_Matrix;
    UShort MouseX,MouseY;
    bool  MouseButton[64];

    ZEventManager()
    {
      ULong i;

      Keyboard_Matrix = SDL_GetKeyboardState(NULL);
      for (i = 0; i<64;i++) MouseButton[i]=false;
      MouseX = 0;
      MouseY = 0;
    }
   ~ZEventManager() {}

    void AddConsumer_ToTail ( ZEventConsumer * EventConsumer )
    {
      ConsumerList.AddToTail(EventConsumer);
    }

    void RemoveConsumer( ZEventConsumer * EventConsumer )
    {
      ZListItem * Item;
      Item = ConsumerList.GetFirst();
      while (Item)
      {
        Item = ConsumerList.GetNext(Item);
        if (EventConsumer == (ZEventConsumer *)Item->GetObject()) { ConsumerList.Remove(Item); return; }
      }
    }

    bool ProcessEvents();

    bool Is_KeyPressed(UShort KeyCode, bool)
    {
      return Keyboard_Matrix[KeyCode];
    }

    bool Is_MouseButtonPressed(UShort ButtonCode, bool Reset)
    {
      bool IsPressed;

      IsPressed = (MouseButton[ButtonCode] ? true : false);
      if (Reset) MouseButton[ButtonCode] = 0;
      return IsPressed;
    }

    // Manual Triggering mouse button events in order to simulate a mouse button click

    void ManualCall_MouseButtonClick(UShort ButtonCode);
    void ManualCall_MouseButtonRelease(UShort ButtonCode);
};
