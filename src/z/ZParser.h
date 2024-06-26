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
 * LParser2.h
 *
 *  Created on: Pre-2000 code.
 *      Author: laurent
 */

#pragma once

#include "ZTypes.h"
#include "ZSeparatorSets.h"
#include "ZString.h"

class ZKeyWordSet;


// ***********************************************************************
// *                           Parser System                             *
// ***********************************************************************
//
// (0) TK_NONE
//
// -No token recognized.
//
// (1) TK_EOF
//
// -End Of file
//
// (2) TK_KEYWORD
//
// KeyWordId = Keyword number.
// 
// (3) TK_CONST
//
// ConstType = Constant type:
//
//      CT_INTEG  = value in: ConstValue.ValInteger
//      CT_ULONG  = value in: ConstValue.ValULong
//      CT_LONG   = value in: ConstValue.ValLong
//      CT_USHORT = value in: ConstValue.ValUShort
//      CT_SHORT  = value in: ConstValue.ValShort
//      CT_UBYTE  = value in: ConstValue.ValUByte
//      CT_BYTE   = value in: ConstValue.ValByte
//      CT_FLOAT  = value in: ConstValue.ValFloat
//      CT_DOUBLE = value in: ConstValue.ValDouble
//      CT_ALPHA  = value in: AlphaValue
//
//
// (4) TK_IDENTIFIER
//
// AlphaValue = Identifier name.
//


class ZParser_State
{
  public:
    char * SText;
    char * SLineStart;
    ULong  SLen;
    ULong  SLineCount;
    char * SDocument;
    ULong  SDocumentTotalLen;
};

class ZParser_Token
{

  public:
  union NumericValue
  {
    Long   ValInteger;
    ULong  ValULong;
    Long   ValLong;
    UShort ValUShort;
    Short  ValShort;
    UByte  ValUByte;
    Byte   ValByte;
    float  ValFloat;
    double ValDouble;
  };
  enum LToken2Type {TK_NONE,TK_EOF,TK_KEYWORD,TK_CONST,TK_IDENTIFIER};
  enum LConstType {CT_INTEGER,CT_ULONG,CT_LONG,CT_USHORT,CT_SHORT,
                   CT_UBYTE,CT_BYTE,CT_FLOAT,CT_DOUBLE,CT_ALPHA};

  LToken2Type TokenType;
  ULong KeyWordId;
  LConstType ConstType;
  NumericValue ConstValue;
  ZString AlphaValue;

  // Location in text.
  ULong  LineCount;
  char * LineStart;
  char * TokenStart;
  char * TokenEnd;
  // Saving the last position
  char * SText;
  char * SLineStart;
  ULong  SLen;
  ULong  SLineCount;
  char * SDocumentStart;
  ULong  SDocumentTotalLen;

  void CopyStart(ZParser_Token & Token)
  {
    TokenType = Token.TokenType;
    KeyWordId = Token.KeyWordId;
    ConstType = Token.ConstType;
    ConstValue= Token.ConstValue;
    AlphaValue= Token.AlphaValue;

    LineCount = Token.LineCount;
    LineStart = Token.LineStart;
    TokenStart= Token.TokenStart;
    TokenEnd  = Token.TokenEnd;
    SText     = Token.SText;
    SLineStart = Token.SLineStart;
    SLen       = Token.SLen;
    SLineCount = Token.SLineCount;
    SDocumentStart= Token.SDocumentStart;
    SDocumentTotalLen = Token.SDocumentTotalLen;
  }
  void CopyEnd(ZParser_Token & Token)
  {
    TokenEnd = Token.TokenEnd;
  }
};



class ZParser
{

  public:
          ZParser();
    bool  Init(char * Text,ULong Len, ZKeyWordSet * KeyWordSet);
    bool  GetToken(ZParser_Token &Token);
    bool  StripNextBlankLine();
    void  SetPointers(char * Text, ULong Len, char * StartLineP, ULong LineCount)
    {
      this->Tp        = Text;
      this->LineP     = StartLineP;
      this->RLen      = Len;
      this->LineCount = LineCount;
    }

    void  GetPointers(char * &Text,ULong &Len,char * &StartLineP,ULong &LineCount)
    {
      Text       = this->Tp;
      StartLineP = this->LineP;
      Len        = this->RLen;
      LineCount  = this->LineCount;
    }

    void  SetDocument(char * Text, ULong Len)
    {
      this->DocumentP = Text;
      this->Tp        = Text;
      this->LineP     = Text;
      this->RLen      = Len;
      this->TotalLen  = Len;
      this->LineCount = 0;
    }

