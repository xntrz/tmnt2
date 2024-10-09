#pragma once

#include "SdTypes.hpp"


void SdSeqPcInit(void);
void SdSeqPcTerm(void);
void SdSeqPcTask(void);
void SdSeqPcKeyOn(int32 _port);
void SdSeqPcKeyOff(int32 _port);
void SdSeqPcSetFreq(int32 _port, int32 _value);
void SdSeqPcSetAR(int32 _port, uint8 _value);
void SdSeqPcSetDR(int32 _port, uint8 _value);
void SdSeqPcSetSR(int32 _port, uint8 _value);
void SdSeqPcSetRR(int32 _port, uint8 _value);
void SdSeqPcSetSL(int32 _port, uint8 _value);
void SdSeqPcSetVolL(int32 _port, int32 _value);
void SdSeqPcSetVolR(int32 _port, int32 _value);
void SdSeqPcSetAddress(int32 _port, int32 _code, SdWaveDataHdr_t* Header);