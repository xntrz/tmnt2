#include "SdLoader.hpp"
#include "SdMem.hpp"
#include "SdLoadTask.hpp"
#include "SdStr.hpp"
#include "SdLog.hpp"


#define SD_LOADER_SYSTEM_WAVE   (0)
#define SD_LOADER_SYSTEM_TRSE   (1)
#define SD_LOADER_SYSTEM_SE     (2)

#define SD_DMA_PAGE             (0x8000)
#define SD_DMA_BLOCK            (0x4000)

#define SD_VOX_DMA_PAGE         (0x20000)
#define SD_VOX_DMA_BLOCK        (0x8000)


static_assert(COUNT_OF(SdWaveTopAddress) == COUNT_OF(SdEnvelopeTable), "should be same count");


struct SdBgmDataHeader_t
{
    int32 Count;
    int32 Offset;
    int32 Size;
};


struct SdBgmHeader_t
{
    char Title[32];
    int32 Version;
    int32 TotalSize;
    SdBgmDataHeader_t Head[32];
};


struct SdSeDataHeader_t
{
    int32 Count;
    int32 Offset;
};


struct SdSeHeader_t
{
    char Title[32];
    int32 Version;
    int32 TotalSize;
    SdSeDataHeader_t Head[16];
};


struct SdEnvelopeDrumHeader_t
{
    uint16 Count;
    uint16 Pad;
    int32 Offset;
};


struct SdEnvelopeWaveHeader_t
{
    uint16 Count;
    uint16 Flag;
    int32 Offset;
    int32 WaveOffset;
    int32 WaveSize;
};


struct SdEnvelopeHeader_t
{
    char Title[32];
    int32 Version;
    int32 TotalSize;
    int32 EnvSize;
    int32 WaveSize;
    SdEnvelopeDrumHeader_t Drum;
    SdEnvelopeWaveHeader_t Wave[256];
};


struct SdStrHeader_t
{
    int32 Size;
    int32 Count;
};


struct SdStrFile_t
{
    uint32 Position;
    uint32 Size;
};


struct SdDmaInfo_t
{
    uint32 Page;
    uint32 Block;
};


struct SdLoadPoint_t
{
    const char* Name;
    uint32 LSN;
    uint32 Size;
};


/*extern*/ SdTransCodeTable_t   SdTransCodeTable[64];
/*extern*/ SdSeCodeTable_t      SdSeCodeTable[16];
/*extern*/ SdEnvelopeTable_t    SdDrumTable;
/*extern*/ SdEnvelopeTable_t    SdEnvelopeTable[128];
/*extern*/ void*                SdWaveTopAddress[128];


static SdLoadPoint_t SdLoadSystem[] =
{
    { "WaveData.bin",   0,  0   },
    { "TrSeData.bin",   0,  0   },
    { "SeData.bin",     0,  0   },
};


static SdLoadPoint_t SdLoadVag[] =
{
    { "strbgm.vas",  0, 0 },
    { "strbgm1.vas", 0, 0 },
    { "strbgm2.vas", 0, 0 },
    { "strbgm3.vas", 0, 0 },
};


static SdLoadPoint_t SdLoadVox[] =
{
    { "voice0.bin",  0, 0 },
    { "voice1.bin",  0, 0 },
    { "voice2.bin",  0, 0 },
    { "voice3.bin",  0, 0 },
    { "voice4.bin",  0, 0 },
    { "voice5.bin",  0, 0 },
    { "voice6.bin",  0, 0 },
    { "voice7.bin",  0, 0 },
    { "voice8.bin",  0, 0 },
    { "voice9.bin",  0, 0 },
    { "voice10.bin", 0, 0 },
    { "voice11.bin", 0, 0 },
    { "voice12.bin", 0, 0 },
    { "voice13.bin", 0, 0 },
    { "voice14.bin", 0, 0 },
    { "voice15.bin", 0, 0 },
};


static int32 SdLoadVoxBank[] =
{
    0,
    1,
    2,
    3,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
};


static SdDmaInfo_t SdDmaInfoVag[] =
{
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
};


static SdDmaInfo_t SdDmaInfoVox[] =
{
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
    { SD_DMA_PAGE, SD_DMA_BLOCK },
};


//
//  Sd host path
//
static char         SdHost[256];
static bool         SdLoaderCleanupPhase = false;


