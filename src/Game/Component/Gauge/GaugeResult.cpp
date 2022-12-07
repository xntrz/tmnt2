#include "GaugeResult.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Sound/MessageManager.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"


class CGaugeResult_Container
{
public:
    CGaugeResult_Container(void);
    ~CGaugeResult_Container(void);
    void Period(void);
    void Draw(void);
    void SetMissionResult(CGaugeResult::RESULTREQ req);
    void SetMissionResult(CGaugeResult::RESULTREQ req, int32 nPlayerNo);
    bool IsMissionResultEnd(void) const;
    void DrawNormal(void);
    void DrawNexus(void);
    void CallFailMessage(void) const;
    RwTexture* GetFailTexture(void) const;

protected:
    uint32 m_uAnimCnt;
    uint32 m_uAnimStep;
    CGaugeResult::RESULTREQ m_ReqMissionResult;
    int32 m_nReqFailedPlayer;
    bool m_bSettingFlag;
    float m_fSpriteRot;
    CSprite m_sprite;
    RwTexture* m_apTexture[10];
};


CGaugeResult_Container::CGaugeResult_Container(void)
: m_uAnimCnt(0)
, m_uAnimStep(0)
, m_ReqMissionResult(CGaugeResult::RESULTREQ_NONE)
, m_nReqFailedPlayer(0)
, m_bSettingFlag(false)
, m_fSpriteRot(0.0f)
{
    std::memset(m_apTexture, 0x00, sizeof(m_apTexture)); 
};


CGaugeResult_Container::~CGaugeResult_Container(void)
{
    ;
};


void CGaugeResult_Container::Period(void)
{
    if (m_bSettingFlag)
        return;

    m_bSettingFlag = true;

    CTextureManager::SetCurrentTextureSet("gg_clear");

    if (CGameData::Attribute().GetGamemode() == GAMETYPES::GAMEMODE_NEXUS)
    {
        m_apTexture[0] = CTextureManager::GetRwTexture("bt_youwin");
        m_apTexture[1] = CTextureManager::GetRwTexture("bt_youlose");
    }
    else
    {
        m_apTexture[0] = CTextureManager::GetRwTexture("gg_cleare");
        m_apTexture[1] = CTextureManager::GetRwTexture("gg_failed");
    };

    m_apTexture[2] = CTextureManager::GetRwTexture("gg_fai_leo");
    m_apTexture[3] = CTextureManager::GetRwTexture("gg_fai_rap");
    m_apTexture[4] = CTextureManager::GetRwTexture("gg_fai_mic");
    m_apTexture[5] = CTextureManager::GetRwTexture("gg_fai_don");
    m_apTexture[6] = CTextureManager::GetRwTexture("gg_fai_sla");
    m_apTexture[7] = CTextureManager::GetRwTexture("gg_fai_cas");
    m_apTexture[8] = CTextureManager::GetRwTexture("gg_fai_kar");
    m_apTexture[9] = CTextureManager::GetRwTexture("gg_fai_spl");

    for (int32 i = 0;i < COUNT_OF(m_apTexture); ++i)
        ASSERT(m_apTexture[i]);
};


void CGaugeResult_Container::Draw(void)
{
    if (!m_bSettingFlag)
        return;

    if (IsMissionResultEnd())
        return;

    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESS, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATEBORDERCOLOR, 0);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREPERSPECTIVE, true);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREFILTER, rwFILTERLINEAR);
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATESHADEMODE, rwSHADEMODEGOURAUD);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

    if (CGameData::Attribute().GetGamemode() == GAMETYPES::GAMEMODE_NEXUS)
        DrawNexus();
    else
        DrawNormal();

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATESHADEMODE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREFILTER);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREPERSPECTIVE);
    RENDERSTATE_POP(rwRENDERSTATEBORDERCOLOR);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESS);
};


void CGaugeResult_Container::SetMissionResult(CGaugeResult::RESULTREQ req)
{
    m_uAnimCnt = 0;
    m_uAnimStep = 0;
    m_ReqMissionResult = req;
    m_nReqFailedPlayer = 0;
};


