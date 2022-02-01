#pragma once

#include "System/Common/SaveLoad/SaveLoadFlow.hpp"


class CPCSaveLoadBaseFlow;


class CPCSaveLoadFlow : public CSaveLoadFlow
{
public:
    CPCSaveLoadFlow(MODE mode, CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData);
    virtual ~CPCSaveLoadFlow(void);
    virtual void Initialize(void) override;
    virtual void Terminate(void) override;
    virtual bool Run(void) override;

private:
    CPCSaveLoadBaseFlow* m_pFlow;
};