//
//  Sd stream load info
//
static int32        SdVagLoadNum = 1;
static int32        SdVoxLoadNum = 4;
static uint32       SdVagMemSize = SD_DMA_PAGE;
static uint32       SdVoxMemSize = SD_DMA_PAGE;


//
//  Sd resource mem map
//
static bool 	SdHeapAllocFlag = false;
static void*	SdHeapAddrBegin = nullptr;
static void*	SdHeapAddrEnd = nullptr;
static void*	SdStrWorkAddr = nullptr;
static uint32	SdStrWorkSize = 0;
static void*	SdVoxHdrAddr = nullptr;
static uint32	SdVoxHdrSize = 0;
static void*	SdVagHdrAddr = nullptr;
static uint32	SdVagHdrSize = 0;
static void*	SdStrMemAddr = nullptr;
static uint32	SdStrMemSize = 0;
static void*	SdWaveAddr = nullptr;
static uint32	SdWavHdrMemSize = 0;
static uint32	SdSeqHdrMemSize = 0;


//
//  Sd wave / trans wave load tasks
//
static void*        SdWavLoadTask[16];
static bool         SdWavLoadTaskBusyFlag = false;
static int32        SdWavLoadTaskCnt = 0;

static void*        SdTransWavLoadTask[16];
static bool         SdTransWavLoadTaskBusyFlag = false;
static int32        SdTransWavLoadTaskCnt = 0;


static inline const SdStrFile_t* SdGetVoxFileLSN(int32 _code)
{
    int32 FileNo    = SD_VOX_FILE_NO(_code);
    int32 BankNo  = SD_VOX_BANK_NO(_code);

    ASSERT(BankNo >= 0);
    ASSERT(BankNo < SdVoxLoadNum);

    return &((SdStrFile_t*)SdLoadVox[BankNo].LSN)[FileNo];
};


static inline const SdStrFile_t* SdGetVagFileLSN(int32 _code)
{
    int32 FileNo = SD_VAG_FILE_NO(_code);
    int32 BankNo = SD_VAG_BANK_NO(_code);

    ASSERT(BankNo >= 0);
    ASSERT(BankNo < SdVagLoadNum);

    return &((SdStrFile_t*)SdLoadVag[BankNo].LSN)[FileNo];
};


static inline const char* SdGetSystemName(int32 _type)
{
    ASSERT(_type >= 0);
    ASSERT(_type < COUNT_OF(SdLoadSystem));

    return (((_type >= 0) && (_type < COUNT_OF(SdLoadSystem))) ? SdLoadSystem[_type].Name : nullptr);
};


static void SdTaskTransWavOnLoad(void* _task)
{
    int32 Bank = (int32)SdLoadTaskGetUser(_task);
    if (SdTransCodeTable[Bank].Count)
    {
        SdKdtTableData_t* TableData = (SdKdtTableData_t*)SdLoadTaskAddress(_task);

        for (int32 i = 0; i < SdTransCodeTable[Bank].Count; ++i)
            TableData[i].TableAddress = (uint8**)((uintptr_t)TableData[i].TableAddress + (uintptr_t)TableData);
    };
};


static void SdTaskWavOnLoad(void* _task)
{
    ;
};


static void SdSetTransInfo(SdBgmHeader_t* Header)
{
    for (int32 i = 0; i < COUNT_OF(SdTransCodeTable); ++i)
    {
        SdTransCodeTable[i].Table   = nullptr;
        SdTransCodeTable[i].Offset  = Header->Head[i].Offset;
        SdTransCodeTable[i].Count   = Header->Head[i].Count;
        SdTransCodeTable[i].Size    = Header->Head[i].Size;
    };

    if (SdTransCodeTable[0].Count)
    {
        SdTransCodeTable[0].CodeStart   = (uint16)0x2000;
        SdTransCodeTable[0].CodeEnd     = (uint16)SdTransCodeTable[0].Count + (uint16)0x1FFF;
    }
    else
    {
        SdTransCodeTable[0].CodeStart   = (uint16)0;
        SdTransCodeTable[0].CodeEnd     = (uint16)0x1FFF;
    };

    for (int32 i = 1; i < COUNT_OF(SdTransCodeTable); ++i)
    {
        if (SdTransCodeTable[i].Count)
        {
            SdTransCodeTable[i].CodeStart   = SdTransCodeTable[i - 1].CodeEnd + 1;
            SdTransCodeTable[i].CodeEnd     = SdTransCodeTable[i].CodeStart + (uint16)SdTransCodeTable[i].Count - 1;
        }
        else
        {
            SdTransCodeTable[i].CodeStart   = 0;
            SdTransCodeTable[i].CodeEnd     = SdTransCodeTable[i - 1].CodeEnd;
        };          
    };
};


