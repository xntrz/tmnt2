#include "GaugeMeter.hpp"
#include "GaugeManager.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"


class CGaugeMeter_Container
{
private:
    enum GAUGETYPE
    {
        GAUGETYPE_KATSU = 0,
        GAUGETYPE_CHU,
        GAUGETYPE_KI,
    };

    enum ANIMREQ
    {
        ANIMREQ_NONE = 0,
        ANIMREQ_FONT,
        ANIMREQ_RECVER,
        ANIMREQ_DMGER,
    };

    enum DEKUSTATE
    {
        DEKUSTATE_IDLE = 0,
        DEKUSTATE_CONSUME,
    };

public:
    CGaugeMeter_Container(void);
    ~CGaugeMeter_Container(void);
    void Period(void);
    void Draw(void);
    void GaugeUpdate(void);
    void GaugeMeterDraw(void);
    void GaugeMeterVertexSet(int32 nVertexNum, int32 aVertexList[]);
    void AnimGaugeFont_Period(void);
    void AnimGaugeFont_Draw(void);
    void LifeRecover_Period(void);
    void LifeRecover_Draw(void);
    void LifeDamage_Period(void);
    void LifeDamage_Draw(void);
    void DekuDraw(void);

private:
    uint32      m_aGaugeCnt[4];
    uint8       m_aGaugeStep[4];
    bool        m_bGaugeMeterSetting;
    CSprite     m_aSprite[7];
    GAUGETYPE   m_GaugeType;
    RwTexture*  m_apTexture[4];
    ANIMREQ     m_PlayerHpAnimReq;
    int32       m_PlayerHp;
    int32       m_PlayerHpOld;
    int32       m_PlayerHpMove;
    int32       m_PlayerHpKatsu;
    int32       m_PlayerHpChu;
    int32       m_PlayerHpKi;
    float       m_afWidth[2];
    float       m_afHeight[2];
    uint8       m_aAlpha[4];
    float       m_fRecoverRot;
    uint32      m_uLifeAnimCnt;
    uint32      m_uLifeAnimCnt2;
    bool        m_bDekuDispFlag;
    DEKUSTATE   m_DekuState;
    uint32      m_uDekuAnimCnt;
};


CGaugeMeter_Container::CGaugeMeter_Container(void)
: m_bGaugeMeterSetting(false)
, m_GaugeType(GAUGETYPE_KATSU)
, m_PlayerHpAnimReq(ANIMREQ_NONE)
, m_PlayerHp(0)
, m_PlayerHpOld(0)
, m_PlayerHpMove(0)
, m_PlayerHpKatsu(0)
, m_PlayerHpChu(0)
, m_PlayerHpKi(0)
, m_fRecoverRot(0.0f)
, m_uLifeAnimCnt(0)
, m_uLifeAnimCnt2(0)
, m_bDekuDispFlag(false)
, m_DekuState(DEKUSTATE_IDLE)
, m_uDekuAnimCnt(0)
{
    std::memset(m_apTexture,  0x00, sizeof(m_apTexture));
    std::memset(m_afWidth,    0x00, sizeof(m_afWidth));
    std::memset(m_afHeight,   0x00, sizeof(m_afHeight));
    std::memset(m_aGaugeStep, 0x00, sizeof(m_aGaugeStep));
    std::memset(m_aAlpha,     0x00, sizeof(m_aAlpha));
    std::memset(m_aGaugeCnt,  0x00, sizeof(m_aGaugeCnt));

    m_bDekuDispFlag = CGameData::Record().Item().IsComebackProcessed();
};


CGaugeMeter_Container::~CGaugeMeter_Container(void)
{
    ;
};


