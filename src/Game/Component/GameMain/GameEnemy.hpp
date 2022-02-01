#pragma once


class CEnemy;


class CGameEnemy
{
public:
    static CGameEnemy& Dummy(void);
    
    CGameEnemy(void);
    ~CGameEnemy(void);
    void Attach(CEnemy* pEnemy);
    void Detach(void);
    void InvokeDeathFloor(void);
    void Activate(void);
    void Inactivate(void);
    bool IsActive(void) const;
    CEnemy* GetEnemy(void);
    void GetPosition(RwV3d* pvPosition);
    bool IsAlive(void) const;
    CEnemy& Enemy(void) const;

private:
    uint32 m_hObj;
    CEnemy* m_pEnemy;
    RwV3d m_vPosition;
    bool m_bActivate;
};