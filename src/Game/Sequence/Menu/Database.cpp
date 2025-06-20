#include "Database.hpp"
#include "DatabaseSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/2d/MenuController.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Screen.hpp"


class CDatabase_Container
{
public:
    enum WNDTYPE
    {
        WNDTYPE_EXIT = 0,
        WNDTYPE_MAIN,
        WNDTYPE_CHR,
        WNDTYPE_ENM,
        WNDTYPE_ART,
        WNDTYPE_ETC,
        
        WNDTYPE_MAX,
    };

    enum SUNADOKEI_STATE
    {
        SUNADOKEI_STATE_NONE = 0,
        SUNADOKEI_STATE_ROTATE,
        SUNADOKEI_STATE_SAND,
        SUNADOKEI_STATE_IDLE,
    };

public:
    CDatabase_Container(void);
    ~CDatabase_Container(void);
    const wchar* GetDatabaseName(WNDTYPE windowType, int32 itemNo);
    void DB_TextureSetting(void);
    void DB_DrawNewIcon(float x, float y);
    void DB_DrawNewDisk(float x, float y);
    void DB_DrawNewItem(float x, float y);
    void DB_DrawText(WNDTYPE windowType);
    void DB_DrawSundokei(void);
    void DB_SunadokeiSet(SUNADOKEI_STATE State);
    bool IsDbViewerNew(DBITEMID::VALUE idItem) const;
    bool IsDbViewerUnlocked(DBITEMID::VALUE idItem) const;
    bool IsDbCategoryNew(CDatabaseRecord::GROUP Group) const;
    bool IsDbCategoryUnlocked(CDatabaseRecord::GROUP Group) const;
    
private:
    bool            m_bTextureSettingFlag;
    RwTexture*      m_apTexture[4];
    CSprite         m_sprite;
    int32           m_iDBNewUVStep;
    uint32          m_uDbAnimCnt;
    SUNADOKEI_STATE m_eSunadokeiState;
    uint32          m_uSunadokeiAnimCnt;
    float           m_fSunadokeiRot;
    int32           m_iSunadokeiSandStep;
};


CDatabase_Container::CDatabase_Container(void)
: m_bTextureSettingFlag(false)
, m_apTexture()
, m_sprite()
, m_iDBNewUVStep(0)
, m_uDbAnimCnt(0)
, m_eSunadokeiState(SUNADOKEI_STATE_NONE)
, m_uSunadokeiAnimCnt(0)
, m_fSunadokeiRot(0.0f)
, m_iSunadokeiSandStep(0)
{
    for (int32 i = 0; i < COUNT_OF(m_apTexture); ++i)
        m_apTexture[i] = nullptr;
};


CDatabase_Container::~CDatabase_Container(void)
{
    ;
};


