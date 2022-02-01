#include "ItemGimmick.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/ItemID.hpp"
#include "Game/Component/GameMain/ItemInfo.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/Gimmick/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Sound/MessageManager.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"


static RwIm3DVertex s_aVertex[4] = { 0 };


CItemGimmick::CItemGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vVelocity(Math::VECTOR3_ZERO)
, m_pTexture(nullptr)
, m_pModuleManager(nullptr)
, m_fTimer(0.0f)
, m_fRadian(0.0f)
, m_fOffsetY(0.0f)
, m_fDeathHeight(0.0f)
, m_state(STATE_INIT)
, m_hAppearEffect(0)
{
    init(pParam);
    
    m_pModuleManager = new CModuleManager;
    ASSERT(m_pModuleManager);

    m_pModuleManager->Include(CCircleShadowModule::New(
        this,
        1.5f,
        1.5f,
        true
    ));
};


CItemGimmick::~CItemGimmick(void)
{
    if (m_pModuleManager)
    {
        delete m_pModuleManager;
        m_pModuleManager = nullptr;
    };

    if (m_hAppearEffect)
    {
        CEffectManager::Finish(m_hAppearEffect);
        m_hAppearEffect = 0;
    };
};


void CItemGimmick::Draw(void) const
{
    if (isBlinkDisappear())
        return;

    RwMatrix matView;
    RwMatrixSetIdentityMacro(&matView);
    CGameProperty::GetCameraViewMatrix(&matView);

    RwMatrix matBillboard;
    RwMatrixSetIdentityMacro(&matBillboard);
    Math::Matrix_Invert(&matBillboard, &matView);
    matBillboard.pos.x = m_vPosition.x;
    matBillboard.pos.y = m_vPosition.y + m_fOffsetY;
    matBillboard.pos.z = m_vPosition.z;

    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    
    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    const uint32 uFlags = rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV;

    std::memcpy(s_aVertex, m_aVertex, sizeof(m_aVertex));
    
    if (RwIm3DTransform(s_aVertex, COUNT_OF(s_aVertex), &matBillboard, uFlags))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
        RwIm3DEnd();
    };

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
};


void CItemGimmick::GetPosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);

    *pvPosition = m_vPosition;
};


void CItemGimmick::PostMove(void)
{
    switch (m_state)
    {
    case STATE_INIT:
        {
            m_fDeathHeight = CWorldMap::GetMapHeight(&m_vPosition) + 1.0f;
            if (IS_FLAG_SET(CWorldMap::GetCollisionResultAttribute(), MAPTYPES::ATTRIBUTE_DEATH))
                m_fDeathHeight = CWorldMap::GetCharacterDeathHeight();
            
            m_state = STATE_FALL;
        }
        break;
        
    case STATE_FALL:
        {
            if (checkItemGet())
            {
                m_state = STATE_END;
            }
            else
            {
                float fGravity = CGameProperty::GetGravity();
                float dt = CGameProperty::GetElapsedTime();

                m_vVelocity.y += fGravity * dt;

                m_vPosition.x += (m_vVelocity.x * dt);
                m_vPosition.y += (m_vVelocity.y * dt);
                m_vPosition.z += (m_vVelocity.z * dt);

                if (CWorldMap::GetCharacterDeathHeight() >= m_vPosition.y)
                {
                    m_state = STATE_END;
                }
                else
                {
                    if (m_vPosition.y <= m_fDeathHeight)
                    {
                        m_vVelocity = Math::VECTOR3_ZERO;
                        m_vPosition.y = m_fDeathHeight;
                        m_state = STATE_WAIT;
                    };
                };
            };
        }
        break;
        
    case STATE_WAIT:
        {
            if (checkItemGet() || (isTimelimitEnable() && m_fTimer > 20))
            {
                m_state = STATE_END;                
            }
            else
            {
                float dt = CGameProperty::GetElapsedTime();
                
                m_fRadian += (dt * 2.0f);
                if (m_fRadian > Math::PI2)
                    m_fRadian -= Math::PI2;

                m_fOffsetY = Math::Sin(m_fRadian) * 0.6f;
            };
        }
        break;
        
    case STATE_END:
        {
            Release();
        }
        break;
        
    default:
        ASSERT(false);
        break;
    };

    if (m_hAppearEffect)
    {
        RwV3d vEffectPos = m_vPosition;
        vEffectPos.y += m_fOffsetY;

        CEffectManager::SetPosition(m_hAppearEffect, &vEffectPos);        
    };

    if (m_pModuleManager)
        m_pModuleManager->Run();

    setVertex(0.6f, { 0xFF, 0xFF, 0xFF, 0xFF });

    if (m_state >= STATE_WAIT)
        m_fTimer += CGameProperty::GetElapsedTime();
};


void CItemGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    ;
};


void CItemGimmick::init(void* pParam)
{
    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_ITEMBOX* pInitParam = (GIMMICKPARAM::GIMMICK_ITEMBOX*)pParam;

    m_vPosition = pInitParam->m_vPosition;
    m_vPosition.y += 0.6f;
    
    m_vVelocity = Math::VECTOR3_ZERO;

    setVertex(0.6f, { 0xFF, 0xFF, 0xFF, 0xFF });

    m_nItemID = pInitParam->m_nItem;
    ASSERT(m_nItemID > ITEMID::ID_NONE && m_nItemID < ITEMID::ID_MAX);

    const char* pszTexture = "pizza";
    switch (m_nItemID)
    {
    case ITEMID::ID_HEAL_SMALL:
        pszTexture = "soda";
        break;

    case ITEMID::ID_HEAL_FULL:
        pszTexture = "pizza";
        break;

    case ITEMID::ID_MISSILE:
        pszTexture = "shuriken";
        break;

    case ITEMID::ID_CRY_RED:
        pszTexture = "cry_red";
        break;

    case ITEMID::ID_CRY_GREEN:
        pszTexture = "cry_green";
        break;

    case ITEMID::ID_CRY_WHITE:
        pszTexture = "cry_white";
        break;

    case ITEMID::ID_CRY_ORANGE:
        pszTexture = "cry_orange";
        break;

    case ITEMID::ID_ANTIQUE:
        pszTexture = "antique";
        break;

    case ITEMID::ID_COMEBACK:
        pszTexture = "deku";
        break;

    case ITEMID::ID_INVINCIBLE:
        pszTexture = "muteki";
        break;

    case ITEMID::ID_DON_LASER:
        pszTexture = "d_laser";
        break;

    default:
        ASSERT(false);
        break;
    };

    CTextureManager::SetCurrentTextureSet("item");
    m_pTexture = CTextureManager::GetRwTexture(pszTexture);
    ASSERT(m_pTexture);

    if (isCrystalItem())
    {
        EFFECTID::VALUE idEffect = EFFECTID::ID_UNKNOWN;
        RwV3d vEffectPost = m_vPosition;
        
        switch (m_nItemID)
        {
        case ITEMID::ID_CRY_RED:
            idEffect = EFFECTID::ID_I_CRYSHINE_R;
            break;

        case ITEMID::ID_CRY_GREEN:
            idEffect = EFFECTID::ID_I_CRYSHINE_G;
            break;

        case ITEMID::ID_CRY_ORANGE:
            idEffect = EFFECTID::ID_I_CRYSHINE_O;
            break;

        case ITEMID::ID_CRY_WHITE:
            idEffect = EFFECTID::ID_I_CRYSHINE_W;
            break;

        default:
            ASSERT(false);
            break;
        };

        m_hAppearEffect = CEffectManager::Play(idEffect, &vEffectPost);
        ASSERT(m_hAppearEffect);
    };
};


void CItemGimmick::setVertex(float fRadius, const RwRGBA& rColor)
{
    CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
    ASSERT(pMapCamera);

    float sw = float(CScreen::Width());
    float sh = float(CScreen::Height());

    float w = pMapCamera->GetViewRatio() * fRadius;
    float h = (sw / sh) * w;
    
    m_aVertex[0].objVertex.x = w * -1.0f;
    m_aVertex[0].objVertex.y = h;
    m_aVertex[0].objVertex.z = 0.0f;
    m_aVertex[0].u = 0.01f;
    m_aVertex[0].v = 0.99f;
    m_aVertex[0].color = COLOR_TO_INTEGER_RWRGBA(rColor);

    m_aVertex[1].objVertex.x = w * -1.0f;
    m_aVertex[1].objVertex.y = h * -1.0f;
    m_aVertex[1].objVertex.z = 0.0f;
    m_aVertex[1].u = 0.01f;
    m_aVertex[1].v = 0.01f;
    m_aVertex[1].color = COLOR_TO_INTEGER_RWRGBA(rColor);

    m_aVertex[2].objVertex.x = w;
    m_aVertex[2].objVertex.y = h;
    m_aVertex[2].objVertex.z = 0.0f;
    m_aVertex[2].u = 0.99f;
    m_aVertex[2].v = 0.99f;
    m_aVertex[2].color = COLOR_TO_INTEGER_RWRGBA(rColor);

    m_aVertex[3].objVertex.x = w;
    m_aVertex[3].objVertex.y = h * -1.0f;
    m_aVertex[3].objVertex.z = 0.0f;
    m_aVertex[3].u = 0.99f;
    m_aVertex[3].v = 0.01f;
    m_aVertex[3].color = COLOR_TO_INTEGER_RWRGBA(rColor);
};


