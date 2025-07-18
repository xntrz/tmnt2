#include "ItemGimmick.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/ItemID.hpp"
#include "Game/Component/GameMain/ItemInfo.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
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


#define SPRITE_RADIUS (0.6f)


static RwIm3DVertex s_aVertex[4] = { 0 };


CItemGimmick::CItemGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vVelocity(Math::VECTOR3_ZERO)
, m_aVertex()
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
    m_pModuleManager->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, true));
    m_pModuleManager->Run(); // update shadow position to item
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

    matBillboard.pos = m_vPosition;
    matBillboard.pos.y += m_fOffsetY;

    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, m_pTexture ? RwTextureGetRasterMacro(m_pTexture) : NULL);    

    const uint32 flags = rwIM3D_VERTEXRGBA
                       | rwIM3D_VERTEXXYZ
                       | rwIM3D_VERTEXUV;

    std::memcpy(s_aVertex, m_aVertex, sizeof(m_aVertex));
    
    if (RwIm3DTransform(s_aVertex, COUNT_OF(s_aVertex), &matBillboard, flags))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
        RwIm3DEnd();
    };

    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
};


void CItemGimmick::GetPosition(RwV3d* pvPosition) const
{
    *pvPosition = m_vPosition;
};


void CItemGimmick::PostMove(void)
{
    switch (m_state)
    {
    case STATE_INIT:
        {
            m_fDeathHeight = (CWorldMap::GetMapHeight(&m_vPosition) + (SPRITE_RADIUS * 2.0f));

            MAPTYPES::ATTRIBUTE attribute = CWorldMap::GetCollisionResultAttribute();
            if (attribute == MAPTYPES::ATTRIBUTE_DEATH)
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
                float fGravity = CGameProperty::GetGravity() * SPRITE_RADIUS;
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
            bool bIsGet = checkItemGet();
            bool bIsTimeout = (isTimelimitEnable() && (m_fTimer > 20.0f));
            
            if (bIsGet || bIsTimeout)
            {
                m_state = STATE_END;                
            }
            else
            {
                float dt = CGameProperty::GetElapsedTime();
                
                m_fRadian += (dt * 2.0f);
                if (m_fRadian > MATH_PI2)
                    m_fRadian -= MATH_PI2;

                m_fOffsetY = Math::Sin(m_fRadian) * SPRITE_RADIUS;
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

    setVertex(SPRITE_RADIUS, { 0xFF, 0xFF, 0xFF, 0xFF });

    if (m_state >= STATE_WAIT)
        m_fTimer += CGameProperty::GetElapsedTime();
};


void CItemGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    ;
};


void CItemGimmick::init(void* pParam)
{
    GIMMICKPARAM::GIMMICK_ITEMBOX* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_ITEMBOX*>(pParam);
    ASSERT(pInitParam != nullptr);

    m_vPosition = pInitParam->m_vPosition;
    m_vPosition.y += SPRITE_RADIUS;
    
    m_vVelocity = Math::VECTOR3_ZERO;

    setVertex(SPRITE_RADIUS, { 0xFF, 0xFF, 0xFF, 0xFF });

    m_nItemID = pInitParam->m_nItem;
    ASSERT(m_nItemID > ITEMID::ID_NONE);
    ASSERT(m_nItemID < ITEMID::ID_MAX);

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

        m_hAppearEffect = CEffectManager::Play(idEffect, &m_vPosition);
        ASSERT(m_hAppearEffect);
    };
};


void CItemGimmick::setVertex(float fRadius, const RwRGBA& rColor)
{
    CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
    ASSERT(pMapCamera);

    float sw = static_cast<float>(CScreen::Width());
    float sh = static_cast<float>(CScreen::Height());

    float w = pMapCamera->GetViewRatio() * fRadius;
    float h = (sw / sh) * w;
    
    m_aVertex[0].objVertex.x = w * -1.0f;
    m_aVertex[0].objVertex.y = h;
    m_aVertex[0].objVertex.z = 0.0f;
    m_aVertex[0].u = 0.01f;
    m_aVertex[0].v = 0.99f;
    m_aVertex[0].color = RWRGBALONGEX(rColor);

    m_aVertex[1].objVertex.x = w * -1.0f;
    m_aVertex[1].objVertex.y = h * -1.0f;
    m_aVertex[1].objVertex.z = 0.0f;
    m_aVertex[1].u = 0.01f;
    m_aVertex[1].v = 0.01f;
    m_aVertex[1].color = RWRGBALONGEX(rColor);

    m_aVertex[2].objVertex.x = w;
    m_aVertex[2].objVertex.y = h;
    m_aVertex[2].objVertex.z = 0.0f;
    m_aVertex[2].u = 0.99f;
    m_aVertex[2].v = 0.99f;
    m_aVertex[2].color = RWRGBALONGEX(rColor);

    m_aVertex[3].objVertex.x = w;
    m_aVertex[3].objVertex.y = h * -1.0f;
    m_aVertex[3].objVertex.z = 0.0f;
    m_aVertex[3].u = 0.99f;
    m_aVertex[3].v = 0.01f;
    m_aVertex[3].color = RWRGBALONGEX(rColor);
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

	int32 nPlayerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < nPlayerNum; ++i)
    {
		IGamePlayer* pGamePlayer = CGameProperty::Player(i);
		if (!pGamePlayer->IsAlive())
			continue;

        RwV3d vPosition = Math::VECTOR3_ZERO;
		pGamePlayer->GetPosition(&vPosition);

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

    getItem(nPlayerNo);
    
    return true;
};


bool CItemGimmick::isBlinkDisappear(void) const
{
    if (isTimelimitEnable())
    {
        if (m_fTimer > 15.0f)
        {
            float fRad = 2.0f * (MATH_PI * ((m_fTimer - 15.0f) / 0.5f));

            if (fRad > MATH_PI2)
                fRad -= MATH_PI2;

            fRad = Math::Cos(fRad);
            if (fRad > 0.0f)
                return true;
        };
    };

    return false;
};


bool CItemGimmick::isCrystalItem(void) const
{
    return (m_nItemID == ITEMID::ID_CRY_RED)
        || (m_nItemID == ITEMID::ID_CRY_GREEN)
        || (m_nItemID == ITEMID::ID_CRY_ORANGE)
        || (m_nItemID == ITEMID::ID_CRY_WHITE);
};


bool CItemGimmick::isTimelimitEnable(void) const
{
    return (m_nItemID == ITEMID::ID_HEAL_FULL)
        || (m_nItemID == ITEMID::ID_HEAL_SMALL)
        || (m_nItemID == ITEMID::ID_MISSILE)
        || (m_nItemID == ITEMID::ID_COMEBACK)
        || (m_nItemID == ITEMID::ID_INVINCIBLE);
};

