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
 * ZWorld.h
 *
 *  Created on: 1 janv. 2011
 *      Author: laurent
 */

#pragma once

#include <stdio.h>

#include "z/ZTypes.h"
#include "ZVoxelType.h"
#include "ZVoxelTypeManager.h"
#include "ZVoxelSector.h"
#include "ZSectorRingList.h"
#include "ACompileSettings.h"
#include "ZVoxel.h"
#include "ZSector_ModifTracker.h"
#include "ZVoxelExtension.h"
#include "z/ZObject.h"
#include "z/ZType_ZVector3B.h"
#include "z/ZType_ZVector3L.h"
#include "z/ZType_ZVector3L_CrossFunc.h"
#include "z/ZType_ZVector3d.h"

class ZCamera;
class ZSectorLoader;





struct ZRayCast_in
{
  public:
  // Input fields
    ZCamera      * Camera;
    ULong        MaxCubeIterations;
    Long        PlaneCubeDiff;
    double       MaxDetectionDistance;
};

struct ZRayCast_out
{
  // Output fields
    Bool         Collided;
    UByte        CollisionAxe;  // 0=x , 1=y , 2 = z;
    UByte        CollisionFace; // 0=Front , 1=Back, 2=Left, 3=Right, 4=Top, 5=Bottom;
    ZVoxelCoords PointedVoxel;
    ZVoxelCoords PredPointedVoxel;
    ZVector3d    CollisionPoint;
    ZVector2d    PointInCubeFace;
    double       CollisionDistance;
};






class ZVoxelWorld : public ZObject
{
    ZVoxelSector * WorkingFullSector;
    ZVoxelSector * WorkingEmptySector;
    ZVoxelSector * WorkingScratchSector;

  public:
    ZSectorRingList * SectorEjectList;
  public:
    ZVoxelSector * SectorList;

    ZVoxelSector ** SectorTable;
    ZVoxelTypeManager * VoxelTypeManager;
    ZSectorLoader * SectorLoader;

    ZMemSize       TableSize;
    ULong Size_x;
    ULong Size_y;
    ULong Size_z;

    ULong UniverseNum;

    static ZVector3B ProxLoc[6];

    ZVoxelWorld();
    ~ZVoxelWorld();

    void           SetVoxelTypeManager( ZVoxelTypeManager * Manager );
    void           SetSectorLoader(ZSectorLoader * SectorLoader);

    ZVoxelSector * FindSector (Long x, Long y, Long z);
    ZVoxelSector * FindSector_Secure(Long x, Long y, Long z); // Create sector if not in memory.


    void           AddSector( ZVoxelSector * Sector );
    void           RemoveSector( ZVoxelSector * Sector );
    void           RequestSector(Long x, Long y, Long z, Long Priority);
    bool           RequestSectorEject( ZVoxelSector * SectorToEject ) { return(SectorEjectList->PushToList(SectorToEject)); }
    void           ProcessNewLoadedSectors();
    void           ProcessOldEjectedSectors();

    void CreateDemoWorld();

    void SetUniverseNum(ULong UniverseNum) { this->UniverseNum = UniverseNum; }


    void SetVoxel(Long x, Long y, Long z, UShort VoxelValue);
    bool SetVoxel_WithCullingUpdate(Long x, Long y, Long z, UShort VoxelValue, UByte ImportanceFactor, bool CreateExtension = true, ZVoxelLocation * Location = nullptr);

    //bool SetVoxel_WithCullingUpdate(Long x, Long y, Long z, UShort VoxelValue, bool ImportanceFactor, bool CreateExtension = true, VoxelLocation * Location = 0);
    inline bool MoveVoxel(Long Sx, Long Sy, Long Sz, Long Ex, Long Ey, Long Ez, UShort ReplacementVoxel, UByte ImportanceFactor);
    inline bool MoveVoxel(ZVector3L * SCoords, ZVector3L * DCoords, UShort ReplacementVoxel, UByte ImportanceFactor);
    inline bool MoveVoxel_Sm(Long Sx, Long Sy, Long Sz, Long Ex, Long Ey, Long Ez, UShort ReplacementVoxel, UByte ImportanceFactor); // Set Moved flag
    inline bool MoveVoxel_Sm( ZVector3L * SCoords, ZVector3L * DCoords, UShort ReplacementVoxel, UByte ImportanceFactor);
    inline bool ExchangeVoxels(Long Sx, Long Sy, Long Sz, Long Dx, Long Dy, Long Dz, UByte ImportanceFactor, bool SetMoved);
    inline bool ExchangeVoxels( ZVector3L * Voxel1, ZVector3L * Voxel2, UByte ImportanceFactor, bool SetMoved);

    bool BlitBox(ZVector3L &SCoords, ZVector3L &DCoords, ZVector3L &Box);

    void Purge(UShort VoxelType);

    inline bool   GetVoxelLocation(ZVoxelLocation * OutLocation, Long x, Long y, Long z);
    inline bool   GetVoxelLocation(ZVoxelLocation * OutLocation, const ZVector3L * Coords);

