#include "Database.hpp"
#include "DatabaseSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/2d/MenuController.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/RenderState.hpp"
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
    const wchar* GetDatabaseName(WNDTYPE Wndtype, int32 ItemNo);
    void DB_TextureSetting(void);
    void DB_DrawNewIcon(float x, float y);
    void DB_DrawText(WNDTYPE Wndtype);
    void DB_DrawSundokei(void);
    void DB_SunadokeiSet(SUNADOKEI_STATE State);
    bool IsDbViewerNew(DBITEMID::VALUE idItem) const;
    bool IsDbViewerUnlocked(DBITEMID::VALUE idItem) const;
    bool IsDbCategoryNew(CDatabaseRecord::GROUP Group) const;
    bool IsDbCategoryUnlocked(CDatabaseRecord::GROUP Group) const;
    
private:
    bool m_bTextureSettingFlag;
    RwTexture* m_pTexDbTime;
    RwTexture* m_pTexDbNew;
    RwTexture* m_pTexDbCd;
    CSprite m_Sprite;
    int32 m_iDbNewUVStep;
    uint32 m_uDbAnimCnt;
    SUNADOKEI_STATE m_eSunadokeiState;
    uint32 m_uSunadokeiAnimCnt;
    float m_fSunadokeiRot;
    int32 m_iSunadokeiSandStep;
};


CDatabase_Container::CDatabase_Container(void)
: m_bTextureSettingFlag(false)
, m_pTexDbTime(nullptr)
, m_pTexDbNew(nullptr)
, m_pTexDbCd(nullptr)
, m_Sprite()
, m_iDbNewUVStep(0)
, m_uDbAnimCnt(0)
, m_eSunadokeiState(SUNADOKEI_STATE_NONE)
, m_uSunadokeiAnimCnt(0)
, m_fSunadokeiRot(0.0f)
, m_iSunadokeiSandStep(0)
{
    ;
};


CDatabase_Container::~CDatabase_Container(void)
{
    ;
};


const wchar* CDatabase_Container::GetDatabaseName(WNDTYPE Wndtype, int32 ItemNo)
{
	const wchar* pwszStr = nullptr;

    switch (Wndtype)
    {
    case WNDTYPE_CHR:
        {
            switch (ItemNo)
            {
            case 2:
                pwszStr = CGameText::GetText(GAMETEXT(0x18));
                break;

            case 3:
            case 4:
                pwszStr = CGameText::GetText(GAMETEXT(0x12));
                break;

            case 5:
                pwszStr = CGameText::GetText(GAMETEXT(0x16));
                break;

            case 6:
            case 7:
                pwszStr = CGameText::GetText(GAMETEXT(0x13));
                break;

            case 8:
                pwszStr = CGameText::GetText(GAMETEXT(0x17));
                break;

            case 9:
            case 10:
                pwszStr = CGameText::GetText(GAMETEXT(0x14));
                break;

            case 11:
                pwszStr = CGameText::GetText(GAMETEXT(0x15));
                break;

            case 12:
                pwszStr = CGameText::GetText(GAMETEXT(0x19));
                break;

            case 13:
                pwszStr = CGameText::GetText(GAMETEXT(0xDA));
                break;

            default:
                pwszStr = CGameText::GetText(GAMETEXT(0x11));
                break;
            }
        }
        break;

    case WNDTYPE_ENM:
        {
            switch (ItemNo)
            {
            case 0:
                pwszStr = CGameText::GetText(GAMETEXT(0xDD));
                break;

            case 1:
                pwszStr = CGameText::GetText(GAMETEXT(0xE0));
                break;

            case 2:
                pwszStr = CGameText::GetText(GAMETEXT(0xCB));
                break;

            case 3:
                pwszStr = CGameText::GetText(GAMETEXT(0xCF));
                break;

            case 4:
                pwszStr = CGameText::GetText(GAMETEXT(0xD0));
                break;

            case 5:
                pwszStr = CGameText::GetText(GAMETEXT(0xE1));
                break;

            case 6:
                pwszStr = CGameText::GetText(GAMETEXT(0xD6));
                break;

            case 7:
                pwszStr = CGameText::GetText(GAMETEXT(0xD7));
                break;

            case 8:
                pwszStr = CGameText::GetText(GAMETEXT(0xD5));
                break;

            case 9:
            case 10:
                pwszStr = CGameText::GetText(GAMETEXT(0x37A));
                break;

            case 11:
                pwszStr = CGameText::GetText(GAMETEXT(0x379));
                break;

            default:
                ASSERT(false);
                break;
            }
        }
        break;

    case WNDTYPE_ART:
        {
            switch (ItemNo)
            {
            case 0:
                pwszStr = CGameText::GetText(GAMETEXT(0x388));
                break;

            case 1:
                pwszStr = CGameText::GetText(GAMETEXT(0x389));
                break;

            case 2:
                pwszStr = CGameText::GetText(GAMETEXT(0x38A));
                break;

            case 3:
                pwszStr = CGameText::GetText(GAMETEXT(0x38B));
                break;

            case 4:
                pwszStr = CGameText::GetText(GAMETEXT(0x37B));
                break;

            case 5:
                pwszStr = CGameText::GetText(GAMETEXT(0x37C));
                break;

            case 6:
                pwszStr = CGameText::GetText(GAMETEXT(0x37D));
                break;

            case 7:
                pwszStr = CGameText::GetText(GAMETEXT(0x37E));
                break;

            default:
                ASSERT(false);
                break;
            }
        }
        break;

    case WNDTYPE_ETC:
        {
            switch (ItemNo)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                pwszStr = CGameText::GetText(GAMETEXT(0x387));
                break;

            case 4:
                pwszStr = CGameText::GetText(GAMETEXT(0x37F));
                break;

            default:
                ASSERT(false);
                break;
            }
        }
        break;

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
        m_pTexDbTime = CTextureManager::GetRwTexture("DB_Time");
        m_pTexDbNew = CTextureManager::GetRwTexture("DB_New");
        m_pTexDbCd = CTextureManager::GetRwTexture("DB_Cd");
    };
};


