#include "Ride2D.hpp"
#include "RideTypes.hpp"
#include "RideStage.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/RenderState.hpp"


class CRide2DMedalAnimation
{
public:
    CRide2DMedalAnimation(RIDETYPES::SCOREKIND scorekind);
    ~CRide2DMedalAnimation(void);
    void Period(void);
    void Draw(void);
    bool IsEnd(void);
    void Delete(void);
    void SetNext(CRide2DMedalAnimation* pNext);
    CRide2DMedalAnimation* GetNext(void) const;

private:
    CRide2DMedalAnimation* m_pNext;
    float m_fTime;
    RIDETYPES::SCOREKIND m_scorekind;
};


CRide2DMedalAnimation::CRide2DMedalAnimation(RIDETYPES::SCOREKIND scorekind)
: m_pNext(nullptr)
, m_fTime(0.0f)
, m_scorekind(scorekind)
{
    ASSERT(m_scorekind >= 0 && m_scorekind < RIDETYPES::SCOREKINDNUM);
};


CRide2DMedalAnimation::~CRide2DMedalAnimation(void)
{
    ;
};


void CRide2DMedalAnimation::Period(void)
{
    m_fTime += (CGameProperty::GetElapsedTime() / 0.1666f);
};


void CRide2DMedalAnimation::Draw(void)
{
    float fValue = (IsEnd() ? 1.0f : m_fTime);
    
    switch (m_scorekind)
    {
    case RIDETYPES::SCOREKIND_SILVER:
        CRide2D::DrawSilverMedal(2.0f - fValue, 1.0f - fValue);
        break;
        
    case RIDETYPES::SCOREKIND_GOLD:
        CRide2D::DrawGoldMedal(2.0f - fValue, 1.0f - fValue);
        break;

    default:
        ASSERT(false);
        break;
    };
};


bool CRide2DMedalAnimation::IsEnd(void)
{
    return (m_fTime >= 1.0f);
};


void CRide2DMedalAnimation::Delete(void)
{
    if (m_pNext)
    {
        m_pNext->Delete();
        m_pNext = nullptr;
    };

    delete this;
};


void CRide2DMedalAnimation::SetNext(CRide2DMedalAnimation* pNext)
{
    if (m_pNext)
    {
        m_pNext->SetNext(pNext);
    }
    else
    {
        m_pNext = pNext;
    };
};


CRide2DMedalAnimation* CRide2DMedalAnimation::GetNext(void) const
{
    return m_pNext;
};


static CRide2DMedalAnimation* s_pMedalAnimation = nullptr;


/*static*/ void CRide2D::Initialize(void)
{
    s_pMedalAnimation = nullptr;
};


/*static*/ void CRide2D::Terminate(void)
{
    if (s_pMedalAnimation)
    {
        s_pMedalAnimation->Delete();
        s_pMedalAnimation = nullptr;
    };
};


/*static*/ void CRide2D::Period(void)
{
    CRide2DMedalAnimation* pMedalAnim = s_pMedalAnimation;
    while (pMedalAnim)
    {
        pMedalAnim->Period();
        pMedalAnim = pMedalAnim->GetNext();
    };

    if (s_pMedalAnimation && s_pMedalAnimation->IsEnd())
    {
        CRide2DMedalAnimation* pNext = s_pMedalAnimation->GetNext();
        delete s_pMedalAnimation;
        s_pMedalAnimation = pNext;
    };
};


/*static*/ void CRide2D::Draw(void)
{
    CTextureManager::SetCurrentTextureSet("ride_2d");

    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);

    DrawPersonalInfo();
    DrawGoldMedal(1.0f, 1.0f);
    DrawSilverMedal(1.0f, 1.0f);
    DrawGoldScore();
    DrawSilverScore();
    
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
    DrawMedalAnimation();

    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
};


/*static*/ void CRide2D::DrawGoldMedal(float fScale, float fAlpha)
{
    CSprite sprite;

    sprite.SetTextureAndResize(CTextureManager::GetRwTexture("ride_gold"));
    sprite.SetPositionAndSize(-245.0f, -175.0f, fScale * 64.0f, fScale * 64.0f);
    sprite.SetAlpha(uint8(fAlpha * 255.0f));
    sprite.Draw();
};


/*static*/ void CRide2D::DrawSilverMedal(float fScale, float fAlpha)
{
    CSprite sprite;

    sprite.SetTextureAndResize(CTextureManager::GetRwTexture("ride_silver"));
    sprite.SetPositionAndSize(-154.0f, -175.0f, fScale * 64.0f, fScale * 64.0f);
    sprite.SetAlpha(uint8(fAlpha * 255.0f));
    sprite.Draw();
};


/*static*/ void CRide2D::AddMedalAnimation(RIDETYPES::SCOREKIND scorekind)
{
    CRide2DMedalAnimation* pMedalAnim = new CRide2DMedalAnimation(scorekind);
    ASSERT(pMedalAnim);
    
    if (s_pMedalAnimation)
    {
        s_pMedalAnimation->SetNext(pMedalAnim);
    }
    else
    {
        s_pMedalAnimation = pMedalAnim;
    };
};


