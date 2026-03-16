#ifndef LUASTEAM_EXTRA
#define LUASTEAM_EXTRA

#include "Common.hpp"

namespace luasteam {

// Adds extra functions
void add_Extra(lua_State *L);

void shutdown_Extra(lua_State *L);

} // namespace luasteam

#endif // LUASTEAM_EXTRA