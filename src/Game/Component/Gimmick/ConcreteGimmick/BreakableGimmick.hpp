#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"

#include "Game/Component/Effect/EffectID.hpp"


class CBodyHitData;
class CFallGimmickMove;


class CBreakableGimmick final : public CGimmick
{
public:
    enum SUBID
    {
        SUBID_FIREPLUG = 0,
        SUBID_DUMMY_1,
        SUBID_PC,
        SUBID_DUMMY_2,
        SUBID_SIGNAL,
        SUBID_STREETLAMP,
        SUBID_BILLBOARD,
        SUBID_NEON,
        
        SUBIDNUM,
    };

    enum STATE
    {
        STATE_NORMAL = 0,
        STATE_BREAK,
    };

    enum EFFECTTIMING
    {
        EFFECTTIMING_ATBREAK = 0,
        EFFECTTIMING_AFTERBREAK,

        EFFECTTIMINGNUM,
    };

    enum EFFECTSTATE
    {
        EFFECTSTATE_WAIT = 0,
        EFFECTSTATE_RUN,
        EFFECTSTATE_END,
        
        EFFECTSTATENUM,
    };

    enum DETAILKIND
    {
        DETAILKIND_FIREPLUG_NY = 0,
        DETAILKIND_FIREPLUG_SP,
        DETAILKIND_PC_NY,
        DETAILKIND_PC_SP,
        DETAILKIND_SIGNAL_NY,
        DETAILKIND_STREETLAMP_NY,
        DETAILKIND_STREETLAMP_SP,
        DETAILKIND_BILLBOARD_NY,
        DETAILKIND_BILLBOARD_SP,
        DETAILKIND_NEON_NY,
        DETAILKIND_NEON_SP,
        
        DETAILKINDNUM,
        
        DETAILKIND_UNKNOWN = -1,
    };

    struct INITPARAM
    {
        const char* m_pszBaseLabel;
        int32 m_bExistHitModel;
        int32 m_nLife;
        int32 m_nHitSphere;
        float m_fHitRadius;
    };

    struct DETAILPARAM
    {
        EFFECTID::VALUE m_effectId;
        float m_fInterval;
        float m_fScale;
        RwV3d m_vEffectPosition;
    };

    class CBreakableGimmickEffect
    {
    public:
        CBreakableGimmickEffect(void);
        void Init(CModel* pModel, SUBID subid, EFFECTTIMING timing);
        void Start(void);
        void Run(void);

    private:
        DETAILKIND getDetailKind(SUBID subid);
        void play(void);
        void atBreakEffectPlay(void);
        void afterBreakEffectPlay(void);
        void defaultOneShotEffectRun(void);
        void defaultLoopEffectRun(void);
        void sprayWaterEffectRun(void);

    private:
        float m_fTimer;
        float m_fInterval;
        float m_fIntervalTimer;
        float m_fScale;
        EFFECTID::VALUE m_effectId;
        bool m_bStart;
        DETAILKIND m_detailKind;
        EFFECTTIMING m_effectTiming;
        EFFECTSTATE m_effectState;
        CModel* m_pParentModel;
        RwV3d m_vEffectDir;
        RwV3d m_vEffectPos;
        uint32 m_hEffect;
    };

public:
    CBreakableGimmick(const char* pszName, void* pParam);
    virtual ~CBreakableGimmick(void);
    virtual void Run(void) override;
    virtual void GetPosition(RwV3d* pvPosition) const override;
    virtual void PostMove(void) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;

private:
    void allClear(void);
    void initSubid(void* pParam);
    void initModel(void* pParam);
    void initUVAnim(void);
    void initEffect(void);
    void registHitAtack(void);
    void breakSdCall(void);
    void setAttackerDirection(const CGameObject* pObj);
    void setFlyAwayParam(const CGameObject* pObj);
    void firePlugRotateAtBreak(const CGameObject* pObj);
    void billBoardSetFlyAwayParam(const CGameObject* pObj);
    bool billBoardEraseSlowly(void);
    bool eraseSlowly(void);
    SUBID convertSubid(int32 subid) const;
    
private:
    static bool m_bUvAnimOwnerExist;
    
    CNormalGimmickModel* m_pModel;
    CFallGimmickMove* m_pMove;
    CBreakableGimmickEffect m_aEffect[EFFECTTIMINGNUM];
    STATE m_state;
    SUBID m_subid;
    int32 m_nLife;
    int32 m_nHitSphere;
    float m_fHitRadius;
    CBodyHitData* m_pBodyHit;
    uint32 m_hAtari;
    void* m_pAnimatedMaterials;
    bool m_bUvAnimOwner;
};