static void SdSetSeInfo(SdSeHeader_t* Header, void* Data)
{
    int32 Count = 0;
    
    for (int32 i = 0; i < COUNT_OF(SdSeCodeTable); ++i)
    {
        SdSeCodeTable[i].Table = &((uint8*)Data)[Header->Head[i].Offset];
        SdSeCodeTable[i].Count = Header->Head[i].Count;

        Count += Header->Head[i].Count;
    };

    SdKdtTableData_t* TableData = (SdKdtTableData_t*)Data;

    for (int32 i = 0; i < Count; ++i)
        TableData[i].TableAddress = (uint8**)((uintptr_t)TableData[i].TableAddress + (uintptr_t)Data);
};


static void SdSetWaveInfo(SdEnvelopeHeader_t* Header, void* Data)
{
    static_assert(COUNT_OF(SdEnvelopeTable) <= COUNT_OF(SdEnvelopeHeader_t::Wave), "checkout");
    
    SdDrumTable.Flag        = 0x3;
    SdDrumTable.Count       = Header->Drum.Count;
    SdDrumTable.Table       = (uint8*)Data + Header->Drum.Offset - (SD_SCT_SIZE * 3);
    SdDrumTable.WaveOffset  = 0;
    SdDrumTable.WaveSize    = 0;

    for (int32 i = 0; i < COUNT_OF(SdEnvelopeTable); ++i)
    {
        SdEnvelopeTable[i].Flag         = (int16)Header->Wave[i].Flag;
        SdEnvelopeTable[i].Count        = (int16)Header->Wave[i].Count;
        SdEnvelopeTable[i].Table        = (uint8*)Data + Header->Wave[i].Offset - (SD_SCT_SIZE * 3);
        SdEnvelopeTable[i].WaveOffset   = Header->Wave[i].WaveOffset;
        SdEnvelopeTable[i].WaveSize     = Header->Wave[i].WaveSize;
    };
};


bool SdLoaderInit(const char* Host)
{
    if (std::strlen(Host) >= COUNT_OF(SdHost))
        return false;

    std::strcpy(SdHost, Host);

    bool IsValid = (SdHost[std::strlen(SdHost) - 1] == '\\');
    bool IsFreeSpace = ((COUNT_OF(SdHost) - std::strlen(SdHost)) >= 2);

    if (!IsValid && IsFreeSpace)
    {
        std::strncat(SdHost, "\\", (COUNT_OF(SdHost) - std::strlen(SdHost)) - 1);
        std::strncat(SdHost, "\0", (COUNT_OF(SdHost) - std::strlen(SdHost)) - 1);
        return true;
    };

    return false;
};


void SdLoaderTerm(void)
{
    SdLoaderCleanupPhase = true;
    
    SdTransReleaseAll();
    SdWaveReleaseAll();

    SdStrWorkAddr = nullptr;
    SdStrWorkSize = 0;
    SdVoxHdrAddr = nullptr;
    SdVoxHdrSize = 0;
    SdVagHdrAddr = nullptr;
    SdVagHdrSize = 0;
    SdStrMemAddr = nullptr;
    SdStrMemSize = 0;
    SdWaveAddr = nullptr;
    SdWavHdrMemSize = 0;
    SdSeqHdrMemSize = 0;

    if (SdHeapAddrBegin && SdHeapAllocFlag)
    {
        SdMemFree(SdHeapAddrBegin);
        SdHeapAddrBegin = nullptr;
        SdHeapAddrEnd = nullptr;
        SdHeapAllocFlag = false;
    };

    SdLoaderCleanupPhase = false;
};


bool SdGetLoadInfo(void)
{
    return ((SdWavLoadTaskCnt > 0) || (SdTransWavLoadTaskCnt > 0));
};


void SdSetHeap(void* _heap, size_t _size)
{
    if (!SdHeapAddrBegin)
    {
        SdHeapAddrBegin = _heap;
        SdHeapAddrEnd   = (uint8*)SdHeapAddrBegin + _size;
        SdHeapAllocFlag = false;
    };
};


