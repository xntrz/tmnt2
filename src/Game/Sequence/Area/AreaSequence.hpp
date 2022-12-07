#pragma once

#include "Game/System/2d/Anim2DBaseSequence.hpp"


class CDialog;
class CAreaWorkPool;


class CAreaSequence final : public CAnim2DSequence
{
public:
    static bool m_bDebugClearAnim;
    static CProcess* Instance(void);

    CAreaSequence(void);
    virtual ~CAreaSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* param) override;
    virtual void OnDraw(void) const override;
    virtual bool OnRet(void) override;
    bool AreaSelectLoad(void);
    
private:
    CAreaWorkPool* m_pWorkPool;
    CDialog* m_pDlgSure;
    bool m_bDlgRunning;
};