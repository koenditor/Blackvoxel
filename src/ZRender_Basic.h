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

#pragma once

#include "ZSectorSphere.h"
#include "ZRender_Sorter.h"
#include "GL/gl.h"
#include "ZSpecial_RadiusZoning.h"
#include "z/ZObject.h"
#include "z/ZType_ZVector3d.h"
#include "z/ZTypes.h"

class ZCamera;
class ZTextureManager;
class ZVoxelSector;
class ZVoxelTypeManager;
class ZVoxelWorld;
struct ZRayCast_out;


extern GLuint TextureName[1024];

class ZRender_basic_displaydata : public ZObject
{
  public:
    GLint DisplayList_Regular;
    GLint DisplayList_Transparent;


    ZRender_basic_displaydata()
    {
      DisplayList_Regular = 0;
      DisplayList_Transparent = 0;

    }
    ~ZRender_basic_displaydata()
    {
      if (DisplayList_Regular)     glDeleteLists(DisplayList_Regular, 1);
      DisplayList_Regular = 0;
      if (DisplayList_Transparent) glDeleteLists(DisplayList_Transparent, 1);
      DisplayList_Transparent = 0;
    }

};

class ZGame;

class ZRender_Basic
{
  protected:
    ZVoxelWorld * World;
    ZVoxelTypeManager * VoxelTypeManager;
    ZTextureManager  * TextureManager;

    ZCamera     * Camera;
    ZRayCast_out * PointedVoxel;
    ZRadius_Zoning RadiusZones;

    Long hRenderRadius;
    Long vRenderRadius;

    ULong Stat_RenderDrawFaces;
    ULong Stat_FaceTop;
    ULong Stat_FaceBottom;
    ULong Stat_FaceLeft;
    ULong Stat_FaceRight;
    ULong Stat_FaceFront;
    ULong Stat_FaceBack;

    ULong Stat_RefreshWaitingSectorCount;
    Long RefreshWaiters[64];
    Long RefreshToDo[64];

    // Display Dimension and aspect ratio

    ZVector2L ViewportResolution;
    double    VerticalFOV;
    double    NearClip;
    double    PixelAspectRatio;
    double    Optimisation_FCullingFactor;

    // Computed by render()
    double Frustum_V;
    double Frustum_H;
    double Aspect_Ratio;
    double Frustum_CullingLimit;


  public:

    ZGame * GameEnv;

    ZSectorSphere SectorSphere;

    ULong BvProp_CrossHairType;
    bool  BvProp_DisplayCrossHair;
    bool  BvProp_DisplayVoxelSelector;

    ZRender_Sorter RenderSorter;

    ZRender_Basic()
    {
      hRenderRadius = 1;  // 8
      vRenderRadius = 1;  // 3
      World = nullptr;
      VoxelTypeManager = nullptr;
      TextureManager = nullptr;
      Stat_RenderDrawFaces = 0;
      Stat_FaceTop = 0;
      Stat_FaceFront = 0;
      Stat_FaceRight = 0;
      Stat_FaceLeft = 0;
      Stat_FaceBottom = 0;
      Stat_FaceBack = 0;
      PointedVoxel = nullptr;
      Camera = nullptr;
      GameEnv = nullptr;
      Stat_RefreshWaitingSectorCount = 0;
      BvProp_CrossHairType = 1;
      BvProp_DisplayCrossHair     = true;
      BvProp_DisplayVoxelSelector = true;
      ViewportResolution.x = 1920;
      ViewportResolution.y = 1200;
      VerticalFOV = 110; //TODO: FOV
      NearClip = 8.0;
      PixelAspectRatio = 1.0;
      Optimisation_FCullingFactor = 1.0;

      Frustum_V = 0.0;
      Frustum_H = 0.0;
      Aspect_Ratio = 0.0;
      Frustum_CullingLimit = 0.0;
    }

    void SetWorld           ( ZVoxelWorld * World );
    void SetCamera          ( ZCamera * Camera );
    void SetVoxelTypeManager( ZVoxelTypeManager * Manager );
    void SetTextureManager  ( ZTextureManager * Manager ) { this->TextureManager = Manager; }
    void SetPointedVoxel    ( ZRayCast_out * PVoxel)         { this->PointedVoxel = PVoxel; }
    void SetViewportResolution(ZVector2L &Resolution) { ViewportResolution = Resolution; }
    void SetVerticalFOV(double VFov)                  { VerticalFOV = VFov; }
    void SetPixelAspectRatio(double AspectRatio = 1.0){ PixelAspectRatio = AspectRatio; }
    void SetSectorCullingOptimisationFactor(double CullingOptimisationFactor = 1.0) { Optimisation_FCullingFactor = CullingOptimisationFactor; }



    void Init();
    void Cleanup() { }
    Bool LoadVoxelTexturesToGPU();
    Bool LoadTexturesToGPU();


    void Render_DebugLine       ( ZVector3d & Start, ZVector3d & End);
    void Render_VoxelSelector   (ZVoxelCoords * SelectedVoxel, float r, float g, float b);
    void MakeSectorRenderingData(ZVoxelSector * Sector);
    void MakeSectorRenderingData_Sorted(ZVoxelSector * Sector);
    void Render();

    void SetRenderSectorRadius(Long Horizontal, Long Vertical)
    {
      hRenderRadius = Horizontal;
      vRenderRadius = Vertical;
      RadiusZones.SetSize(hRenderRadius * 2 + 1, vRenderRadius * 2 + 1,hRenderRadius * 2 + 1);
      // RadiusZones.DrawZones( 5.0, 3.5, 3.0, 2.0 );
      RadiusZones.DrawZones(5.0,1);
      RadiusZones.DrawZones(3.5,2);
      RadiusZones.DrawZones(3.0,3);
      RadiusZones.DrawZones(2.0,4);
      //RadiusZones.DebugOut();
      SectorSphere.Init(Horizontal, Vertical);
    }

    void SetGameEnv( ZGame * GameEnv ) { this->GameEnv = GameEnv;  }

    void FreeDisplayData(ZVoxelSector * Sector);

    inline bool Is_PointVisible(ZVector3d::ZTransformParam * TransformParam, ZVector3d * const Point)
    {
      ZVector3d Cv;
      bool Visible;

      Cv = *Point;

      // Translation and Rotation


      Cv.Transform(TransformParam);

      // Projection

      Cv.x = Cv.x / Cv.z * NearClip;
      Cv.y = Cv.y / Cv.z * NearClip;

      // Visibility test

      Visible = (
                     (Cv.z > 0.0)
                  && (Cv.x < Frustum_CullingLimit && Cv.x >-Frustum_CullingLimit) // Number replaced by Frustum_CullingLimit was 50.0
                  && (Cv.y < Frustum_CullingLimit && Cv.y >-Frustum_CullingLimit) //
                );

      return(Visible);
    }





    void ComputeAndSetAspectRatio(double VerticalFOV, double PixelAspectRatio, ZVector2L & ViewportResolution);

};
