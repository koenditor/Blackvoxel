/*
 * This file is part of Blackvoxel.
 *
 * Copyright 2010-2015 Laurent Thiebaut & Olivia Merle
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
 * ZOs_Specific_ViewDoc.h
 *
 *  Created on: 4 janv. 2015
 *      Author: laurent
 */

#pragma once 

#include "z/ZTypes.h"

#include "ACompileSettings.h"

class ZViewDoc
{

  public:

  static bool ViewDocPage(ULong Number, bool Online);
  static bool OpenURL(char * URL);

};