void CDatabase_Container::DB_DrawNewIcon(float x, float y)
{
    m_Sprite.SetOffset(0.0f, 0.5f);
    m_Sprite.SetTexture(m_pTexDbNew);
    m_Sprite.Move(x, y - 16.0f);
	m_Sprite.Resize(32.0f, 16.0f);
    m_Sprite.SetUV(
        0.0f,
        float(m_iDbNewUVStep) * 0.25f,
        1.0f,
        (float(m_iDbNewUVStep) * 0.25f) + 0.25f
    );
    m_Sprite.SetRGBA(255, 255, 255, 255);
    m_Sprite.Draw();
    
    m_Sprite.SetUV(0.0f, 0.0f, 1.0f, 1.0f);
};


void CDatabase_Container::DB_DrawText(WNDTYPE Wndtype)
{
    if (!m_bTextureSettingFlag)
        return;

    if (Wndtype == WNDTYPE_EXIT)
        return;
    
    CSystem2D::PushRenderState();
    
    if (Wndtype == WNDTYPE_MAIN)
    {
		CGameFont::SetHeight(CGameFont::GetScreenHeight() * 1.75f);

        const int32 CategoryNum = 4;
        for (int32 i = 0; i < CategoryNum; ++i)
        {
            RwV2d TextPos = Math::VECTOR2_ZERO;
            const RwV2d aTextPos[] =
            {
                { -10.0f, -10.0f },
                { 160.0f, -10.0f },
                { -10.0f, 120.0f },
                { 160.0f, 120.0f },
            };
            static_assert(COUNT_OF(aTextPos) == CategoryNum, "update me");
            TextPos = aTextPos[i];

            if (IsDbCategoryUnlocked(CDatabaseRecord::GROUP(i)))
            {
                const wchar* pwszStr = CGameText::GetText(GAMETEXT(i + 0x376));
                CGameFont::SetRGBA(255, 180, 0, 255);
                CGameFont::Show(
                    pwszStr,
                    TextPos.x - (CGameFont::GetStringWidth(pwszStr) * 0.5f),
                    TextPos.y
                );
                
                if (!IsDbCategoryNew(CDatabaseRecord::GROUP(i)))
                {
                    const RwV2d aIconPos[] =
                    {
                        { TextPos.x - 60.0f, TextPos.y - 90.0f },
                        { TextPos.x - 45.0f, TextPos.y - 90.0f },
                        { TextPos.x - 60.0f, TextPos.y - 90.0f },
                        { TextPos.x - 45.0f, TextPos.y - 90.0f },
                    };
                    static_assert(COUNT_OF(aIconPos) == CategoryNum, "update me");
                    DB_DrawNewIcon(aIconPos[i].x, aIconPos[i].y);
                };
            };
        };
    }
    else
    {
        struct TEXTINFO
        {
            int32 ItemCount;
            RwV2d Position;
            DBITEMID::VALUE DbItemStart;
            int32 ColumnsNum;
        };

        const TEXTINFO aTextInfo[] =
        {
            { 14,   { -235.0f, -127.0f },   DBITEMID::CHARACTERSTART,   3 },
            { 12,   { -235.0f, -127.0f },   DBITEMID::ENEMYSTART,       3 },
            { 8,    { -168.0f, -89.0f },    DBITEMID::ARTSTART,         2 },
            { 5,    { -132.0f, -85.0f },    DBITEMID::ETCSTART,         2 },
        };

        const TEXTINFO* TextInfo = &aTextInfo[Wndtype - 2];

		CGameFont::SetHeight(CGameFont::GetScreenHeight() * 1.5f);

        int32 ColumnNo = 0;
        int32 ColumnMax = TextInfo->ColumnsNum - 1;
        int32 ColumnItem = 0;
        int32 DbItem = TextInfo->DbItemStart;
        for (int32 i = 0; i < TextInfo->ItemCount; ++i)
        {
            float x = (float(ColumnNo) * 161.0f) + TextInfo->Position.x;
            float y = (61.0f * float(ColumnItem)) + TextInfo->Position.y;
            
            if (IsDbViewerUnlocked(DBITEMID::VALUE(DbItem)))
            {
                float OfsY = 98.0f;
                if (Wndtype == WNDTYPE_ART)
                    OfsY = 60.0f;
                else if (Wndtype == WNDTYPE_ETC)
                    OfsY = 56.0f;

                Rt2dBBox BBox = { x, (OfsY - (float(ColumnItem) * 60.0f)), 150.0f, 40.0f };
                const wchar* pwszStr = GetDatabaseName(Wndtype, i);
                CGameFont::SetRGBA(255, 180, 0, 255);
                CGameFont::Flow(pwszStr, &BBox);
                
                if (!IsDbViewerNew(DBITEMID::VALUE(DbItem)))
                    DB_DrawNewIcon(x, y);
            }
            else
            {
                CGameFont::SetRGBA(255, 180, 0, 64);
                CGameFont::Show("?????", x, y);
            };
		
            ColumnNo = InvClamp(ColumnNo + 1, 0, ColumnMax);
            ++DbItem;
            ColumnItem = ((i + 1) / TextInfo->ColumnsNum);
        };
    };
    
    CSystem2D::PopRenderState();

    if (m_iDbNewUVStep == 3)
    {
        //
        //  New icon small pause
        //
        if (float(m_uDbAnimCnt) >= (CScreen::Framerate() * 0.5f))
        {
            m_iDbNewUVStep = 0;
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
				++m_iDbNewUVStep;
                m_uDbAnimCnt = 0;
            }
            else
            {
                ++m_uDbAnimCnt;
            };
        };    
    };
};


