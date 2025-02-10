#pragma once

#include "MovieID.hpp"


#ifdef TMNT2_BUILD_EU

class CMovieText
{
public:
    struct UPDATEINFO
    {
        const void* pFrmObj;
    };

public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(void* pBuffer, uint32 uBufferSize);
    static bool LoadFor(MOVIEID::VALUE movieId);
    static bool IsLoadEnd(void);
    static void OnLoadEnd(void);
    static void Update(UPDATEINFO* pUpdateInfo);
    static void Draw(void);
};

#endif /* TMNT2_BUILD_EU */