    inline UShort GetVoxel(Long x, Long y, Long z);        // Get the voxel at the specified location. Fail with the "voxel not defined" voxeltype 65535 if the sector not in memory.
    inline UShort GetVoxel(ZVector3L * Coords);            // Idem but take coords in another form.
    inline UShort GetVoxel_Secure(Long x, Long y, Long z); // Secure version doesn't fail if sector not loaded. The sector is loaded or created if needed.
    inline UShort GetVoxel_Secure(ZVector3L * Coords);
    inline UShort GetVoxelExt(Long x, Long y, Long z, ZMemSize & OtherInfos);
    inline UShort GetVoxelPlayerCoord(double x, double y, double z);
    inline UShort GetVoxelPlayerCoord_Secure(double x, double y, double z);
    // New Functions
    inline bool ExtractToVoxel(ZVector3L * Coords, ZVoxel * OutVoxel, UByte ImportanceFactor);
    inline bool PlaceFromVoxel(ZVector3L * Coords, ZVoxel * VoxelToPlace, UByte ImportanceFactor);

    inline void Convert_Coords_PlayerToVoxel(double Px,double Py, double Pz, Long & Vx, Long & Vy, Long & Vz);
    inline void Convert_Coords_PlayerToVoxel( const ZVector3d * PlayerCoords, ZVector3L * VoxelCoords);
    inline void Convert_Coords_VoxelToPlayer( Long Vx, Long Vy, Long Vz, double &Px, double &Py, double &Pz );
    inline void Convert_Coords_VoxelToPlayer( const ZVector3L * VoxelCoords, ZVector3d * PlayerCoords );
    static inline void Convert_Location_ToCoords(ZVoxelLocation * InLoc, ZVector3L * OutCoords);

    bool RayCast(const ZRayCast_in * In, ZRayCast_out * Out );
    bool RayCast_Vector(const ZVector3d & Pos, const ZVector3d & Vector, const ZRayCast_in * In, ZRayCast_out * Out, bool InvertCollision = false );
    bool RayCast_Vector_special(const ZVector3d & Pos, const ZVector3d & Vector, const ZRayCast_in * In, ZRayCast_out * Out, bool InvertCollision = false );

    bool RayCast2(double x, double y, double z, double yaw, double pitch, double roll, ZVoxelCoords & PointedCube, ZVoxelCoords CubeBeforePointed  );

    void SectorUpdateFaceCulling(Long x, Long y, Long z, bool Isolated = false);
    void SectorUpdateFaceCulling2(Long x, Long y, Long z, bool Isolated = false); // Old
    ULong SectorUpdateFaceCulling_Partial(Long x, Long y, Long z, UByte FacesToDraw, bool Isolated = false);

    void WorldUpdateFaceCulling();

    bool Save();

    bool Load();
    bool Old_Load();


    ULong Info_GetSectorsInMemory()
    {
      ULong Count;
      ZVoxelSector * Sector;

      Count = 0;
      Sector = SectorList;

      while (Sector)
      {
        Count++;
        Sector = Sector->GlobalList_Next;
      }

      return(Count);
    }

    void Info_PrintHashStats()
    {
      ULong x,y,z, Offset, Count;
      ZVoxelSector * Sector;
      FILE * fh;

      fh = fopen("out.txt","w"); if (!fh) return;


      fprintf(fh,"----------------------------------------------\n");
      for (y=0 ; y<Size_y ; y++)
      {
        fprintf(fh,"Layer : %ld\n",(unsigned long)y);
        for (z=0 ; z<Size_z ; z++)
        {
          for (x=0 ; x<Size_x ; x++)
          {
            Offset = x + (y * Size_x) + (z * Size_x * Size_y);
            Count = 0;
            Sector = SectorTable[Offset];
            while (Sector) { Count++; Sector = Sector->Next; }
            fprintf(fh,"%02lx ", (unsigned long)Count);
          }
          fprintf(fh,"\n");
        }
      }
    }

    ZVoxelSector * GetZoneCopy( ZVector3L * Start, ZVector3L * End )
    {
      ZVector3L Size;
      Long tmp, xs, ys ,zs, xd, yd, zd;
      ZVoxelLocation Loc;
      ULong Offset;
      ZVoxelSector * NewSector;

      if ( Start->x > End->x ) {tmp = Start->x; Start->x = End->x; End->x = tmp;}
      if ( Start->y > End->y ) {tmp = Start->y; Start->y = End->y; End->y = tmp;}
      if ( Start->z > End->z ) {tmp = Start->z; Start->z = End->z; End->z = tmp;}

      Size = *End; Size -= (*Start);
      NewSector = new ZVoxelSector( Size.x + 1, Size.y + 1, Size.z + 1 );

      for ( zs = Start->z,zd = 0 ; zs <= End->z ; zs++, zd++ )
      {
        for ( xs = Start->x, xd = 0 ; xs <= End->x ; xs++, xd++ )
        {
          for ( ys = Start->y, yd = 0 ; ys <= End->y ; ys++, yd++  )
          {
            if (!GetVoxelLocation(&Loc, xs, ys, zs)) { delete NewSector; return nullptr; }

            Offset = (yd % NewSector->Size_y) + ( (xd % NewSector->Size_x ) * NewSector->Size_y )+ (( zd % NewSector->Size_z ) * ( NewSector->Size_y * NewSector->Size_x ));

            #if COMPILEOPTION_BOUNDCHECKINGSLOW==1
            if (Offset >= NewSector->DataSize) MANUAL_BREAKPOINT;
            #endif

            NewSector->Data[Offset]        = Loc.Sector->Data[Loc.Offset];
            if (VoxelTypeManager->VoxelTable[ Loc.Sector->Data[Loc.Offset] ]->Is_HasAllocatedMemoryExtension)
            {
              ZVoxelExtension * Extension;

              Extension = (ZVoxelExtension *) Loc.Sector->OtherInfos[Loc.Offset];
              NewSector->OtherInfos[Offset] = (ZMemSize)Extension->GetNewCopy();
            } else NewSector->OtherInfos[Offset]  = Loc.Sector->OtherInfos[Loc.Offset];
            NewSector->TempInfos[Offset]   = Loc.Sector->TempInfos[Loc.Offset];
            NewSector->FaceCulling[Offset] = Loc.Sector->FaceCulling[Loc.Offset];
          }
        }
      }

      return(NewSector);
    }