void CGaugeMeter_Container::Period(void)
{
    if (m_bGaugeMeterSetting)
    {
        switch (m_PlayerHpAnimReq)
        {
        case ANIMREQ_FONT:
            AnimGaugeFont_Period();
            break;

        case ANIMREQ_RECVER:
            LifeRecover_Period();
            break;

        case ANIMREQ_DMGER:
            LifeDamage_Period();
            break;

        default:
            ASSERT(false);
            break;
        };

        GaugeUpdate();

        m_aSprite[0].Resize(128.0f, 128.0f);
        m_aSprite[0].SetOffset(0.5f, 0.5f);
        m_aSprite[0].Move(-224.0f, -152.0f);

        m_aSprite[6].Resize(64.0f, 64.0f);
        m_aSprite[6].SetOffset(0.5f, 0.5f);
        m_aSprite[6].Move(-192.0f, -120.0f);

        m_aSprite[1].SetTexture(m_apTexture[m_GaugeType]);
    }
    else
    {
        CTextureManager::SetCurrentTextureSet("gg_guage");

        m_aSprite[0].SetTexture(CTextureManager::GetRwTexture("gg_guage_waku"));
        m_aSprite[0].Resize(128.0f, 128.0f);
        m_aSprite[0].SetOffset(0.5f, 0.5f);
        m_aSprite[0].Move(-224.0f, -152.0f);

        m_aSprite[6].SetTexture(CTextureManager::GetRwTexture("gg_deku"));
        m_aSprite[6].Resize(64.0f, 64.0f);
        m_aSprite[6].SetOffset(0.5f, 0.5f);
        m_aSprite[6].Move(-192.0f, -120.0f);

        m_apTexture[0] = CTextureManager::GetRwTexture("gg_guage_type_katsu");
        m_apTexture[1] = CTextureManager::GetRwTexture("gg_guage_type_chu");
        m_apTexture[2] = CTextureManager::GetRwTexture("gg_guage_type_ki");
        m_apTexture[3] = CTextureManager::GetRwTexture("gg_guage_guage");

        m_aSprite[1].SetTexture(m_apTexture[0]);
        m_aSprite[1].Move(-224.0f, -152.0f);
        m_aSprite[1].Resize(64.0f, 64.0f);

        m_aSprite[2].SetTexture(CTextureManager::GetRwTexture("gg_guage_recver_hikari"));
        m_aSprite[2].Move(-224.0f, -152.0f);
        m_aSprite[2].Resize(160.0f, 160.0f);

        m_aSprite[3].SetTexture(CTextureManager::GetRwTexture("gg_guage_type_fuku"));
        m_aSprite[3].Resize(64.0f, 64.0f);

        m_aSprite[4].SetTexture(CTextureManager::GetRwTexture("gg_guage_type_fuku2"));
        m_aSprite[4].Resize(64.0f, 64.0f);

        m_aSprite[5].SetTexture(CTextureManager::GetRwTexture("gg_guage_waku_dame"));
        m_aSprite[5].Resize(128.0f, 128.0f);

        m_bGaugeMeterSetting = true;
        GaugeUpdate();
    };
};


void CGaugeMeter_Container::Draw(void)
{
    if (!m_bGaugeMeterSetting)
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

    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ALPHA);
    m_aSprite[0].Draw();
    GaugeMeterDraw();

    switch (m_PlayerHpAnimReq)
    {
    case ANIMREQ_FONT:
        AnimGaugeFont_Draw();
        break;

    case ANIMREQ_RECVER:
        LifeRecover_Draw();
        break;

    case ANIMREQ_DMGER:
        LifeDamage_Draw();
        break;

    default:
        ASSERT(false);
        break;
    };

    DekuDraw();
};


void CGaugeMeter_Container::GaugeUpdate(void)
{
    IGamePlayer* pGameplayer = CGameProperty::Player(0);

    if (m_PlayerHpAnimReq)
    {
        if ((m_PlayerHpAnimReq == ANIMREQ_FONT) || (pGameplayer->GetHP() != m_PlayerHp))
        {
            m_PlayerHpOld = m_PlayerHp;
            m_PlayerHp = pGameplayer->GetHP();

            if (m_PlayerHp > m_PlayerHpOld)
            {
                m_PlayerHpAnimReq = ANIMREQ_RECVER;

                m_aGaugeStep[0] = 0;
                m_aGaugeStep[1] = 0;
                m_aGaugeStep[2] = 0;
                m_aGaugeStep[3] = 0;

                m_aGaugeCnt[0] = 0;
                m_aGaugeCnt[1] = 0;
                m_aGaugeCnt[2] = 0;
                m_aGaugeCnt[3] = 0;

                m_fRecoverRot = 0.0f;
                m_uLifeAnimCnt = 0;
            }
            else if (m_PlayerHp < m_PlayerHpOld)
            {
                m_PlayerHpAnimReq = ANIMREQ_DMGER;
                m_aGaugeStep[1] = 0;
                m_aGaugeCnt[1] = 0;
            };
        };
    }
    else
    {
        m_PlayerHp        = pGameplayer->GetHP();
        m_PlayerHpOld     = pGameplayer->GetHP();
        m_PlayerHpAnimReq = ANIMREQ_FONT;
    };

    m_PlayerHpKatsu = pGameplayer->GetHPMax();
    m_PlayerHpKi    = (m_PlayerHpKatsu / 3);
    m_PlayerHpChu   = (m_PlayerHpKi * 2);

    GAUGETYPE PrevGaugetype = m_GaugeType;

    if (m_PlayerHp >= m_PlayerHpChu)
    {
        m_GaugeType = GAUGETYPE_KATSU;
    }
    else if (m_PlayerHp >= m_PlayerHpKi)
    {
        m_GaugeType = GAUGETYPE_CHU;
    }
    else
    {
        m_GaugeType = GAUGETYPE_KI;
    };

    if (m_GaugeType != PrevGaugetype)
    {
        m_aGaugeStep[0] = 0;
        m_aGaugeCnt[0] = 0;
    };
};


