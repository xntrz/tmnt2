#include "FootprintsModule.hpp"

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/RenderState.hpp"


struct FOOTPRINTSINFO
{
    RwV2d m_vSize;
    float m_fRadius;
};


static const FOOTPRINTSINFO s_aFootprintsInfo[] =
{
    { { 0.45f, 0.45f }, 0.16f },    // leo
    { { 0.45f, 0.45f }, 0.16f },    // rap
    { { 0.45f, 0.45f }, 0.16f },    // mic
    { { 0.45f, 0.45f }, 0.16f },    // don
    { { 0.45f, 0.45f }, 0.16f },    // sla
    { { 0.45f, 0.45f }, 0.16f },    // cas
    { { 0.25f, 0.25f }, 0.16f },    // karai
    { { 0.45f, 0.60f }, 0.16f },    // spl
};


static_assert(COUNT_OF(s_aFootprintsInfo) == PLAYERID::ID_MAX, "update me");


/*static*/ CFootprintsModule* CFootprintsModule::New(CPlayerCharacter* pPlayerChr)
{
    ASSERT(pPlayerChr);

    PLAYERID::VALUE idPlayer = pPlayerChr->GetID();
    ASSERT(idPlayer >= PLAYERID::ID_START && idPlayer < PLAYERID::ID_MAX);

    CFootprintsModule* pRet = new CFootprintsModule(
        pPlayerChr,
        &s_aFootprintsInfo[idPlayer].m_vSize,
        s_aFootprintsInfo[idPlayer].m_fRadius
    );

    ASSERT(pRet);

    return pRet;
};


void CFootprintsModule::WORK::Clear(void)
{
    m_vPosition = Math::VECTOR3_ZERO;
    RwMatrixSetIdentityMacro(&m_matrix);
    m_foottype = FOOTTYPE_LEFT;
    m_uAlphaBasis = 0xFF;
};


CFootprintsModule::CFootprintsModule(CPlayerCharacter* pPlayerChr, const RwV2d* pvSize, float fRadius)
: IModule(MODULETYPE::FOOTPRINTS)
, m_pPlayerChr(pPlayerChr)
, m_nDispNum(0)
, m_pTexture(nullptr)
, m_vFootSize(*pvSize)
, m_fRadius(fRadius)
, m_lastFoottype(FOOTTYPE_LEFT)
, m_fTime(0.0f)
{
    ASSERT(m_pPlayerChr);
    
    for (int32 i = 0; i < COUNT_OF(m_aWork); ++i)
    {
        m_aWork[i].Clear();
        m_listWorkPool.push_back(&m_aWork[i]);
    };

    CTextureManager::SetCurrentTextureSet("common_ef");
    m_pTexture = CTextureManager::GetRwTexture("footprint");
    ASSERT(m_pTexture);

    std::memset(m_aVertex, 0x00, sizeof(m_aVertex));
};


CFootprintsModule::~CFootprintsModule(void)
{
    ;
};


