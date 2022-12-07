#include "GameRadar.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameEnemy.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Sprite.hpp"

#ifdef _DEBUG
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#include "System/Common/RenderState.hpp"
#endif


#define DEG_TO_RAD(deg) (deg / 180 * Math::PI)


static const float SCREEN_AREA_ANGLE[] = { 22.5f, 67.5f, 112.5f, 157.5f, 180.0f };

static const float SCREEN_AREA_0_START = DEG_TO_RAD(0.0f);
static const float SCREEN_AREA_0_END = DEG_TO_RAD(SCREEN_AREA_ANGLE[0]);

static const float SCREEN_AREA_1_START = SCREEN_AREA_0_END;
static const float SCREEN_AREA_1_END = DEG_TO_RAD(SCREEN_AREA_ANGLE[1]);

static const float SCREEN_AREA_2_START = SCREEN_AREA_1_END;
static const float SCREEN_AREA_2_END = DEG_TO_RAD(SCREEN_AREA_ANGLE[2]);

static const float SCREEN_AREA_3_START = SCREEN_AREA_2_END;
static const float SCREEN_AREA_3_END = DEG_TO_RAD(SCREEN_AREA_ANGLE[3]);

static const float SCREEN_AREA_4_START = SCREEN_AREA_3_END;
static const float SCREEN_AREA_4_END = DEG_TO_RAD(SCREEN_AREA_ANGLE[4]);

#ifdef _DEBUG
static bool RADAR_DEBUG_PROGRAMM = false;
#endif


static inline void inverseTransformOneVector(RwV3d* pvOut, const RwV3d* pvIn, RwMatrix* pMatrix)
{
    pvOut->x = Math::Vec3_Dot(pvIn, &pMatrix->right);
    pvOut->y = Math::Vec3_Dot(pvIn, &pMatrix->up);
};


static inline void inverseTransformOnePoint(RwV3d* pvOut, const RwV3d* pvIn, RwMatrix* pMatrix)
{
    RwV3d vec = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vec, pvIn, &pMatrix->pos);
    inverseTransformOneVector(pvOut, &vec, pMatrix);
};


static void transformToCamera(RwV3d* pvOut, const RwV3d* pvIn, RwCamera* pCamera)
{
    ASSERT(pvOut);
    ASSERT(pvIn);
    ASSERT(pCamera);

    RwFrame* pFrame = RwCameraGetFrameMacro(pCamera);
    ASSERT(pFrame);

    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);
    ASSERT(pMatrix);

    inverseTransformOnePoint(pvOut, pvIn, pMatrix);
};


class CGameRadarImpl
{
private:
    static const int32 PART_NUM = 8;
    
    struct PART
    {
        CSprite Sprite;
        float Distance;
        bool DispFlag;
    };

    struct TEXTUREINFO
    {
        RwTexture* Texture;
        const char* TextureName;
    };

public:
    CGameRadarImpl(float fDistance);
    ~CGameRadarImpl(void);
    void Update(void);
    void Draw(void);
    void Reset(void);
    void SetFindEnemyInfo(RwV2d* pvEnemyPosition, float fEnemyDistance);
    void SetEnable(bool bState);
    bool IsEnabled(void) const;
    void DebugProgrammUpdate(void);
	void DebugProgrammDraw(void);
    
private:
    float m_fDistance;
    bool m_bEnableFlag;
    PART m_aPart[PART_NUM];
#ifdef _DEBUG
	RwV2d m_vDbgScrPos;
	bool m_bDbgDispRequestFlag;
#endif
};


