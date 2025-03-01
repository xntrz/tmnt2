#pragma once

#include "EnemyID.hpp"

#include "Game/System/GameObject/GameObject.hpp"


class CEnemyCharacter;


class CEnemyContainer final : public CGameObject
{
public:
    class CServeEnemy
    {
    public:
        CServeEnemy(void);
        ~CServeEnemy(void);
        void Set(CEnemyCharacter* pEnemyChr);
        void Clean(void);
        CEnemyCharacter* GetCharacter(void) const;
        void GetPosition(RwV3d* pvPosition) const;
        bool IsBusy(void) const;
        bool IsValid(void) const;
        ENEMYID::VALUE GetID(void) const;
        uint32 GetOwner(void) const;

    private:
        uint32          m_hEnemy; // handle to 'CEnemy' game object of type 'ENEMY' (!! not CEnemyCharacter !!)
        ENEMYID::VALUE  m_eID;
        bool            m_bBusy;
    };
    
public:
    static CEnemyContainer& Instance(void);

    CEnemyContainer(void);
    virtual ~CEnemyContainer(void);
    virtual void Run(void) override;
    CServeEnemy* FindFirst(void);
    CServeEnemy* FindNext(void);
    void Attach(CEnemyCharacter* pEnemyChr);
    void SetEnemyIDFilter(ENEMYID::VALUE eID);

private:
    static CEnemyContainer* m_pInstance;
    ENEMYID::VALUE          m_eIDFilter;
    CServeEnemy             m_aServeEnemy[24];
    int32                   m_iEnemyFind;
};


class CEnemyWatcher final
{
public:
    CEnemyWatcher(void);
    ~CEnemyWatcher(void);
    void Initialize(CEnemyCharacter* pEnemyChr, bool bEnable);
    bool IsFriendAttack(void);
    bool IsFriendStayFront(float fDistance);
    int32 GetFreePlayerNo(int32 iPlayerPurposeMax = -1);
    CEnemyCharacter* Search(uint32 hEnemy);
    int32 GetPurposePlayerNo(uint32 hEnemy);
    void SetEnemyIDFilter(void);

private:
    CEnemyCharacter* m_pEnemyChr;
    bool m_bEnable;
};