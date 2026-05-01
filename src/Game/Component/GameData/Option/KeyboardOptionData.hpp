#pragma once

#include "OptionTypes.hpp"


class IKeyboardOptionData
{
public:
    struct RAWDATA
    {
        int32 m_aiKeyAssign[OPTIONTYPES::KEYFUNCMAX];
    };

public:
    IKeyboardOptionData(void) {};
    virtual ~IKeyboardOptionData(void) {};
    virtual void Initialize(void) = 0;
    virtual void Terminate(void) = 0;
    virtual void SetDefault(void) = 0;
    virtual void Apply(void) = 0;
    virtual bool IsValid(void) const = 0;
    virtual void Snapshot(RAWDATA& rRawData) const = 0;
    virtual void Restore(const RAWDATA& rRawData) = 0;
    virtual void AssignKey(OPTIONTYPES::KEYFUNC func, int32 iDIKey) = 0;
    virtual bool AssignDownKey(OPTIONTYPES::KEYFUNC func) = 0;
    virtual int32 GetAssignedKey(OPTIONTYPES::KEYFUNC func) const = 0;
    virtual const char* GetKeyName(int32 iDIKey) const = 0;
    virtual void AssignButton(void) = 0;
};