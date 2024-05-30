/*
 * ZVCPU_Chip_Interface.h
 *
 *  Created on: 12 déc. 2014
 *      Author: laurent
 */

#pragma once 

#include "z/ZTypes.h"

#include "z/ZString.h"

class ZStream_SpecialRamStream;

class ZVCPU_Chip_Interface
{
  protected:
    ZMemSize ElapsedCycles;
    ZMemSize FrameCycles;
  public:
    virtual ~ZVCPU_Chip_Interface() {};
    virtual void  StopProgram()=0;
    virtual bool  IsRunningProgram()=0;
    inline ZMemSize GetElapsedCycles() {return (ElapsedCycles); }
    inline ZMemSize GetFrameCycles() { return(FrameCycles); }

    // Interrupt Handling

    virtual void TriggerInterrupt(ULong InterruptLevel)=0;

    // Debug interface

    virtual ULong GetCPURegisterCount(ULong InfoType)=0;
    virtual bool  GetCPURegister_Alpha(ULong InfoType, ULong RegisterNum, ZString & RegisterName, ZString & RegisterValue)=0;

    // Save / Load state

    virtual bool Save(ZStream_SpecialRamStream * Stream)=0;
    virtual bool Load(ZStream_SpecialRamStream * Stream)=0;

};