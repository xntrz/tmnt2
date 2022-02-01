#pragma once


class CGraphicsDevice
{
public:
    static const int32 FLIPINTERVAL_DEFAULT = 0;
    static const int32 FLIPINTERVAL_60 = 1;
    static const int32 FLIPINTERVAL_30 = 2;
    
    static const RwRGBA DEFAULT_CLEAR_COLOR;
    
public:
    CGraphicsDevice(void);
    virtual ~CGraphicsDevice(void);
    virtual bool Initialize(void);
    virtual void Terminate(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual bool RenderBegin(void);
    virtual void RenderEnd(void);
    virtual void Flip(void);
    virtual void SetFlipInterval(int32 iFlipInterval);
    virtual int32 ScreenWidth(void) = 0;
    virtual int32 ScreenHeight(void) = 0;
    virtual int32 ScreenDepth(void) = 0;
    virtual void* Configure(void) = 0;
    virtual int32 Subsystem(void);
    virtual int32 Videomode(void) = 0;
    virtual bool IsExclusive(void) const = 0;
    virtual bool AttachPlugin(void);
    virtual bool CreateFrameBuffer(int32 iWidth = 0, int32 iHeight = 0, bool bFullscreen = true);
    virtual void DestroyFrameBuffer(void);
    virtual bool CreateCamera(void);
    virtual void DestroyCamera(void);
    void CurrentVideomode(int32& iWidth, int32& iHeigh, int32& iDepth);
    void SetFlipEnable(bool bEnable);
    void SetClearColor(const RwRGBA* pColor);
    void ResetClearColor(void);
    int32 GetFlipInternval(void) const;

protected:
    RwRaster* m_pFrameBuffer;
    RwRaster* m_pZBuffer;
    RwCamera* m_pCamera;
    RwRGBA m_ClearColor;
    bool m_bFlipEnable;
    int32 m_iFlipInterval;
};