void CDatabase_Container::DB_DrawSundokei(void)
{
    if (!m_bTextureSettingFlag)
        return;

    m_Sprite.SetOffset(0.5f, 0.5f);
    m_Sprite.SetTexture(m_pTexDbTime);
    m_Sprite.Move(0.0f, 0.0f);
    m_Sprite.Resize(128.0f, 128.0f);
    m_Sprite.SetAlpha(255);

    switch (m_eSunadokeiState)
    {
    case SUNADOKEI_STATE_ROTATE:
        {
            uint8 AlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_uSunadokeiAnimCnt), (CScreen::Framerate() * 0.5f)));
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
            
            m_Sprite.SetUV(0.5f, 0.5f, 1.0f, 1.0f);
            m_Sprite.SetRotate(m_fSunadokeiRot * fRotStep);
            m_Sprite.DrawRotate();
            
            m_Sprite.SetUV(0.5f, 0.5f, 1.0f, 1.0f);
            m_Sprite.SetRotate(m_fSunadokeiRot);
            m_Sprite.SetAlpha(AlphaBasis);
            m_Sprite.DrawRotate();
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
            
            ASSERT((m_iSunadokeiSandStep >= 0) && (m_iSunadokeiSandStep < COUNT_OF(aSandUV)));
            
            m_Sprite.SetUV(
                aSandUV[m_iSunadokeiSandStep][0],
                aSandUV[m_iSunadokeiSandStep][1],
                aSandUV[m_iSunadokeiSandStep][2],
                aSandUV[m_iSunadokeiSandStep][3]
            );

            if (m_iSunadokeiSandStep >= 3)
                m_eSunadokeiState = SUNADOKEI_STATE_IDLE;
            else
                ++m_iSunadokeiSandStep;

            m_Sprite.Draw();
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

            ASSERT((m_iSunadokeiSandStep >= 0) && (m_iSunadokeiSandStep < COUNT_OF(aSandUV)));

            m_Sprite.SetUV(
                aSandUV[m_iSunadokeiSandStep][0],
                aSandUV[m_iSunadokeiSandStep][1],
                aSandUV[m_iSunadokeiSandStep][2],
                aSandUV[m_iSunadokeiSandStep][3]
            );
        
            m_Sprite.Draw();
            
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
    };

    m_Sprite.SetAlpha(255);
    m_Sprite.SetUV(0.0f, 0.0f, 1.0f, 1.0f);
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

    switch (m_animstep)
    {
    case ANIMSTEP_DRAW:
        {
            int32 iMessageNo = 0;

            if (m_pAnimation2D->CheckMessageGetURL("Home"))
            {
                m_bGotoViewerFlag = false;
                BeginFadeout();
            }
            else if (IsAnim2DMessageList(s_apszTextOnStr, COUNT_OF(s_apszTextOnStr), &iMessageNo))
            {
                m_iSelWindowType = (iMessageNo + 1);
            }
            else if (IsAnim2DMessageList(s_apszTextOffStr, COUNT_OF(s_apszTextOffStr), &iMessageNo))
            {
                m_iSelWindowType = 0;
            }
            else if (IsAnim2DMessageList(s_apszCategoryStr, COUNT_OF(s_apszCategoryStr), &iMessageNo))
            {
                CMenuController::KeyLock(CController::DIGITAL_LUP);
                CMenuController::KeyLock(CController::DIGITAL_LDOWN);

                if (s_pDatabase_Container->IsDbCategoryUnlocked(CDatabaseRecord::GROUP(iMessageNo)))
                    m_pAnimation2D->FlashKeyPress(CController::DIGITAL_LUP);
                else
                    m_pAnimation2D->FlashKeyPress(CController::DIGITAL_LDOWN);

                CMenuController::KeyUnlock(CController::DIGITAL_LUP);
                CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
            }
            else if (IsAnim2DMessageList(s_apszViewerNoStr, COUNT_OF(s_apszViewerNoStr), &iMessageNo))
            {
                CMenuController::KeyLock(CController::DIGITAL_LUP);
                CMenuController::KeyLock(CController::DIGITAL_LDOWN);

                if (s_pDatabase_Container->IsDbViewerUnlocked(DBITEMID::VALUE(iMessageNo + 1)))
                {
                    CDatabaseSequence::SetDBViewerNo(iMessageNo + 1);
                    s_pDatabase_Container->DB_SunadokeiSet(CDatabase_Container::SUNADOKEI_STATE_ROTATE);
                    m_pAnimation2D->FlashKeyPress(CController::DIGITAL_LUP);
                }
                else
                {
                    m_pAnimation2D->FlashKeyPress(CController::DIGITAL_LDOWN);
                };

                CMenuController::KeyUnlock(CController::DIGITAL_LUP);
                CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
            }
            else if (m_pAnimation2D->CheckMessageGetURL("Loading"))
            {
                CGameSound::PlaySE(SDCODE_SE(0x1002));
                m_bGotoViewerFlag = true;
                BeginFadeout();
            };
        }
        break;

    case ANIMSTEP_END:
        {
            if (m_bGotoViewerFlag)
                Ret((const void*)PROCLABEL_SEQ_DATABASEVIEWER);
            else
                Ret();
        }
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
};


