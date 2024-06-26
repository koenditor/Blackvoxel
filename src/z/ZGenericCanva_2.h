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
 * ZGenericCanva_2.h
 *
 *  Created on  : 25 févr. 2012
 *  Reworked on : 14 juin 2014
 *      Author: laurent
 */

#pragma once

#include <math.h>

#include "ZTypes.h"
#include "ZBitmapImage.h"
#include "ZString.h"

template <typename Type>
class ZGenericCanva
{
  public:
    struct ZMinMax { double Min, Max; };

    Type     * Canva;         // Main canva "bitmap" storage.
    ZMemSize   ElementCount;  // Element Count
    Long       Width, Height; // Width and Height.

    ZMinMax  * MinMax_H;      // Tables used for filling functions.
    ZMinMax  * MinMax_V;      //

    ZGenericCanva(Long Width, Long Height)
    {
      this->Width = Width; this->Height = Height;
      ElementCount = Width * Height;
      Canva = new Type[ElementCount];
      MinMax_H = nullptr;
      MinMax_V = nullptr;
    }

    ZGenericCanva()
    {
      Width = 0;
      Height = 0;
      ElementCount = 0;
      Canva = nullptr;
      MinMax_H = nullptr;
      MinMax_V = nullptr;
    }

    void SetSize(Long Width, Long Height)
    {
      if (Canva) delete[] Canva;
      this->Width = Width; this->Height = Height;
      ElementCount = Width * Height;
      Canva = new Type[ElementCount];
    }

    ~ZGenericCanva()
    {
      if (MinMax_H) { delete MinMax_H;  }
      if (MinMax_V) { delete MinMax_V;  }
      if (Canva) delete[] Canva;

      Canva = nullptr;
      ElementCount = 0;
      Width = Height = 0;
      MinMax_H = nullptr;
      MinMax_V = nullptr;
    }

    void Clear(Type ClearData = 0)
    {
      ZMemSize i;

      for (i=0;i<ElementCount;i++) Canva[i]=ClearData;
    }

    inline Type GetPoint_Fast(Long x,Long y)
    {
      return( Canva[x+y*Width] );

    }
    inline void SetPoint_Fast(Long x, Long y, Type Color)
    {
      Canva[x+y*Width] = Color;
    }

    inline Type GetPoint_Secure(Long x,Long y)
    {
      if (x<0 || y<0 || x>=Width || y>Height) return(0);
      return( Canva[x+y*Width] );
    }

    inline void SetPoint_Secure(Long x, Long y, Type Color)
    {
      if (x<0 || y<0 || x>=Width || y>=Height) return;
      Canva[x+y*Width] = Color;
    }

    void DrawBox(Long Sx,Long Sy, Long Ex,Long Ey, Type Color)
    {
      ZMemSize StartX, StartY, EndX, EndY,x,y;

      // Clipping the box if it goes outside of the canva

      if      (Sx<0      ) Sx = 0;
      else if (Sx>=Width ) Sx = Width - 1;
      if      (Sy<0      ) Sy = 0;
      else if (Sy>=Height) Sy = Height - 1;
      if      (Ex<0      ) Ex = 0;
      else if (Ex>=Width ) Ex = Width - 1;
      if      (Ey<0      ) Ey = 0;
      else if (Ey>=Height) Ey = Height - 1;

      StartX = Sx;
      EndX   = Ex;
      StartY = Sy * Width;
      EndY   = Ey * Width;

      for (y=StartY;y<=EndY; y+=Height)
        for (x=StartX;x<=EndX; x++)
        {
          //if ( (x+y)>= CanvaMemSize) *(char *)0=1;
          Canva[x+y]=Color;
        }
    }

    enum { CLIP_IN = 0 , CLIP_LEFT = 1, CLIP_RIGHT = 2, CLIP_TOP = 4, CLIP_BOTTOM = 8 };

    inline ULong EvalPoint( double x, double y )
    {
      ULong Code;
      Code = CLIP_IN;
      if      ( x < 0 )    Code |= CLIP_LEFT;
      else if ( x >= Width)  Code |= CLIP_RIGHT;
      if      ( y < 0 )    Code |= CLIP_TOP;
      else if ( y >= Height) Code |= CLIP_BOTTOM;
      return Code;
    }

