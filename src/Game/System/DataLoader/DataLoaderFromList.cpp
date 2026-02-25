#include "DataLoaderImpl.hpp"
#include "DataLoader.hpp"
#include "ListFile.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Gimmick/GimmickData.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/Map/CameraDataMan.hpp"
#include "Game/System/Map/MapClumpModelMan.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/MotionParameter/MotionParameterManager.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Movie/MovieText.hpp"
#include "Game/System/Misc/UVAnim.hpp"
#include "System/Common/SystemText.hpp"
#include "System/Common/Font.hpp"
#include "System/Common/File/Filename.hpp"
#include "System/Common/File/FileQueue.hpp"
#include "System/Common/File/FileTypes.hpp"


class CDataLoaderFromList : public IDataLoaderImpl
{
private:
    struct LISTFILE_INFO
    {
        char        szPath[256];
        CListFile*  pListFile;
        int32       iPathNo;
    };

public:
    CDataLoaderFromList(void);
    virtual ~CDataLoaderFromList(void);
    virtual void Eval(void* pBufferOrg, uint32 sizeBuffer, const void* fid) override;
    void ReadFile(void* pBufferOrg, uint32 sizeBuffer);    
    void ReadList(void* pBufferOrg, uint32 sizeBuffer);
    bool ReadNextListFile(void);
    void SetCurrentPath(const CListFile::CPath* pPath);
    void ReadModel(void* pBufferOrg, uint32 sizeBuffer);
    void ReadModelToon(void* pBufferOrg, uint32 sizeBuffer);
    void ReadMotionSet(void* pBufferOrg, uint32 sizeBuffer);
    void ReadMotion(void* pBufferOrg, uint32 sizeBuffer);
    void ReadMotionParameter(void* pBufferOrg, uint32 sizeBuffer);
    void ReadTextureDictionary(void* pBufferOrg, uint32 sizeBuffer);
    void ReadAddTextureDictionary(void* pBufferOrg, uint32 sizeBuffer);
    void ReadModelBsp(void* pBufferOrg, uint32 sizeBuffer);
    void ReadCamera(void* pBufferOrg, uint32 sizeBuffer);
    void ReadEffect(void* pBufferOrg, uint32 sizeBuffer);
    void ReadAnimation2D(void* pBufferOrg, uint32 sizeBuffer);
    void ReadUVAnim(void* pBufferOrg, uint32 sizeBuffer);
    void ReadFont(void* pBufferOrg, uint32 sizeBuffer);
    void ReadMapModel(void* pBufferOrg, uint32 sizeBuffer);
    void ReadMapInfo(void* pBufferOrg, uint32 sizeBuffer);
    void ReadGimmick(void* pBufferOrg, uint32 sizeBuffer);
    void ReadStrings(void* pBufferOrg, uint32 sizeBuffer);
    void ReadEnemyParameter(void* pBufferOrg, uint32 sizeBuffer);
    void ReadIconsPS2(void* pBufferOrg, uint32 sizeBuffer);
    void ReadIconsNGC(void* pBufferOrg, uint32 sizeBuffer);
    void ReadMovieText(void* pBufferOrg, uint32 sizeBuffer);

private:
    const char* m_pszCurrentFilePath;
    const char* m_pszCurrentFileName;
    const char* m_pszCurrentFileExt;
    LISTFILE_INFO m_listFileInfoStack[32];
    int32 m_iListFileInfoNo;
    const CListFile::CPath* m_pCurrentPath;
};


static inline const char* GetFilePathDir(const char* pszFilePath)
{
    static char s_szPath[256];
    std::strcpy(s_szPath, pszFilePath);
    CFilename::ConvPathPlatform(s_szPath);

    char* pszDir = std::strrchr(s_szPath, '\\');
    if (!pszDir)
        pszDir = std::strrchr(s_szPath, '/');
    
    pszDir = (pszDir ? ++pszDir : s_szPath);
    *pszDir = '\0';

    return s_szPath;
};


CDataLoaderFromList::CDataLoaderFromList(void)
: m_pszCurrentFilePath(nullptr)
, m_pszCurrentFileName(nullptr)
, m_pszCurrentFileExt(nullptr)
, m_listFileInfoStack()
, m_iListFileInfoNo(0)
, m_pCurrentPath(nullptr)
{
    ;
};


CDataLoaderFromList::~CDataLoaderFromList(void)
{
    ;
};


void CDataLoaderFromList::Eval(void* pBufferOrg, uint32 sizeBuffer, const void* fid)
{
    const char* pszPath = reinterpret_cast<const char*>(fid);

    m_pszCurrentFilePath = pszPath;

    if (!m_pCurrentPath)
    {
        static CListFile::CPath s_path;
        s_path.Parse(pszPath, std::strlen(pszPath));

        if (!s_path.IsValid())
            return;
        
        SetCurrentPath(&s_path);
    };

    ReadFile(pBufferOrg, sizeBuffer);
    ReadNextListFile();
};


