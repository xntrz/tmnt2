#pragma once

#include "OptionTypes.hpp"


class CGamepadOptionData
{
public:
    struct RAWDATA
    {
        uint32 m_auButtonAssign[OPTIONTYPES::BTNFUNCMAX];
        bool m_bVibration;
    };

public:
    CGamepadOptionData(void);
    ~CGamepadOptionData(void);
    void Initialize(int32 port);
    void Terminate(void);
    void SetDefault(void);
    void Apply(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void SetEnableVibration(bool bSet);
    void AssignButton(OPTIONTYPES::BTNFUNC func, uint32 uButton);
    bool IsEnabledVibration(void) const;
    uint32 GetAssignedButton(OPTIONTYPES::BTNFUNC func) const;

private:
    uint32 extractSingleButton(uint32 button) const;
    OPTIONTYPES::BTNFUNC findFunction(uint32 button) const;

private:
    int32 m_port;
    uint32 m_auButtonAssign[OPTIONTYPES::BTNFUNCMAX];
    bool m_bVibration;
};