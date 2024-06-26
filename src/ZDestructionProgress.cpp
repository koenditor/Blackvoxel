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
 * ZDestructionProgress.cpp
 *
 *  Created on: 18 nov. 2017
 *      Author: laurent
 */

#include "ZDestructionProgress.h"
#include "ZGame.h"
#include "ACompileSettings.h"
#include "ZGameWindow_ProgressBar.h"
#include "ZSound.h"


ZDestructionProgress::ZDestructionProgress()
{
  GameEnv = nullptr;
  MiningNoTarget = true;
  MiningInProgress = false;
  Mining_Hardness = 1000000.0;
  Mining_MaterialResistanceCounter = 0;
  SoundHandle = nullptr;
}

void ZDestructionProgress::Start(double MiningHardness, ZVoxelCoords & PointedVoxel)
{
  // So do it...
  SetNewtarget(MiningHardness, PointedVoxel);
  GameEnv->GameProgressBar->SetCompletion(0.0f);
  GameEnv->GameProgressBar->Show();
  #if COMPILEOPTION_FNX_SOUNDS_1 == 1
  if (SoundHandle == nullptr) SoundHandle = GameEnv->Sound->Start_PlaySound(5,true,true,1.0,nullptr);
  #endif
}

void ZDestructionProgress::SetNewtarget(double MiningHardness, ZVoxelCoords & PointedVoxel)
{
  if (LastMinedVoxel == PointedVoxel){
    std::swap(Mining_MaterialResistanceCounter, Last_Mining_MaterialResistanceCounter);
  } else {
    Last_Mining_MaterialResistanceCounter = Mining_MaterialResistanceCounter;
    Mining_MaterialResistanceCounter = MiningHardness;
  }
  LastMinedVoxel = MinedVoxel;
  this->Mining_Hardness            = MiningHardness;
  MiningInProgress                 = true;
  MiningNoTarget                   = false;
  MinedVoxel                       = PointedVoxel;
  GameEnv->GameProgressBar->SetCompletion(0.0f);
}

void ZDestructionProgress::SetNoTarget()
{
  Mining_MaterialResistanceCounter = 1000.0;
  Mining_Hardness                  = 1000.0;
  MiningNoTarget                   = true;
  GameEnv->GameProgressBar->SetCompletion(0.0f);
}

void ZDestructionProgress::Stop()
{
  if (!MiningInProgress) return;

  Mining_MaterialResistanceCounter = 0;
  Last_Mining_MaterialResistanceCounter = 0;
  MiningInProgress = false;
  MiningNoTarget = true;
  GameEnv->GameProgressBar->SetCompletion(0.0f);
  GameEnv->GameProgressBar->Hide();
  #if COMPILEOPTION_FNX_SOUNDS_1 == 1
  if (SoundHandle != nullptr) { GameEnv->Sound->Stop_PlaySound(SoundHandle); SoundHandle = nullptr; }
  #endif
}

bool ZDestructionProgress::DoMine(double Amount)
{
   if (MiningNoTarget) return(false);

   Mining_MaterialResistanceCounter -= Amount;

   GameEnv->GameProgressBar->SetCompletion( 100.0f - 100.0f*Mining_MaterialResistanceCounter/Mining_Hardness );

   if (Mining_MaterialResistanceCounter < 0.0)
   {
     Mining_MaterialResistanceCounter = 100000.0;
     #if COMPILEOPTION_FNX_SOUNDS_1 == 1
     GameEnv->Sound->PlaySound(6);
     if (SoundHandle != nullptr) { GameEnv->Sound->Stop_PlaySound(SoundHandle); SoundHandle = nullptr; }
     #endif
     return(true);
   }

   return(false);
}