void SdSetResources(void)
{
    if (!SdHeapAddrBegin)
    {
        const size_t SdHeapSize = (1024 * 1024) * 8; // 8 MB
        
        SdHeapAddrBegin = SdMemAlloc(SdHeapSize);
        SdHeapAddrEnd   = (uint8*)SdHeapAddrBegin + SdHeapSize;
        SdHeapAllocFlag = true;
    };

    SdStrWorkAddr   = SdHeapAddrBegin;
    SdVoxHdrAddr    = (uint8*)SdStrWorkAddr + SdStrWorkSize;
    SdVagHdrAddr    = (uint8*)SdVoxHdrAddr + SdVoxHdrSize;
    SdStrMemAddr    = (uint8*)SdVoxHdrAddr + SdVoxHdrSize + SdVagHdrSize;
    SdWaveAddr      = (uint8*)SdStrMemAddr + SdStrMemSize + SdSeqHdrMemSize + SdWavHdrMemSize;
};


void SdMakeFilename(char* _buff, const char* _filename)
{
    _buff[0] = '\0';
    
    std::strcpy(_buff, SdHost);
    std::strcat(_buff, _filename);
};


void SdStreamLoad(void)
{
    //
    //  UPDATE RESOURCE MEMORY
    //  NOTE: BGM AND VOICE STREAMS SHOULD BE INITIALIZED AT THIS MOMENT
    //
    SdSetResources();

    //
    //  LOAD VOICE STEP
    //
    void* VoxLoadAddr = SdVoxHdrAddr;
    
    for (int32 i = 0; i < SdVoxLoadNum; ++i)
    {
        if (SdLoadVoxBank[i] == -1)
            continue;

        char Filename[256];
        Filename[0] = '\0';
        
        SdGetVoiceFilename(Filename, i);

        if (SdLoadTaskSync(Filename, VoxLoadAddr, SD_SCT_SIZE, 0))
        {
            SdStrHeader_t* StrHdr = (SdStrHeader_t*)VoxLoadAddr;
            
            SdVoxHdrSize += StrHdr->Size;

            SdLoadVox[i].Size = StrHdr->Size;
            SdLoadVox[i].LSN = (uint32)((uint8*)StrHdr + sizeof(*StrHdr));

            if (StrHdr->Size > SD_SCT_SIZE)
                SdLoadTaskSync(Filename, VoxLoadAddr, StrHdr->Size, 0);

            if (StrHdr->Count)
            {
                SdStrFile_t* StrFile = (SdStrFile_t*)(StrHdr + 1);
                SdDmaInfo_t* StrDmaInfo = (SdDmaInfo_t*)(((uint8*)(StrFile + StrHdr->Count)) + 4);
                
                StrDmaInfo->Page = SD_VOX_DMA_PAGE;
                StrDmaInfo->Block = SD_VOX_DMA_BLOCK;

                SdDmaInfoVox[i].Page = StrDmaInfo->Page;
                SdDmaInfoVox[i].Block = StrDmaInfo->Block;

                if (StrDmaInfo->Page > SdVoxMemSize)
                    SdVoxMemSize = StrDmaInfo->Page;
            };

            VoxLoadAddr = ((uint8*)VoxLoadAddr + StrHdr->Size);
        }
        else
        {
            SdLoadVox[i].Size = 0;
            SdLoadVox[i].LSN = 0;
        };
    };

    SdSetResources();
    
    //
    //  LOAD BGM STEP
    //
    void* VagLoadAddr = SdVagHdrAddr;
    
    for (int32 i = 0; i < SdVagLoadNum; ++i)
    {
        char Filename[256];
        Filename[0] = '\0';

        SdGetBgmFilename(Filename, i);

        if (SdLoadTaskSync(Filename, VagLoadAddr, SD_SCT_SIZE, 0))
        {
            SdStrHeader_t* StrHdr = (SdStrHeader_t*)VagLoadAddr;

            SdVagHdrSize += StrHdr->Size;

            SdLoadVag[i].Size = StrHdr->Size;
            SdLoadVag[i].LSN = (uint32)((uint8*)StrHdr + sizeof(*StrHdr));

            if (StrHdr->Size > SD_SCT_SIZE)
                SdLoadTaskSync(Filename, VagLoadAddr, StrHdr->Size, 0);

            if (StrHdr->Count)
            {
                SdStrFile_t* StrFile = (SdStrFile_t*)(StrHdr + 1);
                SdDmaInfo_t* StrDmaInfo = (SdDmaInfo_t*)(((uint8*)(StrFile + StrHdr->Count)) + 4);

                SdDmaInfoVag[i].Page = StrDmaInfo->Page;
                SdDmaInfoVag[i].Block = StrDmaInfo->Block;

                if (StrDmaInfo->Page > SdVagMemSize)
                    SdVagMemSize = StrDmaInfo->Page;
            };

            VagLoadAddr = ((uint8*)VagLoadAddr + StrHdr->Size);
        }
        else
        {
            SdLoadVag[i].Size = 0;
            SdLoadVag[i].LSN = 0;
        };
    };

    SdSetResources();

    //
    //  RESERVE MEMORY FOR STREAM
    //
    int32 VoxStrNum = SdStrGetServerNum(SDSTR_TYPE_VOX);
    int32 VagStrNum = SdStrGetServerNum(SDSTR_TYPE_VAG);
    
    for (int32 i = 0; i < VoxStrNum; ++i)
        SdStrMemSize += SdVoxMemSize;

    for (int32 i = 0; i < VagStrNum; ++i)
        SdStrMemSize += SdVagMemSize;

    SdSetResources();
};


