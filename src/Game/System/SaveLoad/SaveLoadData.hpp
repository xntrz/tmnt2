#pragma once

#include "System/Common/SaveLoad/SaveLoadData.hpp"


class CSaveLoadData : public CSaveLoadDataBase
{
public:
    CSaveLoadData(void);
    virtual ~CSaveLoadData(void);
    virtual void Initialize(const void* pData, uint32 uDataSize) override;
    virtual void Initialize(void) override;
    virtual void Terminate(void) override;
    virtual void Update(void) override;
    virtual bool IsValid(void) const override;
    virtual void SetAutosaveFlag(bool bFlag) const;    
};