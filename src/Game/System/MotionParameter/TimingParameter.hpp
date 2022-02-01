#pragma once


class CTimingParameter : public CListNode<CTimingParameter>
{
public:
    enum TIMING_KIND
    {
        TIMING_ATOMIC_ON = 0,
        TIMING_ATOMIC_OFF,
        TIMING_LOCUS_ON,
        TIMING_LOCUS_OFF,
        TIMING_ATTACK,
        TIMING_ATTACK_2,
        TIMING_INVINCIBLE_ON,
        TIMING_INVINCIBLE_OFF,
        TIMING_KIND_MAX,
    };
    
    struct INIT_PARAMETER
    {
        TIMING_KIND m_kind;
        float m_fTime;
        int32 m_nParam;
    };

public:
    CTimingParameter(INIT_PARAMETER* pParam);
    virtual ~CTimingParameter(void);
    void Initialize(INIT_PARAMETER* pParam);
    TIMING_KIND GetKind(void) const;
    float GetTime(void) const;
    int32 GetParam(void) const;
    bool IsLocus(void) const;
    bool IsAtomic(void) const;
    bool IsAttack(void) const;
    bool IsInvincible(void) const;

private:
    TIMING_KIND m_kind;
    float m_fTime;
    int32 m_nParam;
};