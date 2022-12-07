#include "EnbuProc.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/GameText.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/Motion/MotionController.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/RenderState.hpp"


class CEnbuProc_Container
{
private:
    friend CEnbuProc;
    
public:
    CEnbuProc_Container(void);
    ~CEnbuProc_Container(void);
    void Settings(void);
    void Period(void);
    void Draw2D(void);
    void Draw3D(void);
    void EnbuBgDraw(void);
    void EnbuRankDraw(void);    
    void EnbuAtomicSet(void);
    void EnbuBandanaSet(void);
    const char* GetMvpNameStr(void) const;
    const char* GetRankNameStr(void) const;

private:
    PLAYERID::VALUE m_idPlayerMvp;
    GAMETYPES::COSTUME m_Costume;
    GAMETYPES::CLEARRANK m_RankMvp;
    GAMETYPES::CLEARRANK m_RankTotal;
    CCharacter* m_pCharacter;
    CModel* m_pModel;
    CMotionController* m_pMotionController;
    CBandanaModule* m_pBandanaModule;
    RwTexture* m_apTexture[11];
    CSprite m_sprite;
    float m_fTime;
    float m_afAnimOfsX[3];
    uint32 m_auAnimCnt[10];
    bool m_bFlagEnd;
    bool m_bFlagBandana;
    bool m_bFlagIdle;
};


CEnbuProc_Container::CEnbuProc_Container(void)
: m_idPlayerMvp(PLAYERID::ID_INVALID)
, m_Costume(GAMETYPES::COSTUME_NONE)
, m_RankMvp(GAMETYPES::CLEARRANK_NONE)
, m_RankTotal(GAMETYPES::CLEARRANK_NONE)
, m_pCharacter(nullptr)
, m_pModel(nullptr)
, m_pMotionController(nullptr)
, m_pBandanaModule(nullptr)
, m_fTime(0.0f)
, m_bFlagEnd(false)
, m_bFlagBandana(false)
, m_bFlagIdle(false)
{
    std::memset(m_apTexture, 0x00, sizeof(m_apTexture));
    std::memset(m_afAnimOfsX, 0x00, sizeof(m_afAnimOfsX));
    std::memset(m_auAnimCnt, 0x00, sizeof(m_auAnimCnt));

    int32 nMvp = CGameData::PlayResult().GetMVP();

    m_idPlayerMvp = CGameData::PlayResult().GetPlayerCharacter(nMvp);
	m_RankMvp = CGameData::PlayResult().GetPersonalRank(nMvp);
    m_RankTotal = CGameData::PlayResult().GetTotalRank();
    
    int32 nCharaIndex = CGameData::PlayResult().IndexOfChara(m_idPlayerMvp);
    m_Costume = CGameData::PlayParam().CharaInfo(nCharaIndex).m_Costume;
};


CEnbuProc_Container::~CEnbuProc_Container(void)
{
    if (m_pBandanaModule)
    {
        delete m_pBandanaModule;
        m_pBandanaModule = nullptr;
    };

    if (m_pCharacter)
    {
        delete m_pCharacter;
        m_pCharacter = nullptr;
    };

    if (m_pMotionController)
    {
        delete m_pMotionController;
        m_pMotionController = nullptr;
    };

    if (m_pModel)
    {
        CModelManager::DestroyModel(m_pModel);
        m_pModel = nullptr;
    };
};