void CGaugeMeter_Container::GaugeMeterDraw(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRasterMacro(m_apTexture[3]));

    int32 vertex_list[4] = { 0 };
    vertex_list[0] = 0;
    vertex_list[1] = 1;

    int32 hp = m_PlayerHpOld;

    if (m_PlayerHpAnimReq >= ANIMREQ_RECVER)
        hp = m_PlayerHpMove;

    if (hp > m_PlayerHpKi)
    {
        vertex_list[2] = m_PlayerHpKi;

        if (hp > m_PlayerHpChu)
        {
            vertex_list[3] = m_PlayerHpChu;
            GaugeMeterVertexSet(4, vertex_list);

            vertex_list[0] = 0;
            vertex_list[1] = m_PlayerHpChu;
            vertex_list[2] = hp;
            GaugeMeterVertexSet(3, vertex_list);
        }
        else
        {
            vertex_list[3] = hp;
            GaugeMeterVertexSet(4, vertex_list);
        };
    }
    else
    {
        vertex_list[2] = hp;
        GaugeMeterVertexSet(3, vertex_list);
    };
};


void CGaugeMeter_Container::GaugeMeterVertexSet(int32 nVertexNum, int32 aVertexList [])
{
    RwIm2DVertex aVertices[4] = { 0 };

    float fScale = 64.0f;

    for (int32 i = 0; i < nVertexNum; ++i)
    {
        float u = 0.0f;
        float v = 0.0f;

        if (aVertexList[i])
        {
            if (aVertexList[i] >= m_PlayerHpKi)
            {
                if (aVertexList[i] >= m_PlayerHpChu)
                {
                    v = float(aVertexList[i] - m_PlayerHpChu) / float(m_PlayerHpKi) * fScale;
                    u = v - fScale;
                }
                else
                {
                    float temp = float(aVertexList[i] - m_PlayerHpKi) / float(m_PlayerHpKi) * fScale;
                    u = -temp;
                    v = temp - fScale;
                };
            }
            else
            {
                float temp = float(aVertexList[i]) / float(m_PlayerHpKi) * fScale;
                u = fScale - temp;
                v = -temp;
            };
        }
        else
        {
            u = v = 0.0f;
        };

        float x = u - 224.0f;
        float y = v - 152.0f;

        CSprite::GetRealScreenPos(&x, &y);

        RwRGBA Color = { 255, 255, 255, 255 };

        aVertices[i].x = x;
        aVertices[i].y = y;
        aVertices[i].z = RwIm2DGetNearScreenZMacro();
        aVertices[i].rhw = 1.0f;
        aVertices[i].emissiveColor = RWRGBALONGEX(Color);
        aVertices[i].u = (u + fScale) * (1.0f / 128.0f);
        aVertices[i].v = (v + fScale) * (1.0f / 128.0f);
    };

    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, aVertices, nVertexNum);
};