void CFootprintsModule::Run(void)
{
    float dt = CGameProperty::GetElapsedTime();
    
    for (WORK& it : m_listWorkAlloc)
    {
        int32 nAlphaBasis = int32( float(it.m_uAlphaBasis) - (dt * 256.0f) );
        if (nAlphaBasis < 0)
            nAlphaBasis = 0;

        it.m_uAlphaBasis = uint8(nAlphaBasis);
    };

    float fStampTime = 0.0f;
    
    switch (m_pPlayerChr->GetStatus())
    {
    case PLAYERTYPES::STATUS_WALK:
        {
            m_fTime += dt;
            if (m_fTime >= 0.30f)
            {
                m_fTime = 0.0f;
                
                RwV3d vPosition = Math::VECTOR3_ZERO;
                m_pPlayerChr->GetPosition(&vPosition);

                float fDirection = m_pPlayerChr->GetDirection();

                if (m_lastFoottype)
                {
                    vPosition.x += m_fRadius * (Math::Sin(fDirection - Math::PI05));
                    vPosition.z += m_fRadius * (Math::Cos(fDirection - Math::PI05));                    
                    Stamp(&vPosition, fDirection, FOOTTYPE_LEFT);
                    m_lastFoottype = FOOTTYPE_LEFT;
                }
                else
                {
                    vPosition.x += m_fRadius * (Math::Sin(fDirection + Math::PI05));
                    vPosition.z += m_fRadius * (Math::Cos(fDirection + Math::PI05));
                    Stamp(&vPosition, fDirection, FOOTTYPE_RIGHT);
                    m_lastFoottype = FOOTTYPE_RIGHT;
                };
            };
        }
        break;

    case PLAYERTYPES::STATUS_RUN:
        {
            m_fTime += dt;
            if (m_fTime >= 0.20f)
            {
                m_fTime = 0.0f;
                
                RwV3d vPosition = Math::VECTOR3_ZERO;
                m_pPlayerChr->GetPosition(&vPosition);

                float fDirection = m_pPlayerChr->GetDirection();

                if (m_lastFoottype)
                {
                    vPosition.x += m_fRadius * (Math::Sin(fDirection - Math::PI05));
                    vPosition.z += m_fRadius * (Math::Cos(fDirection - Math::PI05));
                    Stamp(&vPosition, fDirection, FOOTTYPE_LEFT);
                    m_lastFoottype = FOOTTYPE_LEFT;
                }
                else
                {
                    vPosition.x += m_fRadius * (Math::Sin(fDirection + Math::PI05));
                    vPosition.z += m_fRadius * (Math::Cos(fDirection + Math::PI05));
                    Stamp(&vPosition, fDirection, FOOTTYPE_RIGHT);
                    m_lastFoottype = FOOTTYPE_RIGHT;
                };
            };
        }
        break;

    default:
        m_fTime = 0.0f;
        break;
    };
};


void CFootprintsModule::Draw(void)
{
    if (m_nDispNum)
    {
        SetVertex();
        DrawFootprints();
    };
};


void CFootprintsModule::Stamp(const RwV3d* pvPosition, float fDirection, FOOTTYPE foottype)
{
    float fMapHeight = CWorldMap::GetMapHeight(pvPosition);
    float fHeight = (pvPosition->y - fMapHeight);
    
    const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
    ASSERT(pCollisionResult);

    if (fHeight >= 0.1f)
        return;

    if (FLAG_TEST(pCollisionResult->m_attribute, MAPTYPES::ATTRIBUTE_SNOW) ||
        FLAG_TEST(pCollisionResult->m_attribute, MAPTYPES::ATTRIBUTE_SAND))
    {
        RwV3d vAxisX = Math::VECTOR3_ZERO;
        RwV3d vAxisY = Math::VECTOR3_ZERO;
        RwV3d vAxisZ = Math::VECTOR3_ZERO;

        Math::Vec3_Cross(&vAxisX, &Math::VECTOR3_AXIS_Z, &pCollisionResult->m_vNormal);
        Math::Vec3_Cross(&vAxisZ, &pCollisionResult->m_vNormal, &Math::VECTOR3_AXIS_X);
        Math::Vec3_Normalize(&vAxisX, &vAxisX);
        Math::Vec3_Normalize(&vAxisY, &pCollisionResult->m_vNormal);
        Math::Vec3_Normalize(&vAxisZ, &vAxisZ);

        WORK* pWork = GetWork();
        ASSERT(pWork);
        
        RwMatrix matView;
        RwMatrix matRotY;
        RwMatrixSetIdentityMacro(&matView);
        RwMatrixSetIdentityMacro(&matRotY);
        RwMatrixSetIdentityMacro(&pWork->m_matrix);
        
        Math::Matrix_Update(
            &matView,
            &vAxisX,
            &vAxisY,
            &vAxisZ,
            &Math::VECTOR3_ZERO
        );

        Math::Matrix_RotateY(&matRotY, fDirection);
        Math::Matrix_Multiply(&pWork->m_matrix, &pWork->m_matrix, &matRotY);
        Math::Matrix_Multiply(&pWork->m_matrix, &pWork->m_matrix, &matView);
        pWork->m_vPosition = *pvPosition;
        pWork->m_vPosition.y = fMapHeight + 0.05f;
        pWork->m_foottype = foottype;

        if (FLAG_TEST(pCollisionResult->m_attribute, MAPTYPES::ATTRIBUTE_SNOW))
            CEffectManager::Play(EFFECTID::ID_ALL_DASH_SNOW, &pWork->m_vPosition);
    };
};


