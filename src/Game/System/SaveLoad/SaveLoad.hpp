#pragma once


class CSaveLoad
{
public:
    enum TYPE
    {
        TYPE_CHECK,
        TYPE_LOAD,
        TYPE_SAVE,
        TYPE_AUTOSAVE,

        TYPENUM,
    };
    
public:
    static void Initialize(TYPE type);
    static void Terminate(void);
    static bool Run(void);
    static bool IsNewSave(void);
};
