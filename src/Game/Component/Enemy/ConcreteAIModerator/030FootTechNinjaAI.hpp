#pragma once

#include "BaseFootNinjaAI.hpp"


class C030FootTechNinjaAI : public CBaseFootNinjaAI
{
private:
    class CTransparent
    {
    public:
        CTransparent(CEnemyCharacter* pEnemyChr);
        virtual ~CTransparent(void);
        virtual void Run(void);
        virtual void SetState(bool bActive);
        void PermissionToTheStateChanging(bool bPermission);
        void SetModelDrawEnable(bool bEnable);

    private:
        CEnemyCharacter* m_pEnemyChr;
        bool m_bPermission;
        bool m_bActive;
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C030FootTechNinjaAI(CEnemyCharacter* pEnemyChr);
    virtual ~C030FootTechNinjaAI(void);
    virtual void Run(void) override;
    void SetInvisible(bool bState);
    void CancelInvisible(void);

private:
    CTransparent m_transparentControl;
    ENEMYTYPES::STATUS m_statusPrev;
};