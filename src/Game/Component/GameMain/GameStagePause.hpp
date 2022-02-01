#pragma once


class CMessageWindow;


class IGameStagePause
{
public:
    virtual void Start(void* param) = 0;
    virtual void Stop(void) = 0;
    virtual bool Update(void) = 0;
    virtual void Draw(void) = 0;
};


class CDummyStagePause final : public IGameStagePause
{
public:
    static CDummyStagePause* Instance(void);
    
    virtual void Start(void* param) override;
    virtual void Stop(void) override;
    virtual bool Update(void) override;
    virtual void Draw(void) override;
};


class CMenuStagePause final : public IGameStagePause
{
public:
    static CMenuStagePause* Instance(void);

    virtual void Start(void* param) override;
    virtual void Stop(void) override;
    virtual bool Update(void) override;
    virtual void Draw(void) override;
};


class CTutorialStagePause final : public IGameStagePause
{
private:
    struct TUTORIALINFO
    {
        int32 m_nTextID;
        int32 m_nSeGroup;
    };
    
public:
    static CTutorialStagePause* Instance(void);

    virtual void Start(void* param) override;
    virtual void Stop(void) override;
    virtual bool Update(void) override;
    virtual void Draw(void) override;
    
private:
    static const TUTORIALINFO m_aTutorialMessageInfo[];
    CMessageWindow* m_pMsgWnd;
    float m_fTimer;
	bool m_bComplete;
};