void CGaugeResult_Container::SetMissionResult(CGaugeResult::RESULTREQ req, int32 nPlayerNo)
{
    m_uAnimCnt = 0;
    m_uAnimStep = 0;
    m_ReqMissionResult = req;
    m_nReqFailedPlayer = nPlayerNo;
};


bool CGaugeResult_Container::IsMissionResultEnd(void) const
{
    return (m_ReqMissionResult == CGaugeResult::RESULTREQ_NONE);
};


void CGaugeResult_Container::DrawNormal(void)
{
    float fWidth = 512.0f;
    float fHeight = 256.0f;
    uint8 uAlphaBasis = 255;
    
    float fWidth_f = 256.0f;
    float fHeight_f = 128.0f;
    uint8 uAlphaBasis_f = 255;
    
    float fT = float(m_uAnimCnt);
    float x = 0.0f;
    float y = 0.0f;
    float x_f = 0.0f;
    float y_f = 0.0f;
    
    CGaugeResult::RESULTREQ req = m_ReqMissionResult;
    
    switch (req)
    {
    case CGaugeResult::RESULTREQ_SUCCESS:
        {
            x = 0.0f;
            y = 0.0f;
            fWidth = 512.0f;
            fHeight = 256.0f;

            switch (m_uAnimStep)
            {
            case 0:
                {
                    float fDuration = CScreen::Framerate() * 0.316f;
                    
                    fWidth = Math::LinearTween(1228.8f, -716.8f, fT, fDuration);
                    fHeight = Math::LinearTween(614.4f, -358.4f, fT, fDuration);
                    uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, fT, fDuration));

                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_uAnimCnt = 0;
                        m_uAnimStep = 1;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;

            case 1:
                {
                    float fDuration = CScreen::Framerate() * 0.03f;

                    fWidth = Math::LinearTween(512.0f, 25.6f, fT, fDuration);
                    fHeight = Math::LinearTween(256.0f, 12.8f, fT, fDuration);

                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_uAnimCnt = 0;
                        m_uAnimStep = 2;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;

            case 2:
                {
                    float fDuration = CScreen::Framerate() * 0.03f;

                    fWidth = Math::LinearTween(537.6f, -25.6f, fT, fDuration);
                    fHeight = Math::LinearTween(268.8f, -12.8f, fT, fDuration);

                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_uAnimCnt = 0;
                        m_uAnimStep = 3;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;
                
            case 3:
                {
                    float fDuration = CScreen::Framerate() * 2.0f;
                    
                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_ReqMissionResult = CGaugeResult::RESULTREQ_NONE;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;
            };
        }
        break;

    case CGaugeResult::RESULTREQ_FAIL:
        {
            x = 50.0f;
            y = 0.0f;
            x_f = -60.0f;
            y_f = -57.0f;
			uAlphaBasis_f = 0;
            fWidth = 512.0f;
            fHeight = 128.0f;
            fWidth_f = 256.0f;
            fHeight_f = 128.0f;
            
            switch (m_uAnimStep)
            {
            case 0:
                {
                    float fDuration = CScreen::Framerate() * 0.316f;

                    fWidth = Math::LinearTween(1228.8f, -716.8f, fT, fDuration);
                    fHeight = Math::LinearTween(307.2f, -179.2f, fT, fDuration);
                    uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, fT, fDuration));

                    CallFailMessage();

                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_uAnimCnt = 0;
                        m_uAnimStep = 1;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;

            case 1:
                {
                    float fDuration = CScreen::Framerate() * 0.03f;

                    fWidth = Math::LinearTween(512.0f, 25.6f, fT, fDuration);
                    fHeight = Math::LinearTween(128.0f, 6.4f, fT, fDuration);

                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_uAnimCnt = 0;
                        m_uAnimStep = 2;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;

            case 2:
                {
                    float fDuration = CScreen::Framerate() * 0.03f;

                    fWidth = Math::LinearTween(537.6f, -25.6f, fT, fDuration);
                    fHeight = Math::LinearTween(134.4f, -6.4f, fT, fDuration);

                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_uAnimCnt = 0;
                        m_uAnimStep = 3;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;

            case 3:
                {
                    float fDuration = CScreen::Framerate() * 0.316f;

                    fWidth_f = Math::LinearTween(614.4f, -358.4f, fT, fDuration);
                    fHeight_f = Math::LinearTween(307.2f, -179.2f, fT, fDuration);
                    uAlphaBasis_f = uint8(Math::LinearTween(0.0f, 255.0f, fT, fDuration));

                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_uAnimCnt = 0;
                        m_uAnimStep = 4;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;

            case 4:
                {
                    float fDuration = CScreen::Framerate() * 0.03f;

                    fWidth_f = Math::LinearTween(256.0f, 12.8f, fT, fDuration);
                    fHeight_f = Math::LinearTween(128.0f, 6.4f, fT, fDuration);
                    uAlphaBasis_f = 255;
                    
                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_uAnimCnt = 0;
                        m_uAnimStep = 5;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;

            case 5:
                {
                    float fDuration = CScreen::Framerate() * 0.03f;

                    fWidth_f = Math::LinearTween(268.8f, -12.8f, fT, fDuration);
                    fHeight_f = Math::LinearTween(134.4f, -6.4f, fT, fDuration);
                    uAlphaBasis_f = 255;
                    
                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_uAnimCnt = 0;
                        m_uAnimStep = 6;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;

            case 6:
                {
                    float fDuration = CScreen::Framerate() * 2.0f;

					uAlphaBasis_f = 255;

                    if (m_uAnimCnt >= uint32(fDuration))
                    {
                        m_ReqMissionResult = CGaugeResult::RESULTREQ_NONE;
                    }
                    else
                    {
                        ++m_uAnimCnt;
                    };
                }
                break;
            };
        }        
        break;

    default:
        ASSERT(false);
        break;
    };

    if (req == CGaugeResult::RESULTREQ_SUCCESS)
        m_sprite.SetTexture(m_apTexture[0]);
    else
        m_sprite.SetTexture(m_apTexture[1]);

    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.Move(x, y);
    m_sprite.SetRGBA(255, 255, 255, uAlphaBasis);
    m_sprite.Resize(fWidth, fHeight);
    m_sprite.Draw();

    if (req == CGaugeResult::RESULTREQ_FAIL)
    {
        m_sprite.SetTexture(GetFailTexture());
        m_sprite.Move(x_f, y_f);
        m_sprite.SetRGBA(255, 255, 255, uAlphaBasis_f);
        m_sprite.Resize(fWidth_f, fHeight_f);
        m_sprite.Draw();
    };
};


void CGaugeResult_Container::DrawNexus(void)
{
    float fWidth = 512.0f;
    float fHeight = 128.0f;
    uint8 uAlphaBasis = 255;
    CGaugeResult::RESULTREQ req = m_ReqMissionResult;
    float fT = float(m_uAnimCnt);

    switch (m_uAnimStep)
    {
    case 0:
        {
            float fDuration = CScreen::Framerate();
            
            fWidth = Math::LinearTween(0.0f, 563.0f, fT, fDuration);
            fHeight = Math::LinearTween(0.0f, 140.0f, fT, fDuration);
            m_fSpriteRot = Math::LinearTween(0.0f, 18.53f, fT, fDuration);
            uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, fT, fDuration));
            
            if (m_uAnimCnt >= uint32(fDuration))
            {
                m_uAnimCnt = 0;
                m_uAnimStep = 1;
            }
            else
            {
                ++m_uAnimCnt;
            };
        }
        break;

    case 1:
        {
            float fDuration = CScreen::Framerate() * 0.03f;

            fWidth = Math::LinearTween(563.0f, -51.0f, fT, fDuration);
            fHeight = Math::LinearTween(140.0f, -12.0f, fT, fDuration);

            if (m_uAnimCnt >= uint32(fDuration))
            {
                m_uAnimCnt = 0;
                m_uAnimStep = 2;
            }
            else
            {
                ++m_uAnimCnt;
            };
        }
        break;

    case 2:
        {
            float fDuration = CScreen::Framerate() * 2.0f;
            
            if (m_uAnimCnt >= uint32(fDuration))
                m_ReqMissionResult = CGaugeResult::RESULTREQ_NONE;
            else
                ++m_uAnimCnt;
        }
        break;
    };

    if (req == CGaugeResult::RESULTREQ_SUCCESS)
        m_sprite.SetTexture(m_apTexture[0]);
    else
        m_sprite.SetTexture(m_apTexture[1]);

    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.Move(0.0f, 0.0f);
    m_sprite.SetRGBA(255, 255, 255, uAlphaBasis);
    m_sprite.SetRotate(m_fSpriteRot);
    m_sprite.Resize(fWidth, fHeight);
    m_sprite.DrawRotate();
};


void CGaugeResult_Container::CallFailMessage(void) const
{
    switch (CGameProperty::Player(m_nReqFailedPlayer).GetCurrentCharacterID())
    {
    case PLAYERID::ID_LEO:
        CMessageManager::Request(SEGROUPID::VALUE(47), PLAYERID::VALUE(-1));
        break;

    case PLAYERID::ID_RAP:
        CMessageManager::Request(SEGROUPID::VALUE(48), PLAYERID::VALUE(-1));
        break;

    case PLAYERID::ID_MIC:
        CMessageManager::Request(SEGROUPID::VALUE(49), PLAYERID::VALUE(-1));
        break;

    case PLAYERID::ID_DON:
        CMessageManager::Request(SEGROUPID::VALUE(50), PLAYERID::VALUE(-1));
        break;

    case PLAYERID::ID_SLA:
        CMessageManager::Request(SEGROUPID::VALUE(54), PLAYERID::VALUE(-1));
        break;

    case PLAYERID::ID_CAS:
        CMessageManager::Request(SEGROUPID::VALUE(51), PLAYERID::VALUE(-1));
        break;

    case PLAYERID::ID_KAR:
        CMessageManager::Request(SEGROUPID::VALUE(52), PLAYERID::VALUE(-1));
        break;

    case PLAYERID::ID_SPL:
        CMessageManager::Request(SEGROUPID::VALUE(53), PLAYERID::VALUE(-1));
        break;

    default:
        ASSERT(false);
        break;
    };
};


RwTexture* CGaugeResult_Container::GetFailTexture(void) const
{
    int32 nIndex = CGameProperty::Player(m_nReqFailedPlayer).GetCurrentCharacterID() + 2;
    ASSERT(nIndex >= 0 && nIndex < COUNT_OF(m_apTexture));
    
    return m_apTexture[nIndex];
};


static CGaugeResult_Container* s_pGaugeResultContainer = nullptr;


static CGaugeResult_Container& GaugeResultContainer(void)
{
    ASSERT(s_pGaugeResultContainer);
    return *s_pGaugeResultContainer;
};


/*static*/ void CGaugeResult::Initialize(void)
{
    if (!s_pGaugeResultContainer)
    {
        s_pGaugeResultContainer = new CGaugeResult_Container;
    };
};


/*static*/ void CGaugeResult::Terminate(void)
{
    if (s_pGaugeResultContainer)
    {
        delete s_pGaugeResultContainer;
        s_pGaugeResultContainer = nullptr;
    };
};


/*static*/ void CGaugeResult::Period(void)
{
    GaugeResultContainer().Period();
};


/*static*/ void CGaugeResult::Draw(void)
{
    GaugeResultContainer().Draw();
};


/*static*/ void CGaugeResult::SetMissionResult(RESULTREQ req)
{
    GaugeResultContainer().SetMissionResult(req);
};


/*static*/ void CGaugeResult::SetMissionResult(RESULTREQ req, int32 nPlayerNo)
{
    GaugeResultContainer().SetMissionResult(req, nPlayerNo);
};


/*static*/ bool CGaugeResult::IsMissionResultEnd(void)
{
    return GaugeResultContainer().IsMissionResultEnd();
};