    bool ClipCoords( ZLineCoords * LineCoords )
    {
      ULong Start_ClipCode, End_ClipCode;
      double sx,sy,ex,ey, dwidth,dheight;


      sx = LineCoords->Start.x;
      sy = LineCoords->Start.y;
      ex = LineCoords->End.x;
      ey = LineCoords->End.y;
      dwidth = Width-1;
      dheight = Height-1;

      Start_ClipCode = EvalPoint(sx, sy);
      End_ClipCode   = EvalPoint(ex, ey);

      while ( true )
      {

        if (!(Start_ClipCode | End_ClipCode))
        {
          LineCoords->Start.x = sx; LineCoords->Start.y = sy; LineCoords->End.x = ex; LineCoords->End.y = ey;
          return(true);  // Line is visible and in the frame, so draw it.
        }
        if (Start_ClipCode & End_ClipCode)
        {
          LineCoords->Start.x = sx; LineCoords->Start.y = sy; LineCoords->End.x = ex; LineCoords->End.y = ey;
          return(false); // Line is out of the frame, so don't draw it.
        }

        if ((Start_ClipCode))
        {
          if      (Start_ClipCode & CLIP_LEFT ) { sy = sy + (ey - sy) / (ex - sx) * (0      - sx); sx = 0; }
          else if (Start_ClipCode & CLIP_RIGHT) { sy = sy + (ey - sy) / (ex - sx) * (dwidth - sx); sx = dwidth; }
          else if (Start_ClipCode & CLIP_TOP  ) { sx = sx + (ex - sx) / (ey - sy) * (0      - sy); sy = 0; }
          else if (Start_ClipCode & CLIP_BOTTOM){ sx = sx + (ex - sx) / (ey - sy) * (dheight- sy); sy = dheight; }
          Start_ClipCode = EvalPoint(sx, sy);
        }
        else
        {
          if      (End_ClipCode & CLIP_LEFT ) { ey = sy + (ey - sy) / (ex - sx) * (0      - sx); ex = 0; }
          else if (End_ClipCode & CLIP_RIGHT) { ey = sy + (ey - sy) / (ex - sx) * (dwidth - sx); ex = dwidth; }
          else if (End_ClipCode & CLIP_TOP  ) { ex = sx + (ex - sx) / (ey - sy) * (0      - sy); ey = 0; }
          else if (End_ClipCode & CLIP_BOTTOM){ ex = sx + (ex - sx) / (ey - sy) * (dheight- sy); ey = dheight; }
          End_ClipCode   = EvalPoint(ex, ey);
        }
      }
      return(false);
    }

    void Polygon_Start()
    {
      Long i;

      if (!MinMax_H) MinMax_H = new ZMinMax[Width];
      if (!MinMax_V) MinMax_V = new ZMinMax[Height];

      for (i=0;i<Width;i++) { MinMax_H[i].Min = Width;  MinMax_H[i].Max = 0; }
      for (i=0;i<Height;i++){ MinMax_V[i].Min = Height; MinMax_V[i].Max = 0; }
    }

    void Polygon_Segment( ZLineCoords * LineCoords )
    {
      double dx, dy, Divider, x,y, dwidth, dheight;
      Long xint,yint;
      Long Steps;

      dwidth = Width;
      dheight = Height;
      x = LineCoords->Start.x;
      y = LineCoords->Start.y;
      dx = LineCoords->End.x - LineCoords->Start.x;
      dy = LineCoords->End.y - LineCoords->Start.y;

      Divider =  ( fabs(dy) > fabs(dx) ) ? fabs(dy) : fabs(dx);

      dx /= Divider;
      dy /= Divider;

      for (Steps = Divider;Steps>=0;Steps--)
      {

        if (x>=0.0 && x<dwidth)
        {
          xint = x;
          if (y>MinMax_H[xint].Max) MinMax_H[xint].Max = y;
          if (y<MinMax_H[xint].Min) MinMax_H[xint].Min = y;
        }
        if ( y >= 0.0 && y<dheight )
        {
          yint = y;
          if (x>MinMax_V[yint].Max) MinMax_V[yint].Max = x;
          if (x<MinMax_V[yint].Min) MinMax_V[yint].Min = x;
        }
        x += dx;
        y += dy;
      }
    }

