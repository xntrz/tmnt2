#pragma once


class CHitAttackData;


class CAttackParameter : public CListNode<CAttackParameter>
{
public:
    struct INIT_PARAMETER
    {
        int32    m_no;
        int32    m_nBoneID;
        float    m_fStart;
        float    m_fEnd;
        RwSphere m_sphere;
    };

    struct ATTACH_PARAMETER
    {
        int32   m_nNo;
        int32   m_nPower;
        int32   m_nStatus;
        float   m_fStatusVal1;
        float   m_fStatusVal2;
        bool    m_bConfusion;
        bool    m_bGuardImpact;
        bool    m_bSlice;
        bool    m_bReflectShot;
        int32   m_nAntiguard;
        uint32  m_uTarget;
    };
    
public:
    CAttackParameter(INIT_PARAMETER* pParam);
    virtual ~CAttackParameter(void);
    void Initialize(INIT_PARAMETER* pParam);
    void Attach(ATTACH_PARAMETER* pParam);
    bool IsAttackEnableTime(float fNow) const;
    void GetParameter(CHitAttackData* pHitAttackData) const;
    const RwSphere* GetSphere(void) const;
    int32 GetBoneID(void) const;

private:
    int32    m_no;
    int32    m_nBoneID;
    RwSphere m_sphere;
    float    m_fStart;
    float    m_fEnd;
    int32    m_nPower;
    int32    m_nStatus;
    float    m_fStatusVal1;
    float    m_fStatusVal2;
    bool     m_bConfusion;
    bool     m_bGuardImpact;
    bool     m_bSlice;
    bool     m_bReflectShot;
    int32    m_nAntiguard;
    uint32   m_uTarget;
};