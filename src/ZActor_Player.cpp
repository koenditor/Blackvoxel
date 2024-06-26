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
 * ZActor_Player.cpp
 *
 *  Created on: 20 oct. 2011
 *      Author: laurent
 */

#include "ZActor_Player.h"

#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <math.h>
#include <stdio.h>

#include "ZGame.h"
#include "ZDirs.h"
#include "z/ZStream_SpecialRamStream.h"
#include "z/ZStream_File.h"
#include "ACompileSettings.h"
#include "ZCamera.h"
#include "ZGameWindow_Advertising.h"
#include "ZInventory.h"
#include "ZSettings_Hardware.h"
#include "ZSound.h"
#include "ZTools.h"
#include "ZVoxelExtension.h"
#include "ZVoxelSector.h"
#include "ZVoxelType.h"
#include "ZVoxelTypeManager.h"
#include "z/ZType_ZPolar3d.h"
#include "z/ZType_ZVector3d_CrossFunc.h"


ZActor_Player::ZActor_Player()
{
  ActorMode = 0;
  IsInLiquid     = false;
  IsFootInLiquid = false;
  IsHeadInLiquid = false;
  IsWalking = false;
  PlayerDensity = 1.040;
  LocationDensity = 0.0;
  PlayerSurface.x = 1.0;
  PlayerSurface.z = 1.0;
  PlayerSurface.y = 0.01;
  Speed_Walk = 20.0;
  PlaneSpeed = 0.0;
  PlaneCommandResponsiveness = 0.0;
  PlaneEngineThrust = 0.0;
  if (COMPILEOPTION_PLATFORM_RASPBERRY_PI==1)
  {
    PlaneMinThrust[0] = 2500.0;PlaneMaxThrust[0] = 60000.0;
    PlaneMinThrust[1] = 2500.0;PlaneMaxThrust[1] = 60000.0;
  }
  else
  {
    PlaneMinThrust[0] = 5500.0;PlaneMaxThrust[0] = 60000.0;
    PlaneMinThrust[1] = 10000.0;PlaneMaxThrust[1] = 60000.0;
  }

  PlaneEngineOn = false;
  PlaneTakenOff = false;
  PlaneLandedCounter = 0.0;
  PlaneToohighAlt = false;
  PlaneFreeFallCounter = 0.0;
  PlaneWaitForRectorStartSound = false;
  PlaneReactorSoundHandle = nullptr;
  WalkSoundHandle = nullptr;
  Test_T1 = 0;
  Riding_Voxel = 0;
  Vehicle_Subtype = 0;
  Riding_VoxelInfo = 0;
  Riding_IsRiding = 0;
  LastHelpTime = 1000000.0;
  LastHelpVoxel.x = 0;
  LastHelpVoxel.y = 0;
  LastHelpVoxel.z = 0;

  Inventory = new ZInventory();
  PreviousVoxelTypes = new UShort[ZInventory::Inventory_SlotCount];

  SteerAngle = 0.0;
  CarThrust = 0.0;
  CarEngineSoundHandle = nullptr;


  Train_StoredVoxelCount = 0;
  TrainThrust = 0.0;
  TrainSpeed = 0.0;
  Train_DirPrefGoRight = false;
  Train_Elements_Engines = 0;
  TrainEngineSoundHandle = nullptr;

  Lift_Thrust = 0.0;
  Lift_Direction = 4;
  LiftSoundHandle = nullptr;

  Spinner_Origin = 0.0;
  Spinner_Angle  = 0.0;
  Spinner_Distance = 1000.0;
  Spinner_Height   = 512.0;
  Spinner_Speed    = 10.0;
  Spinner_VomitMode = false;

  Init();
}

ZActor_Player::~ZActor_Player()
{
  if (Inventory) {delete Inventory; Inventory = nullptr;}
  if (PreviousVoxelTypes) {delete [] PreviousVoxelTypes; PreviousVoxelTypes = nullptr;}
}

void ZActor_Player::Init(bool Death)
{

  // Player is alive and have some amount of lifepoints.

  IsDead = false;
  LifePoints = 1000.0;

  // Initial position and view direction

  Location.x = 425.0; Location.y = 0.0; Location.z = 1975.0;
  ViewDirection.pitch = 0.0; ViewDirection.roll = 0.0; ViewDirection.yaw = 0.0;
  Velocity = 0.0;
  Deplacement = 0.0;

  // Camera settings.

  EyesPosition.x = 0;
  EyesPosition.y = 256.0 * 1.75 / COMPILEOPTION_VOXELSIZEFACTOR ;
  EyesPosition.z = 0.0;

  Camera.x = Location.x + EyesPosition.x; Camera.y = Location.y + EyesPosition.y; Camera.z = Location.z + EyesPosition.z;
  Camera.Pitch = ViewDirection.pitch; Camera.Roll  = ViewDirection.roll; Camera.Yaw   = ViewDirection.yaw;
  Camera.ColoredVision.Activate = false;

  if (Death)
  {
    if (Riding_IsRiding)
    {
      if (GameEnv->VoxelTypeManager.VoxelTable[ Riding_Voxel ]->Is_HasAllocatedMemoryExtension) delete ((ZVoxelExtension * )Riding_VoxelInfo);
      Riding_IsRiding = false;
      Riding_VoxelInfo = 0;
      Riding_Voxel = 0;
    }
  }

  PlaneSpeed = 0.0;
  PlaneCommandResponsiveness = 0.0;
  PlaneEngineThrust = 0.0;
  PlaneEngineOn = false;
  PlaneTakenOff = false;
  PlaneLandedCounter = 0.0;
  PlaneToohighAlt = false;
  PlaneFreeFallCounter = 0.0;

  // Actor mode

  ActorMode = 0;

  // Inventory
  if ((!Death) || COMPILEOPTION_DONTEMPTYINVENTORYONDEATH!=1)
  {
    SetInitialInventory(Death);
  }

  // Time
  if (!Death) Time_TotalGameTime = 0;
  Time_ElapsedTimeSinceLastRespawn = 0;

  // Init powers

  if (!Death) for(int i=0;i<ZInventory::Inventory_SlotCount;i++) PreviousVoxelTypes[i]=0;
}

void ZActor_Player::SetInitialInventory(bool Death)
{
  Inventory->Clear();

#if COMPILEOPTION_INVENTORY_DEMOCONTENT == 1

  // Demo Version inventory content

  Inventory->SetActualItemSlotNum(3);

  Inventory->SetSlot(ZInventory::Tools_StartSlot+0, 77,1);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+0 ,107,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+1 , 75,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+2 , 49,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+3 ,  1,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+4 ,  4,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+5 ,  3,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+6 , 22,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+7 , 63,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+8 , 11,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+9 , 30,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+10 , 37,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+11 , 47,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+12 , 26,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+13 , 74,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+14 ,109,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+15 ,110,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+16 ,111,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+17 ,112,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+18 , 27,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+19 , 52,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+20 , 87,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+21 , 92,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+22 , 88,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+23, 99,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+24 ,100,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+25 ,101,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+26 ,102,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+27, 96,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+28 ,108,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+29 , 90,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+30 , 94,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+31 , 97,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+32 , 98,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+33 ,103,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+34 ,104,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+35 ,105,4096);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+36 ,106,4096);
#else

  // Full version inventory content.

  Inventory->SetSlot(ZInventory::Tools_StartSlot+0, 42,1);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+0,107,1);
  Inventory->SetSlot(ZInventory::Inventory_StartSlot+1, 75,1);

#endif
}

void ZActor_Player::SetPosition( ZVector3d &NewLocation )
{
  Location.x = NewLocation.x;
  Location.y = NewLocation.y;
  Location.z = NewLocation.z;

  Camera.x = Location.x + EyesPosition.x;
  Camera.y = Location.y + EyesPosition.y;
  Camera.z = Location.z + EyesPosition.z;
  Camera.Pitch = ViewDirection.pitch;
  Camera.Roll  = ViewDirection.roll;
  Camera.Yaw   = ViewDirection.yaw;
  // printf("Camera.y: %lf\n", Camera.y);
}

void ZActor_Player::Action_MouseMove(Long Delta_x, Long Delta_y)
{

  if (ActorMode == 7)
  {
    double MouseRatio;
    double MaxThrust = 100.0;
    switch(Riding_Voxel)
    {
      case 274: MaxThrust = 100.0; break;
      case 275: MaxThrust = 400.0; break;
      case 276: MaxThrust = 400.0; break;
    }

    if (IsDead) return;

    MouseRatio = GameEnv->Settings_Hardware->Setting_MouseFactor;
    Lift_Thrust-= Delta_y/(1.0 * MouseRatio);
    if (Lift_Thrust<-MaxThrust) Lift_Thrust = -MaxThrust;
    if (Lift_Thrust>MaxThrust) Lift_Thrust = MaxThrust;
    Lift_Direction = (Lift_Thrust > 0) ? 5 : 4;
  }


  if (ActorMode == 6)
  {
    double MouseRatio;
    double MaxThrust = 100.0;
    switch(Riding_Voxel)
    {
      case 269: MaxThrust = 100.0; break;
      case 270: MaxThrust = 400.0; break;
      case 271: MaxThrust = 400.0; break;
    }

    if (IsDead) return;

    MouseRatio = GameEnv->Settings_Hardware->Setting_MouseFactor;
    TrainThrust+= Delta_y/(1.0 * MouseRatio);
    if (TrainThrust<0.0) TrainThrust = 0.0;
    if (TrainThrust>MaxThrust) TrainThrust = MaxThrust;
  }

  if (ActorMode == 5)
  {
    double MouseRatio;
    double MaxThrust = 100.0;
    switch(Riding_Voxel)
    {
      case 263: MaxThrust = 100.0; break;
      case 264: MaxThrust = 100.0; break;
      case 265: MaxThrust = 200.0; break;
      case 266: MaxThrust = 200.0; break;
      case 267: MaxThrust = 250.0; break;
    }

    if (IsDead) return;

    MouseRatio = GameEnv->Settings_Hardware->Setting_MouseFactor;
    if (IsOnGround) CarThrust+= Delta_y/(1.0 * MouseRatio);
    if (CarThrust<0.0) CarThrust = 0.0;
    if (CarThrust>MaxThrust) CarThrust = MaxThrust;


    if ( (fabs(Velocity.x) + fabs(Velocity.y) + fabs(Velocity.z)) > 5.0)
    {
        switch(Riding_Voxel)
        {
          default:
          case 263:
          case 264:
          case 265: SteerAngle += Delta_x / (6.0 * MouseRatio); break;
          case 266:
          case 267: SteerAngle += Delta_x / (6.0 * MouseRatio) * (1.0 / ( (10.0+CarThrust) / 10.0) ); break;
        }
      if (SteerAngle > 50.0 ) SteerAngle = 50.0;
      if (SteerAngle < -50.0) SteerAngle =-50.0;
    }
  }

  if (ActorMode == 0 || ActorMode == 3)
  {
    double MouseRatio, YMove;

    MouseRatio = GameEnv->Settings_Hardware->Setting_MouseFactor;

    ViewDirection.yaw+=Delta_x/(3.0*MouseRatio);
    if (ViewDirection.yaw >= 360.0) ViewDirection.yaw -= 360.0;
    if (ViewDirection.yaw <0 ) ViewDirection.yaw += 360.0;

    YMove = Delta_y/(3*MouseRatio);
    ViewDirection.pitch-= (GameEnv->Settings_Hardware->Setting_MouseFlipY) ? -YMove : YMove;

    if (ViewDirection.pitch >= 360.0) ViewDirection.pitch -= 360.0;
    if (ViewDirection.pitch <0 ) ViewDirection.pitch += 360.0;
    if (ViewDirection.pitch >= 360.0) ViewDirection.pitch -= 360.0;
    if (ViewDirection.pitch <0 ) ViewDirection.pitch += 360.0;
    ViewDirection.roll = 0.0;

    // Pitch clip
    if (ViewDirection.pitch >=90.0 && ViewDirection.pitch < 180.0) ViewDirection.pitch = 90.0;
    if (ViewDirection.pitch <270.0 && ViewDirection.pitch >=180.0) ViewDirection.pitch = 270.0;

//    printf("Pitch: %lf\n", ViewDirection.pitch);

    Camera.Yaw = ViewDirection.yaw;
    Camera.Pitch = ViewDirection.pitch;
    Camera.Roll = ViewDirection.roll;
  }

  if (ActorMode == 1)
  {
    double MouseRatio;

    MouseRatio = GameEnv->Settings_Hardware->Setting_MouseFactor;

    ViewDirection.roll+=Delta_x/(6*MouseRatio);
// Clip limit

    if (ViewDirection.roll >=90.0 && ViewDirection.roll < 180.0) ViewDirection.roll = 90;
    if (ViewDirection.roll <270.0 && ViewDirection.roll >=180.0) ViewDirection.roll = 270;


    if (ViewDirection.roll >= 360.0) ViewDirection.roll -= 360.0;
    if (ViewDirection.roll <0.0 ) ViewDirection.roll += 360.0;


    ViewDirection.pitch+=Delta_y/(6*MouseRatio) * cos(ViewDirection.roll/57.295779513);
    ViewDirection.yaw  +=Delta_y/(6*MouseRatio) * sin(ViewDirection.roll/57.295779513);
    if (ViewDirection.pitch >= 360.0) ViewDirection.pitch -= 360.0;
    if (ViewDirection.pitch <0.0 ) ViewDirection.pitch += 360.0;
    if (ViewDirection.yaw >= 360.0) ViewDirection.yaw -= 360.0;
    if (ViewDirection.yaw <0.0 ) ViewDirection.yaw += 360.0;

    // ViewDirection.yaw = 0.0;


    Camera.Yaw = ViewDirection.yaw;
    Camera.Pitch = ViewDirection.pitch;
    Camera.Roll = ViewDirection.roll;
  }



  if (ActorMode == 2)
  {
    double MouseRatio;

    if (IsDead) return;


    MouseRatio = GameEnv->Settings_Hardware->Setting_MouseFactor;

    if (!IsOnGround) ViewDirection.roll+=Delta_x/(6*MouseRatio)*PlaneCommandResponsiveness;

    //

    ViewDirection.pitch+=Delta_y/(6*MouseRatio) * cos(ViewDirection.roll/57.295779513) * PlaneCommandResponsiveness;
    if (IsOnGround) { if (PlaneSpeed > 500.0) ViewDirection.yaw += Delta_x/(64.0*MouseRatio);}
    else              ViewDirection.yaw  +=Delta_y/(6*MouseRatio) * sin(ViewDirection.roll/57.295779513) * PlaneCommandResponsiveness;
    if (ViewDirection.pitch >= 360.0) ViewDirection.pitch -= 360.0;
    if (ViewDirection.pitch <0.0 ) ViewDirection.pitch += 360.0;
    if (ViewDirection.yaw >= 360.0) ViewDirection.yaw -= 360.0;
    if (ViewDirection.yaw <0.0 ) ViewDirection.yaw += 360.0;
    if (ViewDirection.roll >= 360.0) ViewDirection.roll -= 360.0;
    if (ViewDirection.roll <0.0 ) ViewDirection.roll += 360.0;

    // Angle Clip limit

    if (ViewDirection.roll >=90.0 && ViewDirection.roll < 180.0) ViewDirection.roll = 90.0;
    if (ViewDirection.roll <270.0 && ViewDirection.roll >=180.0) ViewDirection.roll = 270.0;
    if (ViewDirection.pitch >=90.0 && ViewDirection.pitch < 180.0) ViewDirection.pitch = 90.0;
    if (ViewDirection.pitch <270.0 && ViewDirection.pitch >=180.0) ViewDirection.pitch = 270.0;


    // ViewDirection.yaw = 0.0;


    Camera.Yaw = ViewDirection.yaw;
    Camera.Pitch = ViewDirection.pitch;
    Camera.Roll = ViewDirection.roll;
  }

  if (ActorMode == 8 && 0)
  {
    Spinner_Speed += ((double)Delta_x) * 0.1;
    Spinner_Height += ((double)Delta_y) * 0.1;
    printf("Spinner_Speed %f\n",Spinner_Speed);
  }

  Camera.Yaw = ViewDirection.yaw;
  Camera.Pitch = ViewDirection.pitch;
  Camera.Roll = ViewDirection.roll;
}

void ZActor_Player::Action_MouseButtonClick(ULong Button)
{
  UShort ToolNum;
  ZTool * Tool;

  if (IsDead) return;

  switch(ActorMode)
  {
    case 0:
    case 1:
    case 3:
    case 4:
    default:
            ToolNum = Inventory->GetSlotRef(ZInventory::Tools_StartSlot)->VoxelType;
            Tool = PhysicsEngine->GetToolManager()->GetTool(ToolNum);

            if (Tool)
            {
              Tool->Tool_MouseButtonClick(Button);
            }

            break;

    case 5: if (IsDead) return;
            switch(Button)
            {
              case 0: if (!IsOnGround) break;
                      switch (Riding_Voxel)
                      {
                        case 263: break;
                        case 264: Velocity.y += 2000.0; break;
                        case 265: Velocity.y += 2000.0; break;
                        case 266: Velocity.y += 2000.0; break;
                        case 267: Velocity.y += 2000.0; break;
                      }
                      break;

              case 2: if (!IsOnGround) break;
                      switch (Riding_Voxel)
                      {
                        case 263: break;
                        case 264: break;
                        case 265: break;
                        case 266: break;
                        case 267: Velocity.y += 4000.0; break;
                      }
                      break;

            }
            break;
  }


  /*
  MouseButtonMatrix[Button] = 1;
  if (Button==0) PhysicsEngine->World->SetVoxel_WithCullingUpdate(PointedVoxel.PredPointedVoxel.x, PointedVoxel.PredPointedVoxel.y, PointedVoxel.PredPointedVoxel.z, BuildingMaterial);
  if (Button!=0) PhysicsEngine->World->SetVoxel_WithCullingUpdate(PointedVoxel.PointedVoxel.x, PointedVoxel.PointedVoxel.y, PointedVoxel.PointedVoxel.z,0);
  */
}

void ZActor_Player::Action_MouseButtonRelease( ULong Button)
{
  UShort ToolNum;
  ZTool * Tool;

  ToolNum = Inventory->GetSlotRef(ZInventory::Tools_StartSlot)->VoxelType;
  Tool = PhysicsEngine->GetToolManager()->GetTool(ToolNum);

  switch(ActorMode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    default:
             if (Tool)
             {
               Tool->Tool_MouseButtonRelease(Button);
             }
             break;

    case 5:  if (IsDead) return;
             switch(Button)
             {
               case 0:
                       break;
             }
             break;
  }
}