    void Polygon_Render( Type RenderColor )
    {
      Long y,x;
      Long Min, Max;
      Type * Dp;

      for (y=0;y<Height;y++)
      {
        Min = MinMax_V[y].Min+0.5; Max = MinMax_V[y].Max+0.5;
        if (Max >= Width) Max = Width-1;
        if (Max <  0    ) Max = 0;
        if (Min <  0    ) Min = 0;
        if (Min >= Width) Min = Width-1;
        if (Max>=Min)
        {
          Dp = this->Canva + (y*Width) + Min;
          for(x=Min;x<=Max;x++, Dp++)
          {
            *Dp = RenderColor;
          }
        }
      }

      for (x=0;x<Width;x++)
      {
        Min = MinMax_H[x].Min; Max = MinMax_H[x].Max;
        if (Max >= Height) Max = Height-1;
        if (Max <  0    )  Max = 0;
        if (Min <  0    )  Min = 0;
        if (Min >= Height) Min = Height-1;
        if (Max>=Min)
        {
          Dp = this->Canva + x + Min * Width;
          for(y=Min;y<=Max;y++, Dp+= Width)
          {
            *Dp = RenderColor;
          }
        }
      }

    }

    void DrawCircleFilled(double x, double y, double radius, Type Color, double Precision)
    {
      double SqRadius, px,py;
      radius = fabs(radius);
      // IntRadius = ceil(radius);


      SqRadius = radius * radius;
      // printf("Circle : [x=%fl][y=%fl][radius=%fl][Precision=%fl]\n",x,y,radius,Precision);
      for (py=-radius;py<=radius;py+=Precision)
      {
        for (px=-radius;px<=radius;px+=Precision)
        {
          // printf("[%fl %fl %fl =%fl,%fl %d] ", px,py, px*px + py*py,px+x,py+y, (int)(px*px + py*py) <= radius );
          if ( (px*px + py*py) <= SqRadius) SetPoint_Secure(px+x,py+y,Color);
        }
        // printf("\n");
      }
    }

    void GetFromBitmap(ZBitmapImage * Image, Bool InitSizeFromImage = true, ULong Channel=0)
    {
      Long x,y;

      ZMemSize Offset;
      if (InitSizeFromImage)
      {
        SetSize(Image->Width, Image->Height);
      }
      else if (!Canva || Image->Width < (ULong)Width || Image->Height < (ULong)Height ) return;

      Offset = 0;
      switch (sizeof(Type))
      {
        case 1: for (y=0;y<Height;y++)
                  for (x=0;x<Width; x++)
                  {Canva[Offset++] = Image->BitmapMemory[ ((x + y * Image->Width) * Image->BytesPerPixel) + Channel ];}
                break;
        case 4: for (y=0;y<Height;y++)
                  for (x=0;x<Width; x++)
                  {
                    Canva[Offset++] = Image->GetPixel(x,y);
                  }
                break;
      }
    }

    void GetFromByteTable(const char ** Table, ULong Width, ULong Height)
    {
      ULong x,y;

      SetSize(Width,Height);
      for (y=0;y<Height;y++)
      {
        for (x=0;x<Width; x++)
        {
          SetPoint_Secure(x,y, (Table[y])[x] );
        }
      }
    }

    // To optimize

    ZGenericCanva * GetRectCopy(Long xStart, Long yStart, Long xSize, Long ySize)
    {
      ZGenericCanva * NewCanva;
      Long x,y;

      // Bound Checking

      if (xStart<0) {xSize += xStart; xStart = 0;}
      if (yStart<0) {ySize += yStart; yStart = 0;}
      if (xSize > (Width  - xStart)) xSize = Width - xStart;
      if (ySize > (Height - yStart)) ySize = Height - yStart;
      if (xSize <=0 || ySize<=0 ) return nullptr;

      NewCanva = new ZGenericCanva(xSize,ySize);


      for (y = 0; y < ySize; y++)
        for (x = 0; x < xSize; x++)
        {
          NewCanva->SetPoint_Secure(x, y, GetPoint_Secure(x+xStart,y+yStart));
        }

      return(NewCanva);
    }

    // To optimize

