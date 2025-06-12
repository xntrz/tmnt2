#include "PlayerTracer.hpp"
#include "PlayerCharacter.hpp"

#include "Game/System/Model/Model.hpp"


CPlayerTracer::CPlayerTracer(CPlayerCharacter* pPlayerChr)
: m_pPlayerChr(pPlayerChr)
{
    ;
};


/*virtual*/ CPlayerTracer::~CPlayerTracer(void)
{
    ;
};


/*virtual*/ float CPlayerTracer::GetDirection(void) /*override*/
{   
    return m_pPlayerChr->GetDirection();
};


/*virtual*/ void CPlayerTracer::GetPosition(RwV3d* pvPosition) /*override*/
{
    m_pPlayerChr->GetFootPosition(pvPosition);
};


//
// *********************************************************************************
//


CPlayerTargetTracer::CPlayerTargetTracer(const CPlayerCharacter* pPlayerChr)
: m_pPlayerChr(pPlayerChr)
{
    ;
};


/*virtual*/ CPlayerTargetTracer::~CPlayerTargetTracer(void)
{
    ;
};


/*virtual*/ float CPlayerTargetTracer::GetDirection(void) /*override*/
{
    return m_pPlayerChr->GetDirection();
};


/*virtual*/ void CPlayerTargetTracer::GetPosition(RwV3d* pvPosition) /*override*/
{
    *pvPosition = *m_pPlayerChr->GetModel()->GetBonePositionFromID(10); 
};