    bool WorldToSprite(ZVoxelSector * SourceSector, ZVector3L * Coordinates, UShort Direction)
    {
      ZVector3L Offset_Step;
      ZVector3L Start, End, Step;
      ZVector3L ComputedCoords;
      ZVoxelLocation Loc;

      Long LineLenY = SourceSector->Size_y;
      Long LineLenZ = LineLenY * SourceSector->Size_x;
      Long x,y,z;
      Long OffsetX,OffsetZ,Offset;
      ZVector3L ComputedLocation;


      Offset_Step.x = LineLenY; Offset_Step.y = 1; Offset_Step.z = LineLenZ;

      SourceSector->ModifTracker.BumpActualCycleNum();

      for (z=0,OffsetZ=0     ; z < SourceSector->Size_z ; z++,OffsetZ+=Offset_Step.z)
        for (x=0,OffsetX=0   ; x < SourceSector->Size_x ; x++,OffsetX+=Offset_Step.x)
          for (y=0,Offset = OffsetZ+OffsetX;y < SourceSector->Size_y; y++,Offset+=Offset_Step.y)
          {

            switch (Direction)
            {
              case 0: ComputedLocation.x = Coordinates->x - SourceSector->Handle_x + x;
                      ComputedLocation.y = Coordinates->y - SourceSector->Handle_y + y;
                      ComputedLocation.z = Coordinates->z - SourceSector->Handle_z + z;
                      break;
              case 1: ComputedLocation.x = Coordinates->x - SourceSector->Handle_x + z;
                      ComputedLocation.y = Coordinates->y - SourceSector->Handle_y + y;
                      ComputedLocation.z = Coordinates->z + SourceSector->Handle_z - x;
                      break;
              case 2: ComputedLocation.x = Coordinates->x + SourceSector->Handle_x - x;
                      ComputedLocation.y = Coordinates->y - SourceSector->Handle_y + y;
                      ComputedLocation.z = Coordinates->z + SourceSector->Handle_z - z;
                      break;
              case 3: ComputedLocation.x = Coordinates->x + SourceSector->Handle_x - z;
                      ComputedLocation.y = Coordinates->y - SourceSector->Handle_y + y;
                      ComputedLocation.z = Coordinates->z - SourceSector->Handle_z + x;
                      break;
            }
            #if COMPILEOPTION_BOUNDCHECKINGSLOW==1
            if (Offset >= (Long)SourceSector->DataSize) MANUAL_BREAKPOINT;
            #endif

            if (!GetVoxelLocation(&Loc, &ComputedLocation)) return(false);


            if (Loc.Sector->Data[Loc.Offset]) SourceSector->ModifTracker.Set(Loc.Offset);
            else                              SourceSector->ModifTracker.Clear();
            SourceSector->Data[Offset] = Loc.Sector->Data[Loc.Offset];
            SourceSector->OtherInfos[Offset] = Loc.Sector->OtherInfos[Loc.Offset];
            Loc.Sector->Data[Loc.Offset] = 0;
            Loc.Sector->OtherInfos[Loc.Offset] = 0;
            Loc.Sector->Flag_Render_Dirty = true;

            this->SetVoxel_WithCullingUpdate(ComputedLocation.x, ComputedLocation.y, ComputedLocation.z, 0, ZVoxelSector::CHANGE_IMPORTANT,1,nullptr );

          }

      return(true); // Operation is successfull
    }