void CDataLoaderFromList::ReadFile(void* pBufferOrg, uint32 sizeBuffer)
{
    struct FILE_LOAD_INFO
    {
        const char* apszFileExts[8];
        void (CDataLoaderFromList::*pfnLoader)(void* pBufferOrg, uint32 sizeBuffer);
    };

    static const FILE_LOAD_INFO s_aFileLoadInfo[] =
    {
        { { ".list"     },  &CDataLoaderFromList::ReadList                   },
        { { ".dff"      },  &CDataLoaderFromList::ReadModel                  },
        { { ".tdff"     },  &CDataLoaderFromList::ReadModelToon              },
        { { ".anm_set"  },  &CDataLoaderFromList::ReadMotionSet              },
        { { ".anm"      },  &CDataLoaderFromList::ReadMotion                 },
        { { ".chr"      },  &CDataLoaderFromList::ReadMotionParameter        },
        { { ".txd"      },  &CDataLoaderFromList::ReadTextureDictionary      },
        { { ".txd_add"  },  &CDataLoaderFromList::ReadAddTextureDictionary   },
        { { ".bsp"      },  &CDataLoaderFromList::ReadModelBsp               },
        { { ".cmd"      },  &CDataLoaderFromList::ReadCamera                 },
        { { ".ef"       },  &CDataLoaderFromList::ReadEffect                 },
        { { ".2d"       },  &CDataLoaderFromList::ReadAnimation2D            },
        { { ".uv"       },  &CDataLoaderFromList::ReadUVAnim                 },
        { { ".fnt"      },  &CDataLoaderFromList::ReadFont                   },
        { { ".mdff"     },  &CDataLoaderFromList::ReadMapModel               },
        { { ".mpd"      },  &CDataLoaderFromList::ReadMapInfo                },
        { { ".mgd"      },  &CDataLoaderFromList::ReadGimmick                },
        { { ".bin"      },  &CDataLoaderFromList::ReadStrings                },
        { { ".epb"      },  &CDataLoaderFromList::ReadEnemyParameter         },
        { { ".mtd"      },  &CDataLoaderFromList::ReadMovieText              },
    };

    for (int32 i = 0; i < COUNT_OF(s_aFileLoadInfo); ++i)
    {
        for (int32 j = 0; j < COUNT_OF(FILE_LOAD_INFO::apszFileExts); ++j)
        {
            if (s_aFileLoadInfo[i].apszFileExts[j] == nullptr)
                break;

            if (!std::strcmp(m_pszCurrentFileExt, s_aFileLoadInfo[i].apszFileExts[j]))
            {
                (this->*s_aFileLoadInfo[i].pfnLoader)(pBufferOrg, sizeBuffer);
                break;
            };
        };
    };
};


void CDataLoaderFromList::ReadList(void* pBufferOrg, uint32 sizeBuffer)
{
    CListFile* pList = new CListFile;
    pList->Parse(reinterpret_cast<const char*>(pBufferOrg), sizeBuffer);

    LISTFILE_INFO info;    
    std::strcpy(info.szPath, GetFilePathDir(m_pszCurrentFilePath));
    info.pListFile = pList;
    info.iPathNo = 0;

    m_listFileInfoStack[m_iListFileInfoNo++] = info;
};


bool CDataLoaderFromList::ReadNextListFile(void)
{
    if (!m_iListFileInfoNo)
        return false;

    m_pCurrentPath = nullptr;

    LISTFILE_INFO& info = m_listFileInfoStack[m_iListFileInfoNo - 1];
    CListFile* pList = info.pListFile;

    int32 iPathNum = pList->GetPathNum();
    for (int32 i = info.iPathNo; i < iPathNum; ++i)
    {
        const CListFile::CPath& path = pList->GetPath(i);
        if (path.IsValid())
        {
            info.iPathNo = i + 1;

            char szPath[256];
            std::strcpy(szPath, info.szPath);
            std::strcat(szPath, path.GetStr());

            CDataLoader::Regist(szPath, true);
            SetCurrentPath(&path);

            return true;
        };
    };    

    delete pList;
    m_listFileInfoStack[--m_iListFileInfoNo].pListFile = nullptr;

    return ReadNextListFile();
};


void CDataLoaderFromList::SetCurrentPath(const CListFile::CPath* pPath)
{
    m_pszCurrentFileName = pPath->GetFileName();
    m_pszCurrentFileExt = pPath->GetFileExt();
    m_pCurrentPath = pPath;
};


void CDataLoaderFromList::ReadModel(void* pBufferOrg, uint32 sizeBuffer)
{
    ASSERT(m_pCurrentPath->GetParamNum() == 1);

    const char* pszTxdSetName = m_pCurrentPath->GetParam(0).AsStr();

    CModelManager::ReadNormal(m_pszCurrentFileName, pBufferOrg, sizeBuffer, pszTxdSetName);
};