/*static*/ void CRide2D::DrawPersonalInfo(void)
{
    int32 nGamePlayerNum = CGameProperty::GetPlayerNum();
    
    for (int32 i = 0; i < nGamePlayerNum; ++i)
    {
        IGamePlayer& GamePlayer = CGameProperty::Player(i);

        float fOfsX = (nGamePlayerNum - i - 1) * -69.0f;
        
        DrawPersonalWindow(fOfsX, i);
        DrawPersonalPlayer(fOfsX, i);
        DrawPersonalMedal(fOfsX);
        DrawPersonalScore(fOfsX + 224.0f, 179.0f, GamePlayer.GetScore(RIDETYPES::SCOREKIND_GOLD));
        DrawPersonalScore(fOfsX + 224.0f, 194.0f, GamePlayer.GetScore(RIDETYPES::SCOREKIND_SILVER));
    };
};


/*static*/ void CRide2D::DrawPersonalWindow(float fOfsX, int32 nPlayerNo)
{
    CSprite sprite;
    
    sprite.SetTextureAndResize(CTextureManager::GetRwTexture("ride_info_window"));
    sprite.SetPositionAndSize(fOfsX + 232.0f, 185.0f, 128.0f, 64.0f);
    sprite.SetRGBA(PLAYERID::GetColor(CGameProperty::Player(nPlayerNo).GetCurrentCharacterID()));
    sprite.Draw();
};


/*static*/ void CRide2D::DrawPersonalPlayer(float fOfsX, int32 nPlayerNo)
{
    CSprite sprite;

    sprite.SetTextureAndResize(CTextureManager::GetRwTexture("ride_chara_p"));
    sprite.SetPositionAndSize(fOfsX + 256.0f, 168.0f, 64.0f, 32.0f);
    sprite.SetRGBA(PLAYERID::GetColor(CGameProperty::Player(nPlayerNo).GetCurrentCharacterID()));
    sprite.SetUV(
        0.0f,
        float(nPlayerNo) * 0.25f,
        1.0f,
        (float(nPlayerNo) + 1.0f) * 32.0f * (1.0f / 128.0f)
    );
    sprite.Draw();
};


/*static*/ void CRide2D::DrawPersonalMedal(float fOfsX)
{
    CSprite sprite;

    sprite.SetTextureAndResize(CTextureManager::GetRwTexture("ride_info_icon"));
    sprite.SetPositionAndSize(fOfsX + 209.0f, 185.0f, 16.0f, 32.0f);    
    sprite.Draw();
};


/*static*/ void CRide2D::DrawPersonalScore(float x, float y, int32 nValue)
{
    for (int32 i = 0; i < RIDETYPES::SCOREKINDNUM; ++i)
    {
        int32 nNumber = 0;
        
        if (i == RIDETYPES::SCOREKIND_GOLD)
        {
            nNumber = nValue % 10;
        }
        else if (i == RIDETYPES::SCOREKIND_SILVER)
        {
            nNumber = (nValue / 10) % 10;
        }
        else
        {
            ASSERT(false);
        };

        DrawPersonalNumber(float(i) * 12.0f + x, y, nNumber);
    };
};


/*static*/ void CRide2D::DrawPersonalNumber(float x, float y, int32 nValue)
{
    CSprite sprite;

    sprite.SetTextureAndResize(CTextureManager::GetRwTexture("ride_coin_num"));
    sprite.SetPositionAndSize(x, y, 16.0f, 16.0f);
    sprite.SetUV(
        float(nValue) * (1.0f / 12.55f),
        0.0f,
        (20.4f * nValue + 16.0f) * (1.0f / 256.0f),
        1.0f
    );
    sprite.Draw();
};


/*static*/ void CRide2D::DrawGoldScore(void)
{
    int32 nScore = CRideStage::GetScore(RIDETYPES::SCOREKIND_GOLD);
    DrawScore(-219.0f, -185.0f, nScore);
};


/*static*/ void CRide2D::DrawSilverScore(void)
{
    int32 nScore = CRideStage::GetScore(RIDETYPES::SCOREKIND_SILVER);
    DrawScore(-129.0f, -185.0f, nScore);
};


/*static*/ void CRide2D::DrawScore(float x, float y, int32 nValue)
{
    for (int32 i = 0; i < 3; ++i)
    {
        int32 nNumber = 0;

        switch (i)
        {
        case 0:
            nNumber = 10;
            break;

        case 1:
            nNumber = nValue / 10 % 10;
            break;

        case 2:
            nNumber = nValue % 10;
            break;
        };

        DrawNumber(float(i) * 14.0f + x, y, nNumber);
    };
};


/*static*/ void CRide2D::DrawNumber(float x, float y, int32 nValue)
{
    CSprite sprite;

    sprite.SetTextureAndResize(CTextureManager::GetRwTexture("ride_num"));
    sprite.SetPositionAndSize(x, y, 20.0f, 32.0f);
    sprite.SetUV(
        float(nValue) * (1.0f / 21.333f),
        0.0f,
        (24.0f * nValue + 20.0f) * (1.0f / 512.0f),
        1.0f
    );
    sprite.Draw();
};


/*static*/ void CRide2D::DrawMedalAnimation(void)
{
    CRide2DMedalAnimation* pMedalAnim = s_pMedalAnimation;
    while (pMedalAnim)
    {
        pMedalAnim->Draw();
        pMedalAnim = pMedalAnim->GetNext();
    };
};