    void  BackTrack(ZParser_Token &Token)
    {
      this->Tp        = Token.SText;
      this->LineP     = Token.SLineStart;
      this->RLen      = Token.SLen;
      this->LineCount = Token.SLineCount;
      //this->TotalLen  = Token.SDocumentTotalLen;
      //this->DocumentP = Token.SDocumentStart;
    }

    void SetErrorPoint(Long Offset = 0) {ErrorPoint = Tp + Offset;}

    void SavePos(ZParser_State & Pos)
    {
      Pos.SText             = this->Tp;
      Pos.SLineStart        = this->LineP;
      Pos.SLen              = this->RLen;
      Pos.SLineCount        = this->LineCount;
      Pos.SDocument         = this->DocumentP;
      Pos.SDocumentTotalLen = this->TotalLen;
    }
    void SavePosFromLineStart(ZParser_State & Pos)
    {
      Pos.SText      = this->LineP;
      Pos.SLineStart = this->LineP;
      Pos.SLen       = this->RLen;
      Pos.SLineCount = this->LineCount;     
      Pos.SDocument         = this->DocumentP;
      Pos.SDocumentTotalLen = this->TotalLen;
    }

    void RestorePos(ZParser_State & Pos)
    {
      this->Tp        = Pos.SText;
      this->LineP     = Pos.SLineStart;
      this->RLen      = Pos.SLen;
      this->LineCount = Pos.SLineCount;
      this->DocumentP = Pos.SDocument;
      this->TotalLen  = Pos.SDocumentTotalLen;
    }
    
    ZString GetErrorText_BeforeError(ZParser_Token & Token, ULong GoBackLines = 0)
    {
      ZString Result;
      ULong i, Count;

      char * StartPoint;

      StartPoint = Token.SLineStart;
      Count = 0;
      while(GoBackLines)
      {
        if (StartPoint <= DocumentP) break;
        if (0x0A == *StartPoint /*|| Count >= 80*/) {GoBackLines--;Count = 0;}

        StartPoint--; Count++;
      }
      if (StartPoint[0] == 0x0D && StartPoint[1] == 0x0A) {StartPoint+=2;Count--;}

      if (! (StartPoint<Token.TokenStart) ) return(Result);

      Count = Token.TokenStart - StartPoint;
      for (i=0;i < Count ;i ++) Result<< (char) *(StartPoint++);

      return(Result);
    }
    ZString GetErrorText_TheError(ZParser_Token & Token)
    {
      ZString Result;
      ULong i, Count;

      if (Token.TokenType == ZParser_Token::TK_EOF) {Result = "[FIN DE FICHIER INATTENDUE]";return(Result);}
      if (Token.TokenStart >= Token.TokenEnd) return(Result);
      
      Count = Token.TokenEnd - Token.TokenStart;
      for (i=0; i<Count; i++) Result<< (char)Token.TokenStart[i];
      return(Result);
    }

    ZString GetErrorText_AfterError(ZParser_Token & Token, ULong NumLines=1)
    {
      ZString Result;
      ULong i,  Len;
      char * Text;

      Text = Token.TokenEnd;
      Len  = Token.SLen - (Token.TokenEnd - Token.SText);
      
      for (i=0;i<Len;i++)
      { 
        if ( 0x0D == Text[i] ) 
        {
          NumLines--;
          if (!NumLines) break;
        }
        if (Text[i]) Result<< (char) Text[i];
      }
      
      return(Result);
    }

    void  StripUToEol();
    void Set_NoErrorOnEof(bool Flag) { NoErrorOnEof = Flag; }
  private:
    void  StripLeadBlank();
    ULong GetKeyWord();
    bool  GetNumConst(Long &Number);
    bool  GetNumHexConst(Long &Number);
    bool  GetNumDoubleConst(double &Number);
    bool  GetAlphaConst(ZString &Const);
    bool  GetAlphaInlineConst(ZString &Const);
    bool  GetIdentifier(ZString &Ident);

  public:

    ZSeparatorSet SeparatorSet_StartString;
    ZSeparatorSet SeparatorSet_EndString;
    ZSeparatorSet SeparatorSet_NextLine;
    ZSeparatorSet SeparatorSet_Blank;
    ZSeparatorSet SeparatorSet_Identifier;
    ZKeyWordSet * KeyWordSet;
  private:
    char * DocumentP;
    ULong  TotalLen;
    char * LineP;
    char * Tp;
    ULong RLen;
    ULong LineCount;
    char * ErrorPoint;

    bool StripBlank;
    bool ParseKeyWords;
    bool ParseConst;
    bool ParseIdent;
    bool NoErrorOnEof;

};
