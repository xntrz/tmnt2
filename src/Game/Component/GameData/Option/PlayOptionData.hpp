#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"


class CPlayOptionData
{
public:
    struct RAWDATA
    {
        bool m_bAutosaveFlag;
        GAMETYPES::DIFFICULTY m_difficulty;
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
    GAMETYPES::DIFFICULTY GetDifficulty(void) const;
    void SetEnableAutosave(bool bSet);
    bool IsAutosaveEnabled(void) const;
    bool IsSimplifiedInput(void) const;

private:
    GAMETYPES::DIFFICULTY m_difficulty;
    bool m_bAutosaveFlag;
};