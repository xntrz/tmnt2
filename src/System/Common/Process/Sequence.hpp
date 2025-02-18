#pragma once

#include "Process.hpp"


class CSequence : public CProcess
{
private:
    class CPrivate;

public:
    static int32 GetCurrently(void);
    
    CSequence(void);
    virtual ~CSequence(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;
    virtual bool Call(int32 iLabel, const void* pParam = nullptr, bool bDrawEnable = false);
    virtual bool Jump(int32 iLabel, const void* pParam = nullptr);
    virtual bool Kill(int32 iBackToLabel, const void* pReturnValue = nullptr);
    virtual bool Ret(const void* pReturnValue = nullptr);
    virtual bool OnAttach(const void* pParam) = 0;
    virtual void OnDetach(void) = 0;
    virtual void OnMove(bool bRet, const void* pReturnValue) = 0;
    virtual void OnDraw(void) const = 0;
    int32 Child(void) const;
    int32 Parent(void) const;
    void DisposeMessage(void);

private:
    inline CPrivate& Private(void) const { return *m_pPrivate; };

private:
    static int32 m_iLabelCurrently;
    CPrivate* m_pPrivate;
};