    bool SpriteToWorld(ZVoxelSector * SourceSector, ZVector3L * Coordinates, UShort Direction, bool UseMask=false)
    {
      ZVector3L Offset_Step;
      ZVector3L Start, End, Step;
      ZVector3L ComputedCoords;
      ZVoxelLocation Loc;

      Long LineLenY = SourceSector->Size_y;
      Long LineLenZ = LineLenY * SourceSector->Size_x;
      Long x,y,z;
      Long OffsetX,OffsetZ,Offset;
      ZVector3L ComputedLocation;


      Offset_Step.x = LineLenY; Offset_Step.y = 1; Offset_Step.z = LineLenZ;

      for (z=0,OffsetZ=0     ; z < SourceSector->Size_z ; z++,OffsetZ+=Offset_Step.z)
        for (x=0,OffsetX=0   ; x < SourceSector->Size_x ; x++,OffsetX+=Offset_Step.x)
          for (y=0,Offset = OffsetZ+OffsetX;y < SourceSector->Size_y; y++,Offset+=Offset_Step.y)
          {

            switch (Direction)
            {
              case 0: ComputedLocation.x = Coordinates->x - SourceSector->Handle_x + x;
                      ComputedLocation.y = Coordinates->y - SourceSector->Handle_y + y;
                      ComputedLocation.z = Coordinates->z - SourceSector->Handle_z + z;
                      break;
              case 1: ComputedLocation.x = Coordinates->x - SourceSector->Handle_x + z;
                      ComputedLocation.y = Coordinates->y - SourceSector->Handle_y + y;
                      ComputedLocation.z = Coordinates->z + SourceSector->Handle_z - x;
                      break;
              case 2: ComputedLocation.x = Coordinates->x + SourceSector->Handle_x - x;
                      ComputedLocation.y = Coordinates->y - SourceSector->Handle_y + y;
                      ComputedLocation.z = Coordinates->z + SourceSector->Handle_z - z;
                      break;
              case 3: ComputedLocation.x = Coordinates->x + SourceSector->Handle_x - z;
                      ComputedLocation.y = Coordinates->y - SourceSector->Handle_y + y;
                      ComputedLocation.z = Coordinates->z - SourceSector->Handle_z + x;
                      break;
            }
            #if COMPILEOPTION_BOUNDCHECKINGSLOW==1
            if (Offset >= (Long)SourceSector->DataSize) MANUAL_BREAKPOINT;
            #endif

            if (!SetVoxel_WithCullingUpdate(ComputedLocation.x, ComputedLocation.y, ComputedLocation.z, SourceSector->Data[Offset], ZVoxelSector::CHANGE_IMPORTANT,0,&Loc ))
            {
              return(false);
            }
            Loc.Sector->OtherInfos[Loc.Offset] = SourceSector->OtherInfos[Offset];
            SourceSector->Data[Offset] = 0;
            SourceSector->OtherInfos[Offset] = 0;

          }

      return(true); // Operation is successfull
    }


    void SpriteMaskMakeFromShape( ZVoxelSector * Sect)
    {
      Long x,y,z;
      ULong Offset;

      Sect->ModifTracker.BumpActualCycleNum();
      Sect->ModifTracker.Clear();
      bool Detected= false;

      for (z=0 ; z < Sect->Size_z ; z++ )
        for (x=0 ; x < Sect->Size_x ; x++ )
        {
          // First pass, from low to high
          for (y=0; y < Sect->Size_y ; y++ )
          {
            Offset = Sect->GetOffset(x,y,z);
            if ((!Detected))
            {
              if (Sect->GetCube(Offset)) Detected = true;
            }
            Sect->ModifTracker.SetEx(Offset,Detected);
          }
          if (!Detected) continue; // If nothing in this column, go ahead.
          Detected = false;
          // Second pass, from high to low.
          for (y=Sect->Size_y-1; y>=0; y--)
          {
            Offset = Sect->GetOffset(x,y,z);
            if (Sect->GetCube(Offset)) break;
            Sect->ModifTracker.SetEx(Offset,false);
          }

        }
    }

    void BlitZoneCopy( ZVoxelSector * SourceSector, ZVector3L * Position, bool FillVoids = false )
    {
      Long xs,ys,zs,xd,yd,zd, LineX,LineZ;
      ULong Offset,i;
      ZVoxelLocation Loc;

      LineX = SourceSector->Size_y;
      LineZ = LineX * SourceSector->Size_x;

      ZVoxelSector * SectorTable[8192];
      ULong          TableOffset;

      TableOffset = 0;
      SectorTable[0] = nullptr;

      for ( zs = 0,zd = Position->z - SourceSector->Handle_z ; zs < SourceSector->Size_z ; zs++, zd++ )
      {
        for ( xs = 0, xd = Position->x - SourceSector->Handle_x ; xs < SourceSector->Size_x ; xs++, xd++ )
        {
          for ( ys = 0, yd = Position->y - SourceSector->Handle_y ; ys < SourceSector->Size_y ; ys++, yd++  )
          {
              Offset = ys + xs * LineX + zs * LineZ;
              if (FillVoids || SourceSector->Data[Offset] > 0)
              {
                if (SetVoxel_WithCullingUpdate(xd,yd,zd,SourceSector->Data[Offset], ZVoxelSector::CHANGE_CRITICAL , false, &Loc))
                {
                  if (VoxelTypeManager->VoxelTable[ SourceSector->Data[Offset] ]->Is_HasAllocatedMemoryExtension)
                  {
                    Loc.Sector->OtherInfos[Loc.Offset] = (ZMemSize) (((ZVoxelExtension *)SourceSector->OtherInfos[Offset])->GetNewCopy() );
                  }
                  else Loc.Sector->OtherInfos[Loc.Offset]  = SourceSector->OtherInfos[Offset];
                  Loc.Sector->TempInfos[Loc.Offset]   = SourceSector->TempInfos[Offset];
                  if (Loc.Sector != SectorTable[TableOffset]) { SectorTable[++TableOffset] = Loc.Sector; }
                }
              }


          }

        }
      }

      // Update face culling.

      for (i=1;i<TableOffset;i++)
      {
        SectorUpdateFaceCulling(SectorTable[i]->Pos_x,SectorTable[i]->Pos_y, SectorTable[i]->Pos_z,false);
        SectorTable[i]->Flag_Render_Dirty = true;
      }


    }



