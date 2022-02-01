#pragma once

#include "GameObjectType.hpp"


class CGameObject;


class CGameObjectManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void SendMessage(CGameObject* pObject, int32 nMessageID, void* pParam = nullptr);
    static uint32 RegistObject(CGameObject* pObject, GAMEOBJECTTYPE::VALUE type, const char* pszParent);
    static void RemoveObject(CGameObject* pObject);
    static void DeleteObject(CGameObject* pObject);
    static CGameObject* GetNext(CGameObject* pObject = nullptr);
    static CGameObject* GetObject(uint32 hObject);
    static CGameObject* GetObject(const char* pszName);
};