#pragma once

#include "OptionTypes.hpp"


class CKeyboardOptionData
{
public:
    struct RAWDATA
    {
        int32 m_aiKeyAssign[OPTIONTYPES::KEYFUNCMAX];
    };

public:
    CKeyboardOptionData(void);
    ~CKeyboardOptionData(void);
    void Initialize(void);
    void Terminate(void);
    void SetDefault(void);
    void Apply(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void AssignKey(OPTIONTYPES::KEYFUNC func, int32 iDIKey);
    bool AssignDownKey(OPTIONTYPES::KEYFUNC func);
    int32 GetAssignedKey(OPTIONTYPES::KEYFUNC func) const;
    const char* GetKeyName(int32 iDIKey) const;
    void InitializeGamepadMapping(void);
    
private:
    void assignDefaultKey(void);
    void assignDefaultButton(void);
    OPTIONTYPES::KEYFUNC findFunction(int32 key) const;

private:
    int32 m_aiKeyAssign[OPTIONTYPES::KEYFUNCMAX];
    uint32 m_auButtonAssign[OPTIONTYPES::BTNFUNCMAX];
};