     inline bool GetVoxelLocationProx(ZVoxelLocation * OutLocation, const ZVector3L * Coords, ULong ProxCode )
     {
       ZVector3L NewLocation;

       NewLocation = *Coords + ProxLoc[ProxCode];

       return(GetVoxelLocation(OutLocation, &NewLocation));
     }






};


inline UShort ZVoxelWorld::GetVoxelPlayerCoord(double x, double y, double z)
{
  ELong lx,ly,lz;

  lx = (((ELong)x) >> 8);
  ly = (((ELong)y) >> 8);
  lz = (((ELong)z) >> 8);

  return( GetVoxel ((Long)lx,(Long)ly,(Long)lz) );
}

inline UShort ZVoxelWorld::GetVoxelPlayerCoord_Secure(double x, double y, double z)
{
  ELong lx,ly,lz;

  lx = (((ELong)x) >> 8);
  ly = (((ELong)y) >> 8);
  lz = (((ELong)z) >> 8);

  return( GetVoxel_Secure ((Long)lx,(Long)ly,(Long)lz) );
}

inline void ZVoxelWorld::Convert_Coords_PlayerToVoxel(double Px,double Py, double Pz, Long & Vx, Long & Vy, Long & Vz)
{
  Vx = (Long)(((ELong)Px) >> 8);
  Vy = (Long)(((ELong)Py) >> 8);
  Vz = (Long)(((ELong)Pz) >> 8);
}

inline void ZVoxelWorld::Convert_Coords_PlayerToVoxel( const ZVector3d * PlayerCoords, ZVector3L * VoxelCoords)
{
  VoxelCoords->x = (Long)(((ELong)PlayerCoords->x) >> 8);
  VoxelCoords->y = (Long)(((ELong)PlayerCoords->y) >> 8);
  VoxelCoords->z = (Long)(((ELong)PlayerCoords->z) >> 8);
}

inline void ZVoxelWorld::Convert_Coords_VoxelToPlayer( Long Vx, Long Vy, Long Vz, double &Px, double &Py, double &Pz )
{
  Px = ((ELong)Vx) << 8;
  Py = ((ELong)Vy) << 8;
  Pz = ((ELong)Vz) << 8;
}

inline void ZVoxelWorld::Convert_Coords_VoxelToPlayer( const ZVector3L * VoxelCoords, ZVector3d * PlayerCoords )
{
  PlayerCoords->x = ((ELong)VoxelCoords->x) << 8;
  PlayerCoords->y = ((ELong)VoxelCoords->y) << 8;
  PlayerCoords->z = ((ELong)VoxelCoords->z) << 8;
}

inline bool ZVoxelWorld::GetVoxelLocation(ZVoxelLocation * OutLocation, Long x, Long y, Long z)
{
  OutLocation->Sector = FindSector( x>>ZVOXELBLOCSHIFT_X , y>>ZVOXELBLOCSHIFT_Y , z>>ZVOXELBLOCSHIFT_Z );

  if (!OutLocation->Sector) {OutLocation->Sector = nullptr; OutLocation->Offset = 0; return(false); }

  OutLocation->Offset =   (y & ZVOXELBLOCMASK_Y)
                       + ((x & ZVOXELBLOCMASK_X) <<  ZVOXELBLOCSHIFT_Y )
                       + ((z & ZVOXELBLOCMASK_Z) << (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X));
  return(true);
}

inline bool ZVoxelWorld::GetVoxelLocation(ZVoxelLocation * OutLocation, const ZVector3L * Coords)
{
  OutLocation->Sector = FindSector( Coords->x>>ZVOXELBLOCSHIFT_X , Coords->y>>ZVOXELBLOCSHIFT_Y , Coords->z>>ZVOXELBLOCSHIFT_Z );

  if (!OutLocation->Sector) {OutLocation->Sector = nullptr; OutLocation->Offset = 0; return(false); }

  OutLocation->Offset =   (Coords->y & ZVOXELBLOCMASK_Y)
                       + ((Coords->x & ZVOXELBLOCMASK_X) <<  ZVOXELBLOCSHIFT_Y )
                       + ((Coords->z & ZVOXELBLOCMASK_Z) << (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X));
  return(true);
}

inline void ZVoxelWorld::Convert_Location_ToCoords(ZVoxelLocation * InLoc, ZVector3L * OutCoords)
{
  OutCoords->x = (InLoc->Sector->Pos_x << ZVOXELBLOCSHIFT_X) + ((InLoc->Offset & (ZVOXELBLOCMASK_X << (ZVOXELBLOCSHIFT_Y))) >> ZVOXELBLOCSHIFT_Y);
  OutCoords->y = (InLoc->Sector->Pos_y << ZVOXELBLOCSHIFT_Y) + (InLoc->Offset & (ZVOXELBLOCMASK_Y));
  OutCoords->z = (InLoc->Sector->Pos_z << ZVOXELBLOCSHIFT_Z) + ((InLoc->Offset & (ZVOXELBLOCMASK_X << (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X))) >> (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X));
}