void SdSystemDataLoadInfo(void)
{
    if (SdSeqHdrMemSize)
        return;

    if (SdWavHdrMemSize)
        return;

    char Filename[SD_FILENAME_LEN];
    Filename[0] = '\0';

    //
    //  load TRANS SE DATA
    //
    void* Buff = SdMemAlloc(SD_SCT_SIZE);
    if (Buff)
    {
        SdMakeFilename(Filename, SdGetSystemName(SD_LOADER_SYSTEM_TRSE));

        if (SdLoadTaskSync(Filename, Buff, SD_SCT_SIZE, 0))
            SdSetTransInfo((SdBgmHeader_t*)Buff);

        SdMemFree(Buff);
        Buff = nullptr;        
    };

    //
    //  load SE DATA
    //
    Buff = SdMemAlloc(SD_SCT_SIZE);
    if (Buff)
    {
        SdMakeFilename(Filename, SdGetSystemName(SD_LOADER_SYSTEM_SE));

        if (SdLoadTaskSync(Filename, Buff, SD_SCT_SIZE, 0))
        {
            uint32 SeDataSize = ((SdSeHeader_t*)Buff)->TotalSize - SD_SCT_SIZE;
            void* SeDataAddr = SdWaveAddr;            
            uint32 SeDataOffset = SD_SCT_SIZE;

            SdSeqHdrMemSize = SeDataSize;
            
            if (((void*)(uint32(SeDataAddr) + SeDataSize)) < SdHeapAddrEnd)
            {
                SdSetResources();
                
                if (SdLoadTaskSync(Filename, SeDataAddr, SeDataSize, SeDataOffset))
                    SdSetSeInfo((SdSeHeader_t*)Buff, SeDataAddr);
            };
        };
        
        SdMemFree(Buff);
        Buff = nullptr;
    };

    //
    //  load WAVE DATA
    //
    Buff = SdMemAlloc(SD_SCT_SIZE * 3);
    if (Buff)
    {
        SdMakeFilename(Filename, SdGetSystemName(SD_LOADER_SYSTEM_WAVE));

        if (SdLoadTaskSync(Filename, Buff, SD_SCT_SIZE * 3, 0))
        {
            uint32 WavDataSize = ((SdEnvelopeHeader_t*)Buff)->EnvSize;
            void* WavDataAddr = SdWaveAddr;
            uint32 WavDataOffset = SD_SCT_SIZE * 3;
            
            SdWavHdrMemSize = WavDataSize;

            if (((void*)(uint32(WavDataAddr) + WavDataSize)) < SdHeapAddrEnd)
            {
                SdSetResources();

                if (SdLoadTaskSync(Filename, WavDataAddr, WavDataSize, WavDataOffset))
                    SdSetWaveInfo((SdEnvelopeHeader_t*)Buff, WavDataAddr);
            };
        };

        SdMemFree(Buff);
        Buff = nullptr;
    };
};