bool ZActor_Player::Action_StillEvents( bool * MouseMatrix, const Uint8 * KeyboardMatrix)
{
  UShort ToolNum;
  ZTool * Tool;

  double FrameTime = GameEnv->Time_GameLoop;

  ToolNum = Inventory->GetSlotRef(ZInventory::Tools_StartSlot)->VoxelType;
  Tool = PhysicsEngine->GetToolManager()->GetTool(ToolNum);

  if (Tool)
  {
    Tool->Tool_StillEvents(FrameTime,MouseMatrix,KeyboardMatrix);
  }

  // Displacement in liquid.

  //if (IsInLiquid) printf("Mouille\n");

  if (ActorMode == 0 && IsInLiquid && KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_Jump])
  {
    Velocity.y += 5.0*FrameTime;
  }


  if (ActorMode == 5 || ActorMode == 6 )
  {
    if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveForward]) Action_MouseMove(0,0.5*FrameTime);
    if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveBackward]) Action_MouseMove(0,-0.5*FrameTime);
  }

  if (ActorMode == 8)
  {

    if (KeyboardMatrix[SDL_SCANCODE_KP_AMPERSAND]) { Spinner_VomitMode = !Spinner_VomitMode; }
    if (!KeyboardMatrix[SDL_SCANCODE_LCTRL])
    {
      if (!KeyboardMatrix[SDL_SCANCODE_LSHIFT])
      {
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveForward])  Spinner_Distance -= 0.1 * FrameTime;
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveBackward]) Spinner_Distance += 0.1 * FrameTime;
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveLeft])     Spinner_Speed    += 0.01 * FrameTime;
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveRight])    Spinner_Speed    -= 0.01 * FrameTime;
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveUp])       Spinner_Height   += 0.1 * FrameTime;
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveDown])     Spinner_Height   -= 0.1 * FrameTime;
      }
      else
      {
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveForward])  Spinner_Origin.x += 0.1 * FrameTime;
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveBackward]) Spinner_Origin.x -= 0.1 * FrameTime;
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveLeft])     Spinner_Origin.z += 0.1 * FrameTime;
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveRight])    Spinner_Origin.z -= 0.1 * FrameTime;
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveUp])       Spinner_Origin.y += 0.1 * FrameTime;
        if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveDown])     Spinner_Origin.y -= 0.1 * FrameTime;
      }
    }
    else
    {
       if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveLeft]) Spinner_Origin = 0.0;
       if (KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveBackward])
       {
         // KeyboardMatrix[GameEnv->Settings_Hardware->Setting_Key_MoveBackward] = 0;
         if (KeyboardMatrix[SDL_SCANCODE_LSHIFT])
         {
           ZString FileSpec;
           ZStream_File Stream;
           FileSpec = GameEnv->Path_UserData;
           FileSpec.AddToPath("Spinner_").Append_ULong(BuildingMaterial).Append_CStringUpToEOL(".dat");
           printf("Saving Spinner : %s\n",FileSpec.String);
           Stream.SetFileName(FileSpec.String);
           if (Stream.OpenWrite())
           {
             Stream << (ULong)1;
             Stream << Spinner_Origin;
             Stream << Spinner_Angle;
             Stream << Spinner_Distance;
             Stream << Spinner_Height;
             Stream << Spinner_Speed;
             Stream << Spinner_VomitMode;
             Stream << Spinner_Origin;
             Stream << Spinner_Origin;
             Stream.Close();
           }
         }
         else
         {
           ZString FileSpec;
           ZStream_File Stream;
           ULong Version;
           FileSpec = GameEnv->Path_UserData;
           FileSpec.AddToPath("Spinner_").Append_ULong(BuildingMaterial).Append_CStringUpToEOL(".dat");

           printf("Loading Spinner : %s ... ",FileSpec.String);
           Stream.SetFileName(FileSpec.String);
           if (Stream.OpenRead())
           {
             Stream >> Version;
             if (Version <= 1)
             {
               Stream >> Spinner_Origin;
               Stream >> Spinner_Angle;
               Stream >> Spinner_Distance;
               Stream >> Spinner_Height;
               Stream >> Spinner_Speed;
               Stream >> Spinner_VomitMode;
               Stream >> Spinner_Origin;
               Stream >> Spinner_Origin;
               Stream.Close();
             } else printf("incompatible file version.\n");
             printf("ok\n");
           }else {printf("error\n");}
         }
       }
    }

  }

  return(true);
}

void ZActor_Player::Event_Collision(double RelativeVelocity )
{
  // 2 Cubes = -2251
  // 3 Cubes = -2753
  // 4 Cubes = -3169
  // 5 Cubes = -3586
  // 6 Cubes = -3919
  // 7 Cubes = -4253
  // 8 Cubes = -4503
  switch (ActorMode)
  {
    case 0:
      if (fabs(RelativeVelocity) > 5100.0)
      {
        // printf("Collision : %lf\n",RelativeVelocity);
        GameEnv->Sound->PlaySound(1);
        //printf("Player is dead : Fatal Fall\n");
        #if COMPILEOPTION_FALLAREFATALS==1
        Event_DeadlyFall();
        #endif
      }
      break;

    case 2:
      RelativeVelocity = fabs(RelativeVelocity);
      if ( (!IsDead) && (RelativeVelocity > (PlaneTakenOff ? 60.0 : 600.0) ))
      {
        // printf("Velocity:%lf CycleTime%lf\n",RelativeVelocity, GameEnv->Time_GameLoop);
        Event_PlaneCrash();
        GameEnv->Sound->PlaySound(1);
      }

      break;

    case 5:
      RelativeVelocity = fabs(RelativeVelocity);
      switch (Riding_Voxel)
      {
        case 263: if (RelativeVelocity >50.0)    { Velocity.y = 10000.0; Stop_Riding(false);  GameEnv->Sound->PlaySound(1);}
                  break;
        case 264: if (RelativeVelocity > 300.0)  { Velocity.y = 15000.0; Stop_Riding(false);  GameEnv->Sound->PlaySound(1); }
                  break;
        case 265: if (RelativeVelocity > 300.0)  { GameEnv->Sound->PlaySound(1); Stop_Riding(); }
                  break;
        case 266: if (RelativeVelocity > 300.0)  { GameEnv->Sound->PlaySound(1); Stop_Riding(); }
                  break;
        case 267: if (RelativeVelocity > 1000.0) { GameEnv->Sound->PlaySound(1); Stop_Riding(); }
                  break;
      }
      break;

  }
}
/*
bool ZActor_Player::Save( ZStream_SpecialRamStream * OutStream )
{
  ULong *Size,StartLen,EndLen;
  OutStream->PutString("BLKPLAYR");
  OutStream->Put( (UShort)1); // Version
  OutStream->Put( (UShort)1); // Compatibility Class;
  Size = OutStream->GetPointer_ULong();
  OutStream->Put((ULong)0xA600DBED); // Size of the chunk, will be written later.
  StartLen = OutStream->GetActualBufferLen();

  OutStream->Put( Location.x ); OutStream->Put( Location.y ); OutStream->Put( Location.z );
  OutStream->Put( Velocity.x ); OutStream->Put( Velocity.y ); OutStream->Put( Velocity.z );
  OutStream->Put( Deplacement.x ); OutStream->Put( Deplacement.y ); OutStream->Put( Deplacement.z );
  OutStream->Put( ViewDirection.pitch ); OutStream->Put( ViewDirection.roll ); OutStream->Put( ViewDirection.yaw );
  OutStream->Put( Camera.x ); OutStream->Put( Camera.y ); OutStream->Put( Camera.z );
  OutStream->Put( Camera.Pitch ); OutStream->Put( Camera.Roll ); OutStream->Put( Camera.Yaw );

  OutStream->Put( (ULong)LifePoints);
  Inventory->Save(OutStream);

  EndLen = OutStream->GetActualBufferLen();
  *Size = EndLen - StartLen;

  return(true);
}
*/

bool ZActor_Player::Save( ZStream_SpecialRamStream * OutStream )
{
  ULong *Size,StartLen,*ExtensionSize,EndLen, StartExtensionLen,i;
  OutStream->PutString("BLKPLYR2");
  OutStream->Put( (UShort)7); // Version
  OutStream->Put( (UShort)7); // Compatibility Class;
  Size = OutStream->GetPointer_ULong();
  OutStream->Put((ULong)0xA600DBED); // Size of the chunk, will be written later.
  StartLen = OutStream->GetActualBufferLen();

  OutStream->Put( Location.x ); OutStream->Put( Location.y ); OutStream->Put( Location.z );
  OutStream->Put( Velocity.x ); OutStream->Put( Velocity.y ); OutStream->Put( Velocity.z );
  OutStream->Put( Deplacement.x ); OutStream->Put( Deplacement.y ); OutStream->Put( Deplacement.z );
  OutStream->Put( ViewDirection.pitch ); OutStream->Put( ViewDirection.roll ); OutStream->Put( ViewDirection.yaw );
  OutStream->Put( Camera.x ); OutStream->Put( Camera.y ); OutStream->Put( Camera.z );
  OutStream->Put( Camera.Pitch ); OutStream->Put( Camera.Roll ); OutStream->Put( Camera.Yaw );
  OutStream->Put( (ULong)LifePoints);

  // printf("Offset : %ld\n",OutStream->GetOffset());

  Inventory->Save(OutStream);

  // New for version 2

  OutStream->Put(ActorMode);
  OutStream->Put(IsInLiquid);
  OutStream->Put(IsFootInLiquid);
  OutStream->Put(IsHeadInLiquid);

  OutStream->Put(IsOnGround); // V4
  OutStream->Put(IsDead);     // V4

  OutStream->Put(LocationDensity);

  OutStream->Put(Riding_IsRiding);
  OutStream->Put(Riding_Voxel);
  OutStream->Put((ULong)Riding_VoxelInfo);
  OutStream->Put(PlaneSpeed);
  OutStream->Put(PlaneCommandResponsiveness);
  OutStream->Put(PlaneEngineThrust);
  OutStream->Put(PlaneEngineOn);
  OutStream->Put(PlaneTakenOff);
  OutStream->Put(PlaneLandedCounter);
  OutStream->Put(PlaneToohighAlt);

  OutStream->Put(SteerAngle); // V6
  OutStream->Put(CarThrust);  // V6

  OutStream->Put(Train_Direction);        // V6
  OutStream->Put(TrainThrust);            // V6
  OutStream->Put(TrainSpeed);             // V6
  OutStream->Put(Train_DirPrefGoRight);   // V6
  OutStream->Put(Train_Weight);           // V6
  OutStream->Put(Train_Elements_Engines); // V6

  OutStream->Put(Lift_Thrust);            // V6
  OutStream->Put(Lift_Direction);         // V6

  OutStream->Put(Spinner_Origin.x); // V7
  OutStream->Put(Spinner_Origin.x); // V7
  OutStream->Put(Spinner_Origin.x); // V7
  OutStream->Put(Spinner_Angle);    // V7
  OutStream->Put(Spinner_Distance); // V7
  OutStream->Put(Spinner_Height);   // V7
  OutStream->Put(Spinner_Speed);    // V7
  OutStream->Put(Spinner_VomitMode);// V7

  OutStream->Put(Time_TotalGameTime);
  OutStream->Put(Time_ElapsedTimeSinceLastRespawn);

  OutStream->PutString("RIDEXTEN");
  ExtensionSize = OutStream->GetPointer_ULong();
  OutStream->Put((ULong) 0xA600DBED );
  StartExtensionLen = OutStream->GetActualBufferLen();
  if (GameEnv->VoxelTypeManager.VoxelTable[Riding_Voxel]->Is_HasAllocatedMemoryExtension)
  {
    OutStream->Put((bool)true);
    ((ZVoxelExtension * )Riding_VoxelInfo)->Save(OutStream);
  }
  else OutStream->Put((bool)false);
  *ExtensionSize = OutStream->GetActualBufferLen() - StartExtensionLen;

  // V5 Train voxels extension
  OutStream->PutString("TRAINVOX");
  ExtensionSize = OutStream->GetPointer_ULong();
  OutStream->Put((ULong) 0xA600DBED );
  StartExtensionLen = OutStream->GetActualBufferLen();
  OutStream->Put(Train_StoredVoxelCount);  // V5

  bool EmbedVersion = true;
  for(i=0;i<Train_StoredVoxelCount;i++)
  {
    Train_VoxelTable[i].Save(OutStream,EmbedVersion); EmbedVersion = false;
  }
  *ExtensionSize = OutStream->GetActualBufferLen() - StartExtensionLen;

  EndLen = OutStream->GetActualBufferLen();
  *Size = EndLen - StartLen;

  return(true);
}

bool ZActor_Player::Load( ZStream_SpecialRamStream * InStream)
{
  ZString Section_Name;
  UShort   Section_Version;
  UShort   Section_CompatibilityClass;
  ULong   Section_Size, Tmp_UL=0,i;
  bool Ok;
  Section_Name.SetLen(8);

  Ok = true;
  Ok &= InStream->GetStringFixedLen(Section_Name.String,8);
  Ok &= InStream->Get(Section_Version);
  Ok &= InStream->Get(Section_CompatibilityClass);
  Ok &= InStream->Get(Section_Size);

  if (! Ok) return(false);

  // Implemented the version 2 format because version 1 was buggy and compatibility can't be assured without changing the section name.
  if (Section_Name == "BLKPLYR2")
  {
    Ok &= InStream->Get( Location.x ); InStream->Get( Location.y ); InStream->Get( Location.z );
    Ok &= InStream->Get( Velocity.x ); InStream->Get( Velocity.y ); InStream->Get( Velocity.z );
    Ok &= InStream->Get( Deplacement.x ); InStream->Get( Deplacement.y ); InStream->Get( Deplacement.z );
    Ok &= InStream->Get( ViewDirection.pitch ); InStream->Get( ViewDirection.roll ); InStream->Get( ViewDirection.yaw );
    Ok &= InStream->Get( Camera.x ); InStream->Get( Camera.y ); InStream->Get( Camera.z );
    Ok &= InStream->Get( Camera.Pitch ); InStream->Get( Camera.Roll ); InStream->Get( Camera.Yaw );
    Ok &= InStream->Get( Tmp_UL ); LifePoints = Tmp_UL; // Corrected
    if (!Ok) return(false);

    if (!Inventory->Load(InStream)) return(false);

    // New for version 2

    ULong ExtensionBlocSize;
    bool IsExtensionToLoad;

    Ok &= InStream->Get(ActorMode);
    Ok &= InStream->Get(IsInLiquid);
    Ok &= InStream->Get(IsFootInLiquid);
    Ok &= InStream->Get(IsHeadInLiquid);
  if (Section_Version > 5)
  { Ok &= InStream->Get(IsOnGround);
    Ok &= InStream->Get(IsDead);
  }
    Ok &= InStream->Get(LocationDensity);

    Ok &= InStream->Get(Riding_IsRiding);
    Ok &= InStream->Get(Riding_Voxel);
    Ok &= InStream->Get(Tmp_UL); Riding_VoxelInfo = Tmp_UL; // Corrected
    Ok &= InStream->Get(PlaneSpeed);
    Ok &= InStream->Get(PlaneCommandResponsiveness);
    Ok &= InStream->Get(PlaneEngineThrust);
    Ok &= InStream->Get(PlaneEngineOn);
    Ok &= InStream->Get(PlaneTakenOff);
    Ok &= InStream->Get(PlaneLandedCounter);
    Ok &= InStream->Get(PlaneToohighAlt);
  if (Section_Version > 5)
  { Ok &= InStream->Get(SteerAngle);
    Ok &= InStream->Get(CarThrust);
  }

  if (Section_Version >5)
  {
    Ok &= InStream->Get(Train_Direction);     // V5
    Ok &= InStream->Get(TrainThrust);         // V5
    Ok &= InStream->Get(TrainSpeed);          // V5
    Ok &= InStream->Get(Train_DirPrefGoRight);// V5
  }

  if (Section_Version >5)
  {
    Ok &= InStream->Get(Train_Weight);     // V5
    Ok &= InStream->Get(Train_Elements_Engines);         // V5
    Ok &= InStream->Get(Lift_Thrust);          // V5
    Ok &= InStream->Get(Lift_Direction);// V5
  }

  if (Section_Version > 6)
  {
    Ok &= InStream->Get(Spinner_Origin.x); // V7
    Ok &= InStream->Get(Spinner_Origin.x); // V7
    Ok &= InStream->Get(Spinner_Origin.x); // V7
    Ok &= InStream->Get(Spinner_Angle);    // V7
    Ok &= InStream->Get(Spinner_Distance); // V7
    Ok &= InStream->Get(Spinner_Height);   // V7
    Ok &= InStream->Get(Spinner_Speed);    // V7
    Ok &= InStream->Get(Spinner_VomitMode);// V7
  }

    Ok &= InStream->Get(Time_TotalGameTime); // New for V3
    Ok &= InStream->Get(Time_ElapsedTimeSinceLastRespawn); // New for V3

    Ok &= InStream->GetStringFixedLen(Section_Name.String,8);
    if (!(Ok && Section_Name == "RIDEXTEN" )) return(false);

    Ok &= InStream->Get(ExtensionBlocSize);

    Ok &= InStream->Get(IsExtensionToLoad);
    if (!Ok) return(false);
    if (GameEnv->VoxelTypeManager.VoxelTable[Riding_Voxel]->Is_HasAllocatedMemoryExtension)
    {
      Riding_VoxelInfo = (ZMemSize)GameEnv->VoxelTypeManager.VoxelTable[Riding_Voxel]->CreateVoxelExtension(); if (Riding_VoxelInfo == 0 ) return(false);
      if (IsExtensionToLoad)
      {
        ((ZVoxelExtension * )Riding_VoxelInfo)->Load(InStream);
      }
    }

    // V5 Train content
    if (Section_Version >=5)
    {
      Ok &= InStream->GetStringFixedLen(Section_Name.String,8);
      if (!(Ok && Section_Name == "TRAINVOX" )) return(false);
      Ok &= InStream->Get(ExtensionBlocSize);

      Ok &= InStream->Get(Train_StoredVoxelCount);  // V5
      ULong FormatVersion = 0;
      for(i=0;i<Train_StoredVoxelCount;i++)
      {
        Train_VoxelTable[i].Load(InStream, FormatVersion);
      }
    }





    return(true);
  }

  if (Section_Name == "BLKPLAYR")
  {
    Ok &= InStream->Get( Location.x ); InStream->Get( Location.y ); InStream->Get( Location.z );
    Ok &= InStream->Get( Velocity.x ); InStream->Get( Velocity.y ); InStream->Get( Velocity.z );
    Ok &= InStream->Get( Deplacement.x ); InStream->Get( Deplacement.y ); InStream->Get( Deplacement.z );
    Ok &= InStream->Get( ViewDirection.pitch ); InStream->Get( ViewDirection.roll ); InStream->Get( ViewDirection.yaw );
    Ok &= InStream->Get( Camera.x ); InStream->Get( Camera.y ); InStream->Get( Camera.z );
    Ok &= InStream->Get( Camera.Pitch ); InStream->Get( Camera.Roll ); InStream->Get( Camera.Yaw );
    Ok &= InStream->Get( Tmp_UL ); LifePoints = Tmp_UL; // Corrected

    if (!Ok) return(false);

    if (!Inventory->Load(InStream)) return(false);

    // New for version 2

    ULong ExtensionBlocSize;
    bool IsExtensionToLoad;

    Ok &= InStream->Get(ActorMode);
    Ok &= InStream->Get(IsInLiquid);
    Ok &= InStream->Get(IsFootInLiquid);
    Ok &= InStream->Get(IsHeadInLiquid);
    Ok &= InStream->Get(LocationDensity);

    Ok &= InStream->Get(Riding_IsRiding);
    Ok &= InStream->Get(Riding_Voxel);
    Ok &= InStream->Get(Tmp_UL); Riding_VoxelInfo = Tmp_UL; // Corrected
    Ok &= InStream->Get(PlaneSpeed);
    Ok &= InStream->Get(PlaneCommandResponsiveness);
    Ok &= InStream->Get(PlaneEngineThrust);
    Ok &= InStream->Get(PlaneEngineOn);
    Ok &= InStream->Get(PlaneTakenOff);
    Ok &= InStream->Get(PlaneLandedCounter);
    Ok &= InStream->Get(PlaneToohighAlt);
/*
    if (Section_Version > 2)
    {
      Ok &= InStream->Get(Time_TotalGameTime);
      Ok &= InStream->Get(Time_ElapsedTimeSinceLastRespawn);
    }
*/

    Ok &= InStream->GetStringFixedLen(Section_Name.String,8);
    if (!(Ok && Section_Name == "RIDEXTEN" )) return(false);

    Ok &= InStream->Get(ExtensionBlocSize);

    Ok &= InStream->Get(IsExtensionToLoad);
    if (!Ok) return(false);
    if (GameEnv->VoxelTypeManager.VoxelTable[Riding_Voxel]->Is_HasAllocatedMemoryExtension)
    {
      Riding_VoxelInfo = (ZMemSize)GameEnv->VoxelTypeManager.VoxelTable[Riding_Voxel]->CreateVoxelExtension(); if (Riding_VoxelInfo == 0 ) return(false);
      if (IsExtensionToLoad)
      {
        ((ZVoxelExtension * )Riding_VoxelInfo)->Load(InStream);
      }
    }

    return(true);
  }

  return(false);
}