inline UShort ZVoxelWorld::GetVoxel(Long x, Long y, Long z)
{
  ZVoxelSector * Sector;
  Long Offset;

  Sector = FindSector( x>>ZVOXELBLOCSHIFT_X , y>>ZVOXELBLOCSHIFT_Y , z>>ZVOXELBLOCSHIFT_Z );

  if (!Sector) return(-1);

  Offset =  (y & ZVOXELBLOCMASK_Y)
         + ((x & ZVOXELBLOCMASK_X) <<  ZVOXELBLOCSHIFT_Y )
         + ((z & ZVOXELBLOCMASK_Z) << (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X));

  return(Sector->Data[Offset]);
}

inline UShort ZVoxelWorld::GetVoxel(ZVector3L * Coords)
{
  ZVoxelSector * Sector;
  Long Offset;

  Sector = FindSector( Coords->x>>ZVOXELBLOCSHIFT_X , Coords->y>>ZVOXELBLOCSHIFT_Y , Coords->z>>ZVOXELBLOCSHIFT_Z );

  if (!Sector) return(-1);

  Offset =  (Coords->y & ZVOXELBLOCMASK_Y)
         + ((Coords->x & ZVOXELBLOCMASK_X) <<  ZVOXELBLOCSHIFT_Y )
         + ((Coords->z & ZVOXELBLOCMASK_Z) << (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X));

  return(Sector->Data[Offset]);
}


// Extract the voxel from the world. It means remove it and store it in the ZVoxel structure provided.

inline bool ZVoxelWorld::ExtractToVoxel(ZVector3L * Coords, ZVoxel * OutVoxel, UByte ImportanceFactor)
{
  ZVoxelSector * Sector;
  Long Offset;

  Sector = FindSector( Coords->x>>ZVOXELBLOCSHIFT_X , Coords->y>>ZVOXELBLOCSHIFT_Y , Coords->z>>ZVOXELBLOCSHIFT_Z );

  if (!Sector) return(false);

  Offset =  (Coords->y & ZVOXELBLOCMASK_Y)
         + ((Coords->x & ZVOXELBLOCMASK_X) <<  ZVOXELBLOCSHIFT_Y )
         + ((Coords->z & ZVOXELBLOCMASK_Z) << (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X));

  OutVoxel->VoxelType = Sector->Data[Offset];
  OutVoxel->OtherInfos= Sector->OtherInfos[Offset];
  OutVoxel->Temperature = Sector->TempInfos[Offset];

  Sector->Data[Offset]       = 0;
  Sector->OtherInfos[Offset] = 0;
  Sector->TempInfos[Offset]  = 0;

  return(SetVoxel_WithCullingUpdate(Coords->x, Coords->y, Coords->z, 0, ImportanceFactor, false, nullptr));

}

inline bool ZVoxelWorld::PlaceFromVoxel(ZVector3L * Coords, ZVoxel * VoxelToPlace, UByte ImportanceFactor)
{
  ZVoxelSector * Sector;
  Long Offset;
  UShort OldVoxelType;

  Sector = FindSector( Coords->x>>ZVOXELBLOCSHIFT_X , Coords->y>>ZVOXELBLOCSHIFT_Y , Coords->z>>ZVOXELBLOCSHIFT_Z );

  if (!Sector) return(false);

  Offset =  (Coords->y & ZVOXELBLOCMASK_Y)
         + ((Coords->x & ZVOXELBLOCMASK_X) <<  ZVOXELBLOCSHIFT_Y )
         + ((Coords->z & ZVOXELBLOCMASK_Z) << (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X));


  // Remove old voxel
  OldVoxelType = Sector->Data[Offset];
  Sector->Data[Offset] = 0;
  if (VoxelTypeManager->VoxelTable[OldVoxelType]->Is_HasAllocatedMemoryExtension)
  {
    VoxelTypeManager->VoxelTable[OldVoxelType]->DeleteVoxelExtension(Sector->OtherInfos[Offset]);
  }

  Sector->OtherInfos[Offset] = VoxelToPlace->OtherInfos;
  Sector->TempInfos[Offset]  = VoxelToPlace->Temperature;
  if (!SetVoxel_WithCullingUpdate(Coords->x, Coords->y, Coords->z, VoxelToPlace->VoxelType, ImportanceFactor, false, nullptr)) return(false);

  VoxelToPlace->VoxelType = 0;
  VoxelToPlace->OtherInfos = 0;
  VoxelToPlace->Temperature = 0;

  return(true);
}

inline UShort ZVoxelWorld::GetVoxel_Secure(Long x, Long y, Long z)
{
  ZVoxelSector * Sector;
  Long Offset;

  Sector = FindSector_Secure( x>>ZVOXELBLOCSHIFT_X , y>>ZVOXELBLOCSHIFT_Y , z>>ZVOXELBLOCSHIFT_Z );

  Offset =  (y & ZVOXELBLOCMASK_Y)
         + ((x & ZVOXELBLOCMASK_X) <<  ZVOXELBLOCSHIFT_Y )
         + ((z & ZVOXELBLOCMASK_Z) << (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X));

  return(Sector->Data[Offset]);
}

