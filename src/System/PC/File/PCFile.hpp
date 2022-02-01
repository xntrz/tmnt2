#pragma once

#include "System/Common/File/File.hpp"


class CPCFile : public CFile
{
public:
    CPCFile(int32 nLabel);
    virtual ~CPCFile(void);
};