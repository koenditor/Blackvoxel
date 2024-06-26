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
 * ZObject_Lists.h
 *
 *  Created on: 7 mars 2011
 *      Author: laurent
 */

#pragma once 






#include "ZTypes.h"

#include "ZObject.h"

#include "ZMemPool.h"


class ZListItem
{
  public:
    ZListItem * Next;
    ZListItem * Prev;
    ZObject * Object;
    ZMemSize  PoolMemorySize;
    inline ZObject * GetObject() { return(Object); }
};

class ZList
{
    static ZBasicMemoryPool ZListBasicMemPool;
    ZMemoryPool * MemPool;
  public:

    ZListItem Head;
    ZListItem Tail;
    bool      Flag_FreeObjectsAtEnd;

    ZList()
    {
      MemPool = &ZListBasicMemPool;
      Head.Next = &Tail;
      Head.Prev = nullptr;
      Tail.Next = nullptr;
      Tail.Prev = &Head;
      Flag_FreeObjectsAtEnd = false;
    }

    ~ZList()
    {
      ClearList();
    }

    void SetMemoryPool(ZMemoryPool * MemPool) { this->MemPool = MemPool; }
    void SetOption_FreeObjectsAtEnd(bool Flag) { Flag_FreeObjectsAtEnd = Flag; }

    ZListItem * AddToHead(ZObject * Object);
    ZListItem * AddToTail(ZObject * Object);

    inline ZListItem * GetFirst()
    {
      if (Head.Next == &Tail) return nullptr;
      return(Head.Next);
    }

    inline ZListItem * GetLast()
    {
      if (Tail.Prev == &Head) return nullptr;
      return(Tail.Prev);
    }

    inline ZListItem * GetNext(ZListItem * ListItem)
    {
      if (ListItem->Next == &Tail) return nullptr;
      return(ListItem->Next);
    }

    inline ZListItem * GetPrev(ZListItem * ListItem)
    {
      if (ListItem->Prev == &Head) return nullptr;
      return(ListItem->Prev);
    }

    inline void Remove(ZListItem * ListItem)
    {
      ListItem->Next->Prev = ListItem->Prev;
      ListItem->Prev->Next = ListItem->Next;
      ListItem->Next = nullptr;
      ListItem->Prev = nullptr;
      MemPool->FreeMem(ListItem);
    }

    ZObject * RemoveTail()
    {
      ZListItem * ListItem;
      ZObject * Object;

      ListItem = Tail.Prev;
      if (ListItem == &Head) return nullptr;

      Object = ListItem->GetObject();

      ListItem->Next->Prev = ListItem->Prev;
      ListItem->Prev->Next = ListItem->Next;
      ListItem->Next = nullptr;
      ListItem->Prev = nullptr;
      MemPool->FreeMem(ListItem);

      return(Object);
    }

    ZObject * RemoveHead()
    {
      ZListItem * ListItem;
      ZObject * Object;

      ListItem = Head.Next;
      if (ListItem == &Tail) return nullptr;

      Object = ListItem->GetObject();

      ListItem->Next->Prev = ListItem->Prev;
      ListItem->Prev->Next = ListItem->Next;
      ListItem->Next = nullptr;
      ListItem->Prev = nullptr;
      MemPool->FreeMem(ListItem);

      return(Object);
    }

    inline void RemoveAndFree(ZListItem * ListItem)
    {
      ListItem->Next->Prev = ListItem->Prev;
      ListItem->Prev->Next = ListItem->Next;
      ListItem->Next = nullptr;
      ListItem->Prev = nullptr;
      delete(ListItem->Object);
      MemPool->FreeMem(ListItem);
    }

    void RemoveObject(ZObject * Object)
    {
      ZListItem * Item;

      Item = GetFirst();
      while (Item)
      {
        if (Item->GetObject() == Object) Remove(Item);
        Item = GetNext(Item);
      }
    }

    void ClearList()
    {
      ZListItem * Item, * NextItem;

      Item = GetFirst();
      while(Item)
      {
        NextItem = GetNext(Item);
        if ( Flag_FreeObjectsAtEnd ) RemoveAndFree(Item);
        else                         Remove(Item);
        Item = NextItem;
      }
    }

    void Dump();
};