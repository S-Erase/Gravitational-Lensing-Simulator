#pragma once

#include <wx/wx.h>

#if _CONTAINER_DEBUG_LEVEL > 0
#define SE_ASSERT(cond, msg) _STL_VERIFY(cond, msg)
#endif