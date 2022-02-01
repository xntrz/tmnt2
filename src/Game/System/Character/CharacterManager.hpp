#pragma once


class CCharacter;


class CCharacterManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Regist(CCharacter* pCharacter);
    static void Remove(CCharacter* pCharacter);
    static void Draw(void);
};