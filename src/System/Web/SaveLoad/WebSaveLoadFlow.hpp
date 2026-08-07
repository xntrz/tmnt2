#pragma once

#include "System/Common/SaveLoad/SaveLoadFlow.hpp"


class CWebSaveLoadBaseFlow;


class CWebSaveLoadFlow : public CSaveLoadFlow
{
public:
    CWebSaveLoadFlow(MODE mode, CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData);
    virtual ~CWebSaveLoadFlow(void);
    virtual void Initialize(void) override;
    virtual void Terminate(void) override;
    virtual bool Run(void) override;

private:
    CWebSaveLoadBaseFlow* m_pFlow;
};