#include "debughelper.h"

extern "C"{
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}

int main()
{
    lua_State* pState = luaL_newstate();
    luaL_openlibs(pState);

    CDebugHelper::inst().regDebug(pState);

    luaL_loadfile(pState, "debug_test.lua");
    lua_pcall(pState, 0, LUA_MULTRET, 0);
    
    CDebugHelper::inst().unRegDebug(pState);
    
    lua_close(pState);

    return 0;
}