const wchar* CDatabase_Container::GetDatabaseName(WNDTYPE windowType, int32 itemNo)
{
	const wchar* pwszStr = nullptr;

    switch (windowType)
    {
    case WNDTYPE_CHR:
        {
            switch (itemNo)
            {
            case 2:
                pwszStr = CGameText::GetText(GAMETEXT_DB_CHR_SLA);
                break;

            case 3:
            case 4:
                pwszStr = CGameText::GetText(GAMETEXT_DB_CHR_RAP);
                break;

            case 5:
                pwszStr = CGameText::GetText(GAMETEXT_DB_CHR_CAS);
                break;

            case 6:
            case 7:
                pwszStr = CGameText::GetText(GAMETEXT_DB_CHR_MIC);
                break;

            case 8:
                pwszStr = CGameText::GetText(GAMETEXT_DB_CHR_KAR);
                break;

            case 9:
            case 10:
                pwszStr = CGameText::GetText(GAMETEXT_DB_CHR_DON);
                break;

            case 11:
                pwszStr = CGameText::GetText(GAMETEXT_DB_CHR_SPL);
                break;

            case 12:
                pwszStr = CGameText::GetText(GAMETEXT_DB_CHR_APR);
                break;

            case 13:
                pwszStr = CGameText::GetText(GAMETEXT_ENEMY_085);
                break;

            default:
                pwszStr = CGameText::GetText(GAMETEXT_DB_CHR_LEO);
                break;
            };
        }
        break; /* case WNDTYPE_CHR */

    case WNDTYPE_ENM:
        {
            switch (itemNo)
            {
            case 0:
                pwszStr = CGameText::GetText(GAMETEXT_ENEMY_088);
                break;

            case 1:
                pwszStr = CGameText::GetText(GAMETEXT_ENEMY_091);
                break;

            case 2:
                pwszStr = CGameText::GetText(GAMETEXT_ENEMY_070);
                break;

            case 3:
                pwszStr = CGameText::GetText(GAMETEXT_ENEMY_074);
                break;

            case 4:
                pwszStr = CGameText::GetText(GAMETEXT_ENEMY_075);
                break;

            case 5:
                pwszStr = CGameText::GetText(GAMETEXT_ENEMY_092);
                break;

            case 6:
                pwszStr = CGameText::GetText(GAMETEXT_ENEMY_081);
                break;

            case 7:
                pwszStr = CGameText::GetText(GAMETEXT_ENEMY_082);
                break;

            case 8:
                pwszStr = CGameText::GetText(GAMETEXT_ENEMY_080);
                break;

            case 9:
            case 10:
                pwszStr = CGameText::GetText(GAMETEXT_DB_SECCHR);
                break;

            case 11:
                pwszStr = CGameText::GetText(GAMETEXT_DB_OTHER);
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break; /* case WNDTYPE_ENM */

    case WNDTYPE_ART:
        {
            switch (itemNo)
            {
            case 0:
                pwszStr = CGameText::GetText(GAMETEXT_DB_NY);
                break;

            case 1:
                pwszStr = CGameText::GetText(GAMETEXT_DB_DHO);
                break;

            case 2:
                pwszStr = CGameText::GetText(GAMETEXT_DB_JAP);
                break;

            case 3:
                pwszStr = CGameText::GetText(GAMETEXT_DB_KUR);
                break;

            case 4:
                pwszStr = CGameText::GetText(GAMETEXT_DB_NEXUS);
                break;

            case 5:
                pwszStr = CGameText::GetText(GAMETEXT_DB_VIHICLES);
                break;

            case 6:
                pwszStr = CGameText::GetText(GAMETEXT_DB_ANIM);
                break;

            case 7:
                pwszStr = CGameText::GetText(GAMETEXT_DB_CG);
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break; /* case WNDTYPE_ART */

    case WNDTYPE_ETC:
        {
            switch (itemNo)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                pwszStr = CGameText::GetText(GAMETEXT_DB_PM);
                break;

            case 4:
                pwszStr = CGameText::GetText(GAMETEXT_DB_GALLERY);
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break; /* case WNDTYPE_ETC */

    default:
        ASSERT(false);
        break;
    };
    
    return pwszStr;
};


void CDatabase_Container::DB_TextureSetting(void)
{
    if (!m_bTextureSettingFlag)
    {
        m_bTextureSettingFlag = true;

        CTextureManager::SetCurrentTextureSet("database");

#ifdef TMNT2_BUILD_EU        
        m_apTexture[0] = CTextureManager::GetRwTexture("DB_Time");
        m_apTexture[1] = CTextureManager::GetRwTexture("DB_New");
        m_apTexture[2] = CTextureManager::GetRwTexture("DB_New2");
        m_apTexture[3] = CTextureManager::GetRwTexture("DB_Cd");
#else /* TMNT2_BUILD_EU */
        m_apTexture[0] = CTextureManager::GetRwTexture("DB_Time");
        m_apTexture[1] = CTextureManager::GetRwTexture("DB_New");
        m_apTexture[2] = CTextureManager::GetRwTexture("DB_Cd");
#endif /* TMNT2_BUILD_EU */
    };
};


void CDatabase_Container::DB_DrawNewIcon(float x, float y)
{
#if !defined(TMNT2_BUILD_EU)    
    m_sprite.SetOffset(0.0f, 0.5f);
    m_sprite.SetTexture(m_apTexture[1]);
    m_sprite.Move(x, y - 16.0f);
	m_sprite.Resize(32.0f, 16.0f);
    m_sprite.SetUV(0.0f,
                   float(m_iDBNewUVStep) * 0.25f,
                   1.0f,
                   (float(m_iDBNewUVStep) * 0.25f) + 0.25f);
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.Draw();    
    m_sprite.ResetUV();
#endif /* !defined(TMNT2_BUILD_EU) */
};


void CDatabase_Container::DB_DrawNewDisk(float x, float y)
{
#ifdef TMNT2_BUILD_EU
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetTexture(m_apTexture[2]);
    m_sprite.Move(x, y);
    m_sprite.Resize(128.0f, 128.0f);
    m_sprite.SetAlpha(255);
    m_sprite.Draw();
    m_sprite.ResetUV();
#endif /* TMNT2_BUILD_EU */
};


void CDatabase_Container::DB_DrawNewItem(float x, float y)
{
#ifdef TMNT2_BUILD_EU
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetTexture(m_apTexture[1]);
    m_sprite.Move(x, y);
    m_sprite.Resize(256.0f, 32.0f);
    m_sprite.SetAlpha(255);
    m_sprite.Draw();
    m_sprite.ResetUV();
#endif /* TMNT2_BUILD_EU */
};


void CDatabase_Container::DB_DrawText(WNDTYPE windowType)
{
    if (!m_bTextureSettingFlag)
        return;

    if (windowType == WNDTYPE_EXIT)
        return;
    
    CSystem2D::PushRenderState();
    
    if (windowType == WNDTYPE_MAIN)
    {
        CGameFont::SetHeightScaled(1.75f);

        const int32 CategoryNum = 4;

#ifdef TMNT2_BUILD_EU
        for (int32 i = 0; i < CategoryNum; ++i)
        {
            if (IsDbCategoryUnlocked(CDatabaseRecord::GROUP(i)))
            {
                const wchar* pwszStr = CGameText::GetText(GAMETEXT(GAMETEXT_DB_CHR + i));
                CGameFont::SetRGBA(255, 180, 0, 255);
                CGameFont::Show(pwszStr,
                                -150.0f,
                                float(i) * 74.0f - 100.0f);

                if (!IsDbCategoryNew(CDatabaseRecord::GROUP(i)))
                    DB_DrawNewDisk(-194.0f, float(i) * 73.0f - 96.0f);
            };
        };
#else /* TMNT2_BUILD_EU */
        for (int32 i = 0; i < CategoryNum; ++i)
        {
            RwV2d vTextPos = Math::VECTOR2_ZERO;
            
            const RwV2d aTextPos[] =
            {
                { -10.0f, -10.0f },
                { 160.0f, -10.0f },
                { -10.0f, 120.0f },
                { 160.0f, 120.0f },
            };

            static_assert(COUNT_OF(aTextPos) == CategoryNum, "update me");

            vTextPos = aTextPos[i];

            if (IsDbCategoryUnlocked(CDatabaseRecord::GROUP(i)))
            {
                const wchar* pwszStr = CGameText::GetText(GAMETEXT(GAMETEXT_DB_CHR + i));
                CGameFont::SetRGBA(255, 180, 0, 255);
                CGameFont::Show(pwszStr,
                                vTextPos.x - (CGameFont::GetStringWidth(pwszStr) * 0.5f),
                                vTextPos.y);
                
                if (!IsDbCategoryNew(CDatabaseRecord::GROUP(i)))
                {
                    const RwV2d aIconPos[] =
                    {
                        { vTextPos.x - 60.0f, vTextPos.y - 90.0f },
                        { vTextPos.x - 45.0f, vTextPos.y - 90.0f },
                        { vTextPos.x - 60.0f, vTextPos.y - 90.0f },
                        { vTextPos.x - 45.0f, vTextPos.y - 90.0f },
                    };

                    static_assert(COUNT_OF(aIconPos) == CategoryNum, "update me");

                    DB_DrawNewIcon(aIconPos[i].x, aIconPos[i].y);
                };
            };
        };
#endif /* TMNT2_BUILD_EU */
    }
    else
    {
        struct TEXTINFO
        {
            int32           numItem;
            RwV2d           position;
            DBITEMID::VALUE startDBItem;
            int32           numColumns; // numRows for TMNT2_BUILD_EU cases
        };

        const TEXTINFO aTextInfo[] =
        {
#ifdef TMNT2_BUILD_EU
            { DBITEMID::CHARACTERNUM,   { -240.0f, 102.0f },   DBITEMID::CHARACTERSTART,   8 },
            { DBITEMID::ENEMYNUM,       { -240.0f, 102.0f },   DBITEMID::ENEMYSTART,       6 },
            { DBITEMID::ARTNUM,         { -150.0f, 125.0f },   DBITEMID::ARTSTART,         8 },
            { DBITEMID::ETCNUM,         { -150.0f, 125.0f },   DBITEMID::ETCSTART,         8 },
#else /* TMNT2_BUILD_EU */
            { DBITEMID::CHARACTERNUM,   { -235.0f, -127.0f },   DBITEMID::CHARACTERSTART,   3 },
            { DBITEMID::ENEMYNUM,       { -235.0f, -127.0f },   DBITEMID::ENEMYSTART,       3 },
            { DBITEMID::ARTNUM,         { -168.0f,  -89.0f },   DBITEMID::ARTSTART,         2 },
            { DBITEMID::ETCNUM,         { -132.0f,  -85.0f },   DBITEMID::ETCSTART,         2 },
#endif /* TMNT2_BUILD_EU */
        };

        const TEXTINFO* pTextInfo = &aTextInfo[windowType - 2]; // exclude exit & main wnd

        CGameFont::SetHeightScaled(1.5f);

#ifdef TMNT2_BUILD_EU        
        float fOfsX = 0.0f;

        int32 itemNo = 0;
        int32 dbItem = pTextInfo->startDBItem;
        for (int32 i = 0; i < pTextInfo->numItem; ++i)
        {
            Rt2dBBox bbox;
            bbox.x = pTextInfo->position.x + fOfsX;
            bbox.y = pTextInfo->position.y - (float(itemNo) * 36.0f) + 2.0f;
            bbox.w = 150.0f;
            bbox.h = 40.0f;

            if (IsDbViewerUnlocked(DBITEMID::VALUE(dbItem)))
            {
                const wchar* pwszStr = GetDatabaseName(windowType, i);
                ASSERT(pwszStr != nullptr);

                CGameFont::SetRGBA(255, 180, 0, 255);
                CGameFont::Flow(pwszStr, &bbox);

                if (!IsDbViewerNew(DBITEMID::VALUE(dbItem)))
                {
                    float x = 0.0f;
                    float y = 0.0f;

                    if (windowType >= WNDTYPE_ART)
                    {
                        // ART & ETC
                        x = pTextInfo->position.x + 112.0f;
                        y = pTextInfo->position.y - 274.0f;
                    }
                    else
                    {
                        // CHR & ENM
                        x = pTextInfo->position.x + 110.0f;
                        y = pTextInfo->position.y - 227.0f;
                    };

                    x += fOfsX;
                    y += (float(itemNo) * 36.0f);

                    DB_DrawNewItem(x, y);
                };
            }
            else
            {
                const wchar* pwszHatena = CGameText::GetText(GAMETEXT_DB_HATENA);

                CGameFont::SetRGBA(255, 180, 0, 64);
                CGameFont::Flow(pwszHatena, &bbox);
            };

            if ((++itemNo >= pTextInfo->numColumns) &&
                ((windowType == WNDTYPE_CHR) || (windowType == WNDTYPE_ENM)))
            {
                fOfsX = 251.0f;
                itemNo = 0;
            };

            ++dbItem;
        };
#else /* TMNT2_BUILD_EU */
        int32 colNo   = 0;
        int32 colMax  = pTextInfo->numColumns - 1;
        int32 colItem = 0;
        int32 dbItem  = pTextInfo->startDBItem;
        for (int32 i = 0; i < pTextInfo->numItem; ++i)
        {
            float x = pTextInfo->position.x + (float(colNo) * 161.0f);
            float y = pTextInfo->position.y + (float(colItem) * 61.0f);
            
            if (IsDbViewerUnlocked(DBITEMID::VALUE(dbItem)))
            {
                float OfsY = 98.0f;
                if (windowType == WNDTYPE_ART)
                    OfsY = 60.0f;
                else if (windowType == WNDTYPE_ETC)
                    OfsY = 56.0f;

                Rt2dBBox bbox;
                bbox.x = x;
                bbox.y = (OfsY - (float(colItem) * 60.0f));
                bbox.w = 150.0f;
                bbox.h = 40.0f;

                const wchar* pwszStr = GetDatabaseName(windowType, i);

                CGameFont::SetRGBA(255, 180, 0, 255);
                CGameFont::Flow(pwszStr, &bbox);
                
                if (!IsDbViewerNew(DBITEMID::VALUE(dbItem)))
                    DB_DrawNewIcon(x, y);
            }
            else
            {
                const wchar* pwszHatena = CGameText::GetText(GAMETEXT_DB_HATENA);

                CGameFont::SetRGBA(255, 180, 0, 64);
                CGameFont::Show(pwszHatena, x, y);
            };

            colNo = InvClamp(colNo + 1, 0, colMax);
            colItem = ((i + 1) / pTextInfo->numColumns);
            ++dbItem;
        };
#endif /* TMNT2_BUILD_EU */
    };
    
    CSystem2D::PopRenderState();

#if !defined(TMNT2_BUILD_EU)
    if (m_iDBNewUVStep == 3)
    {
        //
        //  New icon small pause
        //
        if (float(m_uDbAnimCnt) >= (CScreen::Framerate() * 0.5f))
        {
            m_iDBNewUVStep = 0;
            m_uDbAnimCnt = 0;
        }
        else
        {
            ++m_uDbAnimCnt;
        };
    }
    else
    {
        //
        //  New icon move
        //
        for (int32 i = 0; i < 4; ++i)
        {
            if (float(m_uDbAnimCnt) >= (CScreen::Framerate() * 0.2f))
            {
				++m_iDBNewUVStep;
                m_uDbAnimCnt = 0;
            }
            else
            {
                ++m_uDbAnimCnt;
            };
        };    
    };
#endif /* !defined(TMNT2_BUILD_EU) */
};


void CDatabase_Container::DB_DrawSundokei(void)
{
    if (!m_bTextureSettingFlag)
        return;

    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetTexture(m_apTexture[0]);
    m_sprite.Move(0.0f, 0.0f);
    m_sprite.Resize(128.0f, 128.0f);
    m_sprite.SetAlpha(255);

    switch (m_eSunadokeiState)
    {
    case SUNADOKEI_STATE_ROTATE:
        {
            uint8 AlphaBasis = uint8(Math::LinearTween(0.0f,
                                                       255.0f,
                                                       float(m_uSunadokeiAnimCnt),
                                                       (CScreen::Framerate() * 0.5f)));

            float fRotStep = float(m_uSunadokeiAnimCnt) / (CScreen::Framerate() * 0.5f);

            m_fSunadokeiRot = fRotStep * Math::PI;

            if (float(m_uSunadokeiAnimCnt) >= (CScreen::Framerate() * 0.5f))
            {
                m_iSunadokeiSandStep = 0;
                m_eSunadokeiState = SUNADOKEI_STATE_SAND;
                m_uSunadokeiAnimCnt = 0;
                m_fSunadokeiRot = 0.0f;
            }
            else
            {
				++m_uSunadokeiAnimCnt;
            };
            
            m_sprite.SetUV(0.5f, 0.5f, 1.0f, 1.0f);
            m_sprite.SetRotate(m_fSunadokeiRot * fRotStep);
            m_sprite.DrawRotate();
            
            m_sprite.SetUV(0.5f, 0.5f, 1.0f, 1.0f);
            m_sprite.SetRotate(m_fSunadokeiRot);
            m_sprite.SetAlpha(AlphaBasis);
            m_sprite.DrawRotate();
        }
        break;
        
    case SUNADOKEI_STATE_SAND:
        {
            float aSandUV[4][4] =
            {
                { 0.0f, 0.0f, 0.5f, 0.5f },
                { 0.5f, 0.0f, 1.0f, 0.5f },
                { 0.0f, 0.5f, 0.5f, 1.0f },
                { 0.5f, 0.5f, 1.0f, 1.0f },
            };
            
            ASSERT(m_iSunadokeiSandStep >= 0);
            ASSERT(m_iSunadokeiSandStep < COUNT_OF(aSandUV));

            m_sprite.SetUV(aSandUV[m_iSunadokeiSandStep][0],
                           aSandUV[m_iSunadokeiSandStep][1],
                           aSandUV[m_iSunadokeiSandStep][2],
                           aSandUV[m_iSunadokeiSandStep][3]);

            if (m_iSunadokeiSandStep >= 3)
                m_eSunadokeiState = SUNADOKEI_STATE_IDLE;
            else
                ++m_iSunadokeiSandStep;

            m_sprite.Draw();
        }
        break;

    case SUNADOKEI_STATE_IDLE:
        {
            float aSandUV[4][4] =
            {
                { 0.0f, 0.0f, 0.5f, 0.5f },
                { 0.5f, 0.0f, 1.0f, 0.5f },
                { 0.0f, 0.5f, 0.5f, 1.0f },
                { 0.5f, 0.5f, 1.0f, 1.0f },
            };

            ASSERT(m_iSunadokeiSandStep >= 0);
            ASSERT(m_iSunadokeiSandStep < COUNT_OF(aSandUV));

            m_sprite.SetUV(aSandUV[m_iSunadokeiSandStep][0],
                           aSandUV[m_iSunadokeiSandStep][1],
                           aSandUV[m_iSunadokeiSandStep][2],
                           aSandUV[m_iSunadokeiSandStep][3]);
        
            m_sprite.Draw();
            
            if (float(m_uSunadokeiAnimCnt) >= (CScreen::Framerate() * 0.1f))
            {
                if (m_iSunadokeiSandStep < 3)
                    ++m_iSunadokeiSandStep;
                
                m_uSunadokeiAnimCnt = 0;
            }
            else
            {
                ++m_uSunadokeiAnimCnt;
            };
        }
        break;

    default:
        break;
    };

    m_sprite.SetAlpha(255);
    m_sprite.SetUV(0.0f, 0.0f, 1.0f, 1.0f);
};


void CDatabase_Container::DB_SunadokeiSet(SUNADOKEI_STATE State)
{
    m_eSunadokeiState = State;
    m_uSunadokeiAnimCnt = 0;
    m_fSunadokeiRot = 0.0f;
    if (State == SUNADOKEI_STATE_NONE)
        m_iSunadokeiSandStep = 0;
};


bool CDatabase_Container::IsDbViewerNew(DBITEMID::VALUE idItem) const
{
    return CGameData::Record().Database().IsItemRead(idItem);
};


bool CDatabase_Container::IsDbViewerUnlocked(DBITEMID::VALUE idItem) const
{
    return CGameData::Record().Database().IsItemUnlocked(idItem);
};


bool CDatabase_Container::IsDbCategoryNew(CDatabaseRecord::GROUP Group) const
{
    return CGameData::Record().Database().IsItemReadInGroup(Group);
};


bool CDatabase_Container::IsDbCategoryUnlocked(CDatabaseRecord::GROUP Group) const
{
    return (CGameData::Record().Database().CountUnlockedItemInGroup(Group) != 0);
};


static CDatabase_Container* s_pDatabase_Container = nullptr;


static const char* s_apszTextOnStr[] =
{
    "Text_Db",
    "Text_Chara",
    "Text_Enemy",
    "text_Bg",
    "Text_Etc",
};


static const char* s_apszTextOffStr[] =
{
    "Text_Db_End",
    "Text_Chara_End",
    "Text_Enemy_End",
    "Text_Bg_End",
    "Text_Etc_End",
};


static const char* s_apszCategoryStr[] =
{
    "Db_Chara",
    "Db_Enemy",
    "Db_Bg",
    "Db_Etc",
};


static const char* s_apszViewerNoStr[] =
{
    "LeonardoA",
    "LeonardoB",
    "Slashuur",
    "RaphaelA",
    "RaphaelB",
    "Casey",
    "MichelangeloA",
    "MichelangeloB",
    "Karai",
    "DonatelloA",
    "DonatelloB",
    "Splinter",
    "April",
    "MiyamotoUsagi",
    "Shredder",
    "Dorako",
    "RatNinja",
    "Traximus",
    "LetherHead",
    "FootElite",
    "Spasmosaur",
    "Hun",
    "UltimateNinja",
    "ZakoA",
    "ZakoB",
    "Others",
    "Ny",
    "Dhoonib",
    "FeudalJapan",
    "Kuraiyama",
    "TransDimension",
    "Norimono",
    "Anime",
    "Cg",
    "PlaymateA",
    "PlaymateB",
    "PlaymateC",
    "PlaymateD",
    "Gallery",
};


/*static*/ CProcess* CDatabase::Instance(void)
{
    return new CDatabase;
};


CDatabase::CDatabase(void)
: CAnim2DSequence("database")
, m_iSelWindowType(CDatabase_Container::WNDTYPE_EXIT)
, m_bGotoViewerFlag(false)
{
    ;
};


CDatabase::~CDatabase(void)
{
    ;
};


bool CDatabase::OnAttach(const void* pParam)
{
    if (!s_pDatabase_Container)
        s_pDatabase_Container = new CDatabase_Container;

    s_pDatabase_Container->DB_SunadokeiSet(CDatabase_Container::SUNADOKEI_STATE_NONE);

    m_iSelWindowType = CDatabase_Container::WNDTYPE_EXIT;
    m_bGotoViewerFlag = false;
    
    return CAnim2DSequence::OnAttach(FILEID::ID_DATABASE);
};


void CDatabase::OnDetach(void)
{
    if (s_pDatabase_Container)
    {
        delete s_pDatabase_Container;
        s_pDatabase_Container = nullptr;
    };

    CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
    
    CAnim2DSequence::OnDetach();
};


void CDatabase::OnMove(bool bRet, const void* pReturnValue)
{
    CAnim2DSequence::OnMove(bRet, pReturnValue);

    switch (AnimStep())
    {
    case ANIMSTEP_DRAW:
        {
            MessageProc();
        }
        break;

    case ANIMSTEP_END:
        {
            if (m_bGotoViewerFlag)
                Ret(reinterpret_cast<const void*>(PROCLABEL_SEQ_DATABASEVIEWER));
            else
                Ret();
        }
        break;

    default:
        break;
    };
};


void CDatabase::OnDraw(void) const
{
    CAnim2DSequence::OnDraw();

    if (CAnim2DSequence::IsDrawing())
    {
        CRenderStateManager::SetDefault();
        
        RENDERSTATE_PUSH(rwRENDERSTATEBORDERCOLOR, 0);
        RENDERSTATE_PUSH(rwRENDERSTATETEXTUREPERSPECTIVE, true);
        RENDERSTATE_PUSH(rwRENDERSTATETEXTUREFILTER, rwFILTERLINEAR);
        RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESS, rwTEXTUREADDRESSCLAMP);
        RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
        RENDERSTATE_PUSH(rwRENDERSTATESHADEMODE, rwSHADEMODEFLAT);
        RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, 0);
        RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
        
        s_pDatabase_Container->DB_TextureSetting();
        s_pDatabase_Container->DB_DrawText(CDatabase_Container::WNDTYPE(m_iSelWindowType));
        s_pDatabase_Container->DB_DrawSundokei();
        
        RENDERSTATE_POP(rwRENDERSTATECULLMODE);
        RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
        RENDERSTATE_POP(rwRENDERSTATESHADEMODE);
        RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
        RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
        RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESS);
        RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
        RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
        RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
        RENDERSTATE_POP(rwRENDERSTATETEXTUREFILTER);
        RENDERSTATE_POP(rwRENDERSTATETEXTUREPERSPECTIVE);
        RENDERSTATE_POP(rwRENDERSTATEBORDERCOLOR);
        
        CRenderStateManager::SetDefault();
    };

#ifdef TMNT2_BUILD_EU
    CSystem2D::PushRenderState();

    CGameFont::SetHeightScaled(2.0f);
    CGameFont::SetRGBA(255, 255, 255, 255);

    const wchar* pwszText = CGameText::GetText(GAMETEXT_EU_HELP_13);

    if (CGameFont::GetStringWidth(pwszText) >= 500.0f)
    {
        Rt2dBBox bbox;
        bbox.x = -272.0f;
        bbox.y = -230.0f;
        bbox.w =  500.0f;
        bbox.h =   60.0f;

        CGameFont::Flow(pwszText, &bbox);
    }
    else
    {
        CGameFont::Show(pwszText, -272.0f, 190.0f);
    };

    CSystem2D::PopRenderState();
#endif /* TMNT2_BUILD_EU */
};


void CDatabase::BeginFadeIn(void)
{
#ifdef TMNT2_BUILD_EU    
    Animation2D().SetText("DB_T_00",        CGameText::GetText(GAMETEXT_DB_TIT));    
    Animation2D().SetText("DB_I_00",        CGameText::GetText(GAMETEXT_DB_HOME));
    Animation2D().SetText("DB_I_01",        CGameText::GetText(GAMETEXT_DB_PIC));    
    Animation2D().SetText("DB_I_02",        CGameText::GetText(GAMETEXT_DB_TRASH));    
    Animation2D().SetText("DB_I_03",        CGameText::GetText(GAMETEXT_EU_EMPTY));
    Animation2D().SetText("DB_W_00",        CGameText::GetText(GAMETEXT_DB_HOME));
    Animation2D().SetText("DB_W_01",        CGameText::GetText(GAMETEXT_DB_TIT));
    Animation2D().SetText("DB_W_02",        CGameText::GetText(GAMETEXT_DB_CHR));
    Animation2D().SetText("DB_W_03",        CGameText::GetText(GAMETEXT_DB_ENM));
    Animation2D().SetText("DB_W_04",        CGameText::GetText(GAMETEXT_DB_BG));
    Animation2D().SetText("DB_W_05",        CGameText::GetText(GAMETEXT_DB_OTHER));    
    Animation2D().SetText("DB_W_06",        CGameText::GetText(GAMETEXT_EU_BACK));    
    Animation2D().SetText("DATABASE_END",   CGameText::GetText(GAMETEXT_DB_EXIT_MSG));
    Animation2D().SetText("YES",            CGameText::GetText(GAMETEXT_YES));    
    Animation2D().SetText("NO",             CGameText::GetText(GAMETEXT_NO));
#else /* TMNT2_BUILD_EU */
    Animation2D().SetText("DATABASE_END",   CGameText::GetText(GAMETEXT_DB_EXIT_MSG));
#endif /* TMNT2_BUILD_EU */

    /* order is important and should be match to CDatabaseRecord::GROUP */
    static const char* s_apszCdOrgTex[] =
    {
#ifdef TMNT2_BUILD_EU
        "database_218",
        "database_216",
        "database_220",
        "database_222",
#else /* TMNT2_BUILD_EU */
        "database_251",
        "database_249",
        "database_253",
        "database_255",
#endif /* TMNT2_BUILD_EU */
    };

    static_assert(COUNT_OF(s_apszCdOrgTex) == CDatabaseRecord::GROUP_NUM, "table incorrect");

    for (int32 i = 0; i < COUNT_OF(s_apszCdOrgTex); ++i)
    {
        CDatabaseRecord::GROUP group = static_cast<CDatabaseRecord::GROUP>(i);

        if (CGameData::Record().Database().CountUnlockedItemInGroup(group))
            Animation2D().SetTexture(s_apszCdOrgTex[i], "DB_Cd01");
        else
            Animation2D().SetTexture(s_apszCdOrgTex[i], "DB_Cd02");
    };

    struct ITEMINFO
    {
        DBITEMID::VALUE id;
        const char* textureOrg;
        const char* textureNew;
    };
    
    static const ITEMINFO s_aItemInfo[] =
    {
#ifdef TMNT2_BUILD_EU
        { DBITEMID::ID_FIRST,                   "database_237", "DB_01" },
        { DBITEMID::ID_CHARACTER_LEO_B,         "database_239", "DB_02" },
        { DBITEMID::ID_CHARACTER_SLASHUUR,      "database_241", "DB_03" },
        { DBITEMID::ID_CHARACTER_RAP_A,         "database_243", "DB_04" },
        { DBITEMID::ID_CHARACTER_RAP_B,         "database_245", "DB_05" },
        { DBITEMID::ID_CHARACTER_CASEY,         "database_247", "DB_06" },
        { DBITEMID::ID_CHARACTER_MIC_A,         "database_249", "DB_07" },
        { DBITEMID::ID_CHARACTER_MIC_B,         "database_251", "DB_08" },
        { DBITEMID::ID_CHARACTER_KARAI,         "database_253", "DB_09" },
        { DBITEMID::ID_CHARACTER_DON_A,         "database_255", "DB_10" },
        { DBITEMID::ID_CHARACTER_DON_B,         "database_257", "DB_11" },
        { DBITEMID::ID_CHARACTER_SPLINTER,      "database_259", "DB_12" },
        { DBITEMID::ID_CHARACTER_APRIL,         "database_261", "DB_13" },
        { DBITEMID::ID_CHARACTER_USAGI,         "database_263", "DB_23" },
        { DBITEMID::ID_ENEMY_SHREDDER,          "database_277", "DB_14" },
        { DBITEMID::ID_ENEMY_ENEMYDORAKO,       "database_279", "DB_15" },
        { DBITEMID::ID_ENEMY_NINJA_RATS,        "database_281", "DB_16" },
        { DBITEMID::ID_ENEMY_TRAXIMUS,          "database_283", "DB_17" },
        { DBITEMID::ID_ENEMY_LEATHERHEAD,       "database_285", "DB_18" },
        { DBITEMID::ID_ENEMY_ELITE_FOOT,        "database_287", "DB_19" },
        { DBITEMID::ID_ENEMY_SPASMOSAUR,        "database_289", "DB_20" },
        { DBITEMID::ID_ENEMY_HUN,               "database_291", "DB_21" },
        { DBITEMID::ID_ENEMY_ULTIMATE_NINJA,    "database_293", "DB_22" },
        { DBITEMID::ID_ENEMY_ZAKO_A,            "database_295", "DB_24" },
        { DBITEMID::ID_ENEMY_ZAKO_B,            "database_297", "DB_25" },
        { DBITEMID::ID_ENEMY_OTHERS,            "database_299", "DB_26" },
        { DBITEMID::ID_ART_NY,                  "database_304", "DB_27" },
        { DBITEMID::ID_ART_DHOONNIB,            "database_306", "DB_28" },
        { DBITEMID::ID_ART_JAPAN,               "database_308", "DB_29" },
        { DBITEMID::ID_ART_KURAIYAMA,           "database_310", "DB_30" },
        { DBITEMID::ID_ART_TRANS_DIMENSION,     "database_312", "DB_31" },
        { DBITEMID::ID_ART_MACHINE,             "database_314", "DB_32" },
        { DBITEMID::ID_ART_ANIME,               "database_316", "DB_33" },
        { DBITEMID::ID_ART_CG,                  "database_318", "DB_34" },
        { DBITEMID::ID_ETC_PLAYMATES_A,         "database_321", "DB_35" },
        { DBITEMID::ID_ETC_PLAYMATES_B,         "database_323", "DB_36" },
        { DBITEMID::ID_ETC_PLAYMATES_C,         "database_325", "DB_37" },
        { DBITEMID::ID_ETC_PLAYMATES_D,         "database_327", "DB_38" },
        { DBITEMID::ID_ETC_GALLERY,             "database_329", "DB_39" },
#else /* TMNT2_BUILD_EU */
        { DBITEMID::ID_CHARACTER_LEO_A,         "database_276", "DB_01" },
        { DBITEMID::ID_CHARACTER_LEO_B,         "database_286", "DB_02" },
        { DBITEMID::ID_CHARACTER_SLASHUUR,      "database_300", "DB_03" },
        { DBITEMID::ID_CHARACTER_RAP_A,         "database_278", "DB_04" },
        { DBITEMID::ID_CHARACTER_RAP_B,         "database_288", "DB_05" },
        { DBITEMID::ID_CHARACTER_CASEY,         "database_294", "DB_06" },
        { DBITEMID::ID_CHARACTER_MIC_A,         "database_280", "DB_07" },
        { DBITEMID::ID_CHARACTER_MIC_B,         "database_290", "DB_08" },
        { DBITEMID::ID_CHARACTER_KARAI,         "database_298", "DB_09" },
        { DBITEMID::ID_CHARACTER_DON_A,         "database_282", "DB_10" },
        { DBITEMID::ID_CHARACTER_DON_B,         "database_292", "DB_11" },
        { DBITEMID::ID_CHARACTER_SPLINTER,      "database_296", "DB_12" },
        { DBITEMID::ID_CHARACTER_APRIL,         "database_284", "DB_13" },
        { DBITEMID::ID_CHARACTER_USAGI,         "database_311", "DB_23" },
        { DBITEMID::ID_ENEMY_SHREDDER,          "database_323", "DB_14" },
        { DBITEMID::ID_ENEMY_ENEMYDORAKO,       "database_325", "DB_15" },
        { DBITEMID::ID_ENEMY_NINJA_RATS,        "database_327", "DB_16" },
        { DBITEMID::ID_ENEMY_TRAXIMUS,          "database_329", "DB_17" },
        { DBITEMID::ID_ENEMY_LEATHERHEAD,       "database_331", "DB_18" },
        { DBITEMID::ID_ENEMY_ELITE_FOOT,        "database_333", "DB_19" },
        { DBITEMID::ID_ENEMY_SPASMOSAUR,        "database_335", "DB_20" },
        { DBITEMID::ID_ENEMY_HUN,               "database_337", "DB_21" },
        { DBITEMID::ID_ENEMY_ULTIMATE_NINJA,    "database_339", "DB_22" },
        { DBITEMID::ID_ENEMY_ZAKO_A,            "database_341", "DB_24" },
        { DBITEMID::ID_ENEMY_ZAKO_B,            "database_343", "DB_25" },
        { DBITEMID::ID_ENEMY_OTHERS,            "database_345", "DB_26" },
        { DBITEMID::ID_ART_NY,                  "database_354", "DB_27" },
        { DBITEMID::ID_ART_DHOONNIB,            "database_350", "DB_28" },
        { DBITEMID::ID_ART_JAPAN,               "database_356", "DB_29" },
        { DBITEMID::ID_ART_KURAIYAMA,           "database_352", "DB_30" },
        { DBITEMID::ID_ART_TRANS_DIMENSION,     "database_358", "DB_31" },
        { DBITEMID::ID_ART_MACHINE,             "database_360", "DB_32" },
        { DBITEMID::ID_ART_ANIME,               "database_362", "DB_33" },
        { DBITEMID::ID_ART_CG,                  "database_364", "DB_34" },
        { DBITEMID::ID_ETC_PLAYMATES_A,         "database_367", "DB_35" },
        { DBITEMID::ID_ETC_PLAYMATES_B,         "database_375", "DB_36" },
        { DBITEMID::ID_ETC_PLAYMATES_C,         "database_369", "DB_37" },
        { DBITEMID::ID_ETC_PLAYMATES_D,         "database_373", "DB_38" },
        { DBITEMID::ID_ETC_GALLERY,             "database_371", "DB_39" },
#endif /* TMNT2_BUILD_EU */
    };

    static_assert(COUNT_OF(s_aItemInfo) == DBITEMID::NUM, "table incorrect");

    for (int32 i = DBITEMID::ID_FIRST; i < DBITEMID::NUM; ++i)
    {
        if (!CGameData::Record().Database().IsItemUnlocked(s_aItemInfo[i].id))
            Animation2D().SetTexture(s_aItemInfo[i].textureOrg, s_aItemInfo[i].textureNew);
    };
    
    CGameSound::PlayBGM(SDCODE_BGM(0x3024));
    
    CAnim2DSequence::BeginFadeIn();
};


void CDatabase::MessageProc(void)
{
    int32 index = 0;

    if (Animation2D().CheckMessageGetURL("Home"))
    {
        m_bGotoViewerFlag = false;
        BeginFadeOut();
    }
    else if (IsAnim2DMessageList(s_apszTextOnStr, COUNT_OF(s_apszTextOnStr), &index))
    {
        m_iSelWindowType = (index + 1);
    }
    else if (IsAnim2DMessageList(s_apszTextOffStr, COUNT_OF(s_apszTextOffStr), &index))
    {
        m_iSelWindowType = 0;
    }
    else if (IsAnim2DMessageList(s_apszCategoryStr, COUNT_OF(s_apszCategoryStr), &index))
    {
        CMenuController::KeyLock(CController::DIGITAL_LUP);
        CMenuController::KeyLock(CController::DIGITAL_LDOWN);

        if (s_pDatabase_Container->IsDbCategoryUnlocked(CDatabaseRecord::GROUP(index)))
            Animation2D().FlashKeyPress(CController::DIGITAL_LUP);
        else
            Animation2D().FlashKeyPress(CController::DIGITAL_LDOWN);

        CMenuController::KeyUnlock(CController::DIGITAL_LUP);
        CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
    }
    else if (IsAnim2DMessageList(s_apszViewerNoStr, COUNT_OF(s_apszViewerNoStr), &index))
    {
        CMenuController::KeyLock(CController::DIGITAL_LUP);
        CMenuController::KeyLock(CController::DIGITAL_LDOWN);

        if (s_pDatabase_Container->IsDbViewerUnlocked(DBITEMID::VALUE(index + 1)))
        {
            CDatabaseSequence::SetDBViewerNo(index + 1);
            s_pDatabase_Container->DB_SunadokeiSet(CDatabase_Container::SUNADOKEI_STATE_ROTATE);
            Animation2D().FlashKeyPress(CController::DIGITAL_LUP);
        }
        else
        {
            Animation2D().FlashKeyPress(CController::DIGITAL_LDOWN);
        };

        CMenuController::KeyUnlock(CController::DIGITAL_LUP);
        CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
    }
    else if (Animation2D().CheckMessageGetURL("Loading"))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1002));
        m_bGotoViewerFlag = true;
        BeginFadeOut();
    };
};