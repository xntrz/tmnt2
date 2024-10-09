#pragma once

#include "SdTypes.hpp"


void SdDrvInitCB(void);
void SdDrvLoaderCommandCB(int32 _command, int32 _param);
int32 SdDrvVoiceServerSelectionCB(SdSetDriverCode_t* CodeBuff);
int32 SdDrvBGMServerSelectionCB(SdSetDriverCode_t* CodeBuff);
void SdDrvSetBeforeCB(int32 _reason, SdSetDriverCode_t* CodeBuff);
void SdDrvSetAfterCB(int32 _reason, SdSetDriverCode_t* CodeBuff);
void SdDrvVagVolumeCB(int32 _code, int32* _volLeft, int32* _volRight);
void SdDrvVoxVolumeCB(int32 _cabs, int32* _volLeft, int32* _volRight);
void SdDrvSeqVolumeCB(struct SdSeqWork_t* Work, int32* _volLeft, int32* _volRight);
int32 SdDrvSeqPanProcSelectionCB(void);
int32 SdDrvSetChannelBGMCB(SdSetDriverCode_t* CodeBuff, SdKdtTableData_t* TableData, int32 _option, int32 _priority);
int32 SdDrvSetChannelSECB(SdSetDriverCode_t* CodeBuff, SdKdtTableData_t* TableData, int32 _option);
void SdDrvSetToneBeforeCB(struct SdSeqWork_t* Work, int32 _status, struct SdSetEnvelope_t* Env);
void SdDrvSeqCorrectionCB(void);