void CItemGimmick::getItem(int32 nPlayerNo)
{
    if (m_hAppearEffect)
    {
        CEffectManager::Finish(m_hAppearEffect);
        m_hAppearEffect = 0;
    };

    EFFECTID::VALUE idEffect = EFFECTID::ID_UNKNOWN;
    RwV3d vEffectPost = m_vPosition;
    vEffectPost.y += m_fOffsetY;

    switch (m_nItemID)
    {
    case ITEMID::ID_CRY_RED:
        idEffect = EFFECTID::ID_I_CRYGET_R;
        break;

    case ITEMID::ID_CRY_GREEN:
        idEffect = EFFECTID::ID_I_CRYGET_G;
        break;

    case ITEMID::ID_CRY_ORANGE:
        idEffect = EFFECTID::ID_I_CRYGET_O;
        break;

    case ITEMID::ID_CRY_WHITE:
        idEffect = EFFECTID::ID_I_CRYGET_W;
        break;

    case ITEMID::ID_ANTIQUE:
        idEffect = EFFECTID::ID_I_GETANTIQUE;
        break;

    default:
        idEffect = EFFECTID::ID_I_GET;
        break;
    };

    CEffectManager::Play(idEffect, &vEffectPost);
    
    CGameEvent::SetPlayerTakenItem(nPlayerNo, ITEMID::VALUE(m_nItemID));
};


bool CItemGimmick::checkItemGet(void)
{
    if (m_fTimer < 0.5f)
        return false;

    float fNearestDistance = TYPEDEF::FLOAT_MAX;
    int32 nPlayerNo = -1;

    for (int32 i = 0; i < CGameProperty::GetPlayerNum(); ++i)
    {
        if (!CGameProperty::Player(i).IsAlive())
            continue;

        RwV3d vPosition = Math::VECTOR3_ZERO;
        CGameProperty::Player(i).GetPosition(&vPosition);

        RwV3d vDistance = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDistance, &vPosition, &m_vPosition);
        
        float fDistance = Math::Vec3_Length(&vDistance);
        if (fDistance < fNearestDistance)
        {
            fNearestDistance = fDistance;
            nPlayerNo = i;
        };
    };

    if (fNearestDistance >= 1.0f)
        return false;

    ASSERT(nPlayerNo >= 0 && nPlayerNo < CGameProperty::GetPlayerNum());
    getItem(nPlayerNo);
    
    return true;
};


bool CItemGimmick::isBlinkDisappear(void) const
{
    bool bResult = false;
    
    switch (m_nItemID)
    {
    case ITEMID::ID_HEAL_FULL:
    case ITEMID::ID_HEAL_SMALL:
    case ITEMID::ID_MISSILE:
    case ITEMID::ID_COMEBACK:
    case ITEMID::ID_INVINCIBLE:
        {
            if (m_fTimer > 15.0f)
            {
                float fRad = 2.0f * (Math::PI * ((m_fTimer - 15.0f) / 0.5f));
                
                if (fRad > Math::PI2)
                    fRad -= Math::PI2;

                fRad = Math::Cos(fRad);
                if (fRad > 0.0f)
                    bResult = true;
            };
        }
        break;
    };

    return bResult;
};


bool CItemGimmick::isCrystalItem(void) const
{
    return (m_nItemID == ITEMID::ID_CRY_RED     ||
            m_nItemID == ITEMID::ID_CRY_GREEN   ||
            m_nItemID == ITEMID::ID_CRY_ORANGE  ||
            m_nItemID == ITEMID::ID_CRY_WHITE);        
};


bool CItemGimmick::isTimelimitEnable(void) const
{
    switch (m_nItemID)
    {
    case ITEMID::ID_HEAL_FULL:
    case ITEMID::ID_HEAL_SMALL:
    case ITEMID::ID_MISSILE:
    case ITEMID::ID_COMEBACK:
    case ITEMID::ID_INVINCIBLE:
        return true;
    };

    return false;
};

