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
 * ZTool_Constructor.h
 *
 *  Created on: 15 juil. 2011
 *      Author: laurent
 */

#pragma once 






#include "ZTools.h"
#include "ZDestructionProgress.h"
#include "SDL2/SDL_stdinc.h"
#include "z/ZTypes.h"

class ZGame;

class ZTool_Constructor : public ZTool
{
  protected:


    ZDestructionProgress MiningProgress;


    ULong  OldToolNum;
    double ToolForce[ZTOOL_TOOLTYPESCOUNT];
    bool   ToolCompatibleTypes[ZTOOL_TOOLTYPESCOUNT];






  public:
    ZTool_Constructor()
    {
      ULong i;
      OldToolNum = (ULong)-1;
      for (i=0;i<ZTOOL_TOOLTYPESCOUNT;i++) ToolForce[i]=0.0;
      for (i=0;i<ZTOOL_TOOLTYPESCOUNT;i++) ToolCompatibleTypes[i]=false;
      ToolForce[1] = 0.8;
      ToolCompatibleTypes[1] = true;
    }

    void SetGameEnv(ZGame * GameEnv) override { this->GameEnv = GameEnv; MiningProgress.SetGameEnv(GameEnv);  }
    virtual bool Tool_MouseButtonClick(ULong Button) override;
    virtual bool Tool_MouseButtonRelease(ULong Button) override;

    bool Tool_StillEvents(double FrameTime, bool * MouseButtonMatrix, const Uint8 * KeyboardMatrix ) override;

    virtual void Start_Tool() override;
    virtual void End_Tool() override;
    virtual void Display() override;

};

class ZTool_Constructor_P2 : public ZTool_Constructor
{
  public:

    ZTool_Constructor_P2()
    {
      ToolForce[1] = 1.6;
      ToolForce[2] = 0.8;
      ToolCompatibleTypes[1] = true;
      ToolCompatibleTypes[2] = true;
    }
};

class ZTool_Constructor_P3 : public ZTool_Constructor
{
  public:

    ZTool_Constructor_P3()
    {
      ToolForce[1] = 3.2;
      ToolForce[2] = 1.6;
      ToolForce[3] = 0.8;
      ToolCompatibleTypes[1] = true;
      ToolCompatibleTypes[2] = true;
      ToolCompatibleTypes[3] = true;
    }
};

class ZTool_Constructor_P4 : public ZTool_Constructor
{
  public:

    ZTool_Constructor_P4()
    {
      ToolForce[1] = 6.4;
      ToolForce[2] = 6.4;
      ToolForce[3] = 3.2;
      ToolCompatibleTypes[1] = true;
      ToolCompatibleTypes[2] = true;
      ToolCompatibleTypes[3] = true;
    }
};

class ZTool_Constructor_P5 : public ZTool_Constructor
{
  public:

    ZTool_Constructor_P5()
    {
      ToolForce[1] = 12.8;
      ToolForce[2] = 12.8;
      ToolForce[3] = 12.8;
      ToolForce[4] = 12.8;
      ToolForce[5] = 12.8;
      ToolCompatibleTypes[1] = true;
      ToolCompatibleTypes[2] = true;
      ToolCompatibleTypes[3] = true;
      ToolCompatibleTypes[4] = true;
      ToolCompatibleTypes[5] = true;
    }
};
