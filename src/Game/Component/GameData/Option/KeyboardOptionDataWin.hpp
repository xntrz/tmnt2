#pragma once

#include "KeyboardOptionData.hpp"


#if defined(TARGET_PC)

class CKeyboardOptionDataWin final : public IKeyboardOptionData
{
public:
    CKeyboardOptionDataWin(void);
    virtual ~CKeyboardOptionDataWin(void);
    virtual void Initialize(void) override;
    virtual void Terminate(void) override;
    virtual void SetDefault(void) override;
    virtual void Apply(void) override;
    virtual bool IsValid(void) const override;
    virtual void Snapshot(RAWDATA& rRawData) const override;
    virtual void Restore(const RAWDATA& rRawData) override;
    virtual void AssignKey(OPTIONTYPES::KEYFUNC func, int32 iDIKey) override;
    virtual bool AssignDownKey(OPTIONTYPES::KEYFUNC func) override;
    virtual int32 GetAssignedKey(OPTIONTYPES::KEYFUNC func) const override;
    virtual const char* GetKeyName(int32 iDIKey) const override;
    virtual void AssignButton(void) override;

private:
    void assignDefaultKey(void);
    void assignDefaultButton(void);
    OPTIONTYPES::KEYFUNC findFunction(int32 key) const;

private:
    int32 m_aiKeyAssign[OPTIONTYPES::KEYFUNCMAX];
    uint32 m_auButtonAssign[OPTIONTYPES::KEYFUNCMAX - 4];   // exclude up/down/left/right
};

#endif /* defined(TARGET_PC) */