void ZActor_Player::DoPhysic(UELong FrameTime)
{
  double CycleTime = FrameTime / 1000.0;

  if (ActorMode == 0) { DoPhysic_GroundPlayer(CycleTime); return; }
  // if (ActorMode == 1) { DoPhysic_Plane_Old(CycleTime); return; }
  if (ActorMode == 2) { DoPhysic_Plane(CycleTime); return; }
  if (ActorMode == 3) { DoPhysic_SupermanPlayer(CycleTime); return; }
  if (ActorMode == 5) { DoPhysic_Car(CycleTime); return; }
  if (ActorMode == 6) { DoPhysic_Train(CycleTime); return;}
  if (ActorMode == 7) { DoPhysic_Lift(CycleTime); return;}
  if (ActorMode == 8) { DoPhysic_Spinner(CycleTime); return;}
}

void ZActor_Player::DoPhysic_Lift(double CycleTime)
{
  // Limit cycle time in order to avoid some system stuttering to cause inconsistant huge displacements.

  double CapedCycleTime, SpeedMult, Liftspeed,ComputedLiftThrust, SoundFactor;
  ZVector3d NewLocation, Disp;
  Long ViewDirection;


  CapedCycleTime = CycleTime;
  if (CapedCycleTime > 50.0) CapedCycleTime = 50.0;

  // Sound




  // thrust to speed

  switch(Riding_Voxel)
  {
    case 274: SpeedMult = 1.0; break;
    case 275: SpeedMult = 1.0; break;
    case 276: SpeedMult = 10.0;break;
  }

  ComputedLiftThrust = fabs(Lift_Thrust);
  if (ComputedLiftThrust < 10.0) {ComputedLiftThrust = 0.0; Lift_Thrust /= 1.0 + (CapedCycleTime / 1000.0);}

  Liftspeed = ComputedLiftThrust * SpeedMult * CapedCycleTime / 100.0;

  // Moving

  if(!LiftFollowGuide(Location, Liftspeed, Lift_Direction, NewLocation)) {ComputedLiftThrust = Lift_Thrust = 0.0;}

  Disp = NewLocation - Location;
  Location = NewLocation;

  // View
  double ComputedPitch, MaxViewAngle;

  switch(Riding_Voxel)
  {
    case 274: MaxViewAngle = 50.0; break;
    case 275: MaxViewAngle = 50.0; break;
    case 276: MaxViewAngle = 70.0;break;
  }
  ComputedPitch = Lift_Thrust;
  if (ComputedPitch > MaxViewAngle) ComputedPitch = MaxViewAngle;
  if (ComputedPitch < -MaxViewAngle)ComputedPitch =-MaxViewAngle;
  if (fabs(Disp.y)<0.01) ComputedPitch = 0.0;
  LiftFindRailDirection(Location, ViewDirection);
  SetViewToCubeDirection_Progressive(ViewDirection, CapedCycleTime * 0.25);
  this->ViewDirection.pitch = ComputedPitch;
  if (this->ViewDirection.pitch < 0.0) this->ViewDirection.pitch += 360.0;
  //SetViewToCubeDirection(Train_Direction);
  Sync_Camera(640.0); // 448


  if (ComputedLiftThrust <10.0 )
  {
    if ((LiftSoundHandle))        { GameEnv->Sound->Stop_PlaySound(LiftSoundHandle); LiftSoundHandle = nullptr; }
  }
  else
  {
    switch(Riding_Voxel)
    {
      case 274: SoundFactor = 0.5; break;
      case 275: SoundFactor = 0.25; break;
      case 276: SoundFactor = 1.0; break;
      default:  SoundFactor = 1.0; break;
    }

    if (LiftSoundHandle==nullptr) {LiftSoundHandle = GameEnv->Sound->Start_PlaySound(10,true, false, 1.00,nullptr); }
    else                     GameEnv->Sound->ModifyFrequency(LiftSoundHandle, (ComputedLiftThrust*SoundFactor) / 60.0 + 1.0);
  }
}

void ZActor_Player::DoPhysic_Spinner(double CycleTime)
{
  ZVector3d Point;
  double ViewAngle, VertViewAngle;

  Point.x = Location.x + Spinner_Origin.x + sin(Spinner_Angle / 57.295779513) * Spinner_Distance;
  Point.z = Location.z + Spinner_Origin.z + cos(Spinner_Angle / 57.295779513) * Spinner_Distance;
  Point.y = Location.y + Spinner_Origin.y + Spinner_Height - 256.0;

  ViewAngle = Spinner_Angle + 180.0; if (ViewAngle>=360.0) ViewAngle-=360.0;
  VertViewAngle = -atan(Spinner_Height / Spinner_Distance )* 57.295779513;
  if (Spinner_VomitMode) VertViewAngle += (sin(Spinner_Angle/5.0) * 15.0);

  if (VertViewAngle < 0.0) VertViewAngle += 360.0;
  if (VertViewAngle > 360.0) VertViewAngle -= 360.0;

  Camera.x = Point.x; Camera.y = Point.y; Camera.z = Point.z;
  Camera.Yaw = ViewAngle;
  Camera.Pitch = VertViewAngle;
  Camera.Roll  = ViewDirection.roll;

  Spinner_Angle += Spinner_Speed * CycleTime * 0.001;
}

void ZActor_Player::DoPhysic_Train(double CycleTime)
{
  // Limit cycle time in order to avoid some system stuttering to cause inconsistant huge displacements.

  double CapedCycleTime, SpeedMult, SoundFactor;
  ZVector3d NewLocation;
  ULong MaxEngines;

  CapedCycleTime = CycleTime;
  if (CapedCycleTime > 50.0) CapedCycleTime = 50.0;

  // Sound

  switch(Riding_Voxel)
  {
    case 269: SoundFactor = 0.5; break;
    case 270: SoundFactor = 0.25; break;
    case 271: SoundFactor = 1.0; break;
    default:  SoundFactor = 1.0; break;
  }

  if (TrainEngineSoundHandle==nullptr) {TrainEngineSoundHandle = GameEnv->Sound->Start_PlaySound(9,true, false, 1.00,nullptr); }
  else                          GameEnv->Sound->ModifyFrequency(TrainEngineSoundHandle, (TrainThrust*SoundFactor) / 60.0 + 1.0);


  // thrust to speed

  switch(Riding_Voxel)
  {
    case 269: SpeedMult = 1.0; MaxEngines = 4; break;
    case 270: SpeedMult = 1.0; MaxEngines = 8; break;
    case 271: SpeedMult = 10.0;MaxEngines = 16; break;
  }

  SpeedMult *= 1.0 + 0.50*(double)((Train_Elements_Engines > MaxEngines) ? MaxEngines : Train_Elements_Engines);

  TrainSpeed = this->TrainThrust * SpeedMult * CapedCycleTime / 100.0;


  TrainFollowTrack(Location, TrainSpeed, Train_Direction, NewLocation);

  Location = NewLocation;
  SetViewToCubeDirection_Progressive(Train_Direction, CapedCycleTime * 0.25);
  //SetViewToCubeDirection(Train_Direction);
  Sync_Camera(640.0); // 448
}

void ZActor_Player::DoPhysic_Car(double CycleTime)
{
  ULong i;




    // Sound of the Engine

  if (!IsDead)
  {
    double FreqCoef;
    switch (Riding_Voxel)
    {

      case 263: FreqCoef = 1.0; break;
      case 264: FreqCoef = 1.0;  break;
      case 265: FreqCoef = 1.2;  break;
      case 266: FreqCoef = 1.5;  break;
      case 267: FreqCoef = 2.0;  break;
      default:  FreqCoef = 1.5;  break;
    }


     if (CarEngineSoundHandle==nullptr) {CarEngineSoundHandle = GameEnv->Sound->Start_PlaySound(8,true, false, 1.00,nullptr); }
     else                          GameEnv->Sound->ModifyFrequency(CarEngineSoundHandle, (CarThrust*FreqCoef) / 60.0 + 1.0);

  }
  else
  {
    if (CarEngineSoundHandle) {GameEnv->Sound->Stop_PlaySound(CarEngineSoundHandle); CarEngineSoundHandle = nullptr; }
  }

  // Death

  if (IsDead)
  {
    Camera.ColoredVision.Activate = true; Camera.ColoredVision.Red= 0.0f; Camera.ColoredVision.Green = 0.0f; Camera.ColoredVision.Blue = 0.0f; Camera.ColoredVision.Opacity  = 0.5f;
    ViewDirection.roll += 0.3 * CycleTime * ( DeathChronometer / 5000.0 ); if (ViewDirection.roll > 360) ViewDirection.roll -= 360.0;
    ViewDirection.pitch+= 0.5 * CycleTime * ( DeathChronometer / 5000.0 ); if (ViewDirection.pitch > 360) ViewDirection.pitch -= 360.0;
    Camera.Pitch = ViewDirection.pitch;
    Camera.Roll  = ViewDirection.roll;
    Camera.Yaw   = ViewDirection.yaw;
    DeathChronometer-= CycleTime;
    if (DeathChronometer> 4250.0 && DeathChronometer<4500.0)
    { DeathChronometer = 4250.0; GameEnv->GameWindow_Advertising->Advertise("YOU ARE DEAD", ZGameWindow_Advertising::VISIBILITY_HIGH, 0, 3000.0, 0.0); }
    if (DeathChronometer <= 0.0) Event_Death();
    return;
  }

  if (Riding_Voxel==267) // Direction stabilizer
  {
    double Correction = 0.001* CycleTime;
    if (Correction > 0.1) Correction = 0.1;

     if      (ViewDirection.yaw >90.0 && ViewDirection.yaw<95.0)   ViewDirection.yaw-=Correction;
     else if (ViewDirection.yaw >85.0 && ViewDirection.yaw<90.0)   ViewDirection.yaw+=Correction;
     else if (ViewDirection.yaw >180.0 && ViewDirection.yaw<185.0) ViewDirection.yaw-=Correction;
     else if (ViewDirection.yaw >175.0 && ViewDirection.yaw<180.0) ViewDirection.yaw+=Correction;
     else if (ViewDirection.yaw >270.0 && ViewDirection.yaw<275.0) ViewDirection.yaw-=Correction;
     else if (ViewDirection.yaw >265.0 && ViewDirection.yaw<270.0) ViewDirection.yaw+=Correction;
     else if (ViewDirection.yaw >0.0 &&   ViewDirection.yaw<5.0)  {ViewDirection.yaw-=Correction; if (ViewDirection.yaw < 0.0)   ViewDirection.yaw+=360.0;}
     else if (ViewDirection.yaw >355.0 && ViewDirection.yaw<360.0){ViewDirection.yaw+=Correction; if (ViewDirection.yaw >=360.0) ViewDirection.yaw-=360.0;}
  }


  // Limit cycle time in order to avoid some system stuttering to cause inconsistencies

  double CapedCycleTime;

  CapedCycleTime = CycleTime;
  if (CapedCycleTime > 50.0) CapedCycleTime = 50.0;


  // Detection points variables

  ZVoxelWorld * World;
  UShort Voxel[32];
  ZVoxelType * VoxelType[32];
  bool   IsEmpty[32];
  bool   IsJumper;
  bool   IsMiniJumper;
  ZVector3d P[32];

  enum { SENSOR_CUBECENTER = 5, SENSOR_CUBEUNDER=6 };
  enum { SENSOR_STARTCOLISION = 0, SENSOR_ENDCOLLISION=4 };

  bool RedoDetect = false;
  do
  {
    IsMiniJumper = false;
    RedoDetect = false;
    // Define detection points

    P[0] = Location + ZVector3d(0,0,0);   
    P[1] = Location + ZVector3d(-128,128,-128);   
    P[2] = Location + ZVector3d(128,128,-128);   
    P[3] = Location + ZVector3d(128,128,128);   
    P[4] = Location + ZVector3d(-128,128,128);   
    P[SENSOR_CUBECENTER] = Location + ZVector3d(0,128,0);   
    P[SENSOR_CUBEUNDER] = Location + ZVector3d(0,-128,0);

    // Fetch Voxels for detection points

    World = PhysicsEngine->World;
    for (i=1;i<=SENSOR_CUBEUNDER;i++)
    {
      Voxel[i]     = World->GetVoxelPlayerCoord_Secure(P[i].x,P[i].y,P[i].z);
      VoxelType[i] = GameEnv->VoxelTypeManager.VoxelTable[Voxel[i]];
      IsEmpty[i]   = VoxelType[i]->Is_PlayerCanPassThrough;
      if (Voxel[i]==285) IsMiniJumper = true;
    }
    IsOnGround = !IsEmpty[SENSOR_CUBEUNDER];
    IsJumper = Voxel[SENSOR_CUBECENTER] == 268;
    //IsMiniJumper = Voxel[SENSOR_CUBECENTER] == 285;
    if (IsMiniJumper && CarThrust > 20.0) {Location.y += 272.0; RedoDetect = true; }
  } while(RedoDetect);

  // Crash

  if (!VoxelType[SENSOR_CUBECENTER]->Is_PlayerCanPassThrough) this->Stop_Riding();

  // Speed Coef

  double SpeedCoef;
  switch (Riding_Voxel)
  {

      case 263: SpeedCoef = 0.2; break;
      case 264: SpeedCoef = 1.0;  break;
      case 265: SpeedCoef = 1.0;  break;
      case 266: SpeedCoef = 1.0;  break;
      case 267: SpeedCoef = 1.0;  break;
      default:  SpeedCoef = 1.0;  break;
  }

  // direction
  double Vel = sqrt(Velocity.x * Velocity.x + Velocity.y*Velocity.y + Velocity.z * Velocity.z);
  if (   IsOnGround && Vel > 300.0)
  {
    ViewDirection.yaw += SteerAngle/100.0 * CycleTime;
    if (ViewDirection.yaw < 0.0) ViewDirection.yaw += 360.0;
    if (ViewDirection.yaw > 360.0) ViewDirection.yaw -= 360.0;
  }
  ViewDirection.roll = -SteerAngle / 200.0 * CarThrust;

  if (Velocity.y < 0 ) ViewDirection.pitch = Velocity.y / 400.0;
  else                 ViewDirection.pitch = Velocity.y / 500.0;

  SteerAngle = SteerAngle / (1.0 + (CapedCycleTime / 1000.0));

  // Physics interaction





  switch (Riding_Voxel)
  {
    default:
    case 263:
    case 264:
    case 265:
             {
               ZPolar3d Direction = ViewDirection;
               Direction.Len = 1.0;
               ZVector3d CDir = Direction;
               double FrictionCoef = VoxelType[SENSOR_CUBEUNDER]->FrictionCoef*1000.0;
               //printf("Friction : %f\n",FrictionCoef);
               CDir.x = CDir.x * CarThrust * SpeedCoef * 100; CDir.z = CDir.z * CarThrust * 100;
               double MaxAccel;
               switch(Riding_Voxel)
               {
                 case 263:MaxAccel = CapedCycleTime*1.0*FrictionCoef; break;
                 case 264:MaxAccel = CapedCycleTime*5.0*FrictionCoef; break;
                 case 265:MaxAccel = CapedCycleTime*5.0*FrictionCoef; break;
                 default: MaxAccel = CapedCycleTime*5.0*FrictionCoef; break;
               }
               if (IsOnGround)
               {
                 CDir.x = CDir.x - Velocity.x; CDir.z = CDir.z - Velocity.z;

                 if (CDir.x > MaxAccel) CDir.x = MaxAccel;
                 if (CDir.x <-MaxAccel) CDir.x =-MaxAccel;
                 if (CDir.z > MaxAccel) CDir.z = MaxAccel;
                 if (CDir.z <-MaxAccel) CDir.z =-MaxAccel;

                 Velocity.x += CDir.x;
                 Velocity.z += CDir.z;
               }
             }
             break;
    case 266:
    case 267:
             // No Sliding, perfect fit.
             {
               ZPolar3d Direction = ViewDirection;
               Direction.Len = CarThrust * SpeedCoef * 100;
               ZVector3d CDir = Direction;
               Velocity.x = CDir.x;
               Velocity.z = CDir.z;
             }
             break;
  }

  if (!IsOnGround) Velocity.y -= 5.0 * CapedCycleTime;
  if (IsJumper) Velocity.y = 50.0 * CarThrust;

  // The viscous friction loss...

  ZVector3d Frottement;
/*
  FrictionCoef = GameEnv->VoxelTypeManager.VoxelTable[Voxel[0]]->FrictionCoef;
  CarSCX = 2.0;
  Frottement = (Velocity * Velocity * FrictionCoef * CarSCX / 10000.0 * CycleTime) + 1.0;
  Velocity /= Frottement;
*/

  // Velocity to displacement.

  ZVector3d Dep,Dep2;
  double DepLen;

  Dep = Velocity * CapedCycleTime / 1000.0;
  DepLen = sqrt(Dep.x*Dep.x + Dep.y*Dep.y + Dep.z*Dep.z);


  // Collision detection loop and displacement correction

  ZRayCast_in In;
  ZRayCast_out Out[32];
  double DistanceMin;
  Long CollisionIndice;
  Bool Collided;
  Bool Continue;
  Bool PEnable[32];


  In.Camera = nullptr;
  In.MaxCubeIterations = ceil(DepLen / 256)+5; // 6;
  In.PlaneCubeDiff = In.MaxCubeIterations - 3;
  In.MaxDetectionDistance = 3000000.0;

  // ****
  for (i=0;i<24;i++) {PEnable[i] = true;}
  Continue = true;
  if ( (Dep.x == 0) && (Dep.y == 0) && (Dep.z == 0.0) ) { Continue = false; }

  while (Continue)
  {

    Collided = false;
    DistanceMin = 10000000.0;
    CollisionIndice = -1;
    for (i=SENSOR_STARTCOLISION;i<=SENSOR_ENDCOLLISION;i++)
    {

      if (PEnable[i]) // (PEnable[i])
      {
        bool ret;
        bool Redo;

        do
        {
          Redo = false;
          // Ray Casting ensure very precise collision detection. No misses.
          ret = World->RayCast_Vector(P[i],Dep , &In, &(Out[i]), false);         // Normal points.
          if (ret)
          {
            if (Out[i].CollisionDistance < DistanceMin )
            {
              if ( (Out[i].CollisionDistance <= DepLen) && (65535 == World->GetVoxel( Out[i].PointedVoxel.x, Out[i].PointedVoxel.y, Out[i].PointedVoxel.z )) )
              {
                World->GetVoxel_Secure( Out[i].PointedVoxel.x, Out[i].PointedVoxel.y, Out[i].PointedVoxel.z );
                Redo = true;
              }
              else
              {
                Collided = true; DistanceMin = Out[i].CollisionDistance; CollisionIndice = i;
              }
            }

          }
        } while (Redo);


      }
    }
    // printf("\n");

    DepLen = sqrt(Dep.x*Dep.x + Dep.y*Dep.y + Dep.z*Dep.z);

    if (Collided && (DistanceMin < DepLen || DistanceMin <= 1.1) )
    {
      switch(Out[CollisionIndice].CollisionAxe)
      {
        case 0: Dep.x=0.0; Event_Collision(Velocity.x / CycleTime); Velocity.x = 0.0; break;
        case 1: Dep.y=0.0; /*Event_Collision(Velocity.y / CycleTime);*/ Velocity.y = 0.0; ;JumpDebounce = 0;break;
        case 2: Dep.z=0.0; Event_Collision(Velocity.z / CycleTime); Velocity.z = 0.0;  break;
      }
    }
    else
    {

//
      Deplacement = 0.0;
      Continue = false;
    }
  }

  Location += Dep;
  Camera.x = Location.x + 0.0;
  Camera.y = Location.y + 128.0;
  Camera.z = Location.z + 0.0;
  Camera.Pitch = ViewDirection.pitch;
  Camera.Roll  = ViewDirection.roll;
  Camera.Yaw   = ViewDirection.yaw;


}