void CGaugeMeter_Container::AnimGaugeFont_Period(void)
{
    float w = 64.0f;
    float h = 64.0f;
    uint8 uAlpha = 255;

    switch (m_aGaugeStep[0])
    {
    case 0:
        {
            switch (m_GaugeType)
            {
            case GAUGETYPE_KATSU:
                {
                    float fDuration = ANIM_DURATION_FRAMES(9);

                    w = Math::LinearTween(64.0f, 32.0f, float(m_aGaugeCnt[0]), fDuration);
                    h = Math::LinearTween(64.0f, 32.0f, float(m_aGaugeCnt[0]), fDuration);
                    uAlpha = uint8(Math::LinearTween(255.0f, -255.0f, float(m_aGaugeCnt[0]), fDuration));

                    if (m_aGaugeCnt[0] >= uint32(fDuration))
                    {
                        m_aGaugeStep[0] = 2;
                        m_aGaugeCnt[0] = 0;
                    }
                    else
                    {
                        ++m_aGaugeCnt[0];
                    };
                }
                break;

            case GAUGETYPE_CHU:
                {
                    float fDuration = ANIM_DURATION_FRAMES(23);

                    uAlpha = uint8(Math::LinearTween(0.0f, 255.0f, float(m_aGaugeCnt[0]), fDuration));

                    if (m_aGaugeCnt[0] >= uint32(fDuration))
                    {
                        ++m_aGaugeStep[0];
                        m_aGaugeCnt[0] = 0;
                    }
                    else
                    {
                        ++m_aGaugeCnt[0];
                    };
                }
                break;

            case GAUGETYPE_KI:
                {
                    float fDuration = ANIM_DURATION_FRAMES(10);

                    w = Math::LinearTween(96.0f, -32.0f, float(m_aGaugeCnt[0]), fDuration);
                    h = Math::LinearTween(96.0f, -32.0f, float(m_aGaugeCnt[0]), fDuration);
                    uAlpha = uint8(Math::LinearTween(0.0f, 255.0f, float(m_aGaugeCnt[0]), fDuration));

                    if (m_aGaugeCnt[0] >= uint32(fDuration))
                    {
                        m_aGaugeStep[0] = 2;
                        m_aGaugeCnt[0] = 0;
                    }
                    else
                    {
                        ++m_aGaugeCnt[0];
                    };
                }
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case 1:
        {
            if (m_GaugeType == GAUGETYPE_CHU)
            {
                float fDuration = ANIM_DURATION_FRAMES(23);

                uAlpha = uint8(Math::LinearTween(255.0f, -255.0f, float(m_aGaugeCnt[0]), fDuration));

                if (m_aGaugeCnt[0] >= uint32(fDuration))
                {
                    ++m_aGaugeStep[0];
                    m_aGaugeCnt[0] = 0;
                }
                else
                {
                    ++m_aGaugeCnt[0];
                };
            };
        }
        break;

    case 2:
        {
            float fDuration = ANIM_DURATION_FRAMES(60);

            uAlpha = 0;

            if (m_aGaugeCnt[0] >= uint32(fDuration))
            {
                m_aGaugeStep[0] = 0;
                m_aGaugeCnt[0] = 0;
            }
            else
            {
                ++m_aGaugeCnt[0];
            };
        }
        break;

    default:
        break;
    };

    m_afWidth[0] = w;
    m_afHeight[0] = h;
    m_aAlpha[0] = uAlpha;
};


void CGaugeMeter_Container::AnimGaugeFont_Draw(void)
{
    m_aSprite[1].Move(-224.0f, -152.0f);
    m_aSprite[1].Resize(64.0f, 64.0f);
    m_aSprite[1].SetRGBA(255, 255, 255, 255);
    m_aSprite[1].Draw();

    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ADD);
    m_aSprite[1].Move(-224.0f, -152.0f);
    m_aSprite[1].Resize(m_afWidth[0], m_afHeight[0]);
    m_aSprite[1].SetAlpha(m_aAlpha[0]);
    m_aSprite[1].Draw();
    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ALPHA);
};


