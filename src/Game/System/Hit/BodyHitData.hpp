#pragma once


class CBodyHitData : public CListNode<CBodyHitData>
{
public:
    static const uint32 INVALID_HIT_ID = 0;

    enum STATE : uint32
    {
        STATE_NONE          = 0x0,
        STATE_SELF_TO_OTHER = 0x1,
        STATE_TODO_0x2      = 0x2, // TODO naming
        STATE_ENABLE        = 0x4,

        STATE_DEFAULT       = STATE_SELF_TO_OTHER
                            | STATE_TODO_0x2
                            | STATE_ENABLE,
    };

    DEFINE_ENUM_FLAG_OPS(STATE, friend);

public:
    CBodyHitData(void);
    ~CBodyHitData(void);
    void InitData(const RwV3d* pPos, float fHitRadius);
    void SetHitID(uint32 uHitID);
    uint32 GetHitID(void) const;
    void SetState(STATE state, bool bEnable);
    bool IsEnableState(STATE state) const;
    void SetCurrentPos(const RwV3d* pPos);
    const RwV3d* GetCurrentPos(void) const;
    void SetHitRadius(float fHitRadius);
    float GetHitRadius(void) const;
    
private:   
    uint32 m_uHitID;
    uint32 m_uHitState;
    float  m_fHitRadius;
    RwV3d  m_vPos;
};