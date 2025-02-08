#include "DataLoaderImpl.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Gimmick/GimmickData.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/MotionParameter/MotionParameterManager.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Misc/UVAnim.hpp"
#include "Game/System/Map/CameraDataMan.hpp"
#include "Game/System/Map/MapClumpModelMan.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "System/Common/SystemText.hpp"
#include "System/Common/Font.hpp"


#define CHUNK_ALIGNMENT_SIZE    (64)


namespace READERTYPE
{
    enum VALUE
    {
        MODEL = 0,
        MODEL_TOON,
        MOTION_SET,
        MOTION,
        MOTION_PARAMETER,

        TEXTURE_DICTIONARY = 7,
        MODEL_BSP,
        CAMERA,
        EFFECT,
        ANIM2D,
        UVANIM,
        FONT,
        MAP_MODEL,
        MAP_INFO,
        ADD_TEXTURE_DICTIONARY,
        GIMMICK,
        STRINGS,
        ENEMY_PARAMETER,
        ICONS_PS2,
        ICONS_NGC,

        TYPE_MAX,
    };
};


class CDataLoaderFromBinary : public IDataLoaderImpl
{
public:
    class CFileHeader
    {
    public:
        uint32 m_uMagic;
        uint32 m_uChunkNum;
    };

    class CChunkHeader
    {
    public:
        struct HEADER
        {
            uint32 m_uType;
            uint32 m_uSize;
            uint32 m_uReserved[2];
            char m_szName[16];
        };

    public:
        HEADER* m_pHeader;
        void* m_pExtraHeader;
    };

    class IItemReader
    {
    public:
        virtual ~IItemReader(void) {};
        virtual void Eval(CChunkHeader* pChunkHeader, void* pData) = 0;
        virtual const char* GetLabel(void) const = 0;
        virtual READERTYPE::VALUE GetType(void) const = 0;
    };

public:
    CDataLoaderFromBinary(void);
    virtual ~CDataLoaderFromBinary(void);
    virtual void Eval(void* pBufferOrg, uint32 sizeBuffer) override;

private:
    IItemReader* m_apItemReader[READERTYPE::TYPE_MAX];
};


#define DECLARE_READER(name, label, type)                                                           \
    class name final : public CDataLoaderFromBinary::IItemReader                                    \
    {                                                                                               \
    public:                                                                                         \
        virtual void Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData) override; \
        virtual const char* GetLabel(void) const override { return label; };                        \
        virtual READERTYPE::VALUE GetType(void) const override { return type; };                    \
    };


DECLARE_READER(CItemModelReader,                "MODEL",                    READERTYPE::MODEL);
DECLARE_READER(CItemModelToonReader,            "MODEL_TOON",               READERTYPE::MODEL_TOON);
DECLARE_READER(CItemMotionSetReader,            "MOTION_SET",               READERTYPE::MOTION_SET );
DECLARE_READER(CItemMotionReader,               "MOTION",                   READERTYPE::MOTION);
DECLARE_READER(CItemMotionParameterReader,      "MOTION_PARAMETER",         READERTYPE::MOTION_PARAMETER);
DECLARE_READER(CItemTextureDictionaryReader,    "TEXTURE_DICTIONARY",       READERTYPE::TEXTURE_DICTIONARY);
DECLARE_READER(CItemAddTextureDictionaryReader, "ADD_TEXTURE_DICTIONARY",   READERTYPE::ADD_TEXTURE_DICTIONARY);
DECLARE_READER(CItemModelBspReader,             "MODEL_BSP",                READERTYPE::MODEL_BSP);
DECLARE_READER(CItemCameraReader,               "CAMERA",                   READERTYPE::CAMERA);
DECLARE_READER(CItemEffectReader,               "EFFECT",                   READERTYPE::EFFECT);
DECLARE_READER(CItemAnimation2DReader,          "ANIM2D",                   READERTYPE::ANIM2D);
DECLARE_READER(CItemUVAnimReader,               "UVANIM",                   READERTYPE::UVANIM);
DECLARE_READER(CItemFontReader,                 "FONT",                     READERTYPE::FONT);
DECLARE_READER(CItemMapModelReader,             "MAP_MODEL",                READERTYPE::MAP_MODEL);
DECLARE_READER(CItemMapInfoReader,              "MAP_INFO",                 READERTYPE::MAP_INFO);
DECLARE_READER(CItemGimmickReader,              "GIMMICK",                  READERTYPE::GIMMICK);
DECLARE_READER(CItemStringsReader,              "STRINGS",                  READERTYPE::STRINGS);
DECLARE_READER(CItemEnemyParameterReader,       "ENEMY_PARAMETER",          READERTYPE::ENEMY_PARAMETER);
DECLARE_READER(CItemIconsPS2Reader,             "ICONS_PS2",                READERTYPE::ICONS_PS2);
DECLARE_READER(CItemIconsNGCReader,             "ICONS_NGC",                READERTYPE::ICONS_NGC);


