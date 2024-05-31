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
 * ZEventDispatch.cpp
 *
 *  Created on: 11 mai 2011
 *      Author: laurent
 */


#include "ZEventManager.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <sys/types.h>

bool ZEventManager::ProcessEvents()
{
  SDL_Event event;
  ZListItem * Item;
  ushort button;

  while( SDL_PollEvent(&event) )
  {
    switch( event.type )
    {
      case SDL_KEYDOWN: 
                        if ((Item = ConsumerList.GetFirst()))
                          do
                          {
                            ((ZEventConsumer *)Item->Object)->KeyDown( event.key.keysym.scancode);
                          } while((Item = ConsumerList.GetNext(Item)));
                        break;

      case SDL_KEYUP:   
                        if ((Item = ConsumerList.GetFirst()))
                          do
                          {
                            ((ZEventConsumer *)Item->Object)->KeyUp( event.key.keysym.scancode);
                          } while((Item = ConsumerList.GetNext(Item)));
                        break;

                             break;
      case SDL_MOUSEMOTION: MouseX = event.motion.x; MouseY = event.motion.y;
                            if ((Item = ConsumerList.GetFirst()))
                            do
                            {
                              ((ZEventConsumer *)Item->Object)->MouseMove(event.motion.xrel, event.motion.yrel, MouseX,MouseY);
                            } while((Item = ConsumerList.GetNext(Item)));
                            break;

      case SDL_MOUSEBUTTONDOWN:
                                MouseButton[event.button.button]=true;
                                if ((Item = ConsumerList.GetFirst()))
                                do
                                {
                                  ((ZEventConsumer *)Item->Object)->MouseButtonClick( event.button.button, MouseX, MouseY );
                                } while((Item = ConsumerList.GetNext(Item)));
                                break;

      case SDL_MOUSEWHEEL:
                                if (event.wheel.preciseY<0)
                                  button = 4;
                                else 
                                  button = 5;
                                MouseButton[button]=true;
                                if ((Item = ConsumerList.GetFirst()))
                                do
                                {
                                  ((ZEventConsumer *)Item->Object)->MouseButtonClick( button, MouseX, MouseY );
                                } while((Item = ConsumerList.GetNext(Item)));
                                break;

      case SDL_MOUSEBUTTONUP:
                                MouseButton[event.button.button]=true;
                                if ((Item = ConsumerList.GetFirst()))
                                do
                                {
                                  ((ZEventConsumer *)Item->Object)->MouseButtonRelease( event.button.button, MouseX, MouseY );
                                } while((Item = ConsumerList.GetNext(Item)));
                                break;


      case SDL_QUIT: return(false);
                     break;

      default: break;
    }
  }

  return(true);
}

void ZEventManager::ManualCall_MouseButtonClick(UShort ButtonCode)
{
    ZListItem * Item;
    int x,y;

    SDL_GetMouseState(&x,&y);

    MouseButton[ButtonCode]=true;
    if ((Item = ConsumerList.GetFirst()))
    do
    {
      ((ZEventConsumer *)Item->Object)->MouseButtonClick( ButtonCode, x, y );
    } while((Item = ConsumerList.GetNext(Item)));
}


void ZEventManager::ManualCall_MouseButtonRelease(UShort ButtonCode)
{
    ZListItem * Item;
    int x,y;

    SDL_GetMouseState(&x,&y);

    MouseButton[ButtonCode]=true;
    if ((Item = ConsumerList.GetFirst()))
    do
    {
      ((ZEventConsumer *)Item->Object)->MouseButtonRelease( ButtonCode, x, y );
    } while((Item = ConsumerList.GetNext(Item)));
}

