#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CMapModelCtrlGimmick : public CGimmick
{
public:
    CMapModelCtrlGimmick(const char* pszName, void* pParam);
    virtual ~CMapModelCtrlGimmick(void);
    virtual void Draw(void) const override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    CNormalGimmickModel m_model;
};