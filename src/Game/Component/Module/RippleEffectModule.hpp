#pragma once

#include "Module.hpp"


class CCharacter;


class CRippleEffectModule : public IModule
{
private:
    struct RIPPLE_EFFECT_INFO
    {
        uint32 m_hEffect;
        bool m_bEnable;
    };
    
public:
    CRippleEffectModule(CCharacter* pCharacter, float fRadius);
    virtual ~CRippleEffectModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void SetEnable(bool bEnable);
    void InitializeEffect(float fRadius);
    void StartEffect(RIPPLE_EFFECT_INFO* pInfo);
    void StopEffect(RIPPLE_EFFECT_INFO* pInfo);
    bool IsSubjectMoving(void) const;
    
private:
    CCharacter* m_pCharacter;
    RIPPLE_EFFECT_INFO m_aEffectInfo[2];
};