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
 * ZTool.h
 *
 *  Created on: 12 juil. 2011
 *      Author: laurent
 */

#pragma once 






#include "z/ZTypes.h"

#include "SDL2/SDL_stdinc.h"

#define ZTOOL_TOOLTYPESCOUNT 256

class ZGame;

class ZTool
{
  protected:
    ZGame * GameEnv;
  public:

    virtual void SetGameEnv(ZGame * GameEnv) { this->GameEnv = GameEnv; }

    ZTool()                   { GameEnv = nullptr; }
    virtual ~ZTool()          { GameEnv = nullptr; }

    virtual void Start_Tool() { }
    virtual void End_Tool()   { }
    virtual void Display()    { }

    virtual bool Tool_MouseButtonClick(ULong Button) {return(false);}
    virtual bool Tool_MouseButtonRelease(ULong Button) {return(false);}
    virtual bool Tool_StillEvents(double FrameTime, bool * MouseButtonMatrix, const Uint8 * KeyboardMatrix ) {return(true);};

};

class ZToolManager
{
  public: // Allow Direct access to ToolList
    ZTool ** ToolList;

    ZGame * GameEnv;

    ULong   ActualTool_VoxelType;

  public:

    void SetGameEnv(ZGame * GameEnv) { this->GameEnv = GameEnv; }

    ZToolManager()
    {
      ULong i;
      GameEnv = nullptr;

      ToolList = new ZTool *[65536];

      for (i=0; i<65536; i++) ToolList[i] = nullptr;
      ActualTool_VoxelType = (ULong) -1;
    }

    ~ZToolManager()
    {
      ULong i;
      for (i=0;i<65536;i++) if (ToolList[i]) { delete ToolList[i]; ToolList[i] = nullptr; }
      if (ToolList) delete [] ToolList;
    }

    void AddTool(ULong ToolNum, ZTool * Tool) { ToolList[ToolNum] = Tool; Tool->SetGameEnv(GameEnv); }
    void RemoveTool(ULong ToolNum)            { ToolList[ToolNum] = nullptr;    }
    ZTool * GetTool(ULong ToolNum)            { return (ToolList[ToolNum]); }

    void ProcessAndDisplay();

};