void CGaugeMeter_Container::LifeRecover_Period(void)
{
    uint8 uAlpha0 = 255;
    uint8 uAlpha1 = 255;
    uint8 uAlpha2 = 255;
    uint8 uAlpha3 = 255;

    float w0 = 64.0f;
    float h0 = 64.0f;
    float w1 = 64.0f;
    float h1 = 64.0f;

    switch (m_aGaugeStep[0])
    {
    case 0:
        {
            float fDuration = ANIM_DURATION_FRAMES(6);

            w0 = Math::LinearTween(140.8f, -76.8f, float(m_aGaugeCnt[0]), fDuration);
            h0 = Math::LinearTween(140.8f, -76.8f, float(m_aGaugeCnt[0]), fDuration);
            uAlpha0 = uint8(Math::LinearTween(255.0f, -255.0f, float(m_aGaugeCnt[0]), fDuration));

            if (m_aGaugeCnt[0] < uint32(fDuration))
                ++m_aGaugeCnt[0];
        }
        break;
    };

    switch (m_aGaugeStep[1])
    {
    case 0:
        {
            float fDuration = ANIM_DURATION_FRAMES(3);

            w1 = 64.0f;
            h1 = 64.0f;
            uAlpha1 = 255;

            if (m_aGaugeCnt[1] >= uint32(fDuration))
            {
                m_aGaugeCnt[1] = 0;
                ++m_aGaugeStep[1];
            }
            else
            {
                ++m_aGaugeCnt[1];
            };
        }
        break;

    case 1:
        {
            float fDuration = ANIM_DURATION_FRAMES(10);

            w1 = Math::LinearTween(64.0f, 32.0f, float(m_aGaugeCnt[1]), fDuration);
            h1 = Math::LinearTween(64.0f, 32.0f, float(m_aGaugeCnt[1]), fDuration);
            uAlpha1 = uint8(Math::LinearTween(255.0f, -255.0f, float(m_aGaugeCnt[1]), fDuration));

            if (m_aGaugeCnt[1] < uint32(fDuration))
                ++m_aGaugeCnt[1];
        }
        break;

    default:
        break;
    };

    switch (m_aGaugeStep[2])
    {
    case 0:
        {
            float fDuration = ANIM_DURATION_FRAMES(4);

            uAlpha2 = 255;

            if (m_aGaugeCnt[2] >= uint32(fDuration))
            {
                m_aGaugeCnt[2] = 0;
                ++m_aGaugeStep[2];
            }
            else
            {
                ++m_aGaugeCnt[2];
            };
        }
        break;

    case 1:
        {
            float fDuration = ANIM_DURATION_FRAMES(22);

            uAlpha2 = 255;

            if (m_aGaugeCnt[2] >= uint32(fDuration))
            {
                m_aGaugeCnt[2] = 0;
                ++m_aGaugeStep[2];
            }
            else
            {
                ++m_aGaugeCnt[2];
            };
        }
        break;

    case 2:
        {
            float fDuration = ANIM_DURATION_FRAMES(17);

            uAlpha2 = uint8(Math::LinearTween(255.0f, -255.0f, float(m_aGaugeCnt[2]), fDuration));

            if (m_aGaugeCnt[2] < uint32(fDuration))
                ++m_aGaugeCnt[2];
        }
        break;

    default:
        break;
    };

    switch (m_aGaugeStep[3])
    {
    case 0:
        {
            float fDuration = ANIM_DURATION_FRAMES(6);

            uAlpha3 = uint8(Math::LinearTween(255.0f, -255.0f, float(m_aGaugeCnt[3]), fDuration));

            if (m_aGaugeCnt[3] < uint32(fDuration))
                ++m_aGaugeCnt[3];
        }
        break;

    default:
        break;
    };

    float fAnimDur = ANIM_DURATION_FRAMES(40);

    if (m_uLifeAnimCnt >= uint32(fAnimDur))
    {
        m_aGaugeStep[0] = 0;
        m_aGaugeCnt[0] = 0;
        m_PlayerHpAnimReq = ANIMREQ_FONT;
    }
    else
    {
        ++m_uLifeAnimCnt;
    };

    m_fRecoverRot += 0.1f;
    if (m_fRecoverRot >= 360.0f)
        m_fRecoverRot -= 360.0f;

    float fHpRecover = static_cast<float>(m_PlayerHp - m_PlayerHpOld);
    float fAnimRatio = (static_cast<float>(m_uLifeAnimCnt) / fAnimDur);

    m_PlayerHpMove = m_PlayerHpOld + static_cast<int32>(fHpRecover * fAnimRatio);

    m_aAlpha[0] = uAlpha0;
    m_aAlpha[1] = uAlpha1;
    m_aAlpha[2] = uAlpha2;
    m_aAlpha[3] = uAlpha3;

    m_afWidth[0] = w0;
    m_afWidth[1] = w1;
    m_afHeight[0] = h0;
    m_afHeight[1] = h1;
};


void CGaugeMeter_Container::LifeRecover_Draw(void)
{
    m_aSprite[1].Move(-224.0f, -152.0f);
    m_aSprite[1].Resize(64.0f, 64.0f);
    m_aSprite[1].SetRGBA(255, 255, 255, 255);
    m_aSprite[1].Draw();

    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ADD);
    m_aSprite[2].SetRotate(m_fRecoverRot);
    m_aSprite[2].SetRGBA(255, 255, 255, m_aAlpha[3]);
    m_aSprite[2].DrawRotate();

    m_aSprite[3].Move(-224.0f, -152.0f);
    m_aSprite[3].Resize(m_afWidth[0], m_afHeight[0]);
    m_aSprite[3].SetRGBA(255, 255, 255, m_aAlpha[0]);
    m_aSprite[3].Draw();

    m_aSprite[3].Move(-224.0f, -152.0f);
    m_aSprite[3].Resize(m_afWidth[1], m_afHeight[1]);
    m_aSprite[3].SetRGBA(255, 255, 255, m_aAlpha[1]);
    m_aSprite[3].Draw();
    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ALPHA);

    if (m_aGaugeStep[2])
    {
        m_aSprite[4].Move(-224.0f, -152.0f);
        m_aSprite[4].SetRGBA(255, 255, 255, m_aAlpha[2]);
        m_aSprite[4].Draw();
    };
};


