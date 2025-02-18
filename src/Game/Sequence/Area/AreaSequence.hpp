#pragma once

#include "Game/System/2d/Anim2DBaseSequence.hpp"


class CDialog;
class CAreaWorkPool;


class CAreaSequence final : public CAnim2DSequence
{
public:
    static CProcess* Instance(void);

    CAreaSequence(void);
    virtual ~CAreaSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    bool AreaSelectLoad(void);
    
private:
    CAreaWorkPool*  m_pWorkPool;
    CDialog*        m_pDlgSure;
    bool            m_bDlgRunning;
#ifdef _DEBUG
    bool            m_bDebugCall;
#endif /* _DEBUG */
};