void ZActor_Player::DoPhysic_Plane(double CycleTime)
{
  ZVector3d P[32];
  UShort Voxel[32];
  ZVoxelType * VoxelType[32];
  bool   IsEmpty[32];
  ZVoxelWorld * World;
  ZVector3d Dep,Dep2;
  double DepLen;
  double CutingAltitude;
  ULong i;

  double CapedCycleTime;





  // Sound of the reactor

  if (PlaneEngineOn && (!IsDead))
  {

     if (PlaneReactorSoundHandle==nullptr || PlaneWaitForRectorStartSound) {PlaneReactorSoundHandle = GameEnv->Sound->Start_PlaySound(3,true, false, 1.00,nullptr); PlaneWaitForRectorStartSound = false; }
     else                             GameEnv->Sound->ModifyFrequency(PlaneReactorSoundHandle, (PlaneEngineThrust) / 60000.0 + 1.0);

  }


  // Caped cycle time for some calculation to avoid inconsistency

  CapedCycleTime = CycleTime;
  if (CapedCycleTime > 5.0) CapedCycleTime = 5.0;

  // Colored vision off. May be reactivated further.

  Camera.ColoredVision.Activate = false;

  // Define Detection points

  P[0] = Location + ZVector3d(0,0,0);   
  P[1] = Location + ZVector3d(0,128,0);   
  P[2] = Location + ZVector3d(0,-128,0);

  // Get the Voxel Informations

  World = PhysicsEngine->World;
  for (i=0;i<3;i++)
  {
    Voxel[i]     = World->GetVoxelPlayerCoord_Secure(P[i].x,P[i].y,P[i].z);
    VoxelType[i] = GameEnv->VoxelTypeManager.VoxelTable[Voxel[i]];
    IsEmpty[i]   = VoxelType[i]->Is_PlayerCanPassThrough;
  }

  IsOnGround = !IsEmpty[2];



  if (!IsDead)
  {
    // Crash if collision with bloc
    if (!VoxelType[1]->Is_PlayerCanPassThrough) Event_PlaneCrash();
    // Limited airplane will crash if not landing on right material.
    if (IsOnGround && Riding_Voxel == 239 && Voxel[2]!=240 && PlaneSpeed > 50.0) Event_PlaneCrash();
  }

  // Angle computing

  if (ViewDirection.roll >0.0 && ViewDirection.roll <=90.0) ViewDirection.yaw += ( (ViewDirection.roll / 1440.0 * PlaneCommandResponsiveness)  * CycleTime);
  if (ViewDirection.roll <=360 && ViewDirection.roll >= 270.0 ) ViewDirection.yaw -= ((360.0 - ViewDirection.roll) / 1440.0 * PlaneCommandResponsiveness) * CycleTime;

  // Take plane on ground if speed < 8000

  if (! PlaneTakenOff)
  {
    if (PlaneSpeed <= 7000.0)
    {
      if (IsOnGround && !IsDead) { ViewDirection.pitch = 0.0; ViewDirection.roll = 0.0; }
    }
    else
    {
      if (!IsOnGround) PlaneTakenOff = true;
    }

  }

  // Landing : Crash if Landing at speed > 3000

  if (PlaneTakenOff && (!IsDead))
  {
    if (IsOnGround)
    {
      if (   PlaneSpeed > 3000.0  || PlaneSpeed < 1300.0                                     // Too Fast or too slow = Crash
          || (ViewDirection.pitch  > 45.0 && ViewDirection.pitch < 350.0 )                   // Bad pitch = Crash.
          || (ViewDirection.roll  > 45.0 && ViewDirection.roll < 315.0)                      // Bad roll  = Crash.
          )
      {
        Event_PlaneCrash();
      }

      if (ViewDirection.pitch>0.0 && ViewDirection.pitch < 315.0) { ViewDirection.pitch -= 0.0225 * CycleTime; if (ViewDirection.pitch < 0.0 ) ViewDirection.pitch = 0.0; }
      if (ViewDirection.pitch>315.0 && ViewDirection.pitch<= 360.0) { ViewDirection.pitch += 0.0225 * CycleTime; if (ViewDirection.pitch >= 360.0 ) ViewDirection.pitch = 0.0; }
      if (ViewDirection.roll >0.5 && ViewDirection.roll < 180.0) ViewDirection.roll  -= 0.0225 * CycleTime;
      if (ViewDirection.roll >315 && ViewDirection.roll < 360.0) ViewDirection.roll  += 0.0225 * CycleTime;
      PlaneLandedCounter += CycleTime;
      if (PlaneLandedCounter > 2000.0 ) { ViewDirection.pitch = 0.0; ViewDirection.roll = 0.0; PlaneTakenOff = false; PlaneLandedCounter = 0.0; }
    }
  }

  // Engine power

  ZPolar3d Direction = ViewDirection;
  ZVector3d CDir;
  Direction.Len = 1.0;
  CDir = Direction;

  // Engine Thrust acceleration.

  // PlaneSpeed += -(( PlaneSpeed - PlaneEngineThrust ) / 100 * CycleTime);
  PlaneSpeed += PlaneEngineThrust / 8000 * CycleTime * ((PlaneTakenOff) ? 1.0 : 0.50);

  // Altitude change modify cynetic energy.

  if (!PlaneEngineOn)
  {
    if (ViewDirection.pitch <=360.0 && ViewDirection.pitch >= 260.0)
    {
      // PlaneSpeed += -Velocity.y * sin((360.0 - ViewDirection.pitch)/57.295779513) / 100.0;
      PlaneSpeed += sin((360.0 - ViewDirection.pitch)/57.295779513) * CycleTime;
    }
    if (ViewDirection.pitch >0.0 && ViewDirection.pitch <= 100.0)
    {
      // PlaneSpeed +=  -Velocity.y * sin(ViewDirection.pitch/57.295779513) / 100.0;
      PlaneSpeed +=  -sin(ViewDirection.pitch/57.295779513) * CycleTime / 2.0;

    }
  }

  // Insuficient speed make command less responsive.

  PlaneCommandResponsiveness = (PlaneSpeed - 1000) / 2000.0;

  if (PlaneCommandResponsiveness < 0.0) PlaneCommandResponsiveness = 0.0;
  if (PlaneSpeed >= 4000.0 || IsOnGround ) PlaneCommandResponsiveness = 1.0;
  if (PlaneToohighAlt) PlaneCommandResponsiveness = 0.0;
  if (IsDead) PlaneCommandResponsiveness = 0.0;

  // Insuficient speed lower portance and make nasty effects

  if      (PlaneSpeed < 2000.0) CDir.y = 0.0;
  else if (PlaneSpeed < 4000.0) CDir.y *= (PlaneSpeed - 2000.0) / 2000.0;
   if (PlaneCommandResponsiveness < 0.5 && (ViewDirection.roll < 85.0 || ViewDirection.roll >100.0)) ViewDirection.roll+=0.01 * CycleTime;

  // The viscous friction loss...

  double FrictionCoef, PlaneSCX;
  ZVector3d Frottement;

  FrictionCoef = GameEnv->VoxelTypeManager.VoxelTable[Voxel[0]]->FrictionCoef;
  PlaneSCX = 1.0;
  PlaneSpeed /= (PlaneSpeed * PlaneSpeed * PlaneSCX / 100000000000.0 * CycleTime) + 1.0;
  Frottement = (Velocity * Velocity * FrictionCoef * PlayerSurface / 10000000.0 * CycleTime) + 1.0;
  Velocity /= Frottement;

  // Velocity += -(Velocity - (CDir * PlaneSpeed) );  // * PlaneCommandResponsiveness ;

  Velocity += -(Velocity - (CDir * PlaneSpeed) ) * PlaneCommandResponsiveness ;

  if (IsOnGround)
  {
    if (PlaneSpeed >0.0)
    {
      PlaneSpeed-= 0.05*CycleTime;
      if (Riding_Voxel==96 && Voxel[2]==240) PlaneSpeed-=0.2*CycleTime; // If plane Z1 landing on runway,
      if (PlaneSpeed < 0.0) PlaneSpeed = 0.0;
    }
    if (Velocity.x > 0.0) { Velocity.x -= 0.05*CycleTime; }
    if (Velocity.x < 0.0) { Velocity.x += 0.05*CycleTime; }
    if (Velocity.z > 0.0) { Velocity.z -= 0.05*CycleTime; }
    if (Velocity.z < 0.0) { Velocity.z += 0.05*CycleTime; }
  }

  // *** Gravity

  if ( (!PlaneEngineOn) || PlaneSpeed < 4000.0 || (!PlaneTakenOff)  )
  {
    double Gravity, CubeY;
    CubeY = Location.y / 256.0;
    if      (CubeY > 10000.0 && CubeY < 15000.0) { Gravity = 5.0 - (( (CubeY-10000.0) * (CubeY-10000.0)) / 5000000.0); } //5000000.0;
    else if (CubeY <= 10000.0) { Gravity = 5.0; }
    else                       { Gravity = 0.0; }

    Velocity.y -= (10.0 - LocationDensity) * Gravity * CapedCycleTime * ((PlaneTakenOff || IsOnGround) ? 1.0 : 10.0);
    if (PlaneToohighAlt && Location.y > 256000.0 ) {if (Velocity.y < -250.0)  Velocity.y = -250*256.0; }
    else                                           {if (Velocity.y < -50*256.0) Velocity.y = -50*256.0; }
  }

  // If going too high, something nasty will happens

  switch(Riding_Voxel)
  {
    case 239:CutingAltitude = 800.0 * 256.0; break;
    case 96:
    default: CutingAltitude = 5000.0 * 256.0; break;
  }

  if (Location.y > CutingAltitude && (!PlaneToohighAlt) )
  {
    PlaneToohighAlt = true;
    PlaneEngineThrust = 0.0;
    PlaneEngineOn = false;
    if ((PlaneReactorSoundHandle)) { GameEnv->Sound->Stop_PlaySound(PlaneReactorSoundHandle); PlaneReactorSoundHandle = nullptr; }
    GameEnv->GameWindow_Advertising->Advertise("ENGINE STALLED",ZGameWindow_Advertising::VISIBILITY_MEDIUM,0,3000.0, 3000.0 );
    GameEnv->GameWindow_Advertising->Advertise("PLANE IS FREE FALLING",ZGameWindow_Advertising::VISIBILITY_MEDLOW,0,3000.0, 3000.0 );

  }
  if (PlaneToohighAlt)
  {
    if (Location.y > (500.0 * 256.0) ) {Camera.ColoredVision.Activate= true; Camera.ColoredVision.Red = 0.8; Camera.ColoredVision.Green = 0.0; Camera.ColoredVision.Blue = 0.0; Camera.ColoredVision.Opacity = 0.3;}
    ViewDirection.pitch = 270.0;
    ViewDirection.roll = 0.0;
    PlaneFreeFallCounter += CycleTime;
    if (PlaneFreeFallCounter > 1000.0)
    {
      PlaneFreeFallMessage = "FALL WARNING : ALTITUDE ";
      PlaneFreeFallMessage << ((Long)(floor(Location.y/256.0)));
      PlaneFreeFallCounter = 0.0;
      GameEnv->GameWindow_Advertising->Advertise(PlaneFreeFallMessage.String ,ZGameWindow_Advertising::VISIBILITY_LOW,0,800.0, 400.0 );
    }
  }

  // Printing data
/*
  Test_T1++; if (Test_T1 > 100 )
  {
    Test_T1 = 0;
    printf("Speed : %lf Pitch: %lf Thrust:%lf Altitude:%lf IsOnGround:%d TakenOff:%d Gravity:%d CycleTime %lf\n", PlaneSpeed, ViewDirection.pitch, PlaneEngineThrust, Location.y / 256.0, IsOnGround, PlaneTakenOff, GravityApplied, CycleTime);
  }
*/
  // Velocity to displacement.

  Dep = Velocity * CycleTime / 1000.0;
  DepLen = sqrt(Dep.x*Dep.x + Dep.y*Dep.y + Dep.z*Dep.z);

  // Collision detection loop and displacement correction

  ZRayCast_in In;
  ZRayCast_out Out[32];
  double DistanceMin;
  Long CollisionIndice;
  Bool Collided;
  Bool Continue;
  Bool PEnable[32];


  In.Camera = nullptr;
  In.MaxCubeIterations = ceil(DepLen / 256)+5; // 6;
  In.PlaneCubeDiff = In.MaxCubeIterations - 3;
  In.MaxDetectionDistance = 3000000.0;

  // ****
  for (i=0;i<24;i++) {PEnable[i] = true;}
  Continue = true;
  if ( (Dep.x == 0) && (Dep.y == 0) && (Dep.z == 0.0) ) { Continue = false; return; }

  while (Continue)
  {

    Collided = false;
    DistanceMin = 10000000.0;
    CollisionIndice = -1;
    for (i=0;i<1;i++)
    {

      if (PEnable[i]) // (PEnable[i])
      {
        bool ret;
        bool Redo;

        do
        {
          Redo = false;
          ret = World->RayCast_Vector(P[i],Dep , &In, &(Out[i]), false);         // Normal points.
          if (ret)
          {
            if (Out[i].CollisionDistance < DistanceMin )
            {
              if ( (Out[i].CollisionDistance <= DepLen) && (65535 == World->GetVoxel( Out[i].PointedVoxel.x, Out[i].PointedVoxel.y, Out[i].PointedVoxel.z )) )
              {
                World->GetVoxel_Secure( Out[i].PointedVoxel.x, Out[i].PointedVoxel.y, Out[i].PointedVoxel.z );
                Redo = true;
              }
              else
              {
                Collided = true; DistanceMin = Out[i].CollisionDistance; CollisionIndice = i;
              }
            }

          }
        } while (Redo);


      }
    }
    // printf("\n");

    DepLen = sqrt(Dep.x*Dep.x + Dep.y*Dep.y + Dep.z*Dep.z);

    if (Collided && (DistanceMin < DepLen || DistanceMin <= 1.1) )
    {
      switch(Out[CollisionIndice].CollisionAxe)
      {
        case 0: Dep.x=0.0; Event_Collision(Velocity.x / CycleTime); Velocity.x = 0.0; break;
        case 1: Dep.y=0.0; Event_Collision(Velocity.y / CycleTime); Velocity.y = 0.0; JumpDebounce = 0;break;
        case 2: Dep.z=0.0; Event_Collision(Velocity.z / CycleTime); Velocity.z = 0.0; break;
      }
    }
    else
    {
      ZVector3d NewLocation;

      NewLocation = Location + Dep;

      //  SetPosition( NewLocation );
//
      Location = NewLocation;
      Camera.x = Location.x + 0.0;
      Camera.y = Location.y + 128.0;
      Camera.z = Location.z + 0.0;
      Camera.Pitch = ViewDirection.pitch;
      Camera.Roll  = ViewDirection.roll;
      Camera.Yaw   = ViewDirection.yaw;
//
      Deplacement = 0.0;
      Continue = false;
    }
  }

  // Dead Rolling and view

  if (IsDead)
  {
    //ViewDirection.roll += 0.5 * CycleTime * ( DeathChronometer / 10000.0 ); if (ViewDirection.roll > 360) ViewDirection.roll -= 360.0;
    //ViewDirection.pitch+= 1.2 * CycleTime * ( DeathChronometer / 10000.0 ); if (ViewDirection.pitch > 360) ViewDirection.pitch -= 360.0;

    ViewDirection.roll += 0.3 * CycleTime * ( DeathChronometer / 5000.0 ); if (ViewDirection.roll > 360) ViewDirection.roll -= 360.0;
    ViewDirection.pitch+= 0.5 * CycleTime * ( DeathChronometer / 5000.0 ); if (ViewDirection.pitch > 360) ViewDirection.pitch -= 360.0;
    DeathChronometer-= CycleTime;
    if (DeathChronometer> 4250.0 && DeathChronometer<4500.0)
    { DeathChronometer = 4250.0; GameEnv->GameWindow_Advertising->Advertise("YOU ARE DEAD", ZGameWindow_Advertising::VISIBILITY_HIGH, 0, 3000.0, 0.0); }
    if (DeathChronometer <= 0.0) {Event_Death(); return;}
  }

}





// ********************************************************************************
//                          Old version of the plane
// ********************************************************************************