inline UShort ZVoxelWorld::GetVoxel_Secure(ZVector3L * Coords)
{
  ZVoxelSector * Sector;
  Long Offset;

  Sector = FindSector_Secure( Coords->x >>ZVOXELBLOCSHIFT_X , Coords->y >> ZVOXELBLOCSHIFT_Y , Coords->z >>ZVOXELBLOCSHIFT_Z );

  Offset =  (Coords->y & ZVOXELBLOCMASK_Y)
         + ((Coords->x & ZVOXELBLOCMASK_X) <<  ZVOXELBLOCSHIFT_Y )
         + ((Coords->z & ZVOXELBLOCMASK_Z) << (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X));

  return(Sector->Data[Offset]);
}

inline UShort ZVoxelWorld::GetVoxelExt(Long x, Long y, Long z, ZMemSize & OtherInfos)
{
  ZVoxelSector * Sector;
  Long Offset;

  Sector = FindSector( x>>ZVOXELBLOCSHIFT_X , y>>ZVOXELBLOCSHIFT_Y , z>>ZVOXELBLOCSHIFT_Z );

  if (!Sector) return(-1);

  Offset =  (y & ZVOXELBLOCMASK_Y)
         + ((x & ZVOXELBLOCMASK_X) <<  ZVOXELBLOCSHIFT_Y )
         + ((z & ZVOXELBLOCMASK_Z) << (ZVOXELBLOCSHIFT_Y + ZVOXELBLOCSHIFT_X));

  OtherInfos = Sector->OtherInfos[Offset];
  return(Sector->Data[Offset]);
}

bool ZVoxelWorld::MoveVoxel(Long Sx, Long Sy, Long Sz, Long Ex, Long Ey, Long Ez, UShort ReplacementVoxel, UByte ImportanceFactor)
{
  ZVoxelLocation Location1, Location2;

  if (!GetVoxelLocation(&Location1, Sx,Sy,Sz)) return(false);
  if (!SetVoxel_WithCullingUpdate(Ex,Ey,Ez, Location1.Sector->Data[Location1.Offset], ImportanceFactor, false, &Location2 )) return(false);

  // Move Extra infos

  Location2.Sector->OtherInfos[Location2.Offset] = Location1.Sector->OtherInfos[Location1.Offset];
  Location2.Sector->TempInfos[Location2.Offset] = Location1.Sector->TempInfos[Location1.Offset];

  Location1.Sector->Data[Location1.Offset] = 0;
  if (!SetVoxel_WithCullingUpdate(Sx,Sy,Sz, ReplacementVoxel, ImportanceFactor, true, nullptr )) return(false);
  return(true);
}

bool ZVoxelWorld::MoveVoxel( ZVector3L * SCoords, ZVector3L * DCoords, UShort ReplacementVoxel, UByte ImportanceFactor)
{
  ZVoxelLocation Location1, Location2;

  if (!GetVoxelLocation(&Location1, SCoords->x,SCoords->y,SCoords->z)) return(false);
  if (!SetVoxel_WithCullingUpdate(DCoords->x,DCoords->y,DCoords->z, Location1.Sector->Data[Location1.Offset], ImportanceFactor, false, &Location2 )) return(false);

  // Move Extra infos

  Location2.Sector->OtherInfos[Location2.Offset] = Location1.Sector->OtherInfos[Location1.Offset];
  Location2.Sector->TempInfos[Location2.Offset] = Location1.Sector->TempInfos[Location1.Offset];

  Location1.Sector->Data[Location1.Offset] = 0;
  if (!SetVoxel_WithCullingUpdate(SCoords->x,SCoords->y,SCoords->z, ReplacementVoxel, ImportanceFactor, true, nullptr )) return(false);
  return(true);
}

bool ZVoxelWorld::ExchangeVoxels(Long Sx, Long Sy, Long Sz, Long Dx, Long Dy, Long Dz, UByte ImportanceFactor, bool SetMoved)
{
  ZVoxelLocation Location1, Location2;
  UShort VoxelType1,VoxelType2,Temp1,Temp2;
  ZMemSize Extension1,Extension2;

  // Getting Voxel Memory Location Informations

  if (!GetVoxelLocation(&Location1, Sx, Sy, Sz)) return(false);
  if (!GetVoxelLocation(&Location2, Dx, Dy, Dz)) return(false);
  if ((Location1.Offset == Location2.Offset) && (Location1.Sector == Location2.Sector)) return(false);

  // Getting all infos.

  VoxelType1 = Location1.Sector->Data[Location1.Offset];
  Extension1 = Location1.Sector->OtherInfos[Location1.Offset];
  Temp1      = Location1.Sector->TempInfos[Location1.Offset];
  VoxelType2 = Location2.Sector->Data[Location2.Offset];
  Extension2 = Location2.Sector->OtherInfos[Location2.Offset];
  Temp2      = Location2.Sector->TempInfos[Location2.Offset];

  // Setting Extensions to zero to prevent multithreading issues of access to the wrong type of extension.

  Location1.Sector->OtherInfos[Location1.Offset]=0;
  Location2.Sector->OtherInfos[Location2.Offset]=0;

  // Set the voxels

  if (!SetVoxel_WithCullingUpdate(Dx, Dy, Dz, VoxelType1, ImportanceFactor, false, nullptr )) return(false);
  if (!SetVoxel_WithCullingUpdate(Sx, Sy, Sz, VoxelType2, ImportanceFactor, false, nullptr )) return(false);

  // Set Extensions a and temperature informations.

  Location1.Sector->OtherInfos[Location1.Offset] = Extension2;
  Location1.Sector->TempInfos[Location1.Offset]  = Temp2;
  Location2.Sector->OtherInfos[Location2.Offset] = Extension1;
  Location2.Sector->TempInfos[Location2.Offset]  = Temp1;

  // Set moved

  if (SetMoved)
  {
    Location1.Sector->ModifTracker.Set(Location1.Offset);
    Location2.Sector->ModifTracker.Set(Location2.Offset);
  }

  return(true);
}