void CDataLoaderFromList::ReadModelToon(void* pBufferOrg, uint32 sizeBuffer)
{
    ASSERT(m_pCurrentPath->GetParamNum() == 2);

    const char* pszTxdSetName = m_pCurrentPath->GetParam(0).AsStr();
    int32       pattern = m_pCurrentPath->GetParam(1).AsInt();

    CModelManager::ReadToon(m_pszCurrentFileName, pBufferOrg, sizeBuffer, pszTxdSetName, pattern);
};


void CDataLoaderFromList::ReadMotionSet(void* pBufferOrg, uint32 sizeBuffer)
{
    const char* pszParentSetName = CMotionManager::GLOBAL_SET;

    if (m_pCurrentPath->GetParamNum() == 1)
        pszParentSetName = m_pCurrentPath->GetParam(0).AsStr();

    CMotionManager::CreateMotionSet(m_pszCurrentFileName, pszParentSetName);
};


void CDataLoaderFromList::ReadMotion(void* pBufferOrg, uint32 sizeBuffer)
{
    CMotionManager::Read(m_pszCurrentFileName, pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadMotionParameter(void* pBufferOrg, uint32 sizeBuffer)
{
    CMotionParameterManager::Read(m_pszCurrentFileName, pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadTextureDictionary(void* pBufferOrg, uint32 sizeBuffer)
{
    CTextureManager::ReadSet(m_pszCurrentFileName, pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadAddTextureDictionary(void* pBufferOrg, uint32 sizeBuffer)
{
    const char* pszTxdName = m_pszCurrentFileName;

    if (m_pCurrentPath->GetParamNum() == 1)
        pszTxdName = m_pCurrentPath->GetParam(0).AsStr();
    
    CTextureManager::AddSet(pszTxdName, pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadModelBsp(void* pBufferOrg, uint32 sizeBuffer)
{
    CWorldMap::ReadBsp(m_pszCurrentFileName, pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadCamera(void* pBufferOrg, uint32 sizeBuffer)
{
    CCameraDataManager::Read(pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadEffect(void* pBufferOrg, uint32 sizeBuffer)
{
    CEffectManager::Read(m_pszCurrentFileName, pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadAnimation2D(void* pBufferOrg, uint32 sizeBuffer)
{
    CAnimation2DLoader::Open(m_pszCurrentFileName, pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadUVAnim(void* pBufferOrg, uint32 sizeBuffer)
{
    CUVAnim::ReadUVAnim(m_pszCurrentFileName, pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadFont(void* pBufferOrg, uint32 sizeBuffer)
{
    CGameFont::Open(m_pszCurrentFileName, reinterpret_cast<const char*>(pBufferOrg), m_pszCurrentFileName);
};


void CDataLoaderFromList::ReadMapModel(void* pBufferOrg, uint32 sizeBuffer)
{
    ASSERT(m_pCurrentPath->GetParamNum() == 2);

    const char* pszTxdSetName = m_pCurrentPath->GetParam(0).AsStr();
    int32       mode = m_pCurrentPath->GetParam(1).AsInt();

    if (CUVAnim::IsExistsUVAnimDict(m_pszCurrentFileName))
        CUVAnim::SetUVAnimDict(m_pszCurrentFileName);

    CModelManager::ReadNormal(m_pszCurrentFileName, pBufferOrg, sizeBuffer, pszTxdSetName);
    CMapClumpModelManager::Read(m_pszCurrentFileName, mode);
};


void CDataLoaderFromList::ReadMapInfo(void* pBufferOrg, uint32 sizeBuffer)
{
    CWorldMap::ReadMapInfo(pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadGimmick(void* pBufferOrg, uint32 sizeBuffer)
{
    CGimmickDataManager::Read(m_pszCurrentFileName, pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadStrings(void* pBufferOrg, uint32 sizeBuffer)
{
    if (!std::strcmp(m_pszCurrentFileName, "system"))
        CSystemText::Read(pBufferOrg, sizeBuffer);
    else
        CGameText::Read(pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadEnemyParameter(void* pBufferOrg, uint32 sizeBuffer)
{
    CEnemyParameter::Read(pBufferOrg, sizeBuffer);
};


void CDataLoaderFromList::ReadIconsPS2(void* pBufferOrg, uint32 sizeBuffer)
{
    ;
};


void CDataLoaderFromList::ReadIconsNGC(void* pBufferOrg, uint32 sizeBuffer)
{
    ;
};


void CDataLoaderFromList::ReadMovieText(void* pBufferOrg, uint32 sizeBuffer)
{
#ifdef TMNT2_BUILD_EU
    CMovieText::Read(pBufferOrg, sizeBuffer);
#endif /* TMNT2_BUILD_EU */
};


//
// *********************************************************************************
//


IDataLoaderImpl* CreateDataLoaderList(void)
{
    return new CDataLoaderFromList;
};