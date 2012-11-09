#ifndef register_h
#define register_h

#include <string.h>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

template<typename T>
struct TypeHelper{};

void getValue(TypeHelper<void>, lua_State* pState, int index)
{

}

bool getValue(TypeHelper<bool>, lua_State* pState, int index)
{
    return lua_toboolean(pState, index) == 1;
}

char getValue(TypeHelper<char>, lua_State* pState, int index)
{
    return static_cast<char>(lua_tonumber(pState, index));
}

short getValue(TypeHelper<short>, lua_State* pState, int index)
{
    return static_cast<short>(lua_tonumber(pState, index));
}

int getValue(TypeHelper<int>, lua_State* pState, int index)
{
    return static_cast<int>(lua_tonumber(pState, index));
}

long getValue(TypeHelper<long>, lua_State* pState, int index)
{
    return static_cast<long>(lua_tonumber(pState, index));
}

unsigned char getValue(TypeHelper<unsigned char>, lua_State* pState, int index)
{
    return static_cast<unsigned char>(lua_tonumber(pState, index));
}

unsigned short getValue(TypeHelper<unsigned short>, lua_State* pState, int index)
{
    return static_cast<unsigned short>(lua_tonumber(pState, index));
}

unsigned int getValue(TypeHelper<unsigned int>, lua_State* pState, int index)
{
    return static_cast<unsigned int>(lua_tonumber(pState, index));
}

unsigned long getValue(TypeHelper<unsigned long>, lua_State* pState, int index)
{
    return static_cast<unsigned long>(lua_tonumber(pState, index));
}

char* getValue(TypeHelper<char*>, lua_State* pState, int index)
{
    return const_cast<char*>(lua_tostring(pState, index));
}

const char* getValue(TypeHelper<const char*>, lua_State* pState, int index)
{
    return lua_tostring(pState, index);
}

float getValue(TypeHelper<float>, lua_State* pState, int index)
{
    return static_cast<float>(lua_tonumber(pState, index));
}

double getValue(TypeHelper<double>, lua_State* pState, int index)
{
    return static_cast<double>(lua_tonumber(pState, index));
}

void pushValue(lua_State* pState, bool value)
{
    lua_pushboolean(pState, int(value));
}

void pushValue(lua_State* pState, char value)
{
    lua_pushnumber(pState, value);
}

void pushValue(lua_State* pState, short value)
{
    lua_pushnumber(pState, value);
}

void pushValue(lua_State* pState, int value)
{
    lua_pushnumber(pState, value);
}

void pushValue(lua_State* pState, long value)
{
    lua_pushnumber(pState, value);
}

void pushValue(lua_State* pState, unsigned char value)
{
    lua_pushnumber(pState, value);
}

void pushValue(lua_State* pState, unsigned short value)
{
    lua_pushnumber(pState, value);
}

void pushValue(lua_State* pState, unsigned int value)
{
    lua_pushnumber(pState, value);
}

void pushValue(lua_State* pState, unsigned long value)
{
    lua_pushnumber(pState, value);
}

void pushValue(lua_State* pState, char* value)
{
    lua_pushstring(pState, value);
}

void pushValue(lua_State* pState, const char* value)
{
    lua_pushstring(pState, value);
}

void pushValue(lua_State* pState, float value)
{
    lua_pushnumber(pState, value);
}

void pushValue(lua_State* pState, double value)
{
    lua_pushnumber(pState, value);
}

template<typename Ret>
class CCallHelper
{
public:
    static int call(Ret (*func)(), lua_State* pState)
    {
        Ret ret = (*func)();
        pushValue(pState, ret);
        return 1;
    }

    template<typename P1>
    static int call(Ret (*func)(P1), lua_State* pState)
    {
        P1 p1 = getValue(TypeHelper<P1>(), pState, 1);
        Ret ret = (*func)(p1);
        pushValue(pState, ret);
        return 1;
    }

    template<typename P1, typename P2>
    static int call(Ret (*func)(P1, P2), lua_State* pState)
    {
        P1 p1 = getValue(TypeHelper<P1>(), pState, 1);
        P2 p2 = getValue(TypeHelper<P2>(), pState, 2);
        Ret ret = (*func)(p1, p2);
        pushValue(pState, ret);
        return 1;
    }

    template<typename P1, typename P2, typename P3>
    static int call(Ret (*func)(P1, P2, P3), lua_State* pState)
    {
        P1 p1 = getValue(TypeHelper<P1>(), pState, 1);
        P2 p2 = getValue(TypeHelper<P2>(), pState, 2);
        P3 p3 = getValue(TypeHelper<P3>(), pState, 3);
        Ret ret = (*func)(p1, p2, p3);
        pushValue(pState, ret);
        return 1;
    }
};

template<>
class CCallHelper<void>
{
public:
    static int call(void (*func)(), lua_State* pState)
    {
        (*func)();
        return 0;
    } 

    template<typename P1>
    static int call(void (*func)(P1), lua_State* pState)
    {
        P1 p1 = getValue(TypeHelper<P1>(), pState, 1);
        (*func)(p1);
        return 0;
    }

    template<typename P1, typename P2>
    static int call(void (*func)(P1, P2), lua_State* pState)
    {
        P1 p1 = getValue(TypeHelper<P1>(), pState, 1);
        P2 p2 = getValue(TypeHelper<P2>(), pState, 2);
        (*func)(p1, p2);
        return 0;        
    }

    template<typename P1, typename P2, typename P3>
    static int call(void (*func)(P1, P2, P3), lua_State* pState)
    {
        P1 p1 = getValue(TypeHelper<P1>(), pState, 1);
        P2 p2 = getValue(TypeHelper<P2>(), pState, 2);
        P3 p3 = getValue(TypeHelper<P3>(), pState, 3);
        (*func)(p1, p2, p3);
        return 0;        
    }
};

template<typename Func>
class CCallDispatcher
{
public:
    template<typename Ret>
    static int dispatch(Ret (*func)(), lua_State* pState)
    {
        return CCallHelper<Ret>::call(func, pState);
    }

    template<typename Ret, typename P1>
    static int dispatch(Ret (*func)(P1), lua_State* pState)
    {
        return CCallHelper<Ret>::call(func, pState);
    }

    template<typename Ret, typename P1, typename P2>
    static int dispatch(Ret (*func)(P1, P2), lua_State* pState)
    {
        return CCallHelper<Ret>::call(func, pState);
    }

    template<typename Ret, typename P1, typename P2, typename P3>
    static int dispatch(Ret (*func)(P1, P2, P3), lua_State* pState)
    {
        return CCallHelper<Ret>::call(func, pState);
    }
};

void* getRegFunction(lua_State* pState)
{
    return lua_touserdata(pState, lua_upvalueindex(1));
}

template<typename Func>
class CCallRegister
{
public:
    static int call(lua_State* pState)
    {
        Func* func = static_cast<Func*>(getRegFunction(pState));
        return CCallDispatcher<Func>::dispatch(*func, pState);
    }
};

template<typename Func>
void regFunction(lua_State* pState, Func func, const char* funcName)
{
    int funcSize = sizeof(Func);
    void* data = lua_newuserdata(pState, funcSize);
    memcpy(data, &func, funcSize);

    lua_pushcclosure(pState, CCallRegister<Func>::call, 1);
    lua_setglobal(pState, funcName);
}

#define REGISTER_FUNCTION(state, func) regFunction(state, func, #func)

#endif