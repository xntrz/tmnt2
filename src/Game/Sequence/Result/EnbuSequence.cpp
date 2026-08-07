#include "EnbuSequence.hpp"
#include "EnbuProc.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameLoader.hpp"
#include "Game/System/Character/CharacterManager.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Model/ToonManager.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/MotionParameter/MotionParameterManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Camera.hpp"


/*static*/ CProcess* CEnbuSequence::Instance(void)
{
    return new CEnbuSequence;
};


CEnbuSequence::CEnbuSequence(void)
: m_pLight(nullptr)
#if defined(TMNT2_RWDRV_OPENGL)
, m_pLightAmbient(nullptr)
#endif /* defined(TMNT2_RWDRV_OPENGL) */
, m_pWorld(nullptr)
, m_pCamera(nullptr)
, m_step(STEP_INIT)
, m_fCameraRotY(0.0f)
, m_fDistEye(3.5f)
, m_fEyeOfsY(1.0f)
, m_fCameraOfsY(1.25f)
, m_afAnimTime()
, m_uAnimStep(0)
{
    std::memset(m_afAnimTime, 0, sizeof(m_afAnimTime));
};


CEnbuSequence::~CEnbuSequence(void)
{
    ASSERT(!m_pLight);
#if defined(TMNT2_RWDRV_OPENGL)
    ASSERT(!m_pLightAmbient);
#endif /* defined(TMNT2_RWDRV_OPENGL) */
    ASSERT(!m_pWorld);
    ASSERT(!m_pCamera);
};


bool CEnbuSequence::OnAttach(const void* pParam)
{
    CTextureManager::GenerationInc();
    CBodyHitManager::Initialize();
    CModelManager::Initialize();
    CMotionManager::Initialize();
    CMotionParameterManager::Initialize();
    CToonManager::Initialize();
    CWorldMap::Initialize();
    CGameObjectManager::Initialize();
    CGameProperty::Initialize();
    CEffectManager::Initialize();
    CCharacterManager::Initialize();

#ifdef _DEBUG
    CDebugShape::Initialize();
#endif /* _DEBUG */

    CEnbuProc::Initialize();
    
    CGameLoader::LoadStageCommonData(GAMETYPES::STAGEMODE_NONE);
    CGameLoader::LoadEnbu(CEnbuProc::GetEnbuMvp(), CEnbuProc::GetEnbuCostume());

#ifdef TMNT2_BUILD_EU    
    CDataLoader::Regist(FPATH_LANG("Language/English/Enbu_Rank/Enbu_Rank.lpac"));
#endif /* TMNT2_BUILD_EU */
    
    return true;
};


void CEnbuSequence::OnDetach(void)
{
    CGameSound::FadeOut(CGameSound::FADESPEED_NORMAL);

    CEnbuProc::Terminate();

#ifdef _DEBUG
    CDebugShape::Terminate();
#endif /* _DEBUG */

    CCharacterManager::Terminate();
    CEffectManager::Terminate();
    CGameProperty::Terminate();
    CGameObjectManager::Terminate();
    CWorldMap::Terminate();
    CToonManager::Terminate();
    CMotionParameterManager::Terminate();
    CMotionManager::Terminate();
    CModelManager::Terminate();
    CBodyHitManager::Terminate();
    CTextureManager::GenerationDec();

    if (m_pLight)
    {
        RpWorldRemoveLight(m_pWorld, m_pLight);
        RpLightDestroy(m_pLight);
        m_pLight = nullptr;
    };

#if defined(TMNT2_RWDRV_OPENGL)
    if (m_pLightAmbient)
    {
        RpWorldRemoveLight(m_pWorld, m_pLightAmbient);
        RpLightDestroy(m_pLightAmbient);
        m_pLightAmbient = nullptr;
    };
#endif /* defined(TMNT2_RWDRV_OPENGL) */

	if (m_pCamera)
	{
		RpWorldRemoveCamera(m_pWorld, m_pCamera->GetRwCamera());
		m_pCamera->Release();
		m_pCamera = nullptr;
	};

    if (m_pWorld)
    {
        RpWorldDestroy(m_pWorld);
        m_pWorld = nullptr;
    };
};


void CEnbuSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_step)
    {
    case STEP_INIT:
        {
            if (CDataLoader::IsLoadEnd())
            {
                CGameData::Attribute().SetInteractive(true);

                RwBBox bbox =
                {
                    {  100.0f,  100.0f,  100.0f  },
                    { -100.0f, -100.0f, -100.0f  },
                };

                m_pWorld = RpWorldCreate(&bbox);
                ASSERT(m_pWorld);

                m_pLight = RpLightCreate(rpLIGHTDIRECTIONAL);
                ASSERT(m_pLight);
                
                RwFrame* pFrame = RwFrameCreate();
                ASSERT(pFrame);

                RwFrameRotate(pFrame, &Math::VECTOR3_AXIS_X, 150.0f, rwCOMBINEREPLACE);
                RwFrameRotate(pFrame, &Math::VECTOR3_AXIS_Y, 0.0f, rwCOMBINEPOSTCONCAT);
                RpLightSetFrame(m_pLight, pFrame);
                
                m_pCamera = CCamera::GetCamera();
                ASSERT(m_pCamera);

                CGameProperty::SetCurrentRwCamera(m_pCamera->GetRwCamera());

                RpWorldAddCamera(m_pWorld, m_pCamera->GetRwCamera());
                RpWorldAddLight(m_pWorld, m_pLight);

#if defined(TMNT2_RWDRV_OPENGL)
                m_pLightAmbient = RpLightCreate(rpLIGHTAMBIENT);
                if (m_pLightAmbient)
                {
                    RwRGBAReal lightColor = { 0.6f, 0.6f, 0.6f, 1.0f };
                    RpLightSetColor(m_pLightAmbient, &lightColor);

                    RwUInt8 lightFlags = RpLightGetFlags(m_pLightAmbient);
                    lightFlags &= (~rpLIGHTLIGHTWORLD);
                    RpLightSetFlags(m_pLightAmbient, lightFlags);

                    RpWorldAddLight(m_pWorld, m_pLightAmbient);
                };
#endif /* defined(TMNT2_RWDRV_OPENGL) */

                const char* pszTextureSet = nullptr;
#if defined(TARGET_PS2)
                pszTextureSet = "toon";
#elif (defined(TARGET_PC) || \
       defined(TARGET_WEB))
                pszTextureSet = "toonPC";
#else
#error Not implemented for current target
#endif
                CToonManager::SetTextureSet(pszTextureSet);

                CEnbuProc::Settings();

                SetEnbuCameraInit();
                CScreenFade::BlackIn(1.0f);
                CGameSound::PlayBGM(SDCODE_BGM(12326));
                
                m_step = STEP_RUN;
            }
            else
            {
                CDataLoader::Period();
            };
        }
        break;

    case STEP_RUN:
        {
            CEnbuProc::Period();
            EnbuCameraProc();

            RwMatrix matrix;
            RwMatrixSetIdentity(&matrix);
            Math::Matrix_RotateY(&matrix, m_fCameraRotY);

            RwV3d vEye = Math::VECTOR3_ZERO;
            RwV3dTransformPoint(&vEye, &Math::VECTOR3_AXIS_Z, &matrix);
            Math::Vec3_Normalize(&vEye, &vEye);
            Math::Vec3_Scale(&vEye, &vEye, m_fDistEye);

            RwV3d vAt = { 0.0f, m_fCameraOfsY, 0.0f };
            Math::Vec3_Add(&vEye, &vEye, &vAt);

            vEye.y += m_fEyeOfsY;

            Math::Matrix_LookAt(&matrix, &vEye, &vAt, &Math::VECTOR3_AXIS_Y);
            
            RwCamera* pCamera = m_pCamera->GetRwCamera();
            RwFrame* pFrame = RwCameraGetFrame(pCamera);
            RwFrameTransform(pFrame, &matrix, rwCOMBINEREPLACE);

            if (CController::GetDigitalTrigger(CController::CONTROLLER_LOCKED_ON_VIRTUAL, CController::DIGITAL_OK))
            {
                CGameData::Attribute().SetInteractive(false);
                CScreenFade::BlackOut();
                CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
                m_step = STEP_END;
            };
        }
        break;

    case STEP_END:
        {
            CEnbuProc::Period();
            if (!CScreenFade::IsFading())
                m_step = STEP_EOL;
        }
        break;

    case STEP_EOL:
        Ret();
        break;

    default:
        break;
    };
};


