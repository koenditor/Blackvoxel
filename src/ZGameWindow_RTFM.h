/*
 * This file is part of Blackvoxel.
 *
 * Copyright 2010-2017 Laurent Thiebaut & Olivia Merle
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
 * ZGameWindow_RTFM.h
 *
 *  Created on: 16 janv. 2017
 *      Author: laurent
 */

#pragma once 

#include "z/ZString.h"
#include "ZGui.h"
#include "ZGui_FontFrame.h"
#include "z/ZTypes.h"

class ZGame;

class ZGameWindow_RTFM : public ZFrame
{
  protected:
    ZFrame *      MainWindow;
    ZGame * GameEnv;
    bool Flag_Shown;

    ZFrame_FontFrame MainTitle;
    ZFrame_FontFrame MainText;

    ZString Text_MainTitle;
    ZString Text_MainText;


  public:

  ZGameWindow_RTFM()
  {
    GameEnv = nullptr;
    MainWindow = this;
    Flag_Shown = false;
    Text_MainTitle = "GETTING STARTED";
    Text_MainText.Clear();
    Text_MainText << "PRESS ~1:FF0000:F1~1:FFFFFF: ON THE RTFM BLOCK TO GET TUTORIAL\nAND MISSIONS (WEB)\n\n\n\n\n\n";
    Text_MainText << "PRESS ~1:FF0000:F1~1:FFFFFF: ON ANY BLOCK TO GET INFO ON IT (WEB)";


  }

  ~ZGameWindow_RTFM()
  {
  }

  void SetGameEnv(ZGame * GameEnv) {this->GameEnv = GameEnv;}

  void Show();


  void Hide();
  bool Is_Shown() {return(Flag_Shown);}

  Bool MouseButtonClick(UShort nButton, Short Absolute_x, Short Absolute_y);

};