void SdTransLoad(int32 _bank)
{
    SdTransRelease(_bank);

    uint32 Size = SdGetTransWaveSize(_bank);
    if (!Size)
    {
        SDLOG("TRANS WAVE LOAD FAILED: ZERO SIZE!!\n");
        return;
    };
    
    void* Address = SdMemAlloc(ALIGN_ROUND_UP(SdTransCodeTable[_bank].Size, SD_SCT_SIZE));
    if (!Address)
    {
        SDLOG("TRANS WAVE LOAD FAILED: ALLOC FAILED!!!\n");
        return;
    };

    char Filename[SD_FILENAME_LEN];
    Filename[0] = '\0';

    SdMakeFilename(Filename, SdGetSystemName(SD_LOADER_SYSTEM_TRSE));

    SdTransWavLoadTaskBusyFlag = true;

    //
    //  Alloc loader task
    //
    int32 TaskIndex = 0;

    while (SdTransWavLoadTask[TaskIndex])
    {
        ++TaskIndex;

        if (TaskIndex >= COUNT_OF(SdTransWavLoadTask))
        {
            SDLOG("TRANS WAVE LOAD FAILED: NO SPACE FOR LOAD TASK!!\n");            
            return;
        };
    };

    //
    //  Regist async load task
    //
    void* hTask = SdLoadTaskAsync(Filename, Address, Size, SdTransCodeTable[_bank].Offset);
    if (hTask)
    {
        SdLoadTaskSetUser(hTask, (void*)_bank);

        SdTransWavLoadTask[TaskIndex] = hTask;
        SdTransCodeTable[_bank].Table = (SdKdtTableData_t*)Address;

        ++SdTransWavLoadTaskCnt;
    }
    else
    {
        SDLOG("TRANS WAVE LOAD FAILED: ASYNC TASK IS NULL!!\n");
    };

    SdTransWavLoadTaskBusyFlag = false;
};


void SdTransRelease(int32 _bank)
{
    if (_bank >= COUNT_OF(SdTransCodeTable))
        return;

    if (!SdTransCodeTable[_bank].Count)
        return;

    if (SdTransCodeTable[_bank].Table)
    {
        SdMemFree(SdTransCodeTable[_bank].Table);
        SdTransCodeTable[_bank].Table = nullptr;
    };
};


void SdTransReleaseAll(void)
{
    int32 BaseIndex = (SdLoaderCleanupPhase ? 0 : 1);
    
    for (int32 i = BaseIndex; i < COUNT_OF(SdTransCodeTable); ++i)
        SdTransRelease(i);
};


void SdTransLoadTask(void)
{
    if (SdTransWavLoadTaskBusyFlag)
        return;

    for (int32 i = 0; i < COUNT_OF(SdTransWavLoadTask); ++i)
    {
        void** hTask = &SdTransWavLoadTask[i];
        
        if (!*hTask)
            continue;

        if (!SdLoadTaskIsEnd(*hTask))
            continue;

        SdTaskTransWavOnLoad(*hTask);

        *hTask = nullptr;
        
        ASSERT(SdTransWavLoadTaskCnt > 0);
        --SdTransWavLoadTaskCnt;
    };
};


void SdWaveLoad(int32 _bank, uint32 _address)
{
    int32 Size = SdGetWaveSize(_bank);
    if (!Size)
    {
        SDLOG("WAVE LOAD FAILED: ZERO SIZE!!!\n");
        return;
    };

    if ((_address + Size) > (uint32)SdHeapAddrEnd)
    {
        SDLOG("WAVE LOAD FAILED: NO HEAP SPACE!!\n");
        return;
    };

    char Filename[SD_FILENAME_LEN];
    Filename[0] = '\0';

    SdMakeFilename(Filename, SdGetSystemName(SD_LOADER_SYSTEM_WAVE));

    SdWavLoadTaskBusyFlag = true;

    //
    //  Alloc loader task
    //
    int32 TaskIndex = 0;

    while (SdWavLoadTask[TaskIndex])
    {
        ++TaskIndex;

        if (TaskIndex >= COUNT_OF(SdWavLoadTask))
        {
            SDLOG("WAVE LOAD FAILED: NO SPACE FOR LOAD TASK!!\n");
            return;
        };
    };

    //
    //  Regist async load task
    //
    void* hTask = SdLoadTaskAsync(Filename, (void*)_address, Size, SdEnvelopeTable[_bank].WaveOffset);
    if (hTask)
    {
        SdWavLoadTask[TaskIndex] = hTask;
        SdWaveTopAddress[_bank] = (void*)_address;

        ++SdWavLoadTaskCnt;
    }
    else
    {
        SDLOG("WAVE LOAD FAILED: ASYNC TASK IS NULL!!\n");
    };

    SdWavLoadTaskBusyFlag = false;
};