void CDatabase::BeginFadein(void)
{
    m_pAnimation2D->SetText("DATABASE_END", "Return Home?");

    if (CGameData::Record().Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_CHARACTER))
        m_pAnimation2D->SetTexture("database_251", "DB_Cd01");
    else
        m_pAnimation2D->SetTexture("database_251", "DB_Cd02");

    if (CGameData::Record().Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_ENEMY))
        m_pAnimation2D->SetTexture("database_249", "DB_Cd01");
    else
        m_pAnimation2D->SetTexture("database_249", "DB_Cd02");

    if (CGameData::Record().Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_ART))
        m_pAnimation2D->SetTexture("database_253", "DB_Cd01");
    else
        m_pAnimation2D->SetTexture("database_253", "DB_Cd02");

    if (CGameData::Record().Database().CountUnlockedItemInGroup(CDatabaseRecord::GROUP_ETC))
        m_pAnimation2D->SetTexture("database_255", "DB_Cd01");
    else
        m_pAnimation2D->SetTexture("database_255", "DB_Cd02");
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_LEO_A))
        m_pAnimation2D->SetTexture("database_276", "DB_01");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_LEO_B))
        m_pAnimation2D->SetTexture("database_286", "DB_02");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_SLASHUUR))
        m_pAnimation2D->SetTexture("database_300", "DB_03");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_RAP_A))
        m_pAnimation2D->SetTexture("database_278", "DB_04");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_RAP_B))
        m_pAnimation2D->SetTexture("database_288", "DB_05");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_CASEY))
        m_pAnimation2D->SetTexture("database_294", "DB_06");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_MIC_A))
        m_pAnimation2D->SetTexture("database_280", "DB_07");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_MIC_B))
        m_pAnimation2D->SetTexture("database_290", "DB_08");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_KARAI))
        m_pAnimation2D->SetTexture("database_298", "DB_09");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_DON_A))
        m_pAnimation2D->SetTexture("database_282", "DB_10");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_DON_B))
        m_pAnimation2D->SetTexture("database_292", "DB_11");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_SPLINTER))
        m_pAnimation2D->SetTexture("database_296", "DB_12");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_APRIL))
        m_pAnimation2D->SetTexture("database_284", "DB_13");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_CHARACTER_USAGI))
        m_pAnimation2D->SetTexture("database_311", "DB_23");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_SHREDDER))
        m_pAnimation2D->SetTexture("database_323", "DB_14");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_ENEMYDORAKO))
        m_pAnimation2D->SetTexture("database_325", "DB_15");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_NINJA_RATS))
        m_pAnimation2D->SetTexture("database_327", "DB_16");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_TRAXIMUS))
        m_pAnimation2D->SetTexture("database_329", "DB_17");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_LEATHERHEAD))
        m_pAnimation2D->SetTexture("database_331", "DB_18");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_ELITE_FOOT))
        m_pAnimation2D->SetTexture("database_333", "DB_19");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_SPASMOSAUR))
        m_pAnimation2D->SetTexture("database_335", "DB_20");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_HUN))
        m_pAnimation2D->SetTexture("database_337", "DB_21");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_ULTIMATE_NINJA))
        m_pAnimation2D->SetTexture("database_339", "DB_22");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_ZAKO_A))
        m_pAnimation2D->SetTexture("database_341", "DB_24");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_ZAKO_B))
        m_pAnimation2D->SetTexture("database_343", "DB_25");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ENEMY_OTHERS))
        m_pAnimation2D->SetTexture("database_345", "DB_26");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ART_NY))
        m_pAnimation2D->SetTexture("database_354", "DB_27");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ART_DHOONNIB))
        m_pAnimation2D->SetTexture("database_350", "DB_28");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ART_JAPAN))
        m_pAnimation2D->SetTexture("database_356", "DB_29");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ART_KURAIYAMA))
        m_pAnimation2D->SetTexture("database_352", "DB_30");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ART_TRANS_DIMENSION))
        m_pAnimation2D->SetTexture("database_358", "DB_31");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ART_MACHINE))
        m_pAnimation2D->SetTexture("database_360", "DB_32");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ART_ANIME))
        m_pAnimation2D->SetTexture("database_362", "DB_33");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ART_CG))
        m_pAnimation2D->SetTexture("database_364", "DB_34");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_A))
        m_pAnimation2D->SetTexture("database_367", "DB_35");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_B))
        m_pAnimation2D->SetTexture("database_375", "DB_36");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_C))
        m_pAnimation2D->SetTexture("database_369", "DB_37");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_D))
        m_pAnimation2D->SetTexture("database_373", "DB_38");    
    
    if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ETC_GALLERY))
        m_pAnimation2D->SetTexture("database_371", "DB_39");
    
    CGameSound::PlayBGM(SDCODE_BGM(0x3024));
    
    CAnim2DSequence::BeginFadein();
};