#include "CircleShadowModule.hpp"

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Misc/RenderStateStack.hpp"
#include "System/Common/RenderState.hpp"


class CCircleShadowModuleForCharacter final : public CCircleShadowModule
{
public:
    CCircleShadowModuleForCharacter(CCharacter* pCharacter, float w, float h) : CCircleShadowModule(w, h), m_pCharacter(pCharacter) {};
    virtual ~CCircleShadowModuleForCharacter(void) {};
    virtual void GetPosition(RwV3d* pvPosition) const override { return m_pCharacter->GetBodyPosition(pvPosition); };
    virtual float GetDirection(void) const override { return m_pCharacter->GetDirection(); };

private:
    CCharacter* m_pCharacter;
};


class CCircleShadowModuleForGimmick final : public CCircleShadowModule
{
public:
    CCircleShadowModuleForGimmick(CGimmick* pGimmick, float w, float h) : CCircleShadowModule(w, h), m_pGimmick(pGimmick) {};
    virtual ~CCircleShadowModuleForGimmick(void) {};
    virtual void GetPosition(RwV3d* pvPosition) const override { return m_pGimmick->GetPosition(pvPosition); };
    virtual float GetDirection(void) const override { return 0.0f; };

private:
    CGimmick* m_pGimmick;
};


/*static*/ CCircleShadowModule* CCircleShadowModule::New(CGameObject* pGameObject, float w, float h, bool bEnable)
{
    ASSERT(pGameObject);

    CCircleShadowModule* pResult = nullptr;
    
    switch (pGameObject->GetType())
    {
    case GAMEOBJECTTYPE::CHARACTER:
        pResult = new CCircleShadowModuleForCharacter(static_cast<CCharacter*>(pGameObject), w, h);
        break;

    case GAMEOBJECTTYPE::GIMMICK:
        pResult = new CCircleShadowModuleForGimmick(static_cast<CGimmick*>(pGameObject), w, h);
        break;

    default:
        ASSERT(false);
        break;
    };

    if (pResult)
        pResult->SetEnable(bEnable);

    return pResult;
};


/*static*/ void CCircleShadowModule::DrawAll(void)
{
	CRenderStateStack RsStack;
	RsStack.Set(rwRENDERSTATEZWRITEENABLE, false);
	RsStack.Set(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
	RsStack.Set(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RsStack.Push(rwRENDERSTATEVERTEXALPHAENABLE);

    for (CCircleShadowModule& it : m_listDraw)
        it.DrawNode();
};


/*static*/ CList<CCircleShadowModule> CCircleShadowModule::m_listDraw;


CCircleShadowModule::CCircleShadowModule(float w, float h)
: IModule(MODULETYPE::CIRCLE_SHADOW)
, m_pTexture(nullptr)
, m_bEnable(true)
{
    RwMatrixSetIdentityMacro(&m_matrix);
    
    CTextureManager::SetCurrentTextureSet("common_ef");;
    m_pTexture = CTextureManager::GetRwTexture("Shadow1");
    ASSERT(m_pTexture);

    SetSize(w, h);

    m_nodeDraw.data = this;
    m_listDraw.push_back(&m_nodeDraw);
};


CCircleShadowModule::~CCircleShadowModule(void)
{
    ASSERT(!m_listDraw.empty());
    m_listDraw.erase(&m_nodeDraw);
};


void CCircleShadowModule::Run(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    GetPosition(&vPosition);

    float fHeight = CWorldMap::GetMapHeight(&vPosition);

    const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
    ASSERT(pCollisionResult);

    if (!pCollisionResult)
        return;

    float fCharacterHeight = vPosition.y - fHeight;
    fCharacterHeight = Clamp(fCharacterHeight, 0.0f, 5.0f);

    float fScale = (1.0f - 0.5f * (fCharacterHeight * 0.2f));
    uint8 uAlphaBasis = uint8(255.0f - 191.0f * (fCharacterHeight * 0.2f));
    RwV3d vNormal = pCollisionResult->m_vNormal;
	
    vPosition.y = fHeight + 0.05f;

    RwV3d vAxisX = Math::VECTOR3_ZERO;
    RwV3d vAxisY = Math::VECTOR3_ZERO;
    RwV3d vAxisZ = Math::VECTOR3_ZERO;
    RwMatrix matShadow;
    RwMatrix matTranslation;
    RwMatrix matScaling;
    RwMatrix matRotate;

    RwMatrixSetIdentityMacro(&matShadow);
    RwMatrixSetIdentityMacro(&matTranslation);
    RwMatrixSetIdentityMacro(&matScaling);
    RwMatrixSetIdentityMacro(&matRotate);

    Math::Vec3_Cross(&vAxisX, &Math::VECTOR3_AXIS_Z, &vNormal);
    Math::Vec3_Cross(&vAxisZ, &vNormal, &Math::VECTOR3_AXIS_X);
    Math::Vec3_Normalize(&vAxisX, &vAxisX);
    Math::Vec3_Normalize(&vAxisY, &vNormal);
    Math::Vec3_Normalize(&vAxisZ, &vAxisZ);
    
    Math::Matrix_Update(&matShadow, &vAxisX, &vAxisY, &vAxisZ, &Math::VECTOR3_ZERO);
    Math::Matrix_Scale(&matScaling, fScale, fScale, fScale);
    Math::Matrix_RotateY(&matRotate, GetDirection());
    Math::Matrix_Translate(&matTranslation, &vPosition);

    RwMatrixSetIdentityMacro(&m_matrix);
    Math::Matrix_Multiply(&m_matrix, &matScaling, &matRotate);
    Math::Matrix_Multiply(&m_matrix, &m_matrix, &matShadow);
    Math::Matrix_Multiply(&m_matrix, &m_matrix, &matTranslation);

    RwRGBA VertexColor = { 0xFF, 0xFF, 0xFF, uAlphaBasis };
    m_aVertices[0].color = RWRGBALONGEX(VertexColor);
    m_aVertices[1].color = RWRGBALONGEX(VertexColor);
    m_aVertices[2].color = RWRGBALONGEX(VertexColor);
    m_aVertices[3].color = RWRGBALONGEX(VertexColor);
};


void CCircleShadowModule::Draw(void)
{
    m_bDrawRequest = true;
};


void CCircleShadowModule::SetSize(float w, float h)
{
    for (int32 i = 0; i < COUNT_OF(m_aVertices); ++i)
    {
        RwIm3DVertex* pVertex = &m_aVertices[i];

        pVertex->objVertex.x = (2 % (i + 1) != 0 ? -1.0f : 1.0f) * (w * 0.5f);
        pVertex->objVertex.y = 0.0f;
        pVertex->objVertex.z = (2.0f * (i % 2 == 0) - 1) * (h * 0.5f);
        pVertex->objNormal = { 0 };
        pVertex->color = 0xFFFFFFFF;
        pVertex->u = (i / 2 ? 1.0f : 0.0f);
        pVertex->v = (i % 2 ? 0.0f : 1.0f);
    };
};


void CCircleShadowModule::SetEnable(bool bEnable)
{
    m_bEnable = bEnable;
};


bool CCircleShadowModule::IsEnabled(void) const
{
    return m_bEnable;
};


void CCircleShadowModule::DrawNode(void)
{
    if (!m_bEnable || !m_bDrawRequest)
        return;

    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));

    if (RwIm3DTransform(m_aVertices, COUNT_OF(m_aVertices), &m_matrix, rwIM3D_VERTEXUV | rwIM3D_ALLOPAQUE))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
        RwIm3DEnd();
    };

    m_bDrawRequest = false;
};