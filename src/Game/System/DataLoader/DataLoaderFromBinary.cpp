#include "DataLoaderImpl.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/GameText.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/MotionParameter/MotionParameterManager.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Misc/UVAnim.hpp"
#include "Game/System/Map/CameraDataMan.hpp"
#include "Game/System/Map/MapClumpModelMan.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/Component/Gimmick/GimmickData.hpp"
#include "System/Common/SystemText.hpp"
#include "System/Common/Font.hpp"


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


class IItemReader;


class CDataLoaderFromBinary final : public IDataLoaderImpl
{
public:
    static const uint32 CHUNK_ALIGNMENT_SIZE = 64;
    
    class CFileHeader
    {
    public:        
        static const uint32 MAGIC = 'CAPL'; // LPAC
        
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

public:
    CDataLoaderFromBinary(void);
    virtual ~CDataLoaderFromBinary(void);
    virtual void Eval(const void* pBuffer, uint32 uBufferSize) override;

private:
    IItemReader* m_apItemReader[READERTYPE::TYPE_MAX];
};


class IItemReader
{
public:
    virtual void Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData) = 0;
    virtual const char* GetLabel(void) const = 0;
    virtual READERTYPE::VALUE GetType(void) const = 0;
};


#define DECLARE_READER(name)                                                                        \
    class name final : public IItemReader                                                           \
    {                                                                                               \
    public:                                                                                         \
        virtual void Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData) override; \
        virtual const char* GetLabel(void) const override;                                          \
        virtual READERTYPE::VALUE GetType(void) const override;                                     \
    };


DECLARE_READER(CItemModelReader);
DECLARE_READER(CItemModelToonReader);
DECLARE_READER(CItemMotionSetReader);
DECLARE_READER(CItemMotionReader);
DECLARE_READER(CItemMotionParameterReader);
DECLARE_READER(CItemTextureDictionaryReader);
DECLARE_READER(CItemAddTextureDictionaryReader);
DECLARE_READER(CItemModelBspReader);
DECLARE_READER(CItemCameraReader);
DECLARE_READER(CItemEffectReader);
DECLARE_READER(CItemAnimation2DReader);
DECLARE_READER(CItemUVAnimReader);
DECLARE_READER(CItemFontReader);
DECLARE_READER(CItemMapModelReader);
DECLARE_READER(CItemMapInfoReader);
DECLARE_READER(CItemGimmickReader);
DECLARE_READER(CItemStringsReader);
DECLARE_READER(CItemEnemyParameterReader);
DECLARE_READER(CItemIconsPS2Reader);
DECLARE_READER(CItemIconsNGCReader);


IDataLoaderImpl* CreateDataLoaderImplement(void)
{
    return new CDataLoaderFromBinary;
};


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


void CDataLoaderFromBinary::Eval(const void* pBuffer, uint32 uBufferSize)
{
#define ALIGN(value, align)         (uint32)(uint32(value) + (align - 1) & ~(align - 1))
#define ADJUST(value, align)        (uint32)(align - (uint32(value) & (align - 1)))
#define IS_ALIGNED(value, align)    ((uint32(value) & (align - 1)) == 0)
    
    CFileHeader* pFileHeader = (CFileHeader*)pBuffer;
    pBuffer = ((uint8*)pBuffer + ALIGN(sizeof(CFileHeader), CHUNK_ALIGNMENT_SIZE));

    for (uint32 i = 0; i < pFileHeader->m_uChunkNum; ++i)
    {
        CChunkHeader ChunkHeader;
        ChunkHeader.m_pHeader = (CChunkHeader::HEADER*)pBuffer;
        ChunkHeader.m_pExtraHeader = (CChunkHeader::HEADER*)((uint8*)pBuffer + sizeof(CChunkHeader::HEADER));

        pBuffer = ((uint8*)pBuffer + (sizeof(CChunkHeader::HEADER) * 2));
        
        for (uint32 j = 0; j < COUNT_OF(m_apItemReader); ++j)
        {
            if (m_apItemReader[j])
            {
                if (m_apItemReader[j]->GetType() == ChunkHeader.m_pHeader->m_uType)
                {
                    OUTPUT(
                        "[GAME] DataLoaderFromBinary: reading file %-20s\t\t(%-20s -- 0x%x)\n",
						ChunkHeader.m_pHeader->m_szName,
                        m_apItemReader[j]->GetLabel(),
                        m_apItemReader[j]->GetType()
                    );

                    m_apItemReader[j]->Eval(&ChunkHeader, (void*)pBuffer);
                };
            };
        };

        uint32 uChunkSize = ChunkHeader.m_pHeader->m_uSize;
        
        if (!IS_ALIGNED(uChunkSize, CHUNK_ALIGNMENT_SIZE))
            uChunkSize += ADJUST(uChunkSize, CHUNK_ALIGNMENT_SIZE);

        ASSERT(IS_ALIGNED(uChunkSize, CHUNK_ALIGNMENT_SIZE));
        pBuffer = ((uint8*)pBuffer + uChunkSize);
    };
};


