/*
 * This file is part of Blackvoxel.
 *
 * Copyright 2010-2015 Laurent Thiebaut & Olivia Merle
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
 * ZGameWindow_ResumeRequest.h
 *
 *  Created on: 13 mars 2015
 *      Author: laurent
 */

#pragma once 

#include "z/ZString.h"
#include "ZGui.h"
#include "ZGui_TileFrame.h"
#include "ZGui_FontFrame.h"
#include "z/ZTypes.h"

class ZGame;

class ZGameWindow_ResumeRequest : public ZFrame
{
  protected:
    ZFrame           * MainWindow;
    ZTileFrame         CloseBox;
    ZFrame_FontFrame   Title;
    ZFrame_FontFrame   Text;


    ZString            Text_Text;

    ZGame              * GameEnv;
    bool    Flag_Shown;
  public:

  ZGameWindow_ResumeRequest()
  {
    MainWindow = this;
    GameEnv = nullptr;
    Flag_Shown = false;
    Text_Text  = "CLICK TO RESUME GAME";
  }

  ~ZGameWindow_ResumeRequest()
  {

  }

  void SetGameEnv(ZGame * GameEnv) {this->GameEnv = GameEnv;}
  void SetMessage(char * Text) { Text_Text = Text; }

  void Show();
  void Hide();
  bool Is_Shown() {return(Flag_Shown);}

  virtual void Render(Frame_Dimensions * ParentPosition);


  // Overloaded events

  virtual Bool MouseButtonClick  (UShort nButton, Short Absolute_x, Short Absolute_y);

};