/**
 ********************************************************************************
 */


void CItemModelReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    struct MDLHEADER
    {
        char m_szTextureSetName[16];
    };

    MDLHEADER* pMdlHeader = static_cast<MDLHEADER*>(pChunkHeader->m_pExtraHeader);

    CModelManager::ReadNormal(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize,
        pMdlHeader->m_szTextureSetName
    );
};


/**
 ********************************************************************************
 */


void CItemModelToonReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    struct MDLTOONHEADER
    {
        char m_szTextureSetName[16];
        int32 m_pattern;
    };

    MDLTOONHEADER* pMdlToonHeader = static_cast<MDLTOONHEADER*>(pChunkHeader->m_pExtraHeader);

    CModelManager::ReadToon(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize,
        pMdlToonHeader->m_szTextureSetName,
        pMdlToonHeader->m_pattern
    );
};


/**
 ********************************************************************************
 */


void CItemMotionSetReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    struct MOTIONSETHEADER
    {
        uint32 m_bSetName;
        char m_szSetName[16];
    };

    MOTIONSETHEADER* pMotionSetHeader = static_cast<MOTIONSETHEADER*>(pChunkHeader->m_pExtraHeader);

    CMotionManager::CreateMotionSet(
        pChunkHeader->m_pHeader->m_szName,
        (pMotionSetHeader->m_bSetName ? pMotionSetHeader->m_szSetName : CMotionManager::GLOBAL_SET)
    );
};


/**
 ********************************************************************************
 */


void CItemMotionReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CMotionManager::Read(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemMotionParameterReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CMotionParameterManager::Read(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemTextureDictionaryReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CTextureManager::ReadSet(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemAddTextureDictionaryReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CTextureManager::AddSet(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemModelBspReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CWorldMap::ReadBsp(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemCameraReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CCameraDataManager::Read(
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemEffectReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CEffectManager::Read(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemAnimation2DReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CAnimation2DLoader::Open(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemUVAnimReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CUVAnim::ReadUVAnim(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemFontReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
	CGameFont::Open(
        pChunkHeader->m_pHeader->m_szName,
        reinterpret_cast<const char*>(pData),
		pChunkHeader->m_pHeader->m_szName
	);
};


/**
 ********************************************************************************
 */


void CItemMapModelReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    struct MAPMDLHEADER
    {
        char m_szTextureSetName[16];
        int32 m_mode;
    };

    MAPMDLHEADER* pMapMdlHeader = static_cast<MAPMDLHEADER*>(pChunkHeader->m_pExtraHeader);

    if (CUVAnim::IsExistsUVAnimDict(pChunkHeader->m_pHeader->m_szName))        
        CUVAnim::SetUVAnimDict(pChunkHeader->m_pHeader->m_szName);
    
    CModelManager::ReadNormal(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize,
        pMapMdlHeader->m_szTextureSetName
    );
    
    CMapClumpModelManager::Read(
        pChunkHeader->m_pHeader->m_szName,
        pMapMdlHeader->m_mode
    );
};


/**
 ********************************************************************************
 */


void CItemMapInfoReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CWorldMap::ReadMapInfo(
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemGimmickReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CGimmickDataManager::Read(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize
    );
};


/**
 ********************************************************************************
 */


void CItemStringsReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    if (!std::strcmp(pChunkHeader->m_pHeader->m_szName, "system"))
        CSystemText::Read(pData, pChunkHeader->m_pHeader->m_uSize);
    else
        CGameText::Read(pData, pChunkHeader->m_pHeader->m_uSize);
};


/**
 ********************************************************************************
 */


void CItemEnemyParameterReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    CEnemyParameter::Read(pData, pChunkHeader->m_pHeader->m_uSize);
};


/**
 ********************************************************************************
 */


void CItemIconsPS2Reader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    ;
};


/**
 ********************************************************************************
 */


void CItemIconsNGCReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    ;
};


/**
 ********************************************************************************
 */


CDataLoaderFromBinary::CDataLoaderFromBinary(void)
{
    std::memset(&m_apItemReader[0], 0x00, sizeof(m_apItemReader));

    m_apItemReader[READERTYPE::MODEL]                 = new CItemModelReader;
    m_apItemReader[READERTYPE::MODEL_TOON]            = new CItemModelToonReader;
    m_apItemReader[READERTYPE::MOTION_SET]            = new CItemMotionSetReader;
    m_apItemReader[READERTYPE::MOTION]                = new CItemMotionReader;
    m_apItemReader[READERTYPE::MOTION_PARAMETER]      = new CItemMotionParameterReader;
    m_apItemReader[READERTYPE::TEXTURE_DICTIONARY]    = new CItemTextureDictionaryReader;
    m_apItemReader[READERTYPE::MODEL_BSP]             = new CItemModelBspReader;
    m_apItemReader[READERTYPE::CAMERA]                = new CItemCameraReader;
    m_apItemReader[READERTYPE::EFFECT]                = new CItemEffectReader;
    m_apItemReader[READERTYPE::ANIM2D]                = new CItemAnimation2DReader;
    m_apItemReader[READERTYPE::UVANIM]                = new CItemUVAnimReader;
    m_apItemReader[READERTYPE::FONT]                  = new CItemFontReader;
    m_apItemReader[READERTYPE::MAP_MODEL]             = new CItemMapModelReader;
    m_apItemReader[READERTYPE::MAP_INFO]              = new CItemMapInfoReader;
    m_apItemReader[READERTYPE::ADD_TEXTURE_DICTIONARY]= new CItemAddTextureDictionaryReader;
    m_apItemReader[READERTYPE::GIMMICK]               = new CItemGimmickReader;
    m_apItemReader[READERTYPE::STRINGS]               = new CItemStringsReader;
    m_apItemReader[READERTYPE::ENEMY_PARAMETER]       = new CItemEnemyParameterReader;
    m_apItemReader[READERTYPE::ICONS_PS2]             = new CItemIconsPS2Reader;
    m_apItemReader[READERTYPE::ICONS_NGC]             = new CItemIconsNGCReader;
};


CDataLoaderFromBinary::~CDataLoaderFromBinary(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apItemReader); ++i)
    {
        if (m_apItemReader[i])
        {
            delete m_apItemReader[i];
            m_apItemReader[i] = nullptr;
        };
    };
};


void CDataLoaderFromBinary::Eval(void* pBufferOrg, uint32 sizeBuffer)
{
    uint8* pBuffer = static_cast<uint8*>(pBufferOrg)
                    + sizeof(CFileHeader)
                    + (CHUNK_ALIGNMENT_SIZE - (sizeof(CFileHeader) % CHUNK_ALIGNMENT_SIZE));

    uint32 uChunkNum = reinterpret_cast<CFileHeader*>(pBufferOrg)->m_uChunkNum;
    for (uint32 i = 0; i < uChunkNum; ++i)
    {
        CChunkHeader chunkHeader;
        chunkHeader.m_pHeader       = reinterpret_cast<CChunkHeader::HEADER*>(pBuffer);
        chunkHeader.m_pExtraHeader  = reinterpret_cast<CChunkHeader::HEADER*>(pBuffer + sizeof(CChunkHeader::HEADER));

        pBuffer += (sizeof(CChunkHeader::HEADER) * 2);

        for (uint32 j = 0; j < COUNT_OF(m_apItemReader); ++j)
        {
            if (!m_apItemReader[j])
                continue;

            if (m_apItemReader[j]->GetType() != static_cast<READERTYPE::VALUE>(chunkHeader.m_pHeader->m_uType))
                continue;

            //OUTPUT(
            //	"reading file %-20s\t\t(%-20s -- 0x%x)\n",
            //	ChunkHeader.m_pHeader->m_szName,
            //	m_apItemReader[j]->GetLabel(),
            //	m_apItemReader[j]->GetType()
            //);

            m_apItemReader[j]->Eval(&chunkHeader, pBuffer);
        };

        uint32 uChunkSize = chunkHeader.m_pHeader->m_uSize;

        if (uChunkSize % CHUNK_ALIGNMENT_SIZE)
            uChunkSize += (CHUNK_ALIGNMENT_SIZE - (uChunkSize % CHUNK_ALIGNMENT_SIZE));

        pBuffer += uChunkSize;

        ASSERT((uChunkSize % CHUNK_ALIGNMENT_SIZE) == 0);
        ASSERT((reinterpret_cast<uint32>(pBuffer) - reinterpret_cast<uint32>(pBufferOrg)) <= sizeBuffer);
        ASSERT(((reinterpret_cast<uint32>(pBuffer) - reinterpret_cast<uint32>(pBufferOrg)) % CHUNK_ALIGNMENT_SIZE) == 0);
    };
};


IDataLoaderImpl* CreateDataLoaderImplement(void)
{
    return new CDataLoaderFromBinary;
};