#pragma once

#include "ProcessTypes.hpp"


class CProcessMail
{
public:
    class CMailBox
    {
    private:
        static const int32 CAPACITY = 16;

    public:
        CMailBox(void);
        virtual ~CMailBox(void);
        void Clear(int32 iLabel);
        void Clear(void);
        bool Accept(PROCESSTYPES::MAIL& mail);
        bool Accept(int32 iReceiver, PROCESSTYPES::MAIL& mail);
        bool Post(PROCESSTYPES::MAIL& mail);

    private:
        PROCESSTYPES::MAIL m_aMailes[CAPACITY];
        int32 m_size;
    };

public:
    CProcessMail(void);
    virtual ~CProcessMail(void);
    bool Send(int32 iReceiver, PROCESSTYPES::MAIL::TYPE type = PROCESSTYPES::MAIL::TYPE_NONE, const void* param = nullptr);
    bool Send(PROCESSTYPES::MAIL& mail);
    bool Recv(PROCESSTYPES::MAIL& mail);
    void SetCurrent(int32 iReceiver);
    CMailBox& MailPost(void);
    CMailBox& MailCarrier(void);
    int32 CurrentReceiver(void) const;

private:
    CMailBox* m_pMailPost;
    CMailBox* m_pMailCarrier;
    int32 m_iCurrentReceiver;
};