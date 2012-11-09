extern "C"{
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}

#include "reghelper.h"


int test0()
{
    return 0;
}

int test1(int a)
{
    return a;
}

int test2(int a, int b)
{
    return a - b;
}

const char* test3(const char* str)
{
    return str;
}

void test4()
{

}

int main()
{
    lua_State* pState = luaL_newstate();
    luaL_openlibs(pState);

    REGISTER_FUNCTION(pState, test0);
    REGISTER_FUNCTION(pState, test1);
    REGISTER_FUNCTION(pState, test2);
    REGISTER_FUNCTION(pState, test3);
    REGISTER_FUNCTION(pState, test4);

    luaL_loadfile(pState, "reg_test.lua");
    lua_pcall(pState, 0, LUA_MULTRET, 0);
    
    lua_close(pState);

    return 0;
}