CGameRadarImpl::CGameRadarImpl(float fDistance)
: m_fDistance(fDistance)
, m_bEnableFlag(true)
, m_aPart()
#ifdef _DEBUG
, m_vDbgScrPos(Math::VECTOR2_ZERO)
, m_bDbgDispRequestFlag(false)
#endif
{
    ASSERT(m_fDistance > 0.0f);
    
    struct TEXTUREINFO
    {
        RwTexture* Texture;
        const char* TextureName;
    };

    struct PARTINFO
    {
        RwV2d Position;
        RwV2d Size;
        float Rotation;
        int32 TextureIndex;
    };

    TEXTUREINFO aTextureInfo[] =
    {
        { nullptr, "Sign90" },
        { nullptr, "Sign45" },
    };
	
	CTextureManager::SetCurrentTextureSet("EnemyRadar");

    for (int32 i = 0; i < COUNT_OF(aTextureInfo); ++i)
    {
        aTextureInfo[i].Texture = CTextureManager::GetRwTexture(aTextureInfo[i].TextureName);
        ASSERT(aTextureInfo[i].Texture);
    };

    const PARTINFO aPartInfo[] =
    {
        { { 0.0f,       -108.0f }, { 32.0f, 128.0f },   -DEG_TO_RAD(90.0f),    0 },
        { { 0.0f,       148.0f  }, { 32.0f, 128.0f },   DEG_TO_RAD(90.0f),     0 },
        { { -259.0f,    27.0f   }, { 32.0f, 128.0f },   DEG_TO_RAD(180.0f),    0 },        
        { { 259.0f,     27.0f   }, { 32.0f, 128.0f },   DEG_TO_RAD(0.0f),      0 },

        { { -235.0f,    -77.0f  }, { 64.0f, 64.0f },    DEG_TO_RAD(180.0f),    1 },
        { { 235.0f,     -77.0f  }, { 64.0f, 64.0f },    -DEG_TO_RAD(90.0f),    1 },
        { { -235.0f,    129.0f  }, { 64.0f, 64.0f },    DEG_TO_RAD(90.0f),     1 },
        { { 235.0f,     129.0f  }, { 64.0f ,64.0f },    DEG_TO_RAD(0.0f),      1 },
    };

    static_assert(COUNT_OF(aPartInfo) == COUNT_OF(m_aPart), "update me");

    for (int32 i = 0; i < COUNT_OF(m_aPart); ++i)
    {
        m_aPart[i].Sprite.SetTexture(aTextureInfo[aPartInfo[i].TextureIndex].Texture);
        m_aPart[i].Sprite.Move(aPartInfo[i].Position.x, aPartInfo[i].Position.y);
        m_aPart[i].Sprite.Resize(aPartInfo[i].Size.x, aPartInfo[i].Size.y);
        m_aPart[i].Sprite.SetRotate(aPartInfo[i].Rotation);
    };

    Reset();
};


CGameRadarImpl::~CGameRadarImpl(void)
{
    ;
};


void CGameRadarImpl::Update(void)
{
    if (!IsEnabled())
        return;

    CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
    ASSERT(pMapCamera);
    
    Reset();

    int32 EnemyCnt = CGameProperty::GetEnemyMax();
    for (int32 i = 0; i < EnemyCnt; ++i)
    {
        if (!CGameProperty::Enemy(i).IsAlive())
            continue;

        RwV3d vEnemyPos = Math::VECTOR3_ZERO;
        CGameProperty::Enemy(i).GetPosition(&vEnemyPos);

        if (!pMapCamera->IsPosVisible(&vEnemyPos))
        {
            RwV3d vAt = Math::VECTOR3_ZERO;
            pMapCamera->GetLookat(&vAt);
            
            RwV3d vDist = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vDist, &vEnemyPos, &vAt);

            float fDist = Math::Vec3_Length(&vDist);
            if (fDist < m_fDistance)
            {
                transformToCamera(&vEnemyPos, &vEnemyPos, pMapCamera->GetRwCamera());

                RwV2d vEnemyPos2 = { vEnemyPos.x, vEnemyPos.y };
                SetFindEnemyInfo(&vEnemyPos2, fDist);
            };
        };
    };