    void Blit( ZGenericCanva * Source, Long xs, Long ys, Long xSize, Long ySize, Long xd, Long yd)
    {
      Long x,y;
      bool Reloop;


      do
      {
        Reloop = false;
        if (xSize > (Source->Width - xs)) xSize = Source->Width - xs;
        if (ySize > (Source->Height- ys)) ySize = Source->Height- ys;
        if (Width  < (xd + xSize)) xSize += Width - (xd + xSize);
        if (Height < (yd + ySize)) ySize += Height- (yd + ySize);
        if (xd < 0) {xSize += xd; xs -= xd; xd = 0; Reloop = true;}
        if (yd < 0) {ySize += yd; ys -= yd; yd = 0; Reloop = true;}
      } while (Reloop);

      if (xSize <= 0 || ySize <= 0) return;

      for (y=0; y < ySize; y++)
        for (x=0; x < xSize; x++)
        {
          SetPoint_Secure(x+xd,y+yd, Source->GetPoint_Secure(x+xs,y+ys));
        }
    }

    void Line (ZLineCoords * LineCoords, Type Color)
    {
      double dx,dy,x,y;
      ULong i,Steps;

      // Line Clipping algorithm : Clip lines into canva and don't draw if completely out of the drawing area.

      if (!ClipCoords(LineCoords)) return;

      // Compute line delta.

      x = LineCoords->Start.x;
      y = LineCoords->Start.y;
      dx = LineCoords->End.x - LineCoords->Start.x;
      dy = LineCoords->End.y - LineCoords->Start.y;
      Steps = int ((fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy));
      dx /= Steps;
      dy /= Steps;

      for(i=0;i<Steps;i++)
      {
        SetPoint_Secure(x,y, Color);
        x += dx; y += dy;
      }
      SetPoint_Secure(LineCoords->End.x,LineCoords->End.y, Color);

    }

    void SearchAndReplace(Type SearchData, Type NewData)
    {
      Long x,y;

      for (y=0;y<Height;y++)
        for (x=0;x<Width;x++)
        {
          if (SearchData == GetPoint_Fast(x,y)) SetPoint_Fast(x,y,NewData);
        }
    }

    void DumpAscii(ZString & Out, const char * TableDecl)
    {
      Long x,y;
      Long i;
      Type c;
      ZString Number, L[4];

      Out << TableDecl << Out.NewLine();
      Out << "{" << Out.NewLine();

      // Column Numbers
      for (i=1;i<=Width;i++)
      {
        Number = i;
        if (Number.Len >=4) L[3].Append_char(Number.String[Number.Len-4]); else L[3].Append_char(' ');
        if (Number.Len >=3) L[2].Append_char(Number.String[Number.Len-3]); else L[2].Append_char(' ');
        if (Number.Len >=2) L[1].Append_char(Number.String[Number.Len-2]); else L[1].Append_char(' ');
        if (Number.Len >=1) L[0].Append_char(Number.String[Number.Len-1]); else L[0].Append_char(' ');
      }
      Out << "// " << L[2] << Out.NewLine();
      Out << "// " << L[1] << Out.NewLine();
      Out << "// " << L[0] << Out.NewLine();

      for (y=0;y<Height;y++)
      {
        Out << "  \"";
        for (x=0;x<Width;x++)
        {
          c = GetPoint_Secure(x,y);
          if ( (c<'0' || c>'9') && (c<'A' || c>'Z') && (c<'a' || c>'z') ) c = '.';
          Out.Append_char(c);
        }
        Out << "\"";
        if (y!=(Height-1)) Out << ",";
        else               Out << " ";
        Out << " // " << (ULong)(y+1) << Out.NewLine();
        if (y==(Height-1)) Out << "};";
      }
    }

    Type Convert_AscToNum(Type c)
    {
      if ( c >='0' && c<='9' ) return(c-'0');
      if ( c >='A' && c<='Z' ) return(c-'A'+10);
      if ( c >='a' && c<='z' ) return(c-'a'+36);
      return(255);
    }

    Type Convert_NumToAsc(Type Num)
    {
      if (Num >= 0 && Num <= 9)   return(Num + '0');
      if (Num >= 10 && Num <= 35) return(Num + 'A' - 10);
      if (Num >= 36 && Num <= 61) return(Num + 'a' - 36);
      return('*');
    }

};