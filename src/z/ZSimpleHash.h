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
 * ZHash.h
 *
 *  Created on: 31 oct. 2014
 *      Author: laurent
 */

#pragma once 






#include "ZTypes.h"

template <typename Type>
class ZSimpleHash
{
  protected:
    ZMemSize HashSize;
    UShort   SizePowerOfTwo;
    Type  ** Table;
    UELong   Mask;

  public:

    ZSimpleHash()
    {
      HashSize = 0;
      SizePowerOfTwo = 0;
      Table = nullptr;
      Mask = 0;
    }

    ZSimpleHash(ULong SizePowerOfTwo)
    {
      HashSize = 0;
      this->SizePowerOfTwo = 0;
      Table = 0;
      Mask = 0;
      Init(SizePowerOfTwo);
    }

    void Init(ULong SizePowerOfTwo)
    {
      if (Table) delete [] Table;
      this->SizePowerOfTwo = SizePowerOfTwo;
      HashSize = 1 << SizePowerOfTwo;
      Mask = ((1 << (SizePowerOfTwo - 1)) -1);
      Table = new Type * [HashSize];
    }

    ~ZSimpleHash()
    {
      if (Table) delete [] Table;
     Table = nullptr;
      HashSize = 0;
    }

    void Clear()
    {
      ZMemSize i;

      if (!Table) return;
      for (i=0;i<HashSize;i++) Table[i] = nullptr;
    }

    inline void AddItem(char const * String, Type * Data)
    {
      ZMemSize Hash,i;

      Hash = ComputeHash(String);
      if (Hash >= HashSize) return;

      for(i=0;i<HashSize; i++)
      {
        if (!Table[Hash]) { Table[Hash] = Data; return; }
        Hash = (Hash+1) & Mask;
      }
    }

    inline Type * FindItem(char const * String, ZMemSize & FoundHash)
    {

      FoundHash = ComputeHash(String);
      if (FoundHash >= HashSize) return nullptr;

      return(Table[FoundHash]);
    }

    inline Type * FindNext(ZMemSize & FoundHash)
    {
      FoundHash = (FoundHash + 1) & Mask;

      return(Table[FoundHash]);
    }

    inline ZMemSize ComputeHash(char const * String)
    {
      UELong Hash;

      Hash = Hash_FNV64(String);
      Hash = HashReduce(Hash, SizePowerOfTwo);

      return((ZMemSize)Hash);
    }

  protected:

    UELong HashReduce(UELong Hash, ULong NeededBits)
    {
      UELong NewHash, RemainBits;

      RemainBits = 64;
      NewHash = 0;

      while(RemainBits)
      {
        NewHash ^= Hash & Mask;
        Hash >>= NeededBits;
        if (RemainBits < NeededBits) break;
        RemainBits -= NeededBits;
      }

      return(NewHash);
    }

   UELong Hash_FNV64(char const * String )
   {
     UELong c, Data, Hash;
     ULong i;

     // Make packs of 8 chars

     Hash=0;
     do
     {
       for (i=Data=0; i<8; c++)
       {
         if (!(c = *String++)) break;
         Data <<= 8;
         Data |= c;
       }
       Hash ^= Data;
       Hash = Hash * 1099511628211ull;

     } while (c);
     return(Hash);
   }

};