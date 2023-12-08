#pragma once

#include "SdTypes.hpp"


void SdDrvInitCB(void);
void SdDrvLoaderCommandCB(int32 Command, uint32 Param);
void SdDrvSetChannelBGMCB(void);
int32 SdDrvVoiceServerSelectionCB(SdSetDriverCode_t* SdSetDrvCode);
int32 SdDrvBGMServerSelectionCB(SdSetDriverCode_t* SdSetDrvCode);
void SdDrvSetBeforeCB(int32 Reason, SdSetDriverCode_t* SdSetDrvCode);
void SdDrvSetAfterCB(int32 Reason, SdSetDriverCode_t* SdSetDrvCode);
void SdDrvVagVolumeCB(uint32 Code, int32* VolLeft, int32* VolRight);
void SdDrvVoxVolumeCB(uint32 Code, int32* VolLeft, int32* VolRight);
void SdDrvSeqVolumeCB(struct SdSeq_t* SdSeq, int32* VolLeft, int32* VolRight);
int32 SdDrvSeqPanProcSelectionCB(void);
void SdDrvGameThreadCallBack(void);
void SdDrvUserThreadCallBack(void);