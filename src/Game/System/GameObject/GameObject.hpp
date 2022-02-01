#pragma once

#include "GameObjectType.hpp"
#include "GameObjectTypes.hpp"


class CGameObject
{
public:    
    CGameObject(const char* pszName, GAMEOBJECTTYPE::VALUE type, const char* pszParentName = nullptr);
    virtual ~CGameObject(void);
    virtual void Run(void);
    virtual void MessageProc(int32 nMessageID, void* pParam);
    void Period(void);
    void SetObjectFlag(GAMEOBJECTTYPES::FLAG flag, bool bSet);
    bool IsObjectFlagSet(GAMEOBJECTTYPES::FLAG flag) const;
    const char* GetName(void) const;
    GAMEOBJECTTYPE::VALUE GetType(void) const;
    GAMEOBJECTTYPES::FLAG GetObjectFlag(void) const;
    uint32 GetHandle(void) const;    

private:
    static CGameObject* m_pRunningObject;
    char m_szObjName[GAMEOBJECTTYPES::NAME_MAX];
    GAMEOBJECTTYPE::VALUE m_objtype;
    uint32 m_objflag;
    uint32 m_hObject;
};