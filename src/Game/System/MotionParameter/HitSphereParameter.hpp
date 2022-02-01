#pragma once


class CHitSphereParameter : public CListNode<CHitSphereParameter>
{
public:
    struct INIT_PARAMETER
    {
        int32 m_nBoneID;
        RwSphere m_sphere;
    };
    
public:
    CHitSphereParameter(INIT_PARAMETER* pParam);
    virtual ~CHitSphereParameter(void);
    void Initialize(INIT_PARAMETER* pParam);
    int32 GetBoneID(void) const;
    const RwSphere* GetSphere(void) const;

private:
    int32 m_nBoneID;
    RwSphere m_sphere;
};