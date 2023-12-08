#pragma once

#include "SdTypes.hpp"


#define SdGetVagLoaderNo(Code)  (((Code) >> 7) & 3)
#define SdGetVagFileNo(Code)    ((Code) & 0x7F)

#define SdGetVoxLoaderNo(Code)  (((Code) >> 12) & 3)
#define SdGetVoxFileNo(Code)    ((Code) & 0xFFF)


struct SdKdtTableData_t
{
    uint8 Priority;
    uint8 WaveBank;
    uint8 SetMode;
    uint8 Channel;
    uint8 Count;
    uint8 pad[3];
    uint32 Option;
    void* TableAddress;
};


CHECK_SIZE(SdKdtTableData_t, 0x10);


struct SdTransCodeTable_t
{
    SdKdtTableData_t* Table;
    uint32 Offset;
    uint32 Count;
    uint32 Size;
    uint32 CodeStart;
    uint32 CodeEnd;
};


struct SdSeCodeTable_t
{
    void* Table;
    uint32 Count;
};


struct SdEnvelopeTable_t
{
    uint16 Flag;
    uint16 Count;
    void* Table;
    uint32 WaveOffset;
    uint32 WaveSize;
};


extern SdTransCodeTable_t   SdTransCodeTable[64];
extern SdSeCodeTable_t      SdSeCodeTable[16];
extern SdEnvelopeTable_t    SdDrumTable;
extern SdEnvelopeTable_t    SdEnvelopeTable[128];
extern void*                SdWaveTopAddress[128];


bool SdLoaderInit(const char* Host);
void SdLoaderTerm(void);
bool SdGetLoadInfo(void);
void SdSetHeap(void* Heap, uint32 Size);
void SdSetResources(void);
void SdMakeFilename(char* Buff, const char* Filename);
void SdStreamLoad(void);
void SdSystemDataLoadInfo(void);
void SdTransLoad(int32 Bank);
void SdTransRelease(int32 Bank);
void SdTransReleaseAll(void);
void SdTransLoadTask(void);
void SdWaveLoad(int32 Bank, uint32 Address);
void SdWaveRelease(int32 Bank);
void SdWaveReleaseAll(void);
void SdWaveLoadTask(void);
void* SdGetStrWorkAddr(void);
void SdStrWorkMemReserve(uint32 Size);
void* SdGetVoxHdrAddr(void);
void* SdGetVagHdrAddr(void);
void* SdGetStrMemAddr(void);
uint32 SdGetStrVoxMemSize(void);
uint32 SdGetStrVagMemSize(void);
void* SdGetWaveAddr(void);
uint32 SdGetWaveSize(int32 Bank);
uint32 SdGetTransWaveSize(int32 Bank);
uint32 SdGetVoicePosition(uint32 Code);
uint32 SdGetVoiceSize(uint32 Code);
uint32 SdGetVoiceDmaPage(uint32 Code);
uint32 SdGetVoiceDmaBlock(uint32 Code);
void SdGetVoiceFilename(char* FilenameBuff, int32 No);
uint32 SdGetBgmPosition(uint32 Code);
uint32 SdGetBgmSize(uint32 Code);
uint32 SdGetBgmDmaPage(uint32 Code);
uint32 SdGetBgmDmaBlock(uint32 Code);
void SdGetBgmFilename(char* FilenameBuff, int32 No);