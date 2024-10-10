#pragma once

#include "Game/Component/GameMain/DatabaseItemID.hpp"


class CDatabaseRecord
{
public:
    enum STATE : uint8
    {
        STATE_NONE = 0,
        STATE_UNREAD,
        STATE_READ,
    };

    enum DBSTATE
    {
        DBSTATE_NONE = 0,
        DBSTATE_OPEN,
        DBSTATE_NORMAL,
    };

    enum GROUP
    {
        GROUP_CHARACTER = 0,
        GROUP_ENEMY,
        GROUP_ART,
        GROUP_ETC,

        GROUP_NUM,
    };

    struct RAWDATA
    {
        DBSTATE m_dbstate;
        STATE m_aState[DBITEMID::ID_MAX];
    };

public:
    CDatabaseRecord(void);
    ~CDatabaseRecord(void);
    void SetDefault(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void SetDatabaseState(DBSTATE dbstate);
    DBSTATE GetDatabaseState(void) const;
    bool IsNewItemExisted(void) const;
    int32 CountUnlockedItemInGroup(GROUP group) const;
    bool IsItemReadInGroup(GROUP group) const;
    void SetItemUnlocked(DBITEMID::VALUE idItem);
    void SetItemRead(DBITEMID::VALUE idItem);
    bool IsItemUnlocked(DBITEMID::VALUE idItem) const;
    bool IsItemRead(DBITEMID::VALUE idItem) const;

private:
    STATE getItemState(DBITEMID::VALUE idItem) const;
    void setItemState(DBITEMID::VALUE idItem, STATE state);

private:
    DBSTATE m_dbstate;
    STATE m_aState[DBITEMID::ID_MAX];
};