void ZActor_Player::DoPhysic_Plane_Old(double CycleTime)
{
  ZVector3d P[32];
  UShort Voxel[32];
  ZVoxelWorld * World;
  // bool   IsEmpty[32];
  ZVector3d Dep,Dep2;
  double DepLen;
  ULong i;

  // Colored vision off. May be reactivated further.

  Camera.ColoredVision.Activate = false;

  // Define Detection points

  P[0] = Location + ZVector3d(0,0,0);   

  // Get the Voxel Informations

  World = PhysicsEngine->World;
  for (i=0;i<24;i++)
  {
    Voxel[i]     = World->GetVoxelPlayerCoord(P[i].x,P[i].y,P[i].z);
    // IsEmpty[i]   = VoxelType[i]->Is_PlayerCanPassThrough;
  }


  // The gravity...

  // if      (CubeY > 10000.0 && CubeY < 15000.0) { Gravity = 5.0 - (( (CubeY-10000.0) * (CubeY-10000.0)) / 5000000.0); } //5000000.0;
  /* else if (CubeY <= 10000.0) { Gravity = 5.0; }
  else                       { Gravity = 0.0; }
  */

  // Velocity.y -= (PlayerDensity - LocationDensity) * Gravity * CycleTime;

  // Angle computing

  if (ViewDirection.roll >0.0 && ViewDirection.roll <=90.0) ViewDirection.yaw += ViewDirection.roll / 1440.0 * CycleTime;
  if (ViewDirection.roll <=360 && ViewDirection.roll >= 270.0 ) ViewDirection.yaw -= (360.0 - ViewDirection.roll) / 1440.0 * CycleTime;


  // Engine power
  ZPolar3d Direction = ViewDirection;
  ZVector3d CDir;
  Direction.Len = 1.0;
  CDir = Direction;
  Velocity = CDir * 10 * 400.0;

  // Deplacement computation



  // The viscous friction loss...

  double FrictionCoef, PlaneSCX;
  ZVector3d Frottement;
  FrictionCoef = GameEnv->VoxelTypeManager.VoxelTable[Voxel[0]]->FrictionCoef;
  PlaneSCX = 1.0;
  Frottement = (Velocity * Velocity * FrictionCoef * PlaneSCX / 1000000000.0 * CycleTime) + 1.0;
  Velocity /= Frottement;




  Dep = Velocity * CycleTime / 1000.0;
  DepLen = sqrt(Dep.x*Dep.x + Dep.y*Dep.y + Dep.z*Dep.z);


  // Collision detection loop and displacement correction

  ZRayCast_in In;
  ZRayCast_out Out[32];
  double DistanceMin;
  Long CollisionIndice;
  Bool Collided;
  Bool Continue;
  Bool PEnable[32];


  In.Camera = nullptr;
  In.MaxCubeIterations = ceil(DepLen / 256)+5; // 6;
  In.PlaneCubeDiff = In.MaxCubeIterations - 3;
  In.MaxDetectionDistance = 3000000.0;


  // ****
  for (i=0;i<24;i++) {PEnable[i] = true;}
  Continue = true;
  if ( (Dep.x == 0) && (Dep.y == 0) && (Dep.z == 0.0) ) { Continue = false; return; }

  Location_Old = Location;
  while (Continue)
  {

    Collided = false;
    DistanceMin = 10000000.0;
    CollisionIndice = -1;
    for (i=0;i<1;i++)
    {

      if (PEnable[i]) // (PEnable[i])
      {
        bool ret;

        ret = World->RayCast_Vector(P[i],Dep , &In, &(Out[i]), false);         // Normal points.
        if (ret)
        {
          if (Out[i].CollisionDistance < DistanceMin ) { Collided = true; DistanceMin = Out[i].CollisionDistance; CollisionIndice = i; }
        }
      }
    }
    // printf("\n");

    DepLen = sqrt(Dep.x*Dep.x + Dep.y*Dep.y + Dep.z*Dep.z);

    if (Collided && (DistanceMin < DepLen || DistanceMin <= 1.1) )
    {
      switch(Out[CollisionIndice].CollisionAxe)
      {
        case 0: Dep.x=0.0; Event_Collision(Velocity.x); Velocity.x = 0.0; break;
        case 1: Dep.y=0.0; Event_Collision(Velocity.y); Velocity.y = 0.0; JumpDebounce = 0;break;
        case 2: Dep.z=0.0; Event_Collision(Velocity.z); Velocity.z = 0.0; break;
      }
    }
    else
    {
      ZVector3d NewLocation;

      NewLocation = Location + Dep;

        SetPosition( NewLocation );

        Deplacement = 0.0;
        Continue = false;

    }
  }
}




























void ZActor_Player::DoPhysic_GroundPlayer(double CycleTime)
{
  ZVector3d Dep,Dep2;
  ZVector3d Tmp;
  ZVector3d P[32];
  Bool PEnable[32];
  bool PInvert[32];
  UShort Voxel[32];
  bool   IsEmpty[32];
  ZVoxelType * VoxelType[32];
  ULong i;
  ZVoxelWorld * World;
  double DepLen;


  // Voxel Looking event system

  if (   PointedVoxel.PointedVoxel != PointedVoxel_Previous.PointedVoxel
      || PointedVoxel.Collided != PointedVoxel_Previous.Collided)
  {
    ZVoxelLocation Loc;
    ZVector3L Coords;


    if (PointedVoxel_Previous.Collided)
    {
      PointedVoxel_Previous.PointedVoxel.GetCo(&Coords);
      if(GameEnv->World->GetVoxelLocation(&Loc, &Coords))
      {
        GameEnv->VoxelTypeManager.GetVoxelType( Loc.Sector->Data[Loc.Offset] )->Event_End_Selected(&Loc, &Coords);
        PointedVoxel_Previous.Collided = false;
      }
    }

    if (PointedVoxel.Collided)
    {
      PointedVoxel.PointedVoxel.GetCo(&Coords);
      if(GameEnv->World->GetVoxelLocation(&Loc, &Coords))
      {
        GameEnv->VoxelTypeManager.GetVoxelType( Loc.Sector->Data[Loc.Offset] )->Event_Start_Selected(&Loc, &Coords);
      }

    }

    PointedVoxel_Previous = PointedVoxel;
  }
  else if (PointedVoxel.Collided)
  {
    ZVoxelLocation Loc;
    ZVector3L Coords;

    PointedVoxel.PointedVoxel.GetCo(&Coords);
    if(GameEnv->World->GetVoxelLocation(&Loc, &Coords))
    {
      GameEnv->VoxelTypeManager.GetVoxelType( Loc.Sector->Data[Loc.Offset] )->Event_Is_Selected(&Loc, &Coords);
    }
  }



  // Voxel Help System

  if (PointedVoxel.Collided)
  {
    UShort VoxelType;

    LastHelpTime += CycleTime;

    VoxelType = GameEnv->World->GetVoxel(PointedVoxel.PointedVoxel.x, PointedVoxel.PointedVoxel.y, PointedVoxel.PointedVoxel.z);
    if (GameEnv->VoxelTypeManager.VoxelTable[VoxelType]->Is_HasHelpingMessage)
    {
      if (   PointedVoxel.PointedVoxel.x != LastHelpVoxel.x
          || PointedVoxel.PointedVoxel.y != LastHelpVoxel.y
          || PointedVoxel.PointedVoxel.z != LastHelpVoxel.z
          || LastHelpTime > 3005.0                         )
      {
        LastHelpTime = 0.0;
        LastHelpVoxel.x = PointedVoxel.PointedVoxel.x;
        LastHelpVoxel.y = PointedVoxel.PointedVoxel.y;
        LastHelpVoxel.z = PointedVoxel.PointedVoxel.z;
        GameEnv->GameWindow_Advertising->Advertise(GameEnv->VoxelTypeManager.VoxelTable[VoxelType]->HelpingMessage.String, ZGameWindow_Advertising::VISIBILITY_VERYHARDTOREAD, 0, 3000.0,0.0 );
      }
    }

  }


  // Colored vision off. May be reactivated further.

  Camera.ColoredVision.Activate = false;

  // Define Detection points

  P[0] = Location + ZVector3d(-75.0,+0.0,+75.0);
  P[1] = Location + ZVector3d(+75.0,+0.0,+75.0);
  P[2] = Location + ZVector3d(+75.0,+0.0,-75.0);
  P[3] = Location + ZVector3d(-75.0,+0.0,-75.0);

  P[4] = Location + ZVector3d(-75.0,+128.0,+75.0);
  P[5] = Location + ZVector3d(+75.0,+128.0,+75.0);
  P[6] = Location + ZVector3d(+75.0,+128.0,-75.0);
  P[7] = Location + ZVector3d(-75.0,+128.0,-75.0);

  P[8] = Location + ZVector3d(-75.0,+384.0,+75.0);
  P[9] = Location + ZVector3d(+75.0,+384.0,+75.0);
  P[10] = Location + ZVector3d(+75.0,+384.0,-75.0);
  P[11] = Location + ZVector3d(-75.0,+384.0,-75.0);

  P[12] = Location + ZVector3d(-75.0,+475.0,+75.0);
  P[13] = Location + ZVector3d(+75.0,+475.0,+75.0);
  P[14] = Location + ZVector3d(+75.0,+475.0,-75.0);
  P[15] = Location + ZVector3d(-75.0,+475.0,-75.0);

  P[16] = Location + ZVector3d(-70.0,-5.0,-70.0); 
  P[17] = Location + ZVector3d(+70.0,-5.0,-70.0); 
  P[18] = Location + ZVector3d(+70.0,-5.0,+70.0); 
  P[19] = Location + ZVector3d(-70.0,-5.0,+70.0); 

  P[20] = Location + ZVector3d(0,-5,0);  
  P[21] = Location + ZVector3d(0,0,0);   
  P[22] = Location + ZVector3d(0,128,0); 
  P[23] = Location + ZVector3d(0,384,0); 

  // Get the Voxel Informations

  World = PhysicsEngine->World;
  for (i=0;i<24;i++)
  {
    Voxel[i]     = World->GetVoxelPlayerCoord(P[i].x,P[i].y,P[i].z);
    VoxelType[i] = GameEnv->VoxelTypeManager.VoxelTable[Voxel[i]];
    IsEmpty[i]   = VoxelType[i]->Is_PlayerCanPassThrough;
  }

  // Detect player is on ground

  if ( IsEmpty[16] && IsEmpty[17] && IsEmpty[18] && IsEmpty[19] ) IsOnGround = false;
  else                                                            IsOnGround = true;

  // Detect if player is in liquid and compute the mean density.


  IsFootInLiquid = VoxelType[22]->Is_Liquid;
  IsHeadInLiquid = VoxelType[23]->Is_Liquid;
  IsInLiquid = IsFootInLiquid && IsHeadInLiquid;
  LocationDensity = (VoxelType[22]->LiquidDensity + VoxelType[23]->LiquidDensity) / 2.0;
  IsWalking = (Deplacement.x != 0.0) || (Deplacement.z != 0.0 );

  if (Voxel[23]==85) { Camera.ColoredVision.Activate = true; Camera.ColoredVision.Red= 0.0f; Camera.ColoredVision.Green = 0.0f; Camera.ColoredVision.Blue = 1.0f; Camera.ColoredVision.Opacity  = 0.5f; }
  if (Voxel[23]==86) { Camera.ColoredVision.Activate = true; Camera.ColoredVision.Red= 0.0f; Camera.ColoredVision.Green = 1.0f; Camera.ColoredVision.Blue = 0.0f; Camera.ColoredVision.Opacity  = 0.5f; }
  if (Voxel[23]==89) { Camera.ColoredVision.Activate = true; Camera.ColoredVision.Red= 1.0f; Camera.ColoredVision.Green = 1.0f; Camera.ColoredVision.Blue = 0.0f; Camera.ColoredVision.Opacity  = 0.9f; }


  // Voxel collision and player damage.


  bool harming = 0;
  //UShort HarmingVoxel;
  double MaxHarming = 0;
  double VoxelHarming;
  for(i=0;i<16;i++) if (GameEnv->VoxelTypeManager.VoxelTable[Voxel[i]]->Is_Harming)
  {
    harming |= true;
    //HarmingVoxel = Voxel[i];
    VoxelHarming = GameEnv->VoxelTypeManager.VoxelTable[Voxel[i]]->HarmingLifePointsPerSecond;
    if (VoxelHarming > MaxHarming) MaxHarming = VoxelHarming;
  }
  if ((harming))
  {
    Camera.ColoredVision.Activate = true;
    Camera.ColoredVision.Red      = 1.0f;
    Camera.ColoredVision.Green    = 0.0f;
    Camera.ColoredVision.Blue     = 0.0f;
    Camera.ColoredVision.Opacity  = 0.5f;
    // printf("LifePoints: %lf \n",LifePoints);
    LifePoints -= MaxHarming / 1000.0 * CycleTime;
    if (LifePoints < 0.0) LifePoints = 0.0;
  }

  if (LifePoints <= 0.0 && !IsDead)
  {
    this->IsDead = true;
    GameEnv->GameWindow_Advertising->Advertise("YOU ARE DEAD", 0, 0, 10000.0, 10000.0);
    DeathChronometer = 5000.0;
  }

  if (IsDead)
  {
    Camera.ColoredVision.Activate = true; Camera.ColoredVision.Red= 0.0f; Camera.ColoredVision.Green = 0.0f; Camera.ColoredVision.Blue = 0.0f; Camera.ColoredVision.Opacity  = 0.8f;
    DeathChronometer -= CycleTime;
    if (DeathChronometer <= 0.0)
    {
      Event_Death();

    }
    return;
  }

  // Vision change for the head in voxel


  if (Voxel[23] == 52)
  {
    Camera.ColoredVision.Activate = true;
    Camera.ColoredVision.Red     = 1.0f;
    Camera.ColoredVision.Green   = 0.0f;
    Camera.ColoredVision.Blue    = 0.0f;
    Camera.ColoredVision.Opacity = 0.95f;
  }

  // Physical deplacement computation

  if (1)
  {
    ZVector3d Frottement, WalkSpeed, VelocityIncrease, MaxVelocityIncrease, GripFactor;
    double FrictionCoef;

    // Limit the frametime to avoid accidental stuttering nasty effects.

    if (CycleTime > 160.0) CycleTime = 160.0; // Limit frame time

    // The gravity...
    double Gravity, CubeY;
    CubeY = Location.y / 256.0;
    if      (CubeY > 10000.0 && CubeY < 15000.0) { Gravity = 5.0 - (( (CubeY-10000.0) * (CubeY-10000.0)) / 5000000.0); } //5000000.0;
    else if (CubeY <= 10000.0) { Gravity = 5.0; }
    else                       { Gravity = 0.0; }

    Velocity.y -= (PlayerDensity - LocationDensity) * Gravity * CycleTime;

    //printf("Gravity %lf y: %lf\n",Gravity, CubeY);

    // Player legs action

    GripFactor.x = GameEnv->VoxelTypeManager.VoxelTable[Voxel[20]]->Grip_Horizontal;
    GripFactor.z = GripFactor.x;
    GripFactor.y = GameEnv->VoxelTypeManager.VoxelTable[Voxel[22]]->Grip_Vertical;
    WalkSpeed = Deplacement * 50.0;
    if (GameEnv->VoxelTypeManager.VoxelTable[Voxel[20]]->Is_SpaceGripType)
    {
      VelocityIncrease = WalkSpeed * (CycleTime / 16.0)* GripFactor;
      if (WalkSpeed.x > 0.0) {if (Velocity.x > WalkSpeed.x) if (VelocityIncrease.x>0.0) VelocityIncrease.x = 0.0; }
      if (WalkSpeed.x < 0.0) {if (Velocity.x < WalkSpeed.x) if (VelocityIncrease.x<0.0) VelocityIncrease.x = 0.0; }
      if (WalkSpeed.z > 0.0) {if (Velocity.z > WalkSpeed.z) if (VelocityIncrease.z>0.0) VelocityIncrease.z = 0.0; }
      if (WalkSpeed.z < 0.0) {if (Velocity.z < WalkSpeed.z) if (VelocityIncrease.z<0.0) VelocityIncrease.z = 0.0; }
    }
    else
    {
      MaxVelocityIncrease = (WalkSpeed - Velocity) * GripFactor;
      VelocityIncrease = MaxVelocityIncrease * (CycleTime / 16.0);
      VelocityIncrease.y = 0.0;
      if (fabs(VelocityIncrease.x) > fabs(MaxVelocityIncrease.x) ) VelocityIncrease.x = MaxVelocityIncrease.x;
      if (fabs(VelocityIncrease.y) > fabs(MaxVelocityIncrease.y) ) VelocityIncrease.y = MaxVelocityIncrease.y;
      if (fabs(VelocityIncrease.z) > fabs(MaxVelocityIncrease.z) ) VelocityIncrease.z = MaxVelocityIncrease.z;
    }
    Velocity += VelocityIncrease;

    // The viscous friction loss...

    FrictionCoef = GameEnv->VoxelTypeManager.VoxelTable[Voxel[20]]->FrictionCoef;
    FrictionCoef+= GameEnv->VoxelTypeManager.VoxelTable[Voxel[22]]->FrictionCoef;
    FrictionCoef+= GameEnv->VoxelTypeManager.VoxelTable[Voxel[23]]->FrictionCoef;
    Frottement = (Velocity * Velocity * FrictionCoef * PlayerSurface / 1000000000.0 * CycleTime) + 1.0;
    Velocity /= Frottement;

    // VelocityBooster.

    if (Voxel[20] == 16) Velocity *= 1.0 + CycleTime / 40.0;

    // Velocity to deplacement

    Dep = Velocity * CycleTime / 1000.0;
    DepLen = sqrt(Dep.x*Dep.x + Dep.y*Dep.y + Dep.z*Dep.z);
    // printf("Velocity %lf %lf %lf Increase: %lf %lf %lf CycleTime :%lf\n",Velocity.x, Velocity.y, Velocity.z, VelocityIncrease.x, VelocityIncrease.y, VelocityIncrease.z, CycleTime );

  }



  // Disable all control points. Mark anti fall points with invert flag.

  for (i=0;i<24;i++) {PEnable[i] = false; PInvert[i] = false; }
  for (i=16;i<20;i++){PInvert[i]=true;}

  // Vector direction enable some control points.

  if (Dep.x < 0.0 ) { PEnable[4]  = true; PEnable[7] = true; PEnable[8]  = true; PEnable[11] = true; }
  if (Dep.x > 0.0 ) { PEnable[5]  = true; PEnable[6] = true; PEnable[9]  = true; PEnable[10] = true; }
  if (Dep.z > 0.0 ) { PEnable[4]  = true; PEnable[5] = true; PEnable[8]  = true; PEnable[9]  = true; }
  if (Dep.z < 0.0 ) { PEnable[6]  = true; PEnable[7] = true; PEnable[10] = true; PEnable[11] = true; }
  if (Dep.y > 0.0 ) { PEnable[12] = true; PEnable[13]= true; PEnable[14] = true; PEnable[15] = true; }
  if (Dep.y < 0.0 ) { PEnable[0]  = true; PEnable[1] = true; PEnable[2]  = true; PEnable[3]  = true; }

  PEnable[0]  = true; PEnable[1] = true; PEnable[2]  = true; PEnable[3]  = true;

  // Anti Fall test point activation

  if (this->Flag_ActivateAntiFall)
  {
    if (Dep.x >0)
    {
       if   (Dep.z>0) {PEnable[16] = true;}
       else           {PEnable[19] = true;}
    }
    else
    {
      if   (Dep.z>0) {PEnable[17] = true;}
      else           {PEnable[18] = true;}
    }

    if ( Dep.y>0.0)                                                 {PEnable[16]=PEnable[17]=PEnable[18]=PEnable[19] = false; }
    if ( IsEmpty[19] && IsEmpty[18] && IsEmpty[17] && IsEmpty[16] ) {PEnable[16]=PEnable[17]=PEnable[18]=PEnable[19] = false; }
    if ( !IsEmpty[20] )                                             {PEnable[16]=PEnable[17]=PEnable[18]=PEnable[19] = false; }

    if ( IsEmpty[19] && IsEmpty[18] && IsEmpty[17] ) PEnable[16] = true;
    if ( IsEmpty[18] && IsEmpty[17] && IsEmpty[16] ) PEnable[19] = true;
    if ( IsEmpty[17] && IsEmpty[16] && IsEmpty[19] ) PEnable[18] = true;
    if ( IsEmpty[16] && IsEmpty[19] && IsEmpty[18] ) PEnable[17] = true;
  }

  // Collision detection loop and displacement correction

  ZRayCast_in In;
  ZRayCast_out Out[32];


  In.Camera = nullptr;
  In.MaxCubeIterations = ceil(DepLen / 256)+5; // 6;
  In.PlaneCubeDiff = In.MaxCubeIterations - 3;
  In.MaxDetectionDistance = 3000000.0;
  double DistanceMin;
  Long CollisionIndice;
  Bool Collided, IsCollided_h;
  Bool Continue;

  // ****

  Continue = true;
  if ( (Dep.x == 0) && (Dep.y == 0) && (Dep.z == 0.0) ) { Continue = false; return; }

  // printf("Loc: %lf %lf %lf\n",Location.x,Location.y,Location.z);

  Location_Old = Location;
  IsCollided_h = false;
  while (Continue)
  {

    Collided = false;
    DistanceMin = 10000000.0;
    CollisionIndice = -1;
    for (i=0;i<20;i++)
    {

      if (PEnable[i]) // (PEnable[i])
      {
        bool ret;
        if (PInvert[i]) ret = World->RayCast_Vector_special(P[i],Dep , &In, &(Out[i]), PInvert[i]); // If anti fall points, use special routine and invert empty/full detection.
        else            ret = World->RayCast_Vector(P[i],Dep , &In, &(Out[i]), PInvert[i]);         // Normal points.

        if (ret)
        {
          if (Out[i].CollisionDistance < DistanceMin ) { Collided = true; DistanceMin = Out[i].CollisionDistance; CollisionIndice = i; }
        }
      }
    }
    // printf("\n");

    DepLen = sqrt(Dep.x*Dep.x + Dep.y*Dep.y + Dep.z*Dep.z);

    if (Collided && (DistanceMin < DepLen || DistanceMin <= 1.1) )
    {
      // printf("Collided(Loc:%lf %lf %lf dep: %lf %lf %lf Point : %lf %lf %lf Ind:%ld ))\n",Location.x,Location.y,Location.z, Dep.x,Dep.y,Dep.z,Out[CollisionIndice].CollisionPoint.x,Out[CollisionIndice].CollisionPoint.y, Out[CollisionIndice].CollisionPoint.z, CollisionIndice);
      // World->RayCast_Vector(P[CollisionIndice],Dep , &In, &(Out[CollisionIndice]));
      // Dep = Dep - (P[CollisionIndice] - Out[CollisionIndice].CollisionPoint);
      // SetPosition( Out[CollisionIndice].CollisionPoint );
      // Dep = 0.0;
      switch(Out[CollisionIndice].CollisionAxe)
      {
        case 0: Dep.x=0.0; Event_Collision(Velocity.x); Velocity.x = 0.0; IsCollided_h = true; break;
        case 1: Dep.y=0.0;
                           // The snow, a particular physic behavior when falling on it...
                           {
                             bool CollisionOn = true;
                             if (Velocity.y <= -1000.0)
                             {
                               UShort VoxelUnderPlayer = GameEnv->World->GetVoxel(Out[CollisionIndice].PointedVoxel.x, Out[CollisionIndice].PointedVoxel.y, Out[CollisionIndice].PointedVoxel.z);

                               if (GameEnv->VoxelTypeManager.VoxelTable[VoxelUnderPlayer]->BvProp_AccelerateOnFall)
                               {
                                 CollisionOn = false; // No collision event on snow.
                                 // printf("err...%lf\n", Velocity.y);
                                 if ( (!JumpDebounce) || Velocity.y < -3000.0) // Don't try to jump to get velocity on a flat snow surface.
                                 {
                                   double Norm = sqrt(Velocity.x*Velocity.x + Velocity.z * Velocity.z);
                                   double Vel = fabs(Velocity.y);

                                   double NewVelocityFactor = Norm + Vel / 10.0;
                                   if (NewVelocityFactor > 15000.0) NewVelocityFactor = 15000.0; // Limit speed
                                   if (Norm > 1.0)
                                   {
                                     Velocity.x = Velocity.x / Norm * NewVelocityFactor; // Get the new acceleration without changing velocity direction.
                                     Velocity.z = Velocity.z / Norm * NewVelocityFactor; // ...
                                   }
                                   if (Velocity.x > 15000.0) Velocity.x = 15000.0;
                                   if (Velocity.x <-15000.0) Velocity.x = -15000.0;
                                   if (Velocity.z > 15000.0) Velocity.z = 15000.0;
                                   if (Velocity.z <-15000.0) Velocity.z =-15000.0;
                                 }
                                 //printf("Vx %f, Vz %f\n",Velocity.x, Velocity.z);
                               }
                             }
                             // Colision and death are disabled on snow.
                             if (CollisionOn) Event_Collision(Velocity.y);
                             Velocity.y = 0.0;
                             JumpDebounce = 0;
                           }
                           break;
        case 2: Dep.z=0.0; Event_Collision(Velocity.z); Velocity.z = 0.0; IsCollided_h = true; break;
      }
      //Deplacement = 0.0;
      //return;
    }
    else
    {
      ZVector3d NewLocation;

      NewLocation = Location + Dep;


        SetPosition( NewLocation );
        Deplacement = 0.0;
        Continue = false;

    }
  }

  // Son du déplacement.

  #if COMPILEOPTION_FNX_SOUNDS_1 == 1

  bool WalkSoundOn;

  WalkSoundOn = IsWalking && !IsInLiquid && IsOnGround && !IsCollided_h && !Flag_ActivateAntiFall;
  if ( WalkSoundOn ) {if (WalkSoundHandle == nullptr) WalkSoundHandle = GameEnv->Sound->Start_PlaySound(4, true, true, 1.0, nullptr ); }
  else               {if (WalkSoundHandle != nullptr) { GameEnv->Sound->Stop_PlaySound(WalkSoundHandle); WalkSoundHandle = nullptr; }}

  #endif
}