/**
 ********************************************************************************
 */


void CItemModelReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    struct MDLHEADER
    {
        char m_szTextureSetName[16];
    };

    MDLHEADER* pMdlHeader = (MDLHEADER*)pChunkHeader->m_pExtraHeader;

    CModelManager::ReadNormal(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize,
        pMdlHeader->m_szTextureSetName
    );
};


const char* CItemModelReader::GetLabel(void) const
{
    return "MODEL";
};


READERTYPE::VALUE CItemModelReader::GetType(void) const
{
    return READERTYPE::MODEL;
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

    MDLTOONHEADER* pMdlToonHeader = (MDLTOONHEADER*)pChunkHeader->m_pExtraHeader;

    CModelManager::ReadToon(
        pChunkHeader->m_pHeader->m_szName,
        pData,
        pChunkHeader->m_pHeader->m_uSize,
        pMdlToonHeader->m_szTextureSetName,
        pMdlToonHeader->m_pattern
    );
};


const char* CItemModelToonReader::GetLabel(void) const
{
    return "MODEL_TOON";
};


READERTYPE::VALUE CItemModelToonReader::GetType(void) const
{
    return READERTYPE::MODEL_TOON;
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

    MOTIONSETHEADER* pMotionSetHeader = (MOTIONSETHEADER*)pChunkHeader->m_pExtraHeader;

    if (pMotionSetHeader->m_bSetName)
    {
        CMotionManager::CreateMotionSet(
            pChunkHeader->m_pHeader->m_szName,
            pMotionSetHeader->m_szSetName
        );
    }
    else
    {
        CMotionManager::CreateMotionSet(
            pChunkHeader->m_pHeader->m_szName,
            CMotionManager::GLOBAL_SET
        );
    };
};


const char* CItemMotionSetReader::GetLabel(void) const
{
    return "MOTION_SET";
};


