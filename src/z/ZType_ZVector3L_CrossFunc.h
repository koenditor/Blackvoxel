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
 * ZType_ZVector3L_CrossFunc.h
 *
 *  Created on: 10 févr. 2012
 *      Author: laurent
 */

#pragma once 

#include "ZTypes.h"

#include "ZType_ZVector3L.h"

#include "ZType_ZVector3d.h"

inline ZVector3L & ZVector3L::operator = (const ZVector3d & Vec3d)
{
  x = Vec3d.x;
  y = Vec3d.y;
  z = Vec3d.z;
  return(*this);
}

inline ZVector3L::ZVector3L ( const ZVector3d & Vec3d )
{
  x = Vec3d.x;
  y = Vec3d.y;
  z = Vec3d.z;
}

inline ZVector3L & ZVector3L::operator = (const ZVector3B & Vec3B)
{
  x = Vec3B.x;
  y = Vec3B.y;
  z = Vec3B.z;

  return(*this);
}

inline ZVector3L::ZVector3L ( const ZVector3B & Vec3B )
{
  x = Vec3B.x;
  y = Vec3B.y;
  z = Vec3B.z;
}

inline ZVector3L ZVector3L::operator + (const ZVector3B Vec3B) const
{
  ZVector3L Result;

  Result.x = x + Vec3B.x;
  Result.y = y + Vec3B.y;
  Result.z = z + Vec3B.z;

  return(Result);
}

inline ZVector3L & ZVector3L::operator += (const ZVector3B & Vec3B)
{
  x += Vec3B.x;
  y += Vec3B.y;
  z += Vec3B.z;

  return(*this);
}

inline ZVector3L ZVector3L::operator - (const ZVector3B Vec3B) const
{
  ZVector3L Result;

  Result.x = x - Vec3B.x;
  Result.y = y - Vec3B.y;
  Result.z = z - Vec3B.z;

  return(Result);
}

inline ZVector3L & ZVector3L::operator -= (const ZVector3B & Vec3B)
{
  x -= Vec3B.x;
  y -= Vec3B.y;
  z -= Vec3B.z;

  return(*this);
}