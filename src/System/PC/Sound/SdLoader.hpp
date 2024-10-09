#pragma once

#include "SdTypes.hpp"


#define SD_VAG_BANK_COUNT       (4)
#define SD_VAG_COUNT_PER_BANK   (0x80)
#define SD_VAG_BANK_NO(Code)    (((Code) >> 7) & (SD_VAG_BANK_COUNT - 1))
#define SD_VAG_FILE_NO(Code)    ((Code) & (SD_VAG_COUNT_PER_BANK - 1))


#define SD_VOX_BANK_COUNT       (4)
#define SD_VOX_COUNT_PER_BANK   (0x1000)
#define SD_VOX_BANK_NO(Code)    (((Code) >> 12) & (SD_VOX_BANK_COUNT - 1))
#define SD_VOX_FILE_NO(Code)    ((Code) & (SD_VOX_COUNT_PER_BANK - 1))


#define SD_SE_BANK_COUNT        (0x10)
#define SD_SE_COUNT_PER_BANK    (0x100)
#define SD_SE_BANK_OFFSET(Code) ((Code) & (SD_SE_COUNT_PER_BANK - 1)) // returns SE offset for Code relative to its bank
#define SD_SE_BANK_NO(Code) 	(((Code) >> 8) & (SD_SE_BANK_COUNT - 1)) // returns SE bank number for Code


struct SdTransCodeTable_t
{
    SdKdtTableData_t* Table;
    int32 Offset;
    int32 Count;
    int32 Size;
    uint16 CodeStart;
    uint16 CodeEnd;
};


struct SdSeCodeTable_t
{
    void* Table;
    int32 Count;
};


struct SdEnvelopeTable_t
{
    int16 Flag;
    int16 Count;
    void* Table;
    int32 WaveOffset;
    int32 WaveSize;
};


extern SdTransCodeTable_t   SdTransCodeTable[64];
extern SdSeCodeTable_t      SdSeCodeTable[16];
extern SdEnvelopeTable_t    SdDrumTable;
extern SdEnvelopeTable_t    SdEnvelopeTable[128];
extern void*                SdWaveTopAddress[128];


bool SdLoaderInit(const char* _host);
void SdLoaderTerm(void);
bool SdGetLoadInfo(void);
void SdSetHeap(void* _Heap, size_t _size);
void SdSetResources(void);
void SdMakeFilename(char* _buff, const char* _filename);
void SdStreamLoad(void);
void SdSystemDataLoadInfo(void);

void SdTransLoad(int32 _bank);
void SdTransRelease(int32 _bank);
void SdTransReleaseAll(void);
void SdTransLoadTask(void);

void SdWaveLoad(int32 _bank, uint32 _address);
void SdWaveRelease(int32 _bank);
void SdWaveReleaseAll(void);
void SdWaveLoadTask(void);

void* SdGetStrWorkAddr(void);
void SdStrWorkMemReserve(uint32 _size);
void* SdGetVoxHdrAddr(void);
void* SdGetVagHdrAddr(void);
void* SdGetStrMemAddr(void);
uint32 SdGetStrVoxMemSize(void);
uint32 SdGetStrVagMemSize(void);
void* SdGetWaveAddr(void);
uint32 SdGetWaveSize(int32 _bank);
uint32 SdGetTransWaveSize(int32 _bank);

uint32 SdGetVoicePosition(int32 _code);
uint32 SdGetVoiceSize(int32 _code);
uint32 SdGetVoiceDmaPage(int32 _code);
uint32 SdGetVoiceDmaBlock(int32 _code);
void SdGetVoiceFilename(char* _buff, int32 _no);

uint32 SdGetBgmPosition(int32 _code);
uint32 SdGetBgmSize(int32 _code);
uint32 SdGetBgmDmaPage(int32 _code);
uint32 SdGetBgmDmaBlock(int32 _code);
void SdGetBgmFilename(char* _buff, int32 _no);