void CFootprintsModule::SetVertex(void)
{
    RwV3d aPosition[4] = { Math::VECTOR3_ZERO };
    int32 nVertexIndex = 0;
    float w = m_vFootSize.x * 0.5f;
    float h = m_vFootSize.y * 0.5f;

    for (WORK& it : m_listWorkAlloc)
    {
        ASSERT(nVertexIndex < COUNT_OF(m_aVertex));

        aPosition[0].x = h * -1.0f;
        aPosition[0].y = 0.0f;
        aPosition[0].z = w * -1.0f;

        aPosition[1].x = h * -1.0f;
        aPosition[1].y = 0.0f;
        aPosition[1].z = w;

        aPosition[2].x = h;
        aPosition[2].y = 0.0f;
        aPosition[2].z = w * -1.0f;
        
        aPosition[3].x = h;
        aPosition[3].y = 0.0f;
        aPosition[3].z = w;
        
        RwV3dTransformVectors(aPosition, aPosition, COUNT_OF(aPosition), &it.m_matrix);

        Math::Vec3_Add(&aPosition[0], &aPosition[0], &it.m_vPosition);
        Math::Vec3_Add(&aPosition[1], &aPosition[1], &it.m_vPosition);
        Math::Vec3_Add(&aPosition[2], &aPosition[2], &it.m_vPosition);
        Math::Vec3_Add(&aPosition[3], &aPosition[3], &it.m_vPosition);

        RwRGBA color = { 0xFF, 0xFF, 0xFF, 0xFF };
        color.alpha = it.m_uAlphaBasis;
        
        float u0 = 1.0f;
        float u1 = 0.0f;

        if (!it.m_foottype)
        {
            u0 = 0.0f;
            u1 = 1.0f;
        };

        RwIm3DVertex* pVertex = &m_aVertex[ nVertexIndex ];

        pVertex[0].objVertex = aPosition[1];
        pVertex[0].objNormal = Math::VECTOR3_ZERO;
        pVertex[0].color = RWRGBALONGEX(color);
        pVertex[0].u = u1;
        pVertex[0].v = 1.0f;

        pVertex[1].objVertex = aPosition[0];
        pVertex[1].objNormal = Math::VECTOR3_ZERO;
        pVertex[1].color = RWRGBALONGEX(color);
        pVertex[1].u = u1;
        pVertex[1].v = 0.0f;

        pVertex[2].objVertex = aPosition[3];
        pVertex[2].objNormal = Math::VECTOR3_ZERO;
        pVertex[2].color = RWRGBALONGEX(color);
        pVertex[2].u = u0;
        pVertex[2].v = 1.0f;

        pVertex[3].objVertex = aPosition[3];
        pVertex[3].objNormal = Math::VECTOR3_ZERO;
        pVertex[3].color = RWRGBALONGEX(color);
        pVertex[3].u = u0;
        pVertex[3].v = 1.0f;

        pVertex[4].objVertex = aPosition[0];
        pVertex[4].objNormal = Math::VECTOR3_ZERO;
        pVertex[4].color = RWRGBALONGEX(color);
        pVertex[4].u = u1;
        pVertex[4].v = 0.0f;

        pVertex[5].objVertex = aPosition[2];
        pVertex[5].objNormal = Math::VECTOR3_ZERO;
        pVertex[5].color = RWRGBALONGEX(color);
        pVertex[5].u = u0;
        pVertex[5].v = 0.0f;

        nVertexIndex += 6;
    };
};


void CFootprintsModule::DrawFootprints(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    uint32 uFlags = rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV;

    if (RwIm3DTransform(m_aVertex, m_nDispNum * 6, nullptr, uFlags))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
        RwIm3DEnd();
    };

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);    
};


CFootprintsModule::WORK* CFootprintsModule::GetWork(void)
{
    WORK* pRet = nullptr;
    
    if (m_nDispNum >= COUNT_OF(m_aWork))
    {
        ASSERT(!m_listWorkAlloc.empty());

        WORK* pNode = m_listWorkAlloc.back();
        m_listWorkAlloc.erase(pNode);
        m_listWorkAlloc.push_front(pNode);

        pRet = pNode;
    }
    else
    {
        ASSERT(!m_listWorkPool.empty());
        
        WORK* pNode = m_listWorkPool.front();
        m_listWorkPool.erase(pNode);
        m_listWorkAlloc.push_front(pNode);

        pRet = pNode;
        ++m_nDispNum;
    };

    ASSERT(pRet);

    if (pRet)
        pRet->m_uAlphaBasis = 0xFF;

    return pRet;
};