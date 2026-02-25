#pragma once

#ifdef TMNT2_IDFSYST

using FNAME = int32;

#define FNAME_INVALID \
    (-1)

#define FNAME_EQUAL(a, b) \
    ((a) == (b))

#else /* TMNT2_IDFSYST */

using FNAME = const char*;

#define FNAME_INVALID \
    ("")

#define FNAME_EQUAL(a, b) \
    (std::strcmp((a), (b)) == 0)

#endif /* TMNT2_IDFSYST */


//
//  File path wrapper to the "Common/" data container
//
#define FPATH(x) (x)


//
//  File path wrapper to the "Language/" data container
//
#define FPATH_LANG(x) (x)