#pragma once

class CWebMsgBox
{
public:
    enum TYPE
    {
        TYPE_OK = 0,
        TYPE_OK_CANCEL,
    };

    enum ANSWER
    {
        ANSWER_OK = 0,
        ANSWER_CANCEL,
    };

public:
    static ANSWER Invoke(TYPE type, const char* pszTitle, const char* pszText);
};