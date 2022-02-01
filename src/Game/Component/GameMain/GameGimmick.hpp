#pragma once


class CGimmick;


class CGameGimmick final
{
public:
    static CGameGimmick& Dummy(void);
        
    CGameGimmick(void);
    ~CGameGimmick(void);
    void Attach(CGimmick* pGimmick);
    void Detach(void);
    bool IsAlive(void) const;
    CGimmick* GetGimmick(void);
    void GetPosition(RwV3d* pvPosition);
    void Sleep(void);
    bool IsSleep(void) const;
    bool IsAbleToSleep(void) const;
    void Resume(void);
    CGimmick& Gimmick(void) const;

private:
    uint32 m_hObj;    
    CGimmick* m_pGimmick;
    RwV3d m_vPosition;
};