READERTYPE::VALUE CItemMotionSetReader::GetType(void) const
{
    return READERTYPE::MOTION_SET;
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


const char* CItemMotionReader::GetLabel(void) const
{
    return "MOTION";
};


READERTYPE::VALUE CItemMotionReader::GetType(void) const
{
    return READERTYPE::MOTION;
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


const char* CItemMotionParameterReader::GetLabel(void) const
{
    return "MOTION_PARAMETER";
};


READERTYPE::VALUE CItemMotionParameterReader::GetType(void) const
{
    return READERTYPE::MOTION_PARAMETER;
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


const char* CItemTextureDictionaryReader::GetLabel(void) const
{
    return "TEXTURE_DICTIONARY";
};


READERTYPE::VALUE CItemTextureDictionaryReader::GetType(void) const
{
    return READERTYPE::TEXTURE_DICTIONARY;
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


const char* CItemAddTextureDictionaryReader::GetLabel(void) const
{
    return "ADD_TEXTURE_DICTIONARY";
};


READERTYPE::VALUE CItemAddTextureDictionaryReader::GetType(void) const
{
    return READERTYPE::ADD_TEXTURE_DICTIONARY;
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


const char* CItemModelBspReader::GetLabel(void) const
{
    return "MODEL_BSP";
};


READERTYPE::VALUE CItemModelBspReader::GetType(void) const
{
    return READERTYPE::MODEL_BSP;
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


const char* CItemCameraReader::GetLabel(void) const
{
    return "CAMERA";
};


READERTYPE::VALUE CItemCameraReader::GetType(void) const
{
    return READERTYPE::CAMERA;
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


const char* CItemEffectReader::GetLabel(void) const
{
    return "EFFECT";
};


READERTYPE::VALUE CItemEffectReader::GetType(void) const
{
    return READERTYPE::EFFECT;
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


const char* CItemAnimation2DReader::GetLabel(void) const
{
    return "ANIM2D";
};


READERTYPE::VALUE CItemAnimation2DReader::GetType(void) const
{
    return READERTYPE::ANIM2D;
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


const char* CItemUVAnimReader::GetLabel(void) const
{
    return "UVANIM";
};


READERTYPE::VALUE CItemUVAnimReader::GetType(void) const
{
    return READERTYPE::UVANIM;
};


/**
 ********************************************************************************
 */


void CItemFontReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
	CGameFont::Open(
		pChunkHeader->m_pHeader->m_szName,
		(const char*)pData,
		pChunkHeader->m_pHeader->m_szName
	);
};


const char* CItemFontReader::GetLabel(void) const
{
    return "FONT";
};


READERTYPE::VALUE CItemFontReader::GetType(void) const
{
    return READERTYPE::FONT;
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

    MAPMDLHEADER* pMapMdlHeader = (MAPMDLHEADER*)pChunkHeader->m_pExtraHeader;

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


const char* CItemMapModelReader::GetLabel(void) const
{
    return "MAP_MODEL";
};


READERTYPE::VALUE CItemMapModelReader::GetType(void) const
{
    return READERTYPE::MAP_MODEL;
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


const char* CItemMapInfoReader::GetLabel(void) const
{
    return "MAP_INFO";
};


READERTYPE::VALUE CItemMapInfoReader::GetType(void) const
{
    return READERTYPE::MAP_INFO;
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


const char* CItemGimmickReader::GetLabel(void) const
{
    return "GIMMICK";
};


READERTYPE::VALUE CItemGimmickReader::GetType(void) const
{
    return READERTYPE::GIMMICK;
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


const char* CItemStringsReader::GetLabel(void) const
{
    return "STRINGS";
};


READERTYPE::VALUE CItemStringsReader::GetType(void) const
{
    return READERTYPE::STRINGS;
};


/**
 ********************************************************************************
 */


void CItemEnemyParameterReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    ;
};


const char* CItemEnemyParameterReader::GetLabel(void) const
{
    return "ENEMY_PARAMETER";
};


READERTYPE::VALUE CItemEnemyParameterReader::GetType(void) const
{
    return READERTYPE::ENEMY_PARAMETER;
};


/**
 ********************************************************************************
 */


void CItemIconsPS2Reader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    ;
};


const char* CItemIconsPS2Reader::GetLabel(void) const
{
    return "ICONS_PS2";
};


READERTYPE::VALUE CItemIconsPS2Reader::GetType(void) const
{
    return READERTYPE::ICONS_PS2;
};


/**
 ********************************************************************************
 */


void CItemIconsNGCReader::Eval(CDataLoaderFromBinary::CChunkHeader* pChunkHeader, void* pData)
{
    ;
};


const char* CItemIconsNGCReader::GetLabel(void) const
{
    return "ICONS_NGC";
};


READERTYPE::VALUE CItemIconsNGCReader::GetType(void) const
{
    return READERTYPE::ICONS_NGC;
};
