#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"


class CPlayOptionData
{
public:
    struct RAWDATA
    {
        GAMETYPES::DIFFICULTY m_difficulty;
        bool m_bAutosaveFlag;
        bool m_bHelpFlag;
        bool m_bClassicInput;
    };

public:
    CPlayOptionData(void);
    ~CPlayOptionData(void);
    void Initialize(void);
    void Terminate(void);
    void SetDefault(void);
    void Apply(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void SetDifficulty(GAMETYPES::DIFFICULTY difficulty);
    void SetEnableAutosave(bool bSet);
    void SetEnableHelp(bool bSet);
    void SetClassicInput(bool bSet);
    GAMETYPES::DIFFICULTY GetDifficulty(void) const;
    bool IsAutosaveEnabled(void) const;
    bool IsHelpEnabled(void) const;
    bool IsClassicInput(void) const;

private:
    GAMETYPES::DIFFICULTY m_difficulty;
    bool m_bAutosaveFlag;
    bool m_bHelpFlag;
    bool m_bClassicInput;
};