/*
 * This file is part of Blackvoxel.
 *
 * Copyright 2010-2016 Laurent Thiebaut & Olivia Merle
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
 * ZTool_Scan.h
 *
 *  Created on: 13 juil. 2016
 *      Author: laurent
 */

#pragma once 






#include "ZTools.h"
#include "z/ZTypes.h"

class ZTool_Scan : public ZTool
{
    virtual bool Tool_MouseButtonClick(ULong Button);
    virtual bool Tool_MouseButtonRelease(ULong Button);

    virtual void Start_Tool();
    virtual void End_Tool();
    virtual void Display();
};