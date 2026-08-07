#pragma once

#include "GamepadOptionData.hpp"


class CTouchOptionData : public CGamepadOptionData
{
public:
    /*
     *   NOTE:  Trash value possible for touch RAWDATA if loading savedata of PC game on mobile game.
     *          On pure mobile device we have this data in initial state or user changed state by default.
     *          So currently this possible only while testing mobile version on desktop browser.
     *
     *          To reproduce problem play & save game on desktop version,
     *          then restart web page with 'mobile' opt and load same game.
     *
     *          So this case is currently not a bug (maybe, idk)
     */
    struct RAWDATA
    {
        CGamepadOptionData::RAWDATA m_gamepad;
        bool                        m_bLeftHanded;
        bool                        m_bFloating;
        bool                        m_bDynamic;
    };

public:
    CTouchOptionData(void);
    ~CTouchOptionData(void);
    void Initialize(int32 port);
    void SetDefault(void);
    void Apply(void);
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void SetLeftHanded(bool bSet);
    bool IsLeftHanded(void) const;
    void SetFloating(bool bSet);
    bool IsFloating(void) const;
    void SetDynamic(bool bSet);
    bool IsDynamic(void) const;

private:
    bool m_bLeftHanded;
    bool m_bFloating;
    bool m_bDynamic;
};