#ifdef _DEBUG
	if (RADAR_DEBUG_PROGRAMM)
		DebugProgrammUpdate();
#endif    
};

	
void CGameRadarImpl::Draw(void)
{
    if (!IsEnabled())
        return;

    CSystem2D::PushRenderState();

    for (int32 i = 0; i < COUNT_OF(m_aPart); ++i)
    {
        if (m_aPart[i].DispFlag)
            m_aPart[i].Sprite.DrawRotate();
    };

#ifdef _DEBUG
	if (RADAR_DEBUG_PROGRAMM)
		DebugProgrammDraw();    
#endif

	CSystem2D::PopRenderState();
};


void CGameRadarImpl::Reset(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aPart); ++i)
    {
        m_aPart[i].Sprite.SetAlpha(0);
        m_aPart[i].Distance = -1.0f;
        m_aPart[i].DispFlag = false;
    };
};


void CGameRadarImpl::SetFindEnemyInfo(RwV2d* pvEnemyPosition, float fEnemyDistance)
{
    ASSERT(pvEnemyPosition);
    ASSERT((fEnemyDistance > 0.0f) && (fEnemyDistance <= m_fDistance));

    if ((fEnemyDistance > 0.0f) && (fEnemyDistance <= m_fDistance))
    {
        int32 aScreenPartIndex[] =
        {
            2,  // left
            4,  // left-down
            0,  // down
            5,  // right down
            3,  // right
            
            2,  // left
            6,  // left-up
            1,  // up
            7,  // right-up
            3   // right
        };
        
        int32 PartIndex = -1;
		int32 ScrAreaNo = -1;

        bool UpFlag = false;
        RwV2d Pos = Math::VECTOR2_ZERO;

        Math::Vec2_Normalize(&Pos, pvEnemyPosition);
        if (Pos.y < 0.0f)
            UpFlag = true;
        
        float ScrRot = Math::ACos(Pos.x);
        if ((ScrRot >= SCREEN_AREA_0_START) && (ScrRot < SCREEN_AREA_0_END))
        {
			ScrAreaNo = 0;
        }
        else if ((ScrRot >= SCREEN_AREA_1_START) && (ScrRot < SCREEN_AREA_1_END))
        {
			ScrAreaNo = 1;
        }
        else if ((ScrRot >= SCREEN_AREA_2_START) && (ScrRot < SCREEN_AREA_2_END))
        {
			ScrAreaNo = 2;
        }
        else if ((ScrRot >= SCREEN_AREA_3_START) && (ScrRot < SCREEN_AREA_3_END))
        {
			ScrAreaNo = 3;
        }
        else if ((ScrRot >= SCREEN_AREA_4_START) && (ScrRot <= SCREEN_AREA_4_END))
        {
			ScrAreaNo = 4;
        }
        else
        {
            ASSERT(false);
        };

		PartIndex = aScreenPartIndex[((UpFlag) ? ((COUNT_OF(aScreenPartIndex) / 2) + ScrAreaNo) : (ScrAreaNo))];
        ASSERT((PartIndex >= 0) && (PartIndex < COUNT_OF(m_aPart)));
		PART* pPart = &m_aPart[PartIndex];

        if ((pPart->Distance < 0.0f) || (fEnemyDistance < pPart->Distance))
        {
            float AlphaDelta = (1.0f - (fEnemyDistance / m_fDistance)) + 0.05f;
            AlphaDelta = Math::Clamp(AlphaDelta, AlphaDelta, 1.0f);

            uint8 AlphaBasis = uint8(AlphaDelta * 255.0f);

            pPart->Sprite.SetAlpha(AlphaBasis);
            pPart->Distance = fEnemyDistance;
            pPart->DispFlag = true;
        };
    };
};


void CGameRadarImpl::SetEnable(bool bState)
{
    m_bEnableFlag = bState;
};


bool CGameRadarImpl::IsEnabled(void) const
{
    return m_bEnableFlag;
};