void ZActor_Player::DoPhysic_SupermanPlayer(double CycleTime)
{
  ZVector3d Dep,Dep2;
  ZVector3d Tmp;
  ZVector3d P[32];
  Bool PEnable[32];
  bool PInvert[32];
  UShort Voxel[32];
  bool   IsEmpty[32];
  ZVoxelType * VoxelType[32];
  ULong i;
  ZVoxelWorld * World;
  double DepLen;


  // Voxel Help System

  if (PointedVoxel.Collided)
  {
    UShort VoxelType;

    LastHelpTime += CycleTime;

    VoxelType = GameEnv->World->GetVoxel(PointedVoxel.PointedVoxel.x, PointedVoxel.PointedVoxel.y, PointedVoxel.PointedVoxel.z);
    if (GameEnv->VoxelTypeManager.VoxelTable[VoxelType]->Is_HasHelpingMessage)
    {
      if (   PointedVoxel.PointedVoxel.x != LastHelpVoxel.x
          || PointedVoxel.PointedVoxel.y != LastHelpVoxel.y
          || PointedVoxel.PointedVoxel.z != LastHelpVoxel.z
          || LastHelpTime > 3005.0                         )
      {
        LastHelpTime = 0.0;
        LastHelpVoxel.x = PointedVoxel.PointedVoxel.x;
        LastHelpVoxel.y = PointedVoxel.PointedVoxel.y;
        LastHelpVoxel.z = PointedVoxel.PointedVoxel.z;
        GameEnv->GameWindow_Advertising->Advertise(GameEnv->VoxelTypeManager.VoxelTable[VoxelType]->HelpingMessage.String, ZGameWindow_Advertising::VISIBILITY_VERYHARDTOREAD, 0, 3000.0,100.0 );
      }
    }

  }


  // Colored vision off. May be reactivated further.

  Camera.ColoredVision.Activate = false;

  // Define Detection points

  P[0] = Location + ZVector3d(-75.0,+0.0,+75.0);
  P[1] = Location + ZVector3d(+75.0,+0.0,+75.0);
  P[2] = Location + ZVector3d(+75.0,+0.0,-75.0);
  P[3] = Location + ZVector3d(-75.0,+0.0,-75.0);

  P[4] = Location + ZVector3d(-75.0,+128.0,+75.0);
  P[5] = Location + ZVector3d(+75.0,+128.0,+75.0);
  P[6] = Location + ZVector3d(+75.0,+128.0,-75.0);
  P[7] = Location + ZVector3d(-75.0,+128.0,-75.0);

  P[8] = Location + ZVector3d(-75.0,+384.0,+75.0);
  P[9] = Location + ZVector3d(+75.0,+384.0,+75.0);
  P[10] = Location + ZVector3d(+75.0,+384.0,-75.0);
  P[11] = Location + ZVector3d(-75.0,+384.0,-75.0);

  P[12] = Location + ZVector3d(-75.0,+500.0,+75.0);
  P[13] = Location + ZVector3d(+75.0,+500.0,+75.0);
  P[14] = Location + ZVector3d(+75.0,+500.0,-75.0);
  P[15] = Location + ZVector3d(-75.0,+500.0,-75.0);

  P[16] = Location + ZVector3d(-70.0,-5.0,-70.0); 
  P[17] = Location + ZVector3d(+70.0,-5.0,-70.0); 
  P[18] = Location + ZVector3d(+70.0,-5.0,+70.0); 
  P[19] = Location + ZVector3d(-70.0,-5.0,+70.0); 

  P[20] = Location + ZVector3d(0,-5,0);  
  P[21] = Location + ZVector3d(0,0,0);   
  P[22] = Location + ZVector3d(0,128,0); 
  P[23] = Location + ZVector3d(0,384,0); 

  // Get the Voxel Informations

  World = PhysicsEngine->World;
  for (i=0;i<24;i++)
  {
    Voxel[i]     = World->GetVoxelPlayerCoord(P[i].x,P[i].y,P[i].z);
    VoxelType[i] = GameEnv->VoxelTypeManager.VoxelTable[Voxel[i]];
    IsEmpty[i]   = VoxelType[i]->Is_PlayerCanPassThrough;
  }

  // Detect player is on ground

  if ( IsEmpty[16] && IsEmpty[17] && IsEmpty[18] && IsEmpty[19] ) IsOnGround = false;
  else                                                            IsOnGround = true;

  // Detect if player is in liquid and compute the mean density.


  IsFootInLiquid = VoxelType[22]->Is_Liquid;
  IsHeadInLiquid = VoxelType[23]->Is_Liquid;
  IsInLiquid = IsFootInLiquid && IsHeadInLiquid;
  LocationDensity = (VoxelType[22]->LiquidDensity + VoxelType[23]->LiquidDensity) / 2.0;

  if (Voxel[23]==85) { Camera.ColoredVision.Activate = true; Camera.ColoredVision.Red= 0.0f; Camera.ColoredVision.Green = 0.0f; Camera.ColoredVision.Blue = 1.0f; Camera.ColoredVision.Opacity  = 0.5f; }
  if (Voxel[23]==86) { Camera.ColoredVision.Activate = true; Camera.ColoredVision.Red= 0.0f; Camera.ColoredVision.Green = 1.0f; Camera.ColoredVision.Blue = 0.0f; Camera.ColoredVision.Opacity  = 0.5f; }
  if (Voxel[23]==89) { Camera.ColoredVision.Activate = true; Camera.ColoredVision.Red= 1.0f; Camera.ColoredVision.Green = 1.0f; Camera.ColoredVision.Blue = 0.0f; Camera.ColoredVision.Opacity  = 0.9f; }

  // Voxel collision and player damage.


  bool harming = 0;
  //UShort HarmingVoxel;
  double MaxHarming = 0;
  double VoxelHarming;
  for(i=0;i<16;i++) if (GameEnv->VoxelTypeManager.VoxelTable[Voxel[i]]->Is_Harming)
  {
    harming |= true;
    //HarmingVoxel = Voxel[i];
    VoxelHarming = GameEnv->VoxelTypeManager.VoxelTable[Voxel[i]]->HarmingLifePointsPerSecond;
    if (VoxelHarming > MaxHarming) MaxHarming = VoxelHarming;
  }
  if ((harming))
  {
    Camera.ColoredVision.Activate = true;
    Camera.ColoredVision.Red      = 1.0f;
    Camera.ColoredVision.Green    = 0.0f;
    Camera.ColoredVision.Blue     = 0.0f;
    Camera.ColoredVision.Opacity  = 0.5f;
    // printf("LifePoints: %lf \n",LifePoints);
    LifePoints -= MaxHarming / 1000.0 * CycleTime;
    if (LifePoints < 0.0) LifePoints = 0.0;
  }

  if (LifePoints <= 0.0 && !IsDead)
  {
    this->IsDead = true;
    GameEnv->GameWindow_Advertising->Advertise("YOU ARE DEAD", 0, 0, 10000.0, 10000.0);
    DeathChronometer = 10000.0;
  }

  if (IsDead)
  {
    Camera.ColoredVision.Activate = true; Camera.ColoredVision.Red= 0.0f; Camera.ColoredVision.Green = 0.0f; Camera.ColoredVision.Blue = 0.0f; Camera.ColoredVision.Opacity  = 0.8f;
    DeathChronometer -= CycleTime;
    if (DeathChronometer <= 0.0)
    {
      Event_Death();

    }
    return;
  }

  // Vision change for the head in voxel


  if (Voxel[23] == 52)
  {
    Camera.ColoredVision.Activate = true;
    Camera.ColoredVision.Red     = 1.0f;
    Camera.ColoredVision.Green   = 0.0f;
    Camera.ColoredVision.Blue    = 0.0f;
    Camera.ColoredVision.Opacity = 0.95f;
  }

  // Physical deplacement computation


  if (1)
  {
    ZVector3d Frottement, WalkSpeed, VelocityIncrease, MaxVelocityIncrease, GripFactor;
    double FrictionCoef;

    // Limit the frametime to avoid accidental stuttering nasty effects.

    if (CycleTime > 160.0) CycleTime = 160.0; // Limit frame time

    // No gravity for superman...

    // Disabled : Velocity.y -= (PlayerDensity - LocationDensity) * Gravity * CycleTime;

    //printf("Gravity %lf y: %lf\n",Gravity, CubeY);

    // Player legs action

    GripFactor.x = 1.0;
    GripFactor.z = 1.0;
    GripFactor.y = 1.0;
    WalkSpeed = Deplacement * 50.0;

    MaxVelocityIncrease = (WalkSpeed - Velocity) * GripFactor;
    VelocityIncrease = MaxVelocityIncrease * (CycleTime / 16.0);

    if (fabs(VelocityIncrease.x) > fabs(MaxVelocityIncrease.x) ) VelocityIncrease.x = MaxVelocityIncrease.x;
    if (fabs(VelocityIncrease.y) > fabs(MaxVelocityIncrease.y) ) VelocityIncrease.y = MaxVelocityIncrease.y;
    if (fabs(VelocityIncrease.z) > fabs(MaxVelocityIncrease.z) ) VelocityIncrease.z = MaxVelocityIncrease.z;
    Velocity += VelocityIncrease;

    // The viscous friction loss...

    FrictionCoef = GameEnv->VoxelTypeManager.VoxelTable[Voxel[20]]->FrictionCoef;
    FrictionCoef+= GameEnv->VoxelTypeManager.VoxelTable[Voxel[22]]->FrictionCoef;
    FrictionCoef+= GameEnv->VoxelTypeManager.VoxelTable[Voxel[23]]->FrictionCoef;
    Frottement = (Velocity * Velocity * FrictionCoef * PlayerSurface / 1000000000.0 * CycleTime) + 1.0;
    Velocity /= Frottement;

    // VelocityBooster.

    if (Voxel[20] == 16) Velocity *= 1.0 + CycleTime / 40.0;

    // Velocity to deplacement

    Dep = Velocity * CycleTime / 1000.0;
    DepLen = sqrt(Dep.x*Dep.x + Dep.y*Dep.y + Dep.z*Dep.z);
    // printf("Velocity %lf %lf %lf Increase: %lf %lf %lf CycleTime :%lf\n",Velocity.x, Velocity.y, Velocity.z, VelocityIncrease.x, VelocityIncrease.y, VelocityIncrease.z, CycleTime );

  }



  // Disable all control points. Mark anti fall points with invert flag.

  for (i=0;i<24;i++) {PEnable[i] = false; PInvert[i] = false; }
  for (i=16;i<20;i++){PInvert[i]=true;}

  // Vector direction enable some control points.

  if (Dep.x < 0.0 ) { PEnable[4]  = true; PEnable[7] = true; PEnable[8]  = true; PEnable[11] = true; }
  if (Dep.x > 0.0 ) { PEnable[5]  = true; PEnable[6] = true; PEnable[9]  = true; PEnable[10] = true; }
  if (Dep.z > 0.0 ) { PEnable[4]  = true; PEnable[5] = true; PEnable[8]  = true; PEnable[9]  = true; }
  if (Dep.z < 0.0 ) { PEnable[6]  = true; PEnable[7] = true; PEnable[10] = true; PEnable[11] = true; }
  if (Dep.y > 0.0 ) { PEnable[12] = true; PEnable[13]= true; PEnable[14] = true; PEnable[15] = true; }
  if (Dep.y < 0.0 ) { PEnable[0]  = true; PEnable[1] = true; PEnable[2]  = true; PEnable[3]  = true; }

  PEnable[0]  = true; PEnable[1] = true; PEnable[2]  = true; PEnable[3]  = true;

  // Anti Fall test point activation

  if (this->Flag_ActivateAntiFall)
  {
    if (Dep.x >0)
    {
       if   (Dep.z>0) {PEnable[16] = true;}
       else           {PEnable[19] = true;}
    }
    else
    {
      if   (Dep.z>0) {PEnable[17] = true;}
      else           {PEnable[18] = true;}
    }

    if ( Dep.y>0.0)                                                 {PEnable[16]=PEnable[17]=PEnable[18]=PEnable[19] = false; }
    if ( IsEmpty[19] && IsEmpty[18] && IsEmpty[17] && IsEmpty[16] ) {PEnable[16]=PEnable[17]=PEnable[18]=PEnable[19] = false; }
    if ( !IsEmpty[20] )                                             {PEnable[16]=PEnable[17]=PEnable[18]=PEnable[19] = false; }

    if ( IsEmpty[19] && IsEmpty[18] && IsEmpty[17] ) PEnable[16] = true;
    if ( IsEmpty[18] && IsEmpty[17] && IsEmpty[16] ) PEnable[19] = true;
    if ( IsEmpty[17] && IsEmpty[16] && IsEmpty[19] ) PEnable[18] = true;
    if ( IsEmpty[16] && IsEmpty[19] && IsEmpty[18] ) PEnable[17] = true;
  }

  // Collision detection loop and displacement correction

  ZRayCast_in In;
  ZRayCast_out Out[32];


  In.Camera = nullptr;
  In.MaxCubeIterations = ceil(DepLen / 256)+5; // 6;
  In.PlaneCubeDiff = In.MaxCubeIterations - 3;
  In.MaxDetectionDistance = 3000000.0;
  double DistanceMin;
  Long CollisionIndice;
  Bool Collided;
  Bool Continue;

  // ****

  Continue = true;
  if ( (Dep.x == 0) && (Dep.y == 0) && (Dep.z == 0.0) ) { Continue = false; return; }

  // printf("Loc: %lf %lf %lf\n",Location.x,Location.y,Location.z);

  Location_Old = Location;
  while (Continue)
  {

    Collided = false;
    DistanceMin = 10000000.0;
    CollisionIndice = -1;
    for (i=0;i<20;i++)
    {

      if (PEnable[i]) // (PEnable[i])
      {
        bool ret;
        if (PInvert[i]) ret = World->RayCast_Vector_special(P[i],Dep , &In, &(Out[i]), PInvert[i]); // If anti fall points, use special routine and invert empty/full detection.
        else            ret = World->RayCast_Vector(P[i],Dep , &In, &(Out[i]), PInvert[i]);         // Normal points.

        if (ret)
        {
          if (Out[i].CollisionDistance < DistanceMin ) { Collided = true; DistanceMin = Out[i].CollisionDistance; CollisionIndice = i; }
        }
      }
    }
    // printf("\n");

    DepLen = sqrt(Dep.x*Dep.x + Dep.y*Dep.y + Dep.z*Dep.z);

    if (Collided && (DistanceMin < DepLen || DistanceMin <= 1.1) )
    {
      // printf("Collided(Loc:%lf %lf %lf dep: %lf %lf %lf Point : %lf %lf %lf Ind:%ld ))\n",Location.x,Location.y,Location.z, Dep.x,Dep.y,Dep.z,Out[CollisionIndice].CollisionPoint.x,Out[CollisionIndice].CollisionPoint.y, Out[CollisionIndice].CollisionPoint.z, CollisionIndice);
      // World->RayCast_Vector(P[CollisionIndice],Dep , &In, &(Out[CollisionIndice]));
      // Dep = Dep - (P[CollisionIndice] - Out[CollisionIndice].CollisionPoint);
      // SetPosition( Out[CollisionIndice].CollisionPoint );
      // Dep = 0.0;
      switch(Out[CollisionIndice].CollisionAxe)
      {
        case 0: Dep.x=0.0; Event_Collision(Velocity.x); Velocity.x = 0.0; break;
        case 1: Dep.y=0.0; Event_Collision(Velocity.y); Velocity.y = 0.0; JumpDebounce = 0;break;
        case 2: Dep.z=0.0; Event_Collision(Velocity.z); Velocity.z = 0.0; break;
      }
      //Deplacement = 0.0;
      //return;
    }
    else
    {
      ZVector3d NewLocation;

      NewLocation = Location + Dep;


        SetPosition( NewLocation );
        Deplacement = 0.0;
        Continue = false;

    }
  }

}




void ZActor_Player::Action_GoFastForward(double speed)
{
  Deplacement.x += sin(ViewDirection.yaw/180.0 * 3.14159265)*speed;
  Deplacement.z += cos(ViewDirection.yaw/180.0 * 3.14159265)*speed;
}

void ZActor_Player::Action_GoForward()
{
  switch (ActorMode)
  {
    case 0:
             Deplacement.x += sin(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
             Deplacement.z +=cos(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
             break;

    case 1:
             // ViewDirection.pitch+=0.1; if (ViewDirection.pitch >=360.0) ViewDirection.pitch-= 360.0;
             break;
    case 2:  if (IsDead) return;
             if (PlaneEngineOn)
             {
               PlaneEngineThrust += 50.0 * GameEnv->Time_GameLoop;
               if (PlaneEngineThrust > PlaneMaxThrust[Vehicle_Subtype]) PlaneEngineThrust = PlaneMaxThrust[Vehicle_Subtype];
               if (PlaneEngineThrust < PlaneMinThrust[Vehicle_Subtype]) PlaneEngineThrust = PlaneMinThrust[Vehicle_Subtype];
             }
             break;

    case 3:  Deplacement.x += sin(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
             Deplacement.z +=cos(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
             break;

  }
}

void ZActor_Player::Action_GoBackward()
{
  switch (ActorMode)
  {
    case 0:
            Deplacement.x -= sin(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
            Deplacement.z -= cos(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
            break;

    case 1:
            // ViewDirection.pitch-=0.1; if (ViewDirection.pitch <0.0) ViewDirection.pitch+= 360.0;
            break;
    case 2:  if (IsDead) return;
             if (PlaneEngineOn)
             {
               PlaneEngineThrust -= 50.0 * GameEnv->Time_GameLoop;
               if (PlaneEngineThrust < PlaneMinThrust[Vehicle_Subtype]) PlaneEngineThrust = PlaneMinThrust[Vehicle_Subtype];
             }
             break;
    case 3:
            Deplacement.x -= sin(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
            Deplacement.z -= cos(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
            break;
  }
}

void ZActor_Player::Action_GoLeftStraff()
{
  switch (ActorMode)
  {
    case 0:
            Deplacement.x -= cos(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
            Deplacement.z += sin(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
            break;
    case 1:
            // ViewDirection.yaw-=0.1 ; if (ViewDirection.yaw <0.0) ViewDirection.yaw+= 360.0;
            break;
    case 2: if (IsDead) return;
            if (PlaneEngineOn)
            {
              PlaneEngineOn = false;
              PlaneEngineThrust = 0.0;
              if ((PlaneReactorSoundHandle)) { GameEnv->Sound->Stop_PlaySound(PlaneReactorSoundHandle); PlaneReactorSoundHandle = nullptr; }
              GameEnv->GameWindow_Advertising->Advertise("ENGINE OFF",ZGameWindow_Advertising::VISIBILITY_MEDLOW,0,1000.0, 500.0 );
            }
            break;
    case 3:
            Deplacement.x -= cos(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
            Deplacement.z += sin(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
            break;
    case 6: this->Train_DirPrefGoRight = false; break;
  }


}

void ZActor_Player::Action_GoRightStraff()
{
  switch (ActorMode)
  {
     case 0:
             Deplacement.x +=cos(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
             Deplacement.z -=sin(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
             break;
     case 1:
             // ViewDirection.yaw+=0.1; if (ViewDirection.yaw >360.0) ViewDirection.yaw-= 360.0;
             break;
     case 2: if (IsDead) return;
             if (!PlaneEngineOn && ((!PlaneToohighAlt) || Location.y < 250.0 * 256.0) )
             {
               PlaneEngineOn = true;
               PlaneEngineThrust = 0.0;
               if (PlaneToohighAlt) { PlaneToohighAlt = false; PlaneEngineThrust = PlaneMaxThrust[Vehicle_Subtype]; PlaneSpeed = 30000.0; }
               GameEnv->GameWindow_Advertising->Advertise("ENGINE ON",ZGameWindow_Advertising::VISIBILITY_MEDLOW,0,1000.0, 500.0 );
               PlaneToohighAlt = false;
               PlaneWaitForRectorStartSound = false;
               if ((PlaneReactorSoundHandle)) {GameEnv->Sound->Stop_PlaySound(PlaneReactorSoundHandle);PlaneReactorSoundHandle = nullptr;}
               PlaneReactorSoundHandle = GameEnv->Sound->Start_PlaySound(2,false, false, 1.0,&PlaneWaitForRectorStartSound);

             }
             break;
     case 3:
             Deplacement.x +=cos(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
             Deplacement.z -=sin(ViewDirection.yaw/180.0 * 3.14159265)*Speed_Walk;
             break;
     case 6: this->Train_DirPrefGoRight = true; break;

  }
}

void ZActor_Player::Action_GoUp()
{
  switch (ActorMode)
  {
    case 3: Deplacement.y = 1.0 * Speed_Walk;
            break;
  }
}

void ZActor_Player::Action_GoDown()
{
  switch (ActorMode)
  {
    case 3: Deplacement.y = -1.0 * Speed_Walk;
            break;
  }
}

void ZActor_Player::Action_Jump()
{
  switch (ActorMode)
  {
    case 0:
       // GameEnv->Time_GameLoop * 4.0  * 1.5
      if (JumpDebounce==0)
      {
        Velocity.y += 2000.0; JumpDebounce = 64;
        if (IsFootInLiquid) Velocity.y += 500.0;
      }
      break;
  }
}



void ZActor_Player::Start_Riding(Long x, Long y, Long z)
{
  ZVoxelLocation Loc;

  if(Riding_IsRiding) this->Stop_Riding();

  if ( (!Riding_IsRiding) && GameEnv->World->GetVoxelLocation(&Loc, x,y,z))
  {
    if (COMPILEOPTION_ALLOWVEHICLE == 0)
    {
      GameEnv->GameWindow_Advertising->Clear();
      GameEnv->GameWindow_Advertising->Advertise("NOT AVAILLABLE IN DEMO VERSION", ZGameWindow_Advertising::VISIBILITY_MEDIUM, 0, 5000,1500);
      return;
    }
    Riding_Voxel = Loc.Sector->Data[Loc.Offset];
    Riding_VoxelInfo = Loc.Sector->OtherInfos[Loc.Offset];

    if (GameEnv->VoxelTypeManager.VoxelTable[Riding_Voxel]->Is_Rideable && COMPILEOPTION_ALLOWVEHICLE == 1 )
    {
      Loc.Sector->Data[Loc.Offset] = 0;
      Loc.Sector->OtherInfos[Loc.Offset]=0;
      GameEnv->World->SetVoxel_WithCullingUpdate(x,y,z,0,ZVoxelSector::CHANGE_CRITICAL,true,nullptr);
      Riding_IsRiding = true;

      ZVector3d NewPlayerLocation;
      switch(Riding_Voxel)
      {
                 // Airplane Z0 and Z1
        case 239:
        case 96: ActorMode = 2;
                 ViewDirection.pitch = 0.0;
                 ViewDirection.roll  = 0.0;
                 ViewDirection.yaw -= 45.0;                                           if (ViewDirection.yaw < 0.0) ViewDirection.yaw += 360.0;
                 ViewDirection.yaw = (floor(ViewDirection.yaw / 90.0) + 1.0) * 90.0;  if (ViewDirection.yaw >= 360.0) ViewDirection.yaw -= 360.0;


                 GameEnv->World->Convert_Coords_VoxelToPlayer(x,y,z,NewPlayerLocation.x,NewPlayerLocation.y, NewPlayerLocation.z);
                 NewPlayerLocation.x += 128.0; NewPlayerLocation.z += 128.0;
                 Location = NewPlayerLocation;

                 break;

                 // Car H0,H1,H2

        case 263:
        case 264:
        case 265:
        case 266:
        case 267:
                 ActorMode = 5;
                 ViewDirection.pitch = 0.0;
                 ViewDirection.roll  = 0.0;
                 ViewDirection.yaw -= 45.0;                                           if (ViewDirection.yaw < 0.0) ViewDirection.yaw += 360.0;
                 ViewDirection.yaw = (floor(ViewDirection.yaw / 90.0) + 1.0) * 90.0;  if (ViewDirection.yaw >= 360.0) ViewDirection.yaw -= 360.0;

                 CarThrust = 0.0;
                 SteerAngle = 0.0;

                 GameEnv->World->Convert_Coords_VoxelToPlayer(x,y,z,NewPlayerLocation.x,NewPlayerLocation.y, NewPlayerLocation.z);
                 NewPlayerLocation.x += 128.0; NewPlayerLocation.z += 128.0;
                 Location = NewPlayerLocation;
                 break;
        case 269:
        case 270:
        case 271:
                 {
                   ZVector3L VoxelLoc(x,y,z);
                   bool Abort = false;
                   ULong MaxVoxels;

                   switch(Riding_Voxel)
                   {
                     case 269: MaxVoxels = 5;  break;
                     case 270: MaxVoxels = 17; break;
                     case 271:
                     default:  MaxVoxels = ZACTORPLAYER_TRAINMAXLEN-1; break;
                   }

                   Train_StoredVoxelCount = StoreTrainElements(&VoxelLoc, Train_VoxelTable, Train_Direction);
                   if (Train_StoredVoxelCount<1)
                   {
                     Abort = true;
                     GameEnv->GameWindow_Advertising->Clear();
                     GameEnv->GameWindow_Advertising->Advertise("MALFORMED TRAIN", ZGameWindow_Advertising::VISIBILITY_HIGH, 1, 2000.0, 0.0);
                   }
                   if (Train_StoredVoxelCount>MaxVoxels)
                   {
                     GameEnv->GameWindow_Advertising->Clear();
                     GameEnv->GameWindow_Advertising->Advertise("TRAIN TOO LONG", ZGameWindow_Advertising::VISIBILITY_HIGH, 1, 2000.0, 0.0);
                     Abort = true;
                     UnstoreTrainElements(this->Train_StoredVoxelCount, &VoxelLoc, Train_VoxelTable, Train_Direction);
                   }

                   if (Abort)
                   {
                     if (GameEnv->World->SetVoxel_WithCullingUpdate(x, y, z, Riding_Voxel, ZVoxelSector::CHANGE_CRITICAL, false, &Loc))
                     { Loc.Sector->OtherInfos[Loc.Offset] = Riding_VoxelInfo; Riding_Voxel = 0; Riding_VoxelInfo = 0; Riding_IsRiding = false; ActorMode = 0; }
                     return;
                   }

                   // Compute the train weight and count functionnal blocs.

                   TrainComputeWeightAndFunctions(Train_VoxelTable);

                   // Engage train actormode and initialize view, trust and speed.

                   ActorMode = 6;
                   TrainSpeed = 0.0;
                   TrainThrust = 0.0;
                   ViewDirection.pitch = ZDirs::Dir_Vector[ZDirs::Opposite_Dir[Train_Direction]].pitch;
                   ViewDirection.roll  = ZDirs::Dir_Vector[ZDirs::Opposite_Dir[Train_Direction]].roll;
                   ViewDirection.yaw   = ZDirs::Dir_Vector[ZDirs::Opposite_Dir[Train_Direction]].yaw;

                   // Set player location and view to the train

                   GameEnv->World->Convert_Coords_VoxelToPlayer(x,y,z,NewPlayerLocation.x,NewPlayerLocation.y, NewPlayerLocation.z);
                   NewPlayerLocation.x += 128.0; NewPlayerLocation.z += 128.0;
                   Location = NewPlayerLocation;

                   Sync_Camera(128.0);
                 }
                 break;

        case 274:
        case 275:
        case 276:
                 {
                   Long RailDirection;
                   // Engage the lift actormode and initialize view, and speed.

                   ActorMode = 7;
                   Lift_Thrust = 0.0;
                   ViewDirection.pitch = ZDirs::Dir_Vector[ZDirs::Opposite_Dir[Train_Direction]].pitch;
                   ViewDirection.roll  = ZDirs::Dir_Vector[ZDirs::Opposite_Dir[Train_Direction]].roll;
                   ViewDirection.yaw   = ZDirs::Dir_Vector[ZDirs::Opposite_Dir[Train_Direction]].yaw;

                   // Set player location and view to the train

                   GameEnv->World->Convert_Coords_VoxelToPlayer(x,y,z,NewPlayerLocation.x,NewPlayerLocation.y, NewPlayerLocation.z);
                   NewPlayerLocation.x += 128.0; NewPlayerLocation.y += 128.0; NewPlayerLocation.z += 128.0;
                   Location = NewPlayerLocation;

                   if (!LiftFindRailDirection(NewPlayerLocation,RailDirection))
                   {
                     this->Stop_Riding(true);
                   }

                   this->SetViewToCubeDirection_Progressive(RailDirection,1000.0);

                   Sync_Camera(128.0);
                 }
                 break;

        case 284:
                 {
                   ActorMode = 8;
                   Spinner_Angle = 0;
                   Spinner_Distance = 1000.0;
                   Spinner_Height = 512.0;
                   Spinner_Speed = 10.0;
                   Spinner_Origin = 0.0;
                   Spinner_VomitMode = false;

                   // Set player location to the spinner

                   GameEnv->World->Convert_Coords_VoxelToPlayer(x,y,z,NewPlayerLocation.x,NewPlayerLocation.y, NewPlayerLocation.z);

                   // Set player location to the center of the cube

                   NewPlayerLocation.x += 128.0; NewPlayerLocation.y += 128.0; NewPlayerLocation.z += 128.0;

                   // Update player coordinates
                   Location = NewPlayerLocation;
                 }
                 break;
      }

      // Subtype

      switch(Riding_Voxel)
      {
        case 239: Vehicle_Subtype = 1; break;
        case 96:  Vehicle_Subtype = 0; break;

        case 263: Vehicle_Subtype = 0; break;
        case 264: Vehicle_Subtype = 1; break;
        case 265: Vehicle_Subtype = 2; break;
        case 266: Vehicle_Subtype = 3; break;
        case 267: Vehicle_Subtype = 4; break;

        case 269: Vehicle_Subtype = 0; break;
        case 270: Vehicle_Subtype = 1; break;
        case 271: Vehicle_Subtype = 2; break;

        case 274: Vehicle_Subtype = 0; break;
        case 275: Vehicle_Subtype = 1; break;
        case 276: Vehicle_Subtype = 2; break;
      }

    }
  }
}

void ZActor_Player::Stop_Riding(Bool RegiveVoxel)
{
  ZVector3L VLoc;
  ZVoxelLocation Loc;

  if (Riding_IsRiding)
  {


    if ((LiftSoundHandle))        { GameEnv->Sound->Stop_PlaySound(LiftSoundHandle); LiftSoundHandle = nullptr; }
    if ((CarEngineSoundHandle))   { GameEnv->Sound->Stop_PlaySound(CarEngineSoundHandle); CarEngineSoundHandle = nullptr;    }
    if ((TrainEngineSoundHandle)) { GameEnv->Sound->Stop_PlaySound(TrainEngineSoundHandle); TrainEngineSoundHandle = nullptr;  }
    if ((PlaneReactorSoundHandle)){ GameEnv->Sound->Stop_PlaySound(PlaneReactorSoundHandle); PlaneReactorSoundHandle = nullptr; }

    GameEnv->World->Convert_Coords_PlayerToVoxel(Location.x, Location.y, Location.z, VLoc.x, VLoc.y, VLoc.z);


    switch(Riding_Voxel)
    {
      case 239: break;
      case 96:  break;

      case 263: break;
      case 264: break;
      case 265: break;
      case 266: break;
      case 267: break;

      case 269:
      case 270:
      case 271: UnstoreTrainElements(Train_StoredVoxelCount,&VLoc,Train_VoxelTable,Train_Direction);
                Train_StoredVoxelCount = 0;
                break;

    }


    if (GameEnv->World->SetVoxel_WithCullingUpdate(VLoc.x, VLoc.y, VLoc.z, Riding_Voxel, ZVoxelSector::CHANGE_CRITICAL, false, &Loc))
    {


      Loc.Sector->OtherInfos[Loc.Offset] = Riding_VoxelInfo;
      Riding_Voxel = 0;
      Riding_VoxelInfo = 0;
      Riding_IsRiding = false;

      Location.y += 256.0;
      ActorMode = 0;
    }
    if (!RegiveVoxel) GameEnv->World->SetVoxel_WithCullingUpdate(VLoc.x, VLoc.y, VLoc.z, 0, ZVoxelSector::CHANGE_CRITICAL, false, &Loc);
  }
}

void ZActor_Player::Event_Death()
{
  GameEnv->GameWindow_Advertising->Advertise("RESPAWNED TO POINT ZERO", ZGameWindow_Advertising::VISIBILITY_HIGH, 0, 3500.0, 0.0);
  Init(true);
}


void ZActor_Player::Event_PlaneCrash()
{
  GameEnv->GameWindow_Advertising->Clear();
  GameEnv->GameWindow_Advertising->Advertise("CRASH", ZGameWindow_Advertising::VISIBILITY_MEDLOW, 0, 1000.0, 0.0);
  if ((PlaneReactorSoundHandle)) { GameEnv->Sound->Stop_PlaySound(PlaneReactorSoundHandle); PlaneReactorSoundHandle = nullptr; }
  GameEnv->Sound->PlaySound(1);
  IsDead = true;
  DeathChronometer = 5000.0;
  PlaneEngineThrust = 0.0;
}

void ZActor_Player::Event_DeadlyFall()
{
  GameEnv->GameWindow_Advertising->Clear();
  GameEnv->GameWindow_Advertising->Advertise("FATAL FALL : YOU ARE DEAD", ZGameWindow_Advertising::VISIBILITY_HIGH, 0, 5000.0, 0.0);
  GameEnv->Sound->PlaySound(1);
  IsDead = true;
  DeathChronometer = 5000.0;
  Location.y -= 256.0;
}

void ZActor_Player::Process_Powers()
{
  ULong SlotNum;
  ZVoxelType * PreviousVoxelType, * ActualVoxelType;
  ZInventory::Entry * InventoryEntry;

  // Detect if powers have changed in order to start and stop these like services.
  // At game start or resume, powers receive start events.

  for (SlotNum=ZInventory::Powers_StartSlot ; SlotNum<=ZInventory::Powers_EndSlot ; SlotNum++)
  {
    InventoryEntry = Inventory->GetSlotRef(SlotNum);
    ActualVoxelType = GameEnv->VoxelTypeManager.GetVoxelType(InventoryEntry->VoxelType);

    if (InventoryEntry->VoxelType != PreviousVoxelTypes[SlotNum])
    {
      PreviousVoxelType = GameEnv->VoxelTypeManager.GetVoxelType(PreviousVoxelTypes[SlotNum]);

      if (PreviousVoxelType->Is_Power) PreviousVoxelType->Power_End();
      if (ActualVoxelType->Is_Power)   ActualVoxelType->Power_Start();
    }

    if (ActualVoxelType->Is_Power) ActualVoxelType->Power_DoWork();

    PreviousVoxelTypes[SlotNum] = InventoryEntry->VoxelType;
  }

}

void ZActor_Player::Sync_Camera(double VerticalOffset)
{
  Camera.x = Location.x + 0.0; Camera.y = Location.y + VerticalOffset; Camera.z = Location.z;
  Camera.Pitch = ViewDirection.pitch; Camera.Roll  = ViewDirection.roll; Camera.Yaw   = ViewDirection.yaw;
}

void ZActor_Player::SetViewToCubeDirection(Long Direction)
{
  ViewDirection.pitch = ZDirs::Dir_Vector[ZDirs::Opposite_Dir[Direction]].pitch;
  ViewDirection.roll  = ZDirs::Dir_Vector[ZDirs::Opposite_Dir[Direction]].roll;
  ViewDirection.yaw   = ZDirs::Dir_Vector[ZDirs::Opposite_Dir[Direction]].yaw;
}

void ZActor_Player::SetViewToCubeDirection_Progressive(Long Direction, double MaxTurn)
{
  double diff, NewDirection, ActualDirection;

  ActualDirection = ViewDirection.yaw;
  NewDirection = ZDirs::Dir_Vector[ZDirs::Opposite_Dir[Direction]].yaw;
  diff = NewDirection - ActualDirection;

  if (diff > 180) ActualDirection+=360.0;
  if (diff <-180) NewDirection+=360.0;
  diff = NewDirection - ActualDirection;

  if (diff>MaxTurn) diff = MaxTurn;
  if (diff<-MaxTurn) diff = -MaxTurn;

  ViewDirection.yaw += diff;

  if (ViewDirection.yaw < 0.0) ViewDirection.yaw+=360.0;
  if (ViewDirection.yaw > 360.0) ViewDirection.yaw-=360.0;
}


void ZActor_Player::TrainComputeWeightAndFunctions(ZVoxel * VoxelTable)
{
  ULong i;
  UShort VoxelType;

  Train_Weight = 0.0;
  Train_Elements_Engines = 0;

  for (i=0;i<this->Train_StoredVoxelCount;i++)
  {
    VoxelType = Train_VoxelTable[i].VoxelType;
    Train_Weight += GameEnv->VoxelTypeManager.VoxelTable[VoxelType]->GetWeightIncludingContent(Train_VoxelTable[i].OtherInfos);

    switch(VoxelType)
    {
      case 273: Train_Elements_Engines++; break;
      default: break;
    }
  }
}

Long ZActor_Player::StoreTrainElements(ZVector3L * HeadLoc, ZVoxel * VoxelTable, Long & TrainDirection)
{
  ZVector3L NewLocation;
  Long Dir;
  UShort VoxelType;

  // Check if there is track behind the train head as it should be

  NewLocation = *HeadLoc + ZVector3L(0,-1,0);
  VoxelType = GameEnv->World->GetVoxel(&NewLocation);
  if (VoxelType != 272) return(0);

  // Now check in the 4 directions for the remains of the train

  for (Dir=0; Dir<4; Dir++)
  {
    // Compute points coordinates
    NewLocation = *HeadLoc + ZDirs::Std_Dir[Dir];

    // Check if there is some tracks behind

    NewLocation.y--;
    VoxelType = GameEnv->World->GetVoxel(&NewLocation);
    if (VoxelType != 272) continue; // No tracks ? If so, try another direction.

    // Check if there is a block on the track.

    NewLocation.y++;
    VoxelType = GameEnv->World->GetVoxel(&NewLocation);
    if (VoxelType == 0) continue; // No block on the track ? Not the right direction, just try another one.

    // Ok, now we know the direction, let's pick train wagons until the end.

    TrainDirection = Dir;
    return(_GetTrainElement(Dir, 0, &NewLocation, VoxelTable));

  }

  return(0); // Train is not well formed
}

// Search and store train "wagons" until the end is found.
// return -1 if something wrong. And the number of elements.

Long ZActor_Player::_GetTrainElement(Long ActualDirection, ULong ElementCount, ZVector3L * ElementCoords, ZVoxel * VoxelTable)
{
  int Dir;
  ZVector3L NewLocation;
  UShort VoxelType;
  Long RetElements;

  if (ElementCount > ZACTORPLAYER_TRAINMAXLEN) return(0);

  // Pick the element from world and put it in the table.

  if (!GameEnv->World->ExtractToVoxel(ElementCoords,&VoxelTable[ElementCount], ZVoxelSector::CHANGE_CRITICAL)) return(0);

  VoxelType = VoxelTable[ElementCount].VoxelType;

  // If this is the tail of the train, terminate.

  if (VoxelType == 269 || VoxelType == 270 || VoxelType == 271) return(ElementCount+1);

  // Now look arround for other wagons of the train

  for (Dir=0; Dir<4; Dir++)
  {
    // Don't try the direction we are comming from

    if (Dir == ZDirs::Opposite_Dir[ActualDirection]) continue;

    // Compute coordinates of the next try

    NewLocation = *ElementCoords + ZDirs::Std_Dir[Dir];

    // Now look if there is tracks

    NewLocation.y --; // Look under the block
    VoxelType = GameEnv->World->GetVoxel(&NewLocation);
    if (VoxelType != 272) continue;

    // Check if there is a block on the track.

    NewLocation.y++;
    VoxelType = GameEnv->World->GetVoxel(&NewLocation);
    if (VoxelType == 0) continue; // No block on the track ? Not the right direction, just try another one.

    // Block is ok, go to get it and continue.

    RetElements = _GetTrainElement(Dir, ElementCount+1, &NewLocation, VoxelTable);

    // If all is ok, return

    if (RetElements != 0) return(RetElements);

  }

  // Something goes wrong, put back the voxel
  GameEnv->World->PlaceFromVoxel(ElementCoords, &VoxelTable[ElementCount], ZVoxelSector::CHANGE_CRITICAL );
  return(0);
}

Long ZActor_Player::UnstoreTrainElements(ULong StoredVoxels, ZVector3L * HeadLoc, ZVoxel * VoxelTable, Long TrainDirection)
{
  ZVector3L NewLocation;

  NewLocation = *HeadLoc + ZDirs::Std_Dir[TrainDirection];

  return(_PutTrainElement(0, StoredVoxels, &NewLocation, VoxelTable, TrainDirection));

}

bool ZActor_Player::_PutTrainElement(ULong Index, ULong StoredVoxels, ZVector3L * Location, ZVoxel * VoxelTable, Long TrainDirection)
{
  UShort VoxelType;
  ZVector3L NewLocation;

  if (StoredVoxels==0) return(true);

  // Check if the space is clear to unload voxel. If not, throw the voxel away.

  VoxelType = GameEnv->World->GetVoxel(Location);
  if (VoxelType!=0) VoxelTable[Index].Clear();

  // Put back voxel

  GameEnv->World->PlaceFromVoxel(Location, &VoxelTable[Index], ZVoxelSector::CHANGE_CRITICAL);

  // Search direction for next foxel

  for (int Dir=0; Dir<4; Dir++)
  {
    // Don't try the direction we are comming from
    if (Dir==ZDirs::Opposite_Dir[TrainDirection]) continue;

    // Compute the new direction
    NewLocation = * Location + ZDirs::Std_Dir[Dir];

    // Look for track under

    NewLocation.y --;
    VoxelType = GameEnv->World->GetVoxel(&NewLocation);
    if (VoxelType!=272) continue;

    // Ok, just call next step

    NewLocation.y++;
    return(_PutTrainElement(Index+1, StoredVoxels-1, &NewLocation, VoxelTable, Dir));
  }

  // Track have a problem, but unstore voxels anyway to avoid any loss.
  NewLocation = * Location + ZDirs::Std_Dir[TrainDirection];
  return(_PutTrainElement(Index+1, StoredVoxels-1, &NewLocation, VoxelTable, TrainDirection));
}

bool ZActor_Player::TrainFollowTrack(ZVector3d & ActualLocation, double LinearAdvance, Long &Direction, ZVector3d & NewLocation)
{

  Long i;
  double LinPos, StopPoint,FracAdvance, FirstAdvance;
  int Factor;

  // Compute first move to the next point

  switch(Direction)
  {
    case 0: LinPos = ActualLocation.z; Factor = LinPos / 256.0; StopPoint = Factor * 256.0 + 128.0; FirstAdvance = fabs(StopPoint - LinPos); break;
    case 1: LinPos = ActualLocation.x; Factor = LinPos / 256.0; StopPoint = Factor * 256.0 + 128.0; FirstAdvance = fabs(StopPoint - LinPos); break;
    case 2: LinPos = ActualLocation.z; Factor = LinPos / 256.0; StopPoint = Factor * 256.0 - 128.0; FirstAdvance = fabs(StopPoint - LinPos); break;
    case 3: LinPos = ActualLocation.x; Factor = LinPos / 256.0; StopPoint = Factor * 256.0 - 128.0; FirstAdvance = fabs(StopPoint - LinPos); break;
    default: break;
  }

  ZVector3d NewLoc, OldLoc, TestLoc;
  ZVector3L VoxLoc;
  UShort VoxelType;
  bool Clear, Found;
  Long T1,T2,T3;

  NewLoc = ActualLocation;

  for(bool First=true;LinearAdvance>0.0;First=false)
  {
    if (First)  FracAdvance = (LinearAdvance > FirstAdvance) ? FirstAdvance : LinearAdvance;
    else        FracAdvance = (LinearAdvance > 256.0) ? 256.0 : LinearAdvance;

    LinearAdvance -= FracAdvance;

    NewLoc = ActualLocation - ZDirs::Std_DirD[Direction] * FracAdvance;



      // Test if direction is clear.
      TestLoc = NewLoc - ZDirs::Std_DirD[Direction] * 129.0;

      Clear = true;

      // Does we have track ?

      GameEnv->World->Convert_Coords_PlayerToVoxel(&TestLoc, &VoxLoc);
      VoxLoc.y--;
      VoxelType = GameEnv->World->GetVoxel_Secure(&VoxLoc);
      if (VoxelType!=272) Clear = false;

      // Is there some sufficient space to let the train to passe through ?
      for (i=0;i<3;i++)
      {
        VoxLoc.y++;
        VoxelType = GameEnv->World->GetVoxel_Secure(&VoxLoc);
        if (VoxelType!=0) Clear = false;
      }

      if(!Clear && false)
      {
        Clear = true;
        GameEnv->World->SetVoxel_WithCullingUpdate(VoxLoc.x, VoxLoc.y,VoxLoc.z, 272, ZVoxelSector::CHANGE_CRITICAL,false,nullptr);
        GameEnv->World->SetVoxel_WithCullingUpdate(VoxLoc.x, VoxLoc.y+1,VoxLoc.z, 0, ZVoxelSector::CHANGE_CRITICAL,false,nullptr);
        GameEnv->World->SetVoxel_WithCullingUpdate(VoxLoc.x, VoxLoc.y+2,VoxLoc.z, 0, ZVoxelSector::CHANGE_CRITICAL,false,nullptr);
        GameEnv->World->SetVoxel_WithCullingUpdate(VoxLoc.x, VoxLoc.y+3,VoxLoc.z, 0, ZVoxelSector::CHANGE_CRITICAL,false,nullptr);
      }


      if (!Clear)
      {
        Found = false;
        if (Train_DirPrefGoRight) {T1= 1;T2=-3;T3=-2;}
        else                      {T1=-1;T2=3;T3=2;}
        for (Long NewDir=T1; NewDir!=T2;NewDir+=T3)
        {
          //if (NewDir == ZDirs::Opposite_Dir[Direction]) continue;

          TestLoc = NewLoc - ZDirs::Std_DirD[ (Direction+NewDir) & 3] * 256.0;

          Clear = true;

          GameEnv->World->Convert_Coords_PlayerToVoxel(&TestLoc, &VoxLoc);
          VoxLoc.y--;
          VoxelType = GameEnv->World->GetVoxel_Secure(&VoxLoc);
          if (VoxelType!=272) Clear=false;

          for (i=0;i<3;i++)
          {
            VoxLoc.y++;
            VoxelType = GameEnv->World->GetVoxel_Secure(&VoxLoc);
            if (VoxelType!=0) Clear = false;
          }


          // The new direction is found
          if (Clear) {Direction = (Direction+NewDir) & 3; Found = true; break;}
        }

        // No suitable direction was found
        if (!Found) {LinearAdvance = false; NewLocation = ActualLocation; return(false);}
      }

  }

  NewLocation = NewLoc;

  return(true);
}

bool ZActor_Player::LiftFindRailDirection(ZVector3d & ActualLocation, Long & ViewDirectionOut)
{
  ZVector3L VoxLoc, RailLocation;
  UShort VoxelType;
  Long i;


 GameEnv->World->Convert_Coords_PlayerToVoxel(&ActualLocation, &VoxLoc);
  for (i=0;i<4;i++)
  {
    RailLocation = VoxLoc + ZDirs::Std_Dir[i];
    VoxelType = GameEnv->World->GetVoxel_Secure(&RailLocation);
    if (VoxelType==277) {ViewDirectionOut = i; return(true);}
  }
  return(false);
}

bool ZActor_Player::LiftFollowGuide(ZVector3d & ActualLocation, double LinearAdvance, Long &Direction, ZVector3d & NewLocation)
{

  Long i;
  double LinPos, StopPoint,FracAdvance, FirstAdvance;
  int Factor;

  // Compute first move to the next point

  switch(Direction)
  {
    case 4: LinPos = ActualLocation.y; Factor = LinPos / 256.0; StopPoint = Factor * 256.0 - 128.0; FirstAdvance = fabs(StopPoint - LinPos); break;
    case 5: LinPos = ActualLocation.y; Factor = LinPos / 256.0; StopPoint = Factor * 256.0 + 128.0; FirstAdvance = fabs(StopPoint - LinPos); break;

    default: return(false); // Other directions not implemented.
  }

  ZVector3d NewLoc, OldLoc, TestLoc;
  ZVector3L VoxLoc;
  UShort VoxelType;
  bool Clear;

  NewLocation = ActualLocation;
  NewLoc = ActualLocation;

  for(bool First=true;LinearAdvance>0.0;First=false)
  {
    if (First)  FracAdvance = (LinearAdvance > FirstAdvance) ? FirstAdvance : LinearAdvance;
    else        FracAdvance = (LinearAdvance > 256.0) ? 256.0 : LinearAdvance;

    LinearAdvance -= FracAdvance;

    NewLoc = ActualLocation - ZDirs::Std_DirD[Direction] * FracAdvance;

    // Test if we are clear.

    GameEnv->World->Convert_Coords_PlayerToVoxel(&NewLoc, &VoxLoc);
    for (i=0,Clear=false;i<4;i++)
    {
      ZVector3L RailLocation;
      RailLocation = VoxLoc + ZDirs::Std_Dir[i];
      VoxelType = GameEnv->World->GetVoxel_Secure(&RailLocation);
      if (VoxelType==277) {Clear = true; break;}
    }
    VoxelType = GameEnv->World->GetVoxel_Secure(&VoxLoc);
    if (VoxelType!=0) Clear = false;

    if (!Clear) return(false);
    NewLocation = NewLoc; // Location change is validated

    // Test if direction is clear.
    TestLoc = NewLoc - ZDirs::Std_DirD[Direction] * 129.0;

    // Lift track detection
    Clear = false;
    GameEnv->World->Convert_Coords_PlayerToVoxel(&TestLoc, &VoxLoc);
    for (i=0;i<4;i++)
    {
      ZVector3L RailLocation;
      RailLocation = VoxLoc + ZDirs::Std_Dir[i];
      VoxelType = GameEnv->World->GetVoxel_Secure(&RailLocation);
      if (VoxelType==277) {Clear = true; break;}
    }

    // Air space detection
    VoxelType = GameEnv->World->GetVoxel_Secure(&VoxLoc);
    if (VoxelType!=0) Clear = false;

    if (!Clear) return(false);
  }


  return(true);
}
