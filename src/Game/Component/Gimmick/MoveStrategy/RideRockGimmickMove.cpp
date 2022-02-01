#include "RideRockGimmickMove.hpp"


/*static*/ int32 CRideRockGimmickMove::m_iRandomSeed = 0;


CRideRockGimmickMove::CRideRockGimmickMove(void)
: m_vRotDir(Math::VECTOR3_ZERO)
{
    static RwV3d s_avRotDir[16] =
    {
        { 1.0,  0.0,    1.0,    },
        { 1.0,  2.0,    0.0,    },
        { 1.0,  0.5,    0.0,    },
        { 1.0,  1.0,    0.0,    },
        { 0.0,  1.0,    1.0,    },
        { 0.0,  1.0,    -0.5,   },
        { 1.0,  0.0,    -2.0,   },
        { -1.0, 0.0,    0.0,    },
        { 1.0,  -1.0,   0.0,    },
        { 1.0,  0.0,    1.0,    },
        { 1.0,  0.0,    2.0,    },
        { 0.0,  -1.0,   0.5,    },
        { 1.0,  1.0,    0.0,    },
        { 0.0,  1.0,    -1.0,   },
        { 1.0,  0.0,    0.5,    },
        { 1.0,  1.0,    0.0,    },
    };

    m_vRotDir = s_avRotDir[m_iRandomSeed++];

    if (m_iRandomSeed >= COUNT_OF(s_avRotDir))
        m_iRandomSeed = 0;
};


CRideRockGimmickMove::~CRideRockGimmickMove(void)
{
    ;
};


CRideRockGimmickMove::RESULT CRideRockGimmickMove::OnMove(float dt)
{
    float fSpeed = (dt * 1.2566371f);
    
    m_vRotation.x += (fSpeed * m_vRotDir.x);
    m_vRotation.y += (fSpeed * m_vRotDir.y);
    m_vRotation.z += (fSpeed * m_vRotDir.z);

    return RESULT_NONE;
};