void CEnbuProc_Container::Settings(void)
{
    const char* pszMvpName = GetMvpNameStr();

    m_pModel = CModelManager::CreateModelToon(pszMvpName);
    ASSERT(m_pModel);

    RwV3d vPos = { 0.0f, 0.5f, 0.0f };
    m_pModel->SetPosition(&vPos);
    m_pModel->UpdateFrame();

    m_pMotionController = new CMotionController(m_pModel);
    ASSERT(m_pMotionController);

    m_pCharacter = new CCharacter(pszMvpName, CCharacter::TYPE_PLAYER);
    ASSERT(m_pCharacter);

    CBandanaModule::BANDANACOLOR BandanaColor = CBandanaModule::BANDANACOLOR_BLUE;
    RwV3d vBandanaOffset = { 0.0f, 0.15f, 0.05f };
    m_bFlagBandana = true;

    switch (m_idPlayerMvp)
    {
    case PLAYERID::ID_LEO:
        BandanaColor = CBandanaModule::BANDANACOLOR_BLUE;
        break;

    case PLAYERID::ID_RAP:
        BandanaColor = CBandanaModule::BANDANACOLOR_RED;
        break;

    case PLAYERID::ID_MIC:
        BandanaColor = CBandanaModule::BANDANACOLOR_ORANGE;
        break;

    case PLAYERID::ID_DON:
        BandanaColor = CBandanaModule::BANDANACOLOR_PURPLE;
        break;

    case PLAYERID::ID_SLA:
        BandanaColor = CBandanaModule::BANDANACOLOR_SLASHUURGREY;
        vBandanaOffset = { -0.02f, 0.15f, -0.038f };
        break;

    default:
        m_bFlagBandana = false;
        break;
    };

    if (m_Costume)
        m_bFlagBandana = false;

    if (m_bFlagBandana)
    {
        m_pBandanaModule = new CBandanaModule(m_pCharacter, m_pModel, CHARACTERTYPES::BONEID_HEAD, &vBandanaOffset, BandanaColor);
        ASSERT(m_pBandanaModule);        
    };

    CMotionManager::SetCurrentMotionSet("enbu");
    
    char szBuffer[32];
    szBuffer[0] = '\0';

    std::sprintf(szBuffer, "%s_%s1", GetMvpNameStr(), GetRankNameStr());
    m_pMotionController->AddMotion(szBuffer);

    std::sprintf(szBuffer, "%s_%s2", GetMvpNameStr(), GetRankNameStr());
    m_pMotionController->AddMotion(szBuffer);

    std::sprintf(szBuffer, "%s_%s1", GetMvpNameStr(), GetRankNameStr());
    m_pMotionController->SetCurrentMotion(szBuffer, 0.0f, 0.0f, 0.0f, false);
    m_pMotionController->SetPlaymode(CMotionController::PLAYMODE_ONESHOT);
    m_pMotionController->Play();

    CTextureManager::SetCurrentTextureSet("enbu2d");
    m_apTexture[0] = CTextureManager::GetRwTexture("enbu_bg_common_a");
    m_apTexture[1] = CTextureManager::GetRwTexture("enbu_bg_common_b");
    m_apTexture[4] = CTextureManager::GetRwTexture("rank_e");
    m_apTexture[5] = CTextureManager::GetRwTexture("rank_d");
    m_apTexture[6] = CTextureManager::GetRwTexture("rank_c");
    m_apTexture[7] = CTextureManager::GetRwTexture("rank_b");
    m_apTexture[8] = CTextureManager::GetRwTexture("rank_a");
    m_apTexture[9] = CTextureManager::GetRwTexture("rank_s");
    m_apTexture[10] = CTextureManager::GetRwTexture("rank_ss");

    switch (m_idPlayerMvp)
    {
    case PLAYERID::ID_LEO:
    case PLAYERID::ID_SLA:
        CTextureManager::SetCurrentTextureSet("enbu2d_leo");
        m_apTexture[2] = CTextureManager::GetRwTexture("enbu_bg_leo1");
        m_apTexture[3] = CTextureManager::GetRwTexture("enbu_bg_leo");
        break;

    case PLAYERID::ID_RAP:
    case PLAYERID::ID_CAS:
        CTextureManager::SetCurrentTextureSet("enbu2d_rap");
        m_apTexture[2] = CTextureManager::GetRwTexture("enbu_bg_rap1");
        m_apTexture[3] = CTextureManager::GetRwTexture("enbu_bg_rap");
        break;

    case PLAYERID::ID_MIC:
    case PLAYERID::ID_KAR:
        CTextureManager::SetCurrentTextureSet("enbu2d_mic");
        m_apTexture[2] = CTextureManager::GetRwTexture("enbu_bg_mic1");
        m_apTexture[3] = CTextureManager::GetRwTexture("enbu_bg_mic");
        break;

    case PLAYERID::ID_DON:
    case PLAYERID::ID_SPL:
        CTextureManager::SetCurrentTextureSet("enbu2d_don");
        m_apTexture[2] = CTextureManager::GetRwTexture("enbu_bg_don1");
        m_apTexture[3] = CTextureManager::GetRwTexture("enbu_bg_don");
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CEnbuProc_Container::Period(void)
{
    float dt = CGameProperty::GetElapsedTime();
    
    m_pMotionController->Update(dt);
    EnbuBandanaSet();
    EnbuAtomicSet();

    if (!m_bFlagIdle)
    {
        if (m_pMotionController->GetCurrentMotionEndTime() < 3.0f && m_fTime > 3.0f)
            m_bFlagIdle = true;

		m_fTime += CGameProperty::GetElapsedTime();
    };

    if (!m_bFlagEnd)
    {
        if (m_pMotionController->GetCurrentMotionEndTime() <= m_pMotionController->GetTime())
        {
            m_bFlagEnd = true;
            
            if (m_pMotionController->GetCurrentMotionEndTime() > 3.0f)
                m_bFlagIdle = true;
            
            m_pMotionController->Pause();

            char szBuffer[32];
            szBuffer[0] = '\0';

            std::sprintf(szBuffer, "%s_%s2", GetMvpNameStr(), GetRankNameStr());
            m_pMotionController->SetCurrentMotion(szBuffer, 0.0f, 0.0f, 0.0f, false);
            m_pMotionController->SetPlaymode(CMotionController::PLAYMODE_REPEAT);
            m_pMotionController->Play();

            if (m_RankMvp >= GAMETYPES::CLEARRANK_A)
            {
                switch (m_idPlayerMvp)
                {
                case PLAYERID::ID_LEO:
                    CGameSound::PlaySE(SDCODE_SE(8192));
                    break;

                case PLAYERID::ID_RAP:
                    CGameSound::PlaySE(SDCODE_SE(8193));
                    break;

                case PLAYERID::ID_MIC:
                    CGameSound::PlaySE(SDCODE_SE(8194));
                    break;

                case PLAYERID::ID_DON:
                    CGameSound::PlaySE(SDCODE_SE(8195));
                    break;
                };
            };
        };
    };
};


void CEnbuProc_Container::Draw2D(void)
{
    CRenderStateManager::SetDefault();
    CSystem2D::PushRenderState();

    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESS, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATEBORDERCOLOR, 0);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREPERSPECTIVE, true);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREFILTER, rwFILTERLINEAR);
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATESHADEMODE, rwSHADEMODEGOURAUD);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

    m_sprite.SetZ(RwIm2DGetFarScreenZMacro());
    EnbuBgDraw();
    
    m_sprite.SetZ(RwIm2DGetNearScreenZMacro());
    EnbuRankDraw();

    if (m_bFlagIdle)
    {
        const wchar* pwszText = CGameText::GetText(GAMETEXT::VALUE(801));

        CGameFont::m_fHeight = CGameFont::GetScreenSize() / 149.0f;
        CGameFont::SetRGBA(255, 180, 0, 255);
        CGameFont::Show(
            pwszText,
            -(CGameFont::GetStringWidth(pwszText) * 0.5f),
            70.0f
        );
    }
    else
    {
        const wchar* pwszText = CGameText::GetText(GAMETEXT::VALUE(800));

        CGameFont::m_fHeight = CGameFont::GetScreenSize() / 179.0f;
        CGameFont::SetRGBA(255, 180, 0, 255);
        CGameFont::Show(
            pwszText,
            -(CGameFont::GetStringWidth(pwszText) * 0.5f),
            40.0f
        );

        switch (m_idPlayerMvp)
        {
        case PLAYERID::ID_LEO:
            pwszText = CGameText::GetText(GAMETEXT::VALUE(17));
            break;

        case PLAYERID::ID_RAP:
            pwszText = CGameText::GetText(GAMETEXT::VALUE(18));
            break;

        case PLAYERID::ID_MIC:
            pwszText = CGameText::GetText(GAMETEXT::VALUE(19));
            break;
            
        case PLAYERID::ID_DON:
            pwszText = CGameText::GetText(GAMETEXT::VALUE(20));
            break;

        case PLAYERID::ID_SLA:
            pwszText = CGameText::GetText(GAMETEXT::VALUE(24));
            break;

        case PLAYERID::ID_CAS:
            pwszText = CGameText::GetText(GAMETEXT::VALUE(22));
            break;

        case PLAYERID::ID_KAR:
            pwszText = CGameText::GetText(GAMETEXT::VALUE(23));
            break;

        case PLAYERID::ID_SPL:
            pwszText = CGameText::GetText(GAMETEXT::VALUE(21));
            break;

        default:
            ASSERT(false);
            break;
        };

        CGameFont::m_fHeight = CGameFont::GetScreenSize() / 179.0f;
        CGameFont::SetRGBA(PLAYERID::GetColor(m_idPlayerMvp));
        CGameFont::Show(
            pwszText,
            -(CGameFont::GetStringWidth(pwszText) * 0.5f),
            65.0f
        );
    };

    CSystem2D::PopRenderState();
};


