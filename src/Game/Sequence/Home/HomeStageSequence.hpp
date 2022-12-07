#pragma once

#include "HomeTypes.hpp"

#include "Game/Sequence/Stage/StageBaseSequence.hpp"


class CHomeCamera;


class CHomeStageSequence : public CStageBaseSequence
{
public:
    static HOMETYPES::DOORKIND m_eDoorKind;
    static CProcess* Instance(void);

    CHomeStageSequence(void);
    virtual ~CHomeStageSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* param) override;
    virtual void OnDraw(void) const override;
    virtual void OnStateDetached(STATE state) override;
    CHomeCamera* GetCamera(void) const;
    
private:
    CHomeCamera* m_pHomeCamera;
};