bool ZVoxelWorld::ExchangeVoxels( ZVector3L * V1, ZVector3L * V2, UByte ImportanceFactor, bool SetMoved)
{
  ZVoxelLocation Location1, Location2;
  UShort VoxelType1,VoxelType2,Temp1,Temp2;
  ZMemSize Extension1,Extension2;

  // Getting Voxel Memory Location Informations

  if (!GetVoxelLocation(&Location1, V1->x, V1->y, V1->z)) return(false);
  if (!GetVoxelLocation(&Location2, V2->x, V2->y, V2->z)) return(false);
  if ((Location1.Offset == Location2.Offset) && (Location1.Sector == Location2.Sector)) return(false);

  // Getting all infos.

  VoxelType1 = Location1.Sector->Data[Location1.Offset];
  Extension1 = Location1.Sector->OtherInfos[Location1.Offset];
  Temp1      = Location1.Sector->TempInfos[Location1.Offset];
  VoxelType2 = Location2.Sector->Data[Location2.Offset];
  Extension2 = Location2.Sector->OtherInfos[Location2.Offset];
  Temp2      = Location2.Sector->TempInfos[Location2.Offset];

  // Setting Extensions to zero to prevent multithreading issues of access to the wrong type of extension.

  Location1.Sector->OtherInfos[Location1.Offset]=0;
  Location2.Sector->OtherInfos[Location2.Offset]=0;

  // Set the voxels

  if (!SetVoxel_WithCullingUpdate(V2->x, V2->y, V2->z, VoxelType1, ImportanceFactor, false, nullptr )) return(false);
  if (!SetVoxel_WithCullingUpdate(V1->x, V1->y, V1->z, VoxelType2, ImportanceFactor, false, nullptr )) return(false);

  // Set Extensions a and temperature informations.

  Location1.Sector->OtherInfos[Location1.Offset] = Extension2;
  Location1.Sector->TempInfos[Location1.Offset]  = Temp2;
  Location2.Sector->OtherInfos[Location2.Offset] = Extension1;
  Location2.Sector->TempInfos[Location2.Offset]  = Temp1;

  // Set moved

  if (SetMoved)
  {
    Location1.Sector->ModifTracker.Set(Location1.Offset);
    Location2.Sector->ModifTracker.Set(Location2.Offset);
  }

  return(true);
}

// This version set the moved flag.

bool ZVoxelWorld::MoveVoxel_Sm(Long Sx, Long Sy, Long Sz, Long Ex, Long Ey, Long Ez, UShort ReplacementVoxel, UByte ImportanceFactor)
{
  ZVoxelLocation Location1, Location2;

  if (!GetVoxelLocation(&Location1, Sx,Sy,Sz)) return(false);
  if (!SetVoxel_WithCullingUpdate(Ex,Ey,Ez, Location1.Sector->Data[Location1.Offset], ImportanceFactor, false, &Location2 )) return(false);

  // Move Extra infos

  Location2.Sector->OtherInfos[Location2.Offset] = Location1.Sector->OtherInfos[Location1.Offset];
  Location2.Sector->TempInfos[Location2.Offset] = Location1.Sector->TempInfos[Location1.Offset];
  Location2.Sector->ModifTracker.Set(Location2.Offset);

  Location1.Sector->Data[Location1.Offset] = 0;
  if (!SetVoxel_WithCullingUpdate(Sx,Sy,Sz, ReplacementVoxel, ImportanceFactor, true, nullptr )) return(false);
  return(true);
}

bool ZVoxelWorld::MoveVoxel_Sm( ZVector3L * SCoords, ZVector3L * DCoords, UShort ReplacementVoxel, UByte ImportanceFactor)
{
  ZVoxelLocation Location1, Location2;

  if (!GetVoxelLocation(&Location1, SCoords->x,SCoords->y,SCoords->z)) return(false);
  if (!SetVoxel_WithCullingUpdate(DCoords->x,DCoords->y,DCoords->z, Location1.Sector->Data[Location1.Offset], ImportanceFactor, false, &Location2 )) return(false);

  // Move Extra infos

  Location2.Sector->OtherInfos[Location2.Offset] = Location1.Sector->OtherInfos[Location1.Offset];
  Location2.Sector->TempInfos[Location2.Offset] = Location1.Sector->TempInfos[Location1.Offset];
  Location2.Sector->ModifTracker.Set(Location2.Offset);

  Location1.Sector->Data[Location1.Offset] = 0;
  if (!SetVoxel_WithCullingUpdate(SCoords->x,SCoords->y,SCoords->z, ReplacementVoxel, ImportanceFactor, true, nullptr )) return(false);
  return(true);
}