void CGaugeMeter_Container::LifeDamage_Period(void)
{
    switch (m_aGaugeStep[1])
    {
    case 0:
        {
            float fDuration = ANIM_DURATION_FRAMES(10);

            if (m_aGaugeCnt[1] >= uint32(fDuration))
            {
                m_aGaugeStep[0] = 0;
                m_aGaugeCnt[0] = 0;
                m_PlayerHpAnimReq = ANIMREQ_FONT;
            }
            else
            {
                ++m_aGaugeCnt[1];
            };

            float fHpDamaged = static_cast<float>(m_PlayerHpOld - m_PlayerHp);
            float fAnimRatio = (static_cast<float>(m_aGaugeCnt[1]) / fDuration);

            m_PlayerHpMove = m_PlayerHpOld - static_cast<int32>(fHpDamaged * fAnimRatio);
            
            m_aAlpha[0] = uint8(Math::LinearTween(255.0f, -255.0f, float(m_aGaugeCnt[1]), fDuration));
        }
        break;

    default:
        break;
    };
};


void CGaugeMeter_Container::LifeDamage_Draw(void)
{
    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ADD);
    m_aSprite[5].Move(-224.0f, -152.0f);
    m_aSprite[5].SetRGBA(255, 255, 255, m_aAlpha[0]);
    m_aSprite[5].Draw();
    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ALPHA);

    m_aSprite[1].Move(-224.0f, -152.0f);
    m_aSprite[1].Resize(64.0f, 64.0f);
    m_aSprite[1].SetRGBA(255, 255, 255, 255);
    m_aSprite[1].Draw();
};


void CGaugeMeter_Container::DekuDraw(void)
{
    m_bDekuDispFlag = CGameData::Record().Item().IsComebackProcessed();
    
    if (!m_bDekuDispFlag)
        return;

    float w = 64.0f;
    float h = 64.0f;
    uint8 uAlpha = 255;

    switch (m_DekuState)
    {
    case DEKUSTATE_IDLE:
        {
            if (!CGameData::Record().Item().IsComebackProcessed())
            {
                m_DekuState = DEKUSTATE_CONSUME;
                m_uDekuAnimCnt = 0;
            };
        }
        break;

    case DEKUSTATE_CONSUME:
        {
            float fDuration = ANIM_DURATION_FRAMES(40);

            w = Math::LinearTween(64.0f, 32.0f, float(m_uDekuAnimCnt), fDuration);
            h = Math::LinearTween(64.0f, 32.0f, float(m_uDekuAnimCnt), fDuration);
            uAlpha = uint8(Math::LinearTween(255.0f, -255.0f, float(m_uDekuAnimCnt), fDuration));

            if (m_uDekuAnimCnt >= uint32(fDuration))
                m_bDekuDispFlag = false;
            else
                ++m_uDekuAnimCnt;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    m_aSprite[6].Move(-192.0f, -120.0f);
    m_aSprite[6].Resize(w, h);
    m_aSprite[6].SetRGBA(255, 255, 255, uAlpha);
    m_aSprite[6].Draw();
};


static CGaugeMeter_Container* s_pGaugeMeterContainer = nullptr;


static inline CGaugeMeter_Container& GaugeMeterContainer(void)
{
    ASSERT(s_pGaugeMeterContainer);
    return *s_pGaugeMeterContainer;
};


/*static*/ void CGaugeMeter::Initialize(void)
{
    if (!s_pGaugeMeterContainer)
        s_pGaugeMeterContainer = new CGaugeMeter_Container;
};


/*static*/ void CGaugeMeter::Termiante(void)
{
    if (s_pGaugeMeterContainer)
    {
        delete s_pGaugeMeterContainer;
        s_pGaugeMeterContainer = nullptr;
    };
};


/*static*/ void CGaugeMeter::Period(void)
{
    GaugeMeterContainer().Period();
};


/*static*/ void CGaugeMeter::Draw(void)
{
    GaugeMeterContainer().Draw();
};