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
 * ZGameWindow_Advertising.h
 *
 *  Created on: 26 oct. 2011
 *      Author: laurent
 */

#pragma once

#include "z/ZString.h"
#include "z/ZSimpleFile.h"
#include "ZGui.h"
#include "ZGui_FontFrame.h"
#include "z/ZTypes.h"

class ZGame;

class ZGameWindow_Advertising
{
  protected:

    struct DisplayEntry
    {
      ZString Message;
      ULong   Visibility;
      ULong   SoundToPlay;
      double  DisplayTime;
      double  MinimumDisplayTime;
    };


    ZFrame           MainWindow;
    ZString          ActualText;
    ZFrame_FontFrame Frame_Text;

    ZGame * GameEnv;
    bool Flag_Shown;

    ZSimpleFile DisplayFile;
    DisplayEntry * ActualyDisplayedEntry;

  public:
    enum {VISIBILITY_HIGH, VISIBILITY_MEDIUM, VISIBILITY_MEDLOW, VISIBILITY_LOW, VISIBILITY_VERYLOW, VISIBILITY_VERYHARDTOREAD};


  ZGameWindow_Advertising()
  {
    Flag_Shown = false;
    ActualyDisplayedEntry = nullptr;
    GameEnv = nullptr;
  }

  ~ZGameWindow_Advertising()
  {
    DisplayEntry * Entry;

    while ((Entry = (DisplayEntry *)DisplayFile.PullFromFile()))
    {
      if (ActualyDisplayedEntry) { delete ActualyDisplayedEntry; ActualyDisplayedEntry = nullptr; }
      delete Entry;
    }
  }

  void SetGameEnv(ZGame * GameEnv) {this->GameEnv = GameEnv;}

  void Advertise(char const * Text , ULong Visibility, ULong SoundToPlay, double DisplayTime, double MinimumDisplayTime)
  {
    DisplayEntry * Entry;

    Entry = new DisplayEntry;
    Entry->Message = Text;
    Entry->Visibility = Visibility;
    Entry->SoundToPlay = SoundToPlay;
    Entry->DisplayTime = DisplayTime;
    Entry->MinimumDisplayTime = MinimumDisplayTime;

    DisplayFile.PushToFile(Entry);
  }

  void Clear();


  void Advertising_Actions(double FrameTime);

  void Show();
  void Hide();


  bool Is_Shown() {return(Flag_Shown);}

};