void CEnbuProc_Container::Draw3D(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

    ASSERT(m_pModel);
    m_pModel->Draw();

    if (m_bFlagBandana)
    {
        ASSERT(m_pBandanaModule);
        m_pBandanaModule->Draw();
    };
};


void CEnbuProc_Container::EnbuBgDraw(void)
{    
    m_sprite.SetTexture(m_apTexture[3]);
    m_sprite.SetRGBA(255, 255, 255, 128);
    m_sprite.Move(0.0f, 0.0f);
    m_sprite.Resize(640.0f, 512.0f);
    m_sprite.Draw();

    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);

    for (int32 i = COUNT_OF(m_afAnimOfsX); i > 0; --i)
    {
        int32 nIndex = i - 1;

        m_sprite.SetTexture(m_apTexture[nIndex]);
        m_sprite.SetRGBA(255, 255, 255, 128);
        m_sprite.Move(m_afAnimOfsX[nIndex], 0.0f);
        m_sprite.Resize(1024.0f, 512.0f);
        m_sprite.Draw();

        m_sprite.SetTexture(m_apTexture[nIndex]);
        m_sprite.SetRGBA(255, 255, 255, 128);
        m_sprite.Move(m_afAnimOfsX[nIndex] - 1024.0f, 0.0f);
        m_sprite.Resize(1024.0f, 512.0f);
        m_sprite.Draw();
    };

    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);

    float fStep = 16.0f;

    m_afAnimOfsX[0] += (fStep * 4.0f);
    if (m_afAnimOfsX[0] >= 1024.0f)
        m_afAnimOfsX[0] = 0.0f;

    m_afAnimOfsX[1] += (fStep * 2.0f);
    if (m_afAnimOfsX[1] >= 1024.0f)
        m_afAnimOfsX[1] = 0.0f;

    m_afAnimOfsX[2] += fStep;
    if (m_afAnimOfsX[2] >= 1024.0f)
        m_afAnimOfsX[2] = 0.0f;
};