void CGameRadarImpl::DebugProgrammUpdate(void)
{
#ifdef _DEBUG
    CMapCamera* pMapCamera = CGameProperty::GetMapCamera();

	float PosOfs = 5.0f;
	float PosRadius = 1.0f;
	int32 PlayerNo = 0;

    RwV3d vPlayerPos = Math::VECTOR3_ZERO;
    CGameProperty::Player(PlayerNo).GetPosition(&vPlayerPos);

	RwV3d vPlayerDir = Math::VECTOR3_AXIS_Z;
	CGameProperty::Player(PlayerNo).GetCurrentCharacter()->RotateVectorByDirection(&vPlayerDir, &vPlayerDir);

	Math::Vec3_Scale(&vPlayerDir, &vPlayerDir, PosOfs);
	Math::Vec3_Add(&vPlayerPos, &vPlayerPos, &vPlayerDir);

	RwSphere ShapeSphere = { vPlayerPos, PosRadius };
	CDebugShape::ShowSphere(&ShapeSphere, { 0xFF, 0x00, 0x00, 0x80 });

    RwV3d vAt = Math::VECTOR3_ZERO;
    pMapCamera->GetLookat(&vAt);

    RwV3d vDist = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vDist, &vPlayerPos, &vAt);

    float fDist = Math::Vec3_Length(&vDist);
    if (fDist <= m_fDistance)
    {
        transformToCamera(&vPlayerPos, &vPlayerPos, pMapCamera->GetRwCamera());

        RwV2d vPlayerPos2 = { vPlayerPos.x, vPlayerPos.y };
        SetFindEnemyInfo(&vPlayerPos2, fDist);

        m_vDbgScrPos = { vPlayerPos.x, vPlayerPos.y };
        m_bDbgDispRequestFlag = true;
    }
    else
    {
        m_bDbgDispRequestFlag = false;
    };
#endif
};


void CGameRadarImpl::DebugProgrammDraw(void)
{
#ifdef _DEBUG
	if (m_bDbgDispRequestFlag)
	{
		m_bDbgDispRequestFlag = false;

		m_vDbgScrPos = { -m_vDbgScrPos.x, -m_vDbgScrPos.y };
		Math::Vec2_Normalize(&m_vDbgScrPos, &m_vDbgScrPos);

		CSprite sprite;
		sprite.Move(
			m_vDbgScrPos.x * (CSystem2D::VirtualScreenWidth() * 0.5f) * 0.5f,
			m_vDbgScrPos.y * (CSystem2D::VirtualScreenHeight() * 0.5f) * 0.5f
		);
		sprite.Resize(16.0f, 16.0f);
		sprite.SetRGBA(255, 0, 0, 255);
		RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
		sprite.Draw();
	};
#endif
};


static CGameRadarImpl* s_pGameRadarImpl = nullptr;


static inline CGameRadarImpl& GameRadarImpl(void)
{
    ASSERT(s_pGameRadarImpl);
    return *s_pGameRadarImpl;
};


/*static*/ void CGameRadar::Initialize(float fDistance)
{
    if (!s_pGameRadarImpl)
        s_pGameRadarImpl = new CGameRadarImpl(fDistance);
};


/*static*/ void CGameRadar::Terminate(void)
{
    if (s_pGameRadarImpl)
    {
        delete s_pGameRadarImpl;
        s_pGameRadarImpl = nullptr;
    };
};


/*static*/ void CGameRadar::Update(void)
{
    GameRadarImpl().Update();
};


/*static*/ void CGameRadar::Draw(void)
{
    GameRadarImpl().Draw();
};


/*static*/ void CGameRadar::SetEnable(bool bState)
{
    GameRadarImpl().SetEnable(bState);
};


/*static*/ bool CGameRadar::IsEnabled(void)
{
    return GameRadarImpl().IsEnabled();
};