#include "BaseChr6045.hpp"


//
// *********************************************************************************
//

CBaseChr6045::CBaseChr6045(ENEMYID::VALUE enemyId)
: CEnemyCharacter(enemyId)
, m_pThrowObserver(nullptr)
, m_knockBackCtrl()
, m_bFlag(false)
, m_pAIModerator(nullptr)
{
    m_knockBackCtrl.Initialize(this, -1, -1.0f);
    m_knockBackCtrl.SetKnockBackControlEnable(false);
};


/*virtual*/ CBaseChr6045::~CBaseChr6045(void)
{
    ;
};


/*virtual*/ bool CBaseChr6045::Initialize(PARAMETER* pParameter, bool bReplaceParameter) /*override*/
{
    bool bResult = CEnemyCharacter::Initialize(pParameter, bReplaceParameter);
    m_pAIModerator = &AIModerator();
	
	return bResult;
};


/*virtual*/ void CBaseChr6045::Run(void) /*override*/
{
    CEnemyCharacter::Run();
    m_knockBackCtrl.Period();
};


/*virtual*/ void CBaseChr6045::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CEnemyCharacter::OnMessageAttackResult(pCatch);

    if (m_pThrowObserver)
        m_pThrowObserver->SetAttackParam(pCatch);
};


/*virtual*/ void CBaseChr6045::OnMessageReceivedDamage(int32 iAmount) /*override*/
{
    CEnemyCharacter::OnMessageReceivedDamage(iAmount);
    m_knockBackCtrl.ReceivedDamage(iAmount);
};