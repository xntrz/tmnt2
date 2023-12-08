#pragma once

#ifdef _DEBUG

#include "System/Common/Process/Process.hpp"


class CDbgUnlockProcess final : public CProcess
{
public:
    enum : uint32
    {
        UNLOCKFLAG_AREA     = (1 << 0),
        UNLOCKFLAG_CHARA    = (1 << 1),
        UNLOCKFLAG_CRYSTAL  = (1 << 2),
        UNLOCKFLAG_TOURNEY  = (1 << 3),
        UNLOCKFLAG_APRIL    = (1 << 4),
        UNLOCKFLAG_DB       = (1 << 5),

        UNLOCKFLAG_ALL      = UNLOCKFLAG_AREA
                            | UNLOCKFLAG_CHARA
                            | UNLOCKFLAG_CRYSTAL
                            | UNLOCKFLAG_TOURNEY
                            | UNLOCKFLAG_APRIL
                            | UNLOCKFLAG_DB,
    };

public:
    static CProcess* Instance(void);
    static void Launch(CProcess* pCaller, uint32 unlockFlag);

    CDbgUnlockProcess(void) {};
    virtual ~CDbgUnlockProcess(void) {};
    virtual bool Attach(void) override;
    virtual void Detach(void) override {};
    virtual void Move(void) override;
    virtual void Draw(void) const override {};

private:
    uint32 m_unlockFlag;
};

#endif /* _DEBUG */