#pragma once

#ifdef TMNT2_IDFSYST

#define PRImvn "d"

using MVNAME = int32;

#define mvname_cast(x) \
    static_cast<MVNAME>(reinterpret_cast<int32>(x))

#define MVNAME_INVALID \
    (-1)

#define MVNAME_EQUAL(a, b) \
    ((a) == (b))

#else /* TMNT2_IDFSYST */

#define PRImvn "s"

using MVNAME = const char*;

#define mvname_cast(x) \
    static_cast<MVNAME>(x)

#define MVNAME_INVALID \
    ("")

#define MVNAME_EQUAL(a, b) \
    (std::strcmp((a), (b)) == 0)

#endif /* TMNT2_IDFSYST */


#define MVPATH(x) (x)