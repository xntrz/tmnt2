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
    const char* GetName(void) const;
    GAMEOBJECTTYPE::VALUE GetType(void) const;
    uint32 GetObjectFlag(void) const;
    uint32 GetHandle(void) const;    
    void SetObjectFlag(GAMEOBJECTTYPES::FLAG flag);
    void ClearObjectFlag(GAMEOBJECTTYPES::FLAG flag);
    bool TestObjectFlag(GAMEOBJECTTYPES::FLAG flag) const;
    bool CheckObjectFlag(GAMEOBJECTTYPES::FLAG flag) const;

private:
    static CGameObject*   m_pRunningObject;
    char                  m_szObjName[GAMEOBJECTTYPES::NAME_MAX];
    GAMEOBJECTTYPE::VALUE m_objtype;
    GAMEOBJECTTYPES::FLAG m_oflag;
    uint32                m_hObject;
};