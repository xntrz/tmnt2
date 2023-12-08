#pragma once

#include "System/Common/Process/Sequence.hpp"


class CCamera;


class CEnbuSequence final : public CSequence
{
private:
    enum STEP
    {
        STEP_INIT = 0,
        STEP_RUN,
        STEP_END,
        STEP_EOL,
    };
    
public:
    static CProcess* Instance(void);

    CEnbuSequence(void);
    virtual ~CEnbuSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    void EnbuCameraProc(void);
    int32 GetEnbuCameraType(void) const;
    void SetEnbuCameraInit(void);

private:
    RpLight* m_pLight;
    RpWorld* m_pWorld;
    CCamera* m_pCamera;
    STEP m_step;
    float m_fCameraRotY;
    float m_fDistEye;
    float m_fEyeOfsY;
    float m_fCameraOfsY;
    uint32 m_auCnt[3];
    int32 m_uAnimStep;
};