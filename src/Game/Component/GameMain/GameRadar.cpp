#include "GameRadar.hpp"

#include "Game/Component/Enemy/Enemy.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameEnemy.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Utils/RenderWareUtils.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Sprite.hpp"


static const float SCREEN_AREA_ANGLE[] = { 22.5f, 67.5f, 112.5f, 157.5f, 180.0f };

static const float SCREEN_AREA_0_START = MATH_DEG2RAD(0.0f);
static const float SCREEN_AREA_0_END = MATH_DEG2RAD(SCREEN_AREA_ANGLE[0]);

static const float SCREEN_AREA_1_START = SCREEN_AREA_0_END;
static const float SCREEN_AREA_1_END = MATH_DEG2RAD(SCREEN_AREA_ANGLE[1]);

static const float SCREEN_AREA_2_START = SCREEN_AREA_1_END;
static const float SCREEN_AREA_2_END = MATH_DEG2RAD(SCREEN_AREA_ANGLE[2]);

static const float SCREEN_AREA_3_START = SCREEN_AREA_2_END;
static const float SCREEN_AREA_3_END = MATH_DEG2RAD(SCREEN_AREA_ANGLE[3]);

static const float SCREEN_AREA_4_START = SCREEN_AREA_3_END;
static const float SCREEN_AREA_4_END = MATH_DEG2RAD(SCREEN_AREA_ANGLE[4]);


class CGameRadarImpl
{
private:
    static const int32 PART_NUM = 8;
    
    struct PART
    {
        CSprite Sprite;
        float   Distance;
        bool    DispFlag;
    };

    struct TEXTUREINFO
    {
        RwTexture*  Texture;
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
    
private:
    float m_fDistance;
    bool m_bEnableFlag;
    PART m_aPart[PART_NUM];
};


CGameRadarImpl::CGameRadarImpl(float fDistance)
: m_fDistance(fDistance)
, m_bEnableFlag(true)
, m_aPart()
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
        { {  0.0f,      -108.0f  }, { 32.0f, 128.0f },  -MATH_DEG2RAD(90.0f),     0 },
        { {  0.0f,       148.0f  }, { 32.0f, 128.0f },   MATH_DEG2RAD(90.0f),     0 },
        { { -259.0f,     27.0f   }, { 32.0f, 128.0f },   MATH_DEG2RAD(180.0f),    0 },        
        { {  259.0f,     27.0f   }, { 32.0f, 128.0f },   MATH_DEG2RAD(0.0f),      0 },

        { { -235.0f,    -77.0f   }, { 64.0f, 64.0f },    MATH_DEG2RAD(180.0f),    1 },
        { {  235.0f,    -77.0f   }, { 64.0f, 64.0f },   -MATH_DEG2RAD(90.0f),     1 },
        { { -235.0f,     129.0f  }, { 64.0f, 64.0f },    MATH_DEG2RAD(90.0f),     1 },
        { {  235.0f,     129.0f  }, { 64.0f, 64.0f },    MATH_DEG2RAD(0.0f),      1 },
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
        CEnemy* pEnemy = CGameProperty::GetEnemy(i);
        if(!pEnemy)
            continue;

        RwV3d vEnemyPos = Math::VECTOR3_ZERO;
        pEnemy->GetPosition(&vEnemyPos);

        if (pMapCamera->IsPosVisible(&vEnemyPos))
            continue;

        RwV3d vAt = Math::VECTOR3_ZERO;
        pMapCamera->GetLookat(&vAt);

        RwV3d vDist = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDist, &vEnemyPos, &vAt);

        float fDist = Math::Vec3_Length(&vDist);
        if (fDist < m_fDistance)
        {
            RwUtil::Vec3_TransformToCamera(&vEnemyPos, &vEnemyPos, pMapCamera->GetRwCamera());

            RwV2d vEnemyPos2 = { vEnemyPos.x, vEnemyPos.y };
            SetFindEnemyInfo(&vEnemyPos2, fDist);
        };
    }; 
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
            AlphaDelta = Clamp(AlphaDelta, AlphaDelta, 1.0f);

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