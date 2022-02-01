#pragma once

#include "Sprite.hpp"


class CMovie
{
public:
    CMovie(int32 id, int32 iWidth, int32 iHeight, int32 bps);
    virtual ~CMovie(void);
    virtual void Start(void) = 0;
    virtual void Stop(void) = 0;
    virtual void Update(void) = 0;
    virtual void Draw(void) = 0;
    virtual bool IsEnded(void) = 0;
    virtual void PushRenderState(void);
    virtual void PopRenderState(void);

protected:
    int32 m_iWidth;
    int32 m_iHeight;
    CSprite m_frame;    
};