void SdWaveRelease(int32 Bank)
{
    if (Bank >= COUNT_OF(SdEnvelopeTable))
        return;

    if (!SdEnvelopeTable[Bank].Count)
        return;

    SdWaveTopAddress[Bank] = nullptr;
};


void SdWaveReleaseAll(void)
{
    int32 BaseIndex = (SdLoaderCleanupPhase ? 0 : 1);

    for (int32 i = BaseIndex; i < COUNT_OF(SdEnvelopeTable); ++i)
        SdWaveRelease(i);
};


void SdWaveLoadTask(void)
{
    if (SdWavLoadTaskBusyFlag)
        return;

    for (int32 i = 0; i < COUNT_OF(SdWavLoadTask); ++i)
    {
        void** hTask = &SdWavLoadTask[i];

        if (!*hTask)
            continue;

        if (!SdLoadTaskIsEnd(*hTask))
            continue;

        SdTaskWavOnLoad(*hTask);

        *hTask = nullptr;

        ASSERT(SdWavLoadTaskCnt > 0);
        --SdWavLoadTaskCnt;
    };
};


void* SdGetStrWorkAddr(void)
{
    return SdStrWorkAddr;
};


void SdStrWorkMemReserve(uint32 _size)
{
    SdStrWorkSize += _size;
    SdSetResources();
};


void* SdGetVoxHdrAddr(void)
{
    return SdVoxHdrAddr;
};


void* SdGetVagHdrAddr(void)
{
    return SdVagHdrAddr;
};


void* SdGetStrMemAddr(void)
{
    return SdStrMemAddr;
};


uint32 SdGetStrVoxMemSize(void)
{
    return SdVoxMemSize;
};


uint32 SdGetStrVagMemSize(void)
{
    return SdVagMemSize;
};


void* SdGetWaveAddr(void)
{
    return SdWaveAddr;
};


uint32 SdGetWaveSize(int32 _bank)
{
    ASSERT(_bank >= 0);
    ASSERT(_bank < COUNT_OF(SdEnvelopeTable));

    if (!(_bank >= 0) && (_bank < COUNT_OF(SdEnvelopeTable)))
        return 0;

    return (uint32)(SdEnvelopeTable[_bank].Count ? ALIGN_ROUND_UP(SdEnvelopeTable[_bank].WaveSize, SD_SCT_SIZE) : 0);        
};


uint32 SdGetTransWaveSize(int32 _bank)
{
    ASSERT(_bank >= 0);
    ASSERT(_bank < COUNT_OF(SdTransCodeTable));

    if (!(_bank >= 0) && (_bank < COUNT_OF(SdTransCodeTable)))
        return 0;

    return (uint32)(SdTransCodeTable[_bank].Count ? ALIGN_ROUND_UP(SdTransCodeTable[_bank].Size, SD_SCT_SIZE) : 0);
};


uint32 SdGetVoicePosition(int32 _code)
{
    return SdGetVoxFileLSN(_code)->Position;
};


uint32 SdGetVoiceSize(int32 _code)
{
    return SdGetVoxFileLSN(_code)->Size;
};


uint32 SdGetVoiceDmaPage(int32 _code)
{
    return SdDmaInfoVox[SD_VOX_BANK_NO(_code)].Page;
};


uint32 SdGetVoiceDmaBlock(int32 _code)
{
    return SdDmaInfoVox[SD_VOX_BANK_NO(_code)].Block;
};


void SdGetVoiceFilename(char* _buff, int32 _no)
{
    ASSERT(_no >= 0);
    ASSERT(_no < SdVoxLoadNum);

    SdMakeFilename(_buff, SdLoadVox[_no].Name);
};


uint32 SdGetBgmPosition(int32 _code)
{
    return SdGetVagFileLSN(_code)->Position;
};


uint32 SdGetBgmSize(int32 _code)
{
    return SdGetVagFileLSN(_code)->Size;
};


uint32 SdGetBgmDmaPage(int32 _code)
{
    return SdDmaInfoVag[SD_VAG_BANK_NO(_code)].Page;
};


uint32 SdGetBgmDmaBlock(int32 _code)
{
    return SdDmaInfoVag[SD_VAG_BANK_NO(_code)].Block;
};


void SdGetBgmFilename(char* _buff, int32 _no)
{
    ASSERT(_no >= 0);
    ASSERT(_no < SdVagLoadNum);

    SdMakeFilename(_buff, SdLoadVag[_no].Name);
};