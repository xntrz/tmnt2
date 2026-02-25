#pragma once

#include "Sprite.hpp"

#include "crimwsfd.h"


#define MV_WIDTH        (1024)
#define MV_HEIGHT       (512)
#define MV_PIXEL_COMP   (4)
#define MV_BPS          ((MV_WIDTH * MV_HEIGHT) * MV_PIXEL_COMP)


class CMovie
{
public:
    CMovie(int32 iWidth, int32 iHeight, int32 iMaxBps, bool bUsePalMode);
    virtual ~CMovie(void);
    virtual void Update(void) = 0;
    virtual void Draw(void) = 0;
    virtual uint32 GetRwRasterLockFlag(void) const = 0;
    bool IsEnded(void);
    bool IsCreateSuccess(void);
    void PushRenderState(void);
    void PopRenderState(void);
    RwRaster* CreateRaster(int32 iWidth, int32 iHeight);
    MWPLY CreateHandle(char** ppWorkArea, int32 iWidth, int32 iHeight, int32 iMaxBps, bool bUsePalMode);
    void ReleaseMovieFrame(void);
    bool LoadMovieFrame(void);
    void StartAfs(int32 partitionId, int32 fileId);
    void StartFname(const char* pszFilename);
    const MwsfdFrmObj& FrameObj(void) const;

protected:
    MWPLY       m_mwply;
    char*       m_pWorkArea;
    MwsfdFrmObj m_frameObj;
    CSprite     m_sprite;
    CSprite*    m_pRenderSprite;
    float       m_fSpritePosX;
    float       m_fSpritePosY;
    float       m_fSpriteW;
    float       m_fSpriteH;
    int32       m_iMovieW;
    int32       m_iMovieH;
    bool        m_bIsCprmHandleOK;
};