void CEnbuProc_Container::EnbuRankDraw(void)
{
    if (m_bFlagIdle)
    {
        switch (m_RankTotal)
        {
        case GAMETYPES::CLEARRANK_E:
            {
                float fDuration = CScreen::Framerate() * 0.75f;
                
				if (m_auAnimCnt[0] < uint32(fDuration))
					++m_auAnimCnt[0];

                uint8 uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[0]), fDuration));

                m_sprite.SetOffset(0.5f, 0.5f);
                m_sprite.SetTexture(m_apTexture[m_RankTotal + 3]);
                m_sprite.Move(0.0f, 120.0f);
                m_sprite.Resize(128.0f, 64.0f);
                m_sprite.SetRGBA(255, 255, 255, uAlphaBasis);
                m_sprite.Draw();
                
                m_sprite.SetAlpha(255);
            }
            break;

        default:
            {
                for (int32 i = 0; i < COUNT_OF(m_auAnimCnt); ++i)
                {
                    float fDuration = ((60.0f - float(i * 2)) * CScreen::TimerStride() * CScreen::Framerate());
					float fCnt = float(m_auAnimCnt[i]);
                    float fDelta = (fCnt / fDuration);

                    if (m_auAnimCnt[i] < uint32(fDuration))
                        ++m_auAnimCnt[i];

                    float fSize = fDelta + fDelta + 6.0f;

                    m_sprite.SetOffset(0.5f, 0.5f);
                    m_sprite.SetTexture(m_apTexture[m_RankTotal + 3]);
                    m_sprite.Move(0.0f, 120.0f);
                    m_sprite.SetRGBA(255, 255, 255, uint8(fDelta * 255.0f));
                    m_sprite.Resize(
                        fSize * 128.0f - (fSize * 128.0f - 128.0f) * fDelta,
                        fSize * 64.0f - (fSize * 64.0f - 64.0f) * fDelta
                    );

                    if (fDelta < 1.0f && i != 9)
                    {
                        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
                        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
                        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
                    };

                    m_sprite.Draw();

                    if (fDelta < 1.0f && i != 9)
                    {
                        RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
                        RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
                        RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
                    };
                };
            }
            break;
        };
    }
    else
    {
        if (m_RankMvp)
        {
            m_sprite.SetOffset(0.5f, 0.5f);
            m_sprite.SetTexture(m_apTexture[m_RankMvp + 3]);
            m_sprite.Move(0.0f, 120.0f);
            m_sprite.Resize(128.0f, 64.0f);
            m_sprite.SetRGBA(255, 255, 255, 255);
            m_sprite.Draw();
        };
    };
};


void CEnbuProc_Container::EnbuAtomicSet(void)
{
    switch (m_idPlayerMvp)
    {
    case PLAYERID::ID_LEO:
        {
            switch (m_Costume)
            {
            case GAMETYPES::COSTUME_SAMURAI:
                {
                    m_pModel->SetPartsDrawEnable(0, true);
                    m_pModel->SetPartsDrawEnable(1, true);
                    m_pModel->SetPartsDrawEnable(2, true);
                    m_pModel->SetPartsDrawEnable(3, true);
                    m_pModel->SetPartsDrawEnable(4, true);
                    m_pModel->SetPartsDrawEnable(5, true);
                    m_pModel->SetPartsDrawEnable(6, false);
                    m_pModel->SetPartsDrawEnable(7, false);
                    m_pModel->SetPartsDrawEnable(8, true);
                    m_pModel->SetPartsDrawEnable(9, true);
                    m_pModel->SetPartsDrawEnable(10, true);
                    m_pModel->SetPartsDrawEnable(11, true);

                    if (m_RankMvp == GAMETYPES::CLEARRANK_E)
                    {
                        m_pModel->SetPartsDrawEnable(6, true);
                        m_pModel->SetPartsDrawEnable(8, false);
                    }
                    else if (m_RankMvp == GAMETYPES::CLEARRANK_C && m_fTime > 1.5f)
                    {
                        m_pModel->SetPartsDrawEnable(0, true);
                        m_pModel->SetPartsDrawEnable(1, true);
                        m_pModel->SetPartsDrawEnable(2, true);
                        m_pModel->SetPartsDrawEnable(3, true);
                        m_pModel->SetPartsDrawEnable(4, true);
                        m_pModel->SetPartsDrawEnable(5, true);
                        m_pModel->SetPartsDrawEnable(6, true);
                        m_pModel->SetPartsDrawEnable(7, true);
                        m_pModel->SetPartsDrawEnable(8, false);
                        m_pModel->SetPartsDrawEnable(9, false);
                        m_pModel->SetPartsDrawEnable(10, true);
                        m_pModel->SetPartsDrawEnable(11, true);
                    }
                    else if (m_RankMvp == GAMETYPES::CLEARRANK_D && m_fTime < 2.75f)
                    {
                        m_pModel->SetPartsDrawEnable(0, true);
                        m_pModel->SetPartsDrawEnable(1, true);
                        m_pModel->SetPartsDrawEnable(2, true);
                        m_pModel->SetPartsDrawEnable(3, true);
                        m_pModel->SetPartsDrawEnable(4, true);
                        m_pModel->SetPartsDrawEnable(5, true);
                        m_pModel->SetPartsDrawEnable(6, true);
                        m_pModel->SetPartsDrawEnable(7, false);
                        m_pModel->SetPartsDrawEnable(8, false);
                        m_pModel->SetPartsDrawEnable(9, true);
                        m_pModel->SetPartsDrawEnable(10, true);
                        m_pModel->SetPartsDrawEnable(11, true);
                    };
                }
                break;

            case GAMETYPES::COSTUME_NEXUS:
                {
                    m_pModel->SetPartsDrawEnable(0, true);
                    m_pModel->SetPartsDrawEnable(1, true);
                    m_pModel->SetPartsDrawEnable(2, true);
                    m_pModel->SetPartsDrawEnable(3, true);

                    if (m_RankMvp == GAMETYPES::CLEARRANK_E)
                        m_pModel->SetPartsDrawEnable(0, false);
                }
                break;

            default:
                {
                    m_pModel->SetPartsDrawEnable(0, true);
                    m_pModel->SetPartsDrawEnable(1, true);
                    m_pModel->SetPartsDrawEnable(2, true);
                    m_pModel->SetPartsDrawEnable(3, true);
                    m_pModel->SetPartsDrawEnable(4, false);
                    m_pModel->SetPartsDrawEnable(5, false);

                    if (m_RankMvp == GAMETYPES::CLEARRANK_E)
                    {
                        m_pModel->SetPartsDrawEnable(2, false);
                        m_pModel->SetPartsDrawEnable(5, true);
                    }
                    else if (m_RankMvp == GAMETYPES::CLEARRANK_C && m_fTime > 1.5f)
                    {
                        m_pModel->SetPartsDrawEnable(0, true);
                        m_pModel->SetPartsDrawEnable(1, false);
                        m_pModel->SetPartsDrawEnable(2, false);
                        m_pModel->SetPartsDrawEnable(3, true);
                        m_pModel->SetPartsDrawEnable(4, true);
                        m_pModel->SetPartsDrawEnable(5, true);
                    }
                    else if (m_RankMvp == GAMETYPES::CLEARRANK_D && m_fTime < 2.75f)
                    {
                        m_pModel->SetPartsDrawEnable(0, true);
                        m_pModel->SetPartsDrawEnable(1, true);
                        m_pModel->SetPartsDrawEnable(2, false);
                        m_pModel->SetPartsDrawEnable(3, true);
                        m_pModel->SetPartsDrawEnable(4, false);
                        m_pModel->SetPartsDrawEnable(5, true);
                    };
                }
                break;
            };
        }
        break;

    case PLAYERID::ID_RAP:
        {
            ;
        }
        break;

    case PLAYERID::ID_MIC:
        {
            ;
        }
        break;

    case PLAYERID::ID_DON:
        {
            ;
        }
        break;

    case PLAYERID::ID_SLA:
        {
            ;
        }
        break;

    case PLAYERID::ID_CAS:
        {
            bool bEnable = false;
            
            for (int32 i = SECRETID::CHALLENGESTART; i < SECRETID::CHALLENGEMAX; ++i)
            {
                if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::VALUE(i)))
                    bEnable = true;
            };

            m_pModel->SetPartsDrawEnable(3, bEnable);
        }
        break;

    case PLAYERID::ID_KAR:
        {
            m_pModel->SetPartsDrawEnable(2, true);
            m_pModel->SetPartsDrawEnable(3, true);

            if (m_RankMvp == GAMETYPES::CLEARRANK_A)
            {
                m_pModel->SetPartsDrawEnable(2, false);
                m_pModel->SetPartsDrawEnable(3, false);
            }
            else if (m_RankMvp == GAMETYPES::CLEARRANK_S ||
                     m_RankMvp == GAMETYPES::CLEARRANK_SS)
            {
                m_pModel->SetPartsDrawEnable(2, false);
            };
        }
        break;

    case PLAYERID::ID_SPL:
        {
            ;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CEnbuProc_Container::EnbuBandanaSet(void)
{
    if (m_bFlagBandana)
    {
        ASSERT(m_pBandanaModule);
        
        RwV3d vBonePos = *m_pModel->GetBonePositionFromID(CHARACTERTYPES::BONEID_HEAD);
        RwV3d vWind = { 3.5f, vBonePos.y - 1.0f, -1.0f };

        if (m_bFlagEnd)
            vWind.y = vBonePos.y - 1.5f;

        m_pBandanaModule->SetWind(&vWind);
        m_pBandanaModule->Run();
    };
};


const char* CEnbuProc_Container::GetMvpNameStr(void) const
{
    const char* pszResult = nullptr;

    switch (m_idPlayerMvp)
    {
    case PLAYERID::ID_LEO:
        pszResult = "leo";
        break;
        
    case PLAYERID::ID_RAP:
        pszResult = "rap";
        break;
        
    case PLAYERID::ID_MIC:
        pszResult = "mic";
        break;
        
    case PLAYERID::ID_DON:
        pszResult = "don";
        break;
        
    case PLAYERID::ID_SLA:
        pszResult = "sls";
        break;
        
    case PLAYERID::ID_CAS:
        pszResult = "cas";
        break;
        
    case PLAYERID::ID_KAR:
        pszResult = "kri";
        break;
        
    case PLAYERID::ID_SPL:
        pszResult = "spl";
        break;

    default:
        ASSERT(false);
        break;
    };

    return pszResult;
};


const char* CEnbuProc_Container::GetRankNameStr(void) const
{
    const char* pszResult = nullptr;
    
    switch (m_RankMvp)
    {
    case GAMETYPES::CLEARRANK_E:
        pszResult = "E";
        break;

    case GAMETYPES::CLEARRANK_D:
        pszResult = "D";
        break;

    case GAMETYPES::CLEARRANK_C:
        pszResult = "C";
        break;

    case GAMETYPES::CLEARRANK_B:
        pszResult = "B";
        break;

    case GAMETYPES::CLEARRANK_A:
        pszResult = "A";
        break;

    case GAMETYPES::CLEARRANK_S:
        pszResult = "S";
        break;

    case GAMETYPES::CLEARRANK_SS:
        pszResult = "SS";
        break;
        
    default:
        ASSERT(false);
        break;
    };

    return pszResult;
};


static CEnbuProc_Container* s_pEnbuProcContainer = nullptr;


static inline CEnbuProc_Container& EnbuProcContainer(void)
{
    ASSERT(s_pEnbuProcContainer);
    return *s_pEnbuProcContainer;
};


/*static*/ void CEnbuProc::Initialize(void)
{
    if (!s_pEnbuProcContainer)
    {
        s_pEnbuProcContainer = new CEnbuProc_Container;
    };
};


/*static*/ void CEnbuProc::Terminate(void)
{
    if (s_pEnbuProcContainer)
    {
        delete s_pEnbuProcContainer;
        s_pEnbuProcContainer = nullptr;
    };
};


/*static*/ void CEnbuProc::Settings(void)
{
    EnbuProcContainer().Settings();
};


/*static*/ void CEnbuProc::Period(void)
{
    EnbuProcContainer().Period();
};


/*static*/ void CEnbuProc::Draw2D(void)
{
    EnbuProcContainer().Draw2D();
};


/*static*/ void CEnbuProc::Draw3D(void)
{
    EnbuProcContainer().Draw3D();
};


/*static*/ PLAYERID::VALUE CEnbuProc::GetEnbuMvp(void)
{
    return EnbuProcContainer().m_idPlayerMvp;
};


/*static*/ GAMETYPES::CLEARRANK CEnbuProc::GetEnbuRank(void)
{
    return EnbuProcContainer().m_RankMvp;
};


/*static*/ GAMETYPES::COSTUME CEnbuProc::GetEnbuCostume(void)
{
    return EnbuProcContainer().m_Costume;
};


/*static*/ float CEnbuProc::GetEnbuTime(void)
{
    return EnbuProcContainer().m_fTime;
};


/*static*/ float CEnbuProc::GetEnbuEndTime(void)
{
	return EnbuProcContainer().m_pMotionController->GetCurrentMotionEndTime();
};


/*static*/ bool CEnbuProc::IsEnbuIdle(void)
{
    return EnbuProcContainer().m_bFlagIdle;
};