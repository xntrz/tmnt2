#include "084Karai.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/084KaraiAI.hpp"
#include "Game/Component/Module/BandanaModule.hpp"


static const RwV3d BANDANA_OFFSET = { 0.0f, 0.0915f, -0.03f };


C084Karai::C084Karai(void)
: CBaseKaraiChr(ENEMYID::ID_KARAI)
{
    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C084KaraiAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.5f;

    parameter.m_feature.m_fWalkMoveSpeed        = 3.5f;
    parameter.m_feature.m_fRunMoveSpeed         = 8.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 10.4f;
    parameter.m_feature.m_fDashMoveTime         = 0.7f;
    parameter.m_feature.m_fRotateRate           = 5.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 9.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 3.0f;
    parameter.m_feature.m_fAerialAcceleration   = 0.2f;
    parameter.m_feature.m_iHPMax                = 400;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 1.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 20.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 3.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /**
         *  _B - base mode
         *  _W - warning mode (red stars aura)
         *  _F - final mode (red circle aura)
         */

        /* FREQUENCY_ATTACK_A                 0     */  { 30, 30,  30  },
        /* FREQUENCY_ATTACK_B                 1     */  { 20, 20,  20  },
        /* FREQUENCY_ATTACK_C                 2     */  { 30, 30,  30  },
        /* FREQUENCY_ATTACK_D                 3     */  { 30, 30,  30  },
        /* FREQUENCY_ATTACK_E                 4     */  { 30, 30,  30  },
        /* FREQUENCY_ATTACK_F                 5     */  {  5,  5,   5  },
        /* FREQUENCY_ATTACK_SPECIAL           6     */  { 10, 10,  10  },
        /* FREQUENCY_ATTACK_A_UNUSUAL         7     */  { 20, 20,  20  },
        /* FREQUENCY_ATTACK_B_UNUSUAL         8     */  { 20, 20,  20  },
        /* FREQUENCY_ATTACK_SPECIAL_UNUSUAL   9     */  { 80, 80,  90  },
        /* FREQUENCY_ATTACK_AA                10    */  { 30, 30,  40  },
        /* FREQUENCY_ATTACK_AAA               11    */  { 20, 20,  30  },
        /* FREQUENCY_ATTACK_BB                12    */  { 20, 20,  30  },
        /* FREQUENCY_ATTACK_AA_W              13    */  { 60, 80, 100  },
        /* FREQUENCY_ATTACK_AAA_W             14    */  { 30, 50,  80  },
        /* FREQUENCY_ATTACK_BB_W              15    */  { 40, 60, 100  },
        /* FREQUENCY_ATTACK_SPECIAL_W         16    */  { 20, 20,  20  },
        /* FREQUENCY_ATTACK_SPECIAL_F         17    */  { 70, 70,  70  },
        /* FREQUENCY_GUARD_B                  18    */  {  5, 10,  20  },
        /* FREQUENCY_GUARD_W                  19    */  {  5, 10,  50  },
        /* FREQUENCY_GUARD_F                  20    */  { 10, 15,  60  },
        /* FREQUENCY_MOVE_ESCAPE              21    */  { 30, 30,  30  },        
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

    bool bReplace = false;
    Initialize(&parameter, bReplace);

    /* init modules */
    CCharacterCompositor* pChrCompositor = &Compositor();

    CBandanaModule* pBandanaModule = new CBandanaModule(pChrCompositor,
                                                        pChrCompositor->GetModel(),
                                                        10,
                                                        &BANDANA_OFFSET,
                                                        CBandanaModule::BANDANACOLOR_DARKRED);

    pChrCompositor->IncludeModule(pBandanaModule);
};


/*virtual*/ C084Karai::~C084Karai(void)
{
    ;
};
