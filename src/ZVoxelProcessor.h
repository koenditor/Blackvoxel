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
 * ZVoxelProcessor.h
 *
 *  Created on: 26 mars 2011
 *      Author: laurent
 */

#pragma once 


#include "z/ZTypes.h"

#include "z/ZObject.h"

#include "ZWorld.h"

#include "ZActorPhysics.h"

#include "SDL2/SDL_thread.h"

#include "ACompileSettings.h"

#include "ZVoxelReactor.h"

#include "ZHighPerfTimer.h"

#include "ZEgmyScatter.h"

#include "ZRemoteRobotManager.h"

class ZGame;

class test {};

class ZVoxelProcessor : public ZObject
{
  protected:
    ZVoxelWorld * World;
    ZActorPhysicEngine * PhysicEngine;
    bool ThreadContinue;
    void * Thread;
    ZGame * GameEnv;
    ZVoxelReactor VoxelReactor;
    ZEgmyScatter  EgmyScatter;
    ZHighPerfTimer Timer;

    ZVector3L Player_Sector;
    ZVector3d Player_Position;
    ZVector3L Player_Voxel;
    double SectorEjectDistance;

    // The function called by the dedicated thread.

    static int thread_func(void * Data);

    // Thread task processing

    void StartTasks(); // Launched when thread start
    void MakeTasks();  // Repeating
    void EndTasks();   // Launched at thread end.

    // Sub tasks launched by MakeTasks

    void MakeSectorTasks(ZVoxelSector * Sector);


  public:
    ZRemoteRobotManager RemoteRobotManager;
    ZVoxelProcessor();
    ~ZVoxelProcessor();

    // Thread control. These command will start and stop the processor thread.

    void Start();
    void End();

    // Set parameters

    void SetWorld(ZVoxelWorld * World) { this->World = World; }
    void SetGameEnv(ZGame * GameEnv) { this->GameEnv = GameEnv; }
    void SetSectorEjectDistance(double SectorEjectDistance) { this->SectorEjectDistance = SectorEjectDistance; }
    void SetPlayerPosition(double x,double y, double z);

};