void CEnbuSequence::OnDraw(void) const
{
    if ((m_step >  STEP_INIT) &&
        (m_step <= STEP_END))
    {
        if (m_pCamera->BeginScene())
        {
            CRenderStateManager::SetDefault();
            CEnbuProc::Draw3D();
            m_pCamera->EndScene();
        };

        CEnbuProc::Draw2D();
    };
};


void CEnbuSequence::EnbuCameraProc(void)
{
    float fEnbuTime = CEnbuProc::GetEnbuTime();
    if (fEnbuTime == 0.0f)
        return;

    if (CEnbuProc::IsEnbuIdle())
        return;

#define AddTime(timer, duration)                        \
        ((timer) += CScreen::TimerStride(),             \
         (timer) = Clamp((timer), (timer), (duration)), \
         (timer) < (duration))

    int32 nCameraType = GetEnbuCameraType();
    switch (nCameraType)
    {
    case 0:
        {
            ;
        }
        break;
        
    case 1:
        {
            float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate() * CEnbuProc::GetEnbuEndTime());

            m_fCameraOfsY = 0.85f;
            m_fDistEye = Math::LinearTweenAutoRet(4.0f, -1.0f, m_afAnimTime[0], fDuration);

            AddTime(m_afAnimTime[0], fDuration);            
        }
        break;
        
    case 2:
        {
            m_fCameraOfsY = 1.25f;
            m_fEyeOfsY = 0.45f;
        }
        break;
        
    case 3:
        {
            switch (m_uAnimStep)
            {
            case 0:
                {
                    float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate());

                    m_fEyeOfsY = Math::LinearTweenAutoRet(1.0f, -0.5f, m_afAnimTime[0], fDuration);
                    m_fDistEye = Math::LinearTweenAutoRet(3.5f, -1.0f, m_afAnimTime[0], fDuration);

                    if (!AddTime(m_afAnimTime[0], fDuration))
                        ++m_uAnimStep;
                }
                break;

            case 1:
                {
                    float fDurationSub = ANIM_DURATION_FRAMES((CEnbuProc::GetEnbuEndTime() - 1.0f) * CScreen::Framerate());

                    m_fEyeOfsY = Math::LinearTweenAutoRet(0.5f, 0.25f, m_afAnimTime[1], fDurationSub);
                    m_fDistEye = Math::LinearTweenAutoRet(2.5f, 1.0f, m_afAnimTime[1], fDurationSub);

                    AddTime(m_afAnimTime[1], fDurationSub);                    
                }
                break;

            default:
                break;
            };
        }
        break;
        
    case 4:
        {
            float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate() * CEnbuProc::GetEnbuEndTime());

            m_fEyeOfsY = Math::LinearTweenAutoRet(1.0f, -0.5f, m_afAnimTime[0], fDuration);
            m_fDistEye = Math::LinearTweenAutoRet(3.5f, -1.0f, m_afAnimTime[0], fDuration);

            AddTime(m_afAnimTime[0], fDuration);            
        }
        break;
        
    case 5:
    case 6:
    case 7:
        {
            float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate() * CEnbuProc::GetEnbuEndTime());

            if (nCameraType == 6)
            {
                m_fCameraRotY = Math::LinearTweenAutoRet(2.0f, -2.0f, m_afAnimTime[0], fDuration);
            }
            else if (nCameraType == 5)
            {
                m_fCameraRotY = Math::LinearTweenAutoRet(-2.0f, 2.0f, m_afAnimTime[0], fDuration);
            };

            m_fDistEye = Math::LinearTweenAutoRet(3.0f, 0.5f, m_afAnimTime[0], fDuration);

            AddTime(m_afAnimTime[0], fDuration);
        }
        break;
        
    case 8:
        {
            float fDuration = ANIM_DURATION_FRAMES((CEnbuProc::GetEnbuEndTime() - 0.4f) * CScreen::Framerate());

            m_fCameraRotY = Math::LinearTweenAutoRet(0.0f, -3.65f, m_afAnimTime[0], fDuration);

            AddTime(m_afAnimTime[0], fDuration);
        }
        break;
        
    case 9:
        {
            float fDuration = ANIM_DURATION_FRAMES((CScreen::Framerate() * 1.5f) * 2.0f);

            m_fCameraRotY = Math::LinearTweenAutoRet(-2.0f, 2.0f, m_afAnimTime[0], fDuration);

            AddTime(m_afAnimTime[0], fDuration);
        }
        break;
        
    case 10:
        {
            float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate() * CEnbuProc::GetEnbuEndTime());

            m_fCameraRotY = Math::LinearTweenAutoRet(0.0f, -0.08f, m_afAnimTime[0], fDuration);
            m_fDistEye = Math::LinearTweenAutoRet(3.0f, 0.5f, m_afAnimTime[0], fDuration);

            AddTime(m_afAnimTime[0], fDuration);
        }
        break;
        
    case 11:
        {
            float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate() * CEnbuProc::GetEnbuEndTime());

            m_fCameraRotY = Math::LinearTweenAutoRet(0.0f, -0.18f, m_afAnimTime[0], fDuration);
            m_fDistEye = Math::LinearTweenAutoRet(3.0f, 0.5f, m_afAnimTime[0], fDuration);

            AddTime(m_afAnimTime[0], fDuration);
        }
        break;
        
    case 12:
        {
            float fDuration = ANIM_DURATION_FRAMES((CEnbuProc::GetEnbuEndTime() - 1.0f) * CScreen::Framerate());
            
            m_fCameraRotY = Math::LinearTweenAutoRet(0.0f, 0.86f, m_afAnimTime[0], fDuration);
            
            AddTime(m_afAnimTime[0], fDuration);
        }
        break;
        
    case 13:
        {
            switch (m_uAnimStep)
            {
            case 0:
                {
                    float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate() * 3.0f);

                    if (!AddTime(m_afAnimTime[0], fDuration))
                        ++m_uAnimStep;
                }
                break;

            case 1:
                {
                    float fDuration = ANIM_DURATION_FRAMES((CEnbuProc::GetEnbuEndTime() - 3.25f) * CScreen::Framerate());

                    m_fCameraOfsY = Math::LinearTween(1.25f, -0.35f, m_afAnimTime[1], fDuration);

                    AddTime(m_afAnimTime[1], fDuration);                    
                }
                break;

            default:
                break;
            };
        }
        break;
        
    case 14:
        {
            switch (m_uAnimStep)
            {
            case 0:
                {
                    float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate());

                    m_fCameraRotY = Math::LinearTween(0.0, -1.5f, m_afAnimTime[0], fDuration);

                    if (!AddTime(m_afAnimTime[0], fDuration))
                        ++m_uAnimStep;
                }
                break;

            case 1:
                {
                    float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate() * 2.0f);

                    if (!AddTime(m_afAnimTime[1], fDuration))
                        ++m_uAnimStep;
                }
                break;

            case 2:
                {
                    float fDuration = ANIM_DURATION_FRAMES((CEnbuProc::GetEnbuEndTime() - 3.5f) * CScreen::Framerate());

                    m_fCameraRotY = Math::LinearTween(-1.5, 1.38f, m_afAnimTime[2], fDuration);

                    AddTime(m_afAnimTime[2], fDuration);
                }
                break;

            default:
                break;
            };
        }
        break;
        
    case 15:
        {
            switch (m_uAnimStep)
            {
            case 0:
                {
                    float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate() * 3.0f);

                    m_fCameraOfsY = 1.0f;

                    if (!AddTime(m_afAnimTime[0], fDuration))
                        ++m_uAnimStep;
                }
                break;

            case 1:
                {
                    float fDuration = ANIM_DURATION_FRAMES((CEnbuProc::GetEnbuEndTime() - 3.25f) * CScreen::Framerate());

                    m_fCameraOfsY = Math::LinearTween(1.0, -0.15f, m_afAnimTime[1], fDuration);

                    AddTime(m_afAnimTime[1], fDuration);
                }
                break;

            default:
                break;
            };
        }
        break;
        
    case 16:
        {
            float fDuration = ANIM_DURATION_FRAMES(CEnbuProc::GetEnbuEndTime() * CScreen::Framerate());
            
            m_fCameraRotY = Math::LinearTween(0.0, 0.8f, m_afAnimTime[0], fDuration);

            AddTime(m_afAnimTime[0], fDuration);
        }
        break;
        
    case 17:
        {
            float fDuration = ANIM_DURATION_FRAMES((CEnbuProc::GetEnbuEndTime() - 0.25f) * CScreen::Framerate());

            m_fCameraRotY = Math::LinearTween(-2.0f, 1.72f, m_afAnimTime[0], fDuration);
            m_fDistEye = Math::LinearTween(3.0f, 0.5f, m_afAnimTime[0], fDuration);

            AddTime(m_afAnimTime[0], fDuration);
        }
        break;
        
    case 18:
        {
            float fDuration = ANIM_DURATION_FRAMES((CEnbuProc::GetEnbuEndTime() - 0.25f) * CScreen::Framerate());
            
            m_fCameraRotY = Math::LinearTween(0.0, -0.16f, m_afAnimTime[0], fDuration);
            
            AddTime(m_afAnimTime[0], fDuration);
        }
        break;
        
    case 19:
        {
            switch (m_uAnimStep)
            {
            case 0:
                {
                    float fDuration = ANIM_DURATION_FRAMES(CScreen::Framerate() * 2.0f);

                    if (!AddTime(m_afAnimTime[0], fDuration))
                        ++m_uAnimStep;
                }
                break;

            case 1:
                {
                    float fDuration = ANIM_DURATION_FRAMES((CEnbuProc::GetEnbuEndTime() - 2.25f) * CScreen::Framerate());

                    m_fCameraRotY = Math::LinearTween(0.0, -0.6f, m_afAnimTime[1], fDuration);

                    AddTime(m_afAnimTime[1], fDuration);
                }
                break;

            default:
                break;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


int32 CEnbuSequence::GetEnbuCameraType(void) const
{
    static int32 s_aCameraTypeList[PLAYERID::ID_MAX][GAMETYPES::CLEARRANK_NUM - 1] =
    {
        //  E    D    C    B    A    S   SS
        {   0,   0,  19,  12,   0,   7,   7  },  // leo
        {   0,  17,   0,   0,  13,   0,   9  },  // rap
        {   0,   0,   0,   0,   7,   0,   7  },  // mic
        {   0,   0,   0,   0,  18,   0,  10  },  // don
        {   0,   0,   0,   2,   0,   2,   0  },  // sla
        {   0,   0,   0,   0,   7,  11,   0  },  // cas
        {   0,   0,   0,  14,   7,   7,   7  },  // kar
        {   0,  15,  15,   0,  16,   7,   8  },  // spl
    };

    int32 mvp = CEnbuProc::GetEnbuMvp();
    ASSERT(mvp >= 0);
    ASSERT(mvp < COUNT_OF(s_aCameraTypeList));

    int32 rank = CEnbuProc::GetEnbuRank() - 1;
    ASSERT(rank >= 0);
    ASSERT(rank < COUNT_OF(s_aCameraTypeList[0]));

    return s_aCameraTypeList[mvp][rank];
};


void CEnbuSequence::SetEnbuCameraInit(void)
{
    m_fCameraRotY   = 0.0f;
    m_fDistEye      = 3.5f;
    m_fEyeOfsY      = 1.0f;
    m_fCameraOfsY   = 1.25f;
    m_uAnimStep     = 0;
    
    std::memset(m_afAnimTime, 0, sizeof(m_afAnimTime));

    GAMETYPES::CLEARRANK clearrank = CEnbuProc::GetEnbuRank();
    PLAYERID::VALUE idMvp = CEnbuProc::GetEnbuMvp();
    switch (idMvp)
    {
    case PLAYERID::ID_LEO:
        {
            if (clearrank == GAMETYPES::CLEARRANK_A)
                m_fCameraOfsY = 1.0f;
        }
        break;
        
    case PLAYERID::ID_RAP:
        {
            if (clearrank == GAMETYPES::CLEARRANK_E)
                m_fCameraOfsY = 0.9f;
        }
        break;
        
    case PLAYERID::ID_MIC:
        break;
        
    case PLAYERID::ID_DON:
        break;
        
    case PLAYERID::ID_SLA:
        {
            m_fCameraOfsY = 1.5f;
            m_fDistEye = 4.25f;
        }
        break;
        
    case PLAYERID::ID_CAS:
        {
            m_fCameraOfsY = 1.5f;
        }        
        break;
        
    case PLAYERID::ID_KAR:
        {
            if (clearrank == GAMETYPES::CLEARRANK_S)
                m_fCameraOfsY = 1.35f;
        }
        break;
        
    case PLAYERID::ID_SPL:
        {
            if (clearrank == GAMETYPES::CLEARRANK_SS)
                m_fCameraOfsY = 1.15f;
            else
                m_fCameraOfsY = 1.0f;
        }
        break;

    default:
        break;
    };
};