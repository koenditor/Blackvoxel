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
 * ZActor_Player.h
 *
 *  Created on: 19 janv. 2011
 *      Author: laurent
 */

#pragma once

#include "ZActorPhysics.h"
#include "ZVoxel.h"
#include "SDL2/SDL_stdinc.h"
#include "ZWorld.h"
#include "z/ZString.h"
#include "z/ZType_ZVector3L.h"
#include "z/ZType_ZVector3d.h"
#include "z/ZTypes.h"

class ZStream_SpecialRamStream;

#define ZACTORPLAYER_TRAINMAXLEN 34

class ZActor_Player : public ZActor
{
    ULong     ActorMode;       // s

    bool      IsInLiquid;      // s
    bool      IsFootInLiquid;  // s
    bool      IsHeadInLiquid;  // s
    bool      IsWalking;
    double    LocationDensity; // s Density of the voxels where the player is.
    double    Speed_Walk;

    void *    WalkSoundHandle;


    double    PlayerDensity;
    ZVector3d PlayerSurface;

    // Airplane

    double    PlaneMinThrust[4];
    double    PlaneMaxThrust[4];
    double    PlaneSpeed;                  // s
    double    PlaneCommandResponsiveness;  // s
    double    PlaneEngineThrust;           // s
    bool      PlaneEngineOn;               // s
    bool      PlaneTakenOff;               // s
    double    PlaneLandedCounter;          // s
    bool      PlaneToohighAlt;             // s
    double    PlaneFreeFallCounter;        // s
    ZString   PlaneFreeFallMessage;
    bool      PlaneWaitForRectorStartSound;
    void *    PlaneReactorSoundHandle;

    // Car

    double    SteerAngle;
    double    CarThrust;
    void *    CarEngineSoundHandle;

    // Train

    Long      Train_Direction;
    double    TrainThrust;
    double    TrainSpeed;
    bool      Train_DirPrefGoRight;

    ULong     Train_StoredVoxelCount;
    ZVoxel    Train_VoxelTable[ZACTORPLAYER_TRAINMAXLEN];

    double    Train_Weight;
    ULong     Train_Elements_Engines;

    void    * TrainEngineSoundHandle;

    // Lift

    double    Lift_Thrust;
    Long      Lift_Direction;
    void    * LiftSoundHandle;

    // Spinner

    ZVector3d Spinner_Origin;
    double    Spinner_Angle;
    double    Spinner_Distance;
    double    Spinner_Height;
    double    Spinner_Speed;
    bool      Spinner_VomitMode;

    // Misc

    ZVector3L LastHelpVoxel;
    double    LastHelpTime;
    ULong     Test_T1;

    // Riding Voxel
    UShort     Riding_Voxel;
    UShort     Vehicle_Subtype; // Plane type Z0,Z1
    ZMemSize   Riding_VoxelInfo;
    bool       Riding_IsRiding;

    // Backup for inventory in order to detect changes for powers.

    UShort     * PreviousVoxelTypes;

  public:
    ZRayCast_out PointedVoxel;
    ZRayCast_out PointedVoxel_Previous;

    virtual void Init(bool Death=false) override;
    virtual void SetInitialInventory(bool Death);
    ZActor_Player();

    virtual ~ZActor_Player();
    virtual void SetPosition( ZVector3d &NewLocation ) override;
    virtual void Sync_Camera(double VerticalOffset);
    virtual void SetViewToCubeDirection(Long Direction); // Use Sync_Camera() After.
    virtual void SetViewToCubeDirection_Progressive(Long Direction, double MaxTurn); // Use Sync_Camera() After.

    virtual void Action_MouseMove( Long Delta_x, Long Delta_y) override;
    virtual void Action_MouseButtonClick( ULong Button) override;
    virtual void Action_MouseButtonRelease( ULong Button) override;
    virtual bool Action_StillEvents( bool * MouseMatrix, const Uint8 * KeyboardMatrix) override;
    virtual void Action_SetActorMode(ULong ActorMode) override {this->ActorMode = ActorMode;}


    virtual void Event_Collision(double RelativeVelocity ) override;

    virtual void DoPhysic(UELong FrameTime) override ;

    virtual void DoPhysic_GroundPlayer(double CycleTime);
    virtual void DoPhysic_Plane(double CycleTime);
    virtual void DoPhysic_Plane_Old(double CycleTime);
    virtual void DoPhysic_SupermanPlayer(double CycleTime);
    virtual void DoPhysic_Car(double CycleTime);
    virtual void DoPhysic_Train(double CycleTime);
    virtual void DoPhysic_Lift(double CycleTime);
    virtual void DoPhysic_Spinner(double CycleTime);

    virtual bool Save( ZStream_SpecialRamStream * OutStream ) override;
    virtual bool Load( ZStream_SpecialRamStream * InStream  ) override;

    virtual void Action_GoFastForward( double speed) override;
    virtual void Action_GoForward() override;
    virtual void Action_GoBackward() override;
    virtual void Action_GoLeftStraff() override;
    virtual void Action_GoRightStraff() override;
    virtual void Action_GoUp() override;
    virtual void Action_GoDown() override;
    virtual void Action_Jump() override;

    virtual void Start_Riding(Long x, Long y, Long z) override;
    virtual void Stop_Riding(bool RegiveVoxel=true) override;

    virtual void Action_GetInOutOfVehicle() override
    {
      if ( !Riding_IsRiding) return;

      switch(ActorMode)
      {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        default:
                 if (    (Velocity.x<1.0 && Velocity.x>-1.0)
                      && (Velocity.y<1.0 && Velocity.y>-1.0)
                      && (Velocity.z<1.0 && Velocity.z>-1.0)
                     ) Stop_Riding();
                 break;

        case 5:
                 if (    (Velocity.x<1000.0 && Velocity.x>-1000.0)
                      && (Velocity.y<1000.0 && Velocity.y>-1000.0)
                      && (Velocity.z<1000.0 && Velocity.z>-1000.0)
                    ) Stop_Riding();
                 break;
        case 6:
                 if (TrainSpeed < 10.0) Stop_Riding();
                 break;
        case 7:  if (Lift_Thrust >-10.0 && Lift_Thrust<10.0) Stop_Riding();
                 break;
        case 8:  Stop_Riding();
                 break;
      }
    }

    virtual void Event_Death() override;
    virtual void Event_PlaneCrash();
    virtual void Event_DeadlyFall();

    virtual void Process_Powers() override;

    // Train specific functions

    void TrainComputeWeightAndFunctions(ZVoxel * VoxelTable);

    Long StoreTrainElements(ZVector3L * HeadLoc, ZVoxel * VoxelTable, Long & TrainDirection);
      Long  _GetTrainElement(Long ActualDirection, ULong ElementCount, ZVector3L * ElementCoords, ZVoxel * VoxelTable);
    Long UnstoreTrainElements(ULong StoredVoxels, ZVector3L * HeadLoc, ZVoxel * VoxelTable, Long TrainDirection);
      bool _PutTrainElement(ULong Index, ULong StoredVoxels, ZVector3L * Location, ZVoxel * VoxelTable, Long TrainDirection);

    bool TrainFollowTrack(ZVector3d & ActualLocation, double LinearAdvance, Long &Direction, ZVector3d & NewLocation);

    bool LiftFollowGuide(ZVector3d & ActualLocation, double LinearAdvance, Long &Direction, ZVector3d & NewLocation);
    bool LiftFindRailDirection(ZVector3d & ActualLocation, Long & ViewDirectionOut);
};
