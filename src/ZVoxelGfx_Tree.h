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
 * ZVoxelGfx_Tree.h
 *
 *  Created on: 9 févr. 2012
 *      Author: laurent
 */

#pragma once 






#include "z/ZFastRandom.h"
#include "z/ZLinearInterpolator.h"
#include "z/ZType_ZPolar3d.h"
#include "z/ZTypes.h"

class ZVector3d;
class ZVoxelSector;


class ZVoxelGfx_Tree
{
  protected:
    ZVoxelSector * Sector;
    ZLightSpeedRandom Random;
    ZLinearInterpolator ThicknessInterpolator;
  public:
    static ULong Seed;
    ZPolar3d StartingDirection;
    double   TruncHeight;
    ULong    MinSubDivisionsPerNode;
    ULong    MaxSubDivisionsPerNode;
    bool     RandomSubdivs;
    double   BrancheLen;
    double   MaxBranchLenght;
    bool     HasFolliage;
    double   FolliageLenght;


  public:
    ZVoxelGfx_Tree();

protected:
        void DrawBranch(ZVector3d * Position, ZPolar3d * BranchVector, double TotalBranchLen);
        void DrawTrunc(ZVector3d * Position, ZPolar3d * Direction);
public:
       void DrawTree(ZVoxelSector * Sector, ZVector3d * BasePosition);

    };
