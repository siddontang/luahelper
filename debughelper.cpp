#include "debughelper.h"

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


CDebugHelper::CDebugHelper()
{
}

CDebugHelper::~CDebugHelper()
{
}

void CDebugHelper::regDebug(lua_State* pState)
{
    lua_pushcclosure(pState, CDebugHelper::debug, 0);
    lua_setglobal(pState, "db");

    setNoneHook(pState);

    lua_sethook(pState, CDebugHelper::debugHook, LUA_MASKLINE | LUA_MASKRET | LUA_MASKCALL, 0);
}

void CDebugHelper::unRegDebug(lua_State* pState)
{
    lua_pushnil(pState);
    lua_setglobal(pState, "db");
}

int CDebugHelper::debug(lua_State* pState)
{
    if (!CDebugHelper::inst().checkNoneHook())
    {
        return 0;
    }

    CDebugHelper::inst().run(pState);
    return 0;
}

int CDebugHelper::checkNoneHook()
{
    return m_hook == &CDebugHelper::noneHook;
}

void CDebugHelper::debugHook(lua_State* pState, lua_Debug* ar)
{
    CDebugHelper::inst().callHook(pState, ar);
}

void CDebugHelper::callHook(lua_State* pState, lua_Debug* ar)
{
    (this->*m_hook)(pState, ar);
}

void CDebugHelper::setNoneHook(lua_State* pState)
{
    m_hook = &CDebugHelper::noneHook;
    m_curStackDepth = -1;
}

void CDebugHelper::setReturnHook(lua_State* pState)
{
    m_hook = &CDebugHelper::returnHook;
    m_curStackDepth = getStackDepth(pState);
}

void CDebugHelper::setNextHook(lua_State* pState)
{
    m_hook = &CDebugHelper::nextHook;
    m_curStackDepth = getStackDepth(pState);
}

void CDebugHelper::setStepHook(lua_State* pState)
{
    m_hook = &CDebugHelper::stepHook;
    m_curStackDepth = -1;
}

void CDebugHelper::noneHook(lua_State* pState, lua_Debug* ar)
{
    (void)pState;
    (void)ar;
}

void CDebugHelper::returnHook(lua_State* pState, lua_Debug* ar)
{
    if(ar->event != LUA_HOOKRET)
    {
        return;
    }

    int curStackDepth = getStackDepth(pState);
    if(m_curStackDepth < curStackDepth)
    {
        return;
    }

    printLine(pState);
    return run(pState);
}

void CDebugHelper::nextHook(lua_State* pState, lua_Debug* ar)
{
    if(ar->event != LUA_HOOKLINE)
    {
        return;
    }

    int curStackDepth = getStackDepth(pState);
    if(m_curStackDepth < curStackDepth)
    {
        return;
    }

    printLine(pState);
    return run(pState);
}

void CDebugHelper::stepHook(lua_State* pState, lua_Debug* ar)
{
    if(ar->event != LUA_HOOKLINE)
    {
        return;
    }

    printLine(pState);
    return run(pState);
}

void CDebugHelper::run(lua_State* pState)
{
    char buffer[1024];

    char sep[] = " \t\n";
    char* next = NULL;
    char* last = NULL;
    char* cmd = NULL;

    m_curFrame = 0;

    while(true)
    {
        printf("(db): ");
        fgets(buffer, 1024, stdin);

        if(buffer[0] == '\0')
        {
            if(m_lastCmd[0] == '\0')
            {
                continue;
            }
            else
            {
                strcpy(buffer, m_lastCmd);
            }
        }
        else
        {
            strcpy(m_lastCmd, buffer);
        }

        next = strtok_r(buffer, sep, &last);
        cmd = next;

        for(char* p = cmd; *p; ++p)
        {
            *p = tolower(*p);
        }        

        if(strcmp(cmd, "h") == 0 || strcmp(cmd, "help") == 0)
        {
            help();
            continue;
        }
        else if(strcmp(cmd, "c") == 0 || strcmp(cmd, "continue") == 0)
        {
            setNoneHook(pState);
            return;
        }
        else if(strcmp(cmd, "bt") == 0)
        {
            printBacktrace(pState);
            continue;
        }
        else if(strcmp(cmd, "l") == 0 || strcmp(cmd, "list") == 0)
        {
            int beginLine = 0;
            int endLine = 0;
            next = strtok_r(NULL, sep, &last);
            if (next)
            {
                beginLine = atoi(next);
                next = strtok_r(NULL, sep, &last);
                if (next)
                {
                    endLine = atoi(next);
                }
            }

            printSource(pState, beginLine, endLine);
        }
        else if(strcmp(cmd, "f") == 0 || strcmp(cmd, "frame") == 0)
        {
            next = strtok_r(NULL, sep, &last);
            if(next)
            {
                m_curFrame = atoi(next);
            }
            printFrame(pState, m_curFrame);    
            continue;
        }
        else if(strcmp(cmd, "p") == 0 || strcmp(cmd, "print") == 0)
        {
            next = strtok_r(NULL, sep, &last);
            if (next)
            {
                printValue(pState, next);
            }

            continue;
        }
        else if(strcmp(cmd, "n") == 0 || strcmp(cmd, "next") == 0)
        {
            setNextHook(pState);
            return;
        }
        else if(strcmp(cmd, "s") == 0 || strcmp(cmd, "step") == 0)
        {
            setStepHook(pState);
            return;
        }
        else if(strcmp(cmd, "return") == 0)
        {
            setReturnHook(pState);
            return;
        }           
    }
}

void CDebugHelper::help()
{
    const char* msg = ""
    "Lua Debuger Help: \n\n"
    "h/help:         Show Help Infomation\n"
    "c/continue      Continue the program until quit or reach another break\n"
    "bt              Show backtrace\n"
    "l/list s e      list source from line s to e\n"
    "f/frame n       Show a frame, n is the frame level\n";
    "p/print value   Print the value\n";
    "n/next          Step program, proceeding through subroutine calls\n";
    "s/step          Step program until it reaches a different source line\n";
    "return          Run the function until it's return\n";
    "\n";

    printf("%s", msg);
}

int CDebugHelper::getStackDepth(lua_State* pState)
{
    int depth = 0;
    lua_Debug ar;
    while(lua_getstack(pState, depth, &ar))
    {
        depth++;
    }
    return depth;
}

void CDebugHelper::printLine(lua_State* pState)
{
    lua_Debug ar;
    if(!lua_getstack(pState, m_curFrame, &ar))
    {
        printf("no source info\n");
        return;
    }

    lua_getinfo(pState, "nSl", &ar);
    int line = ar.currentline;
    if (line == -1)
    {
        printf("No Source Info\n");
        return;
    }

    const char* source =ar.source;
    if(source[0] == '@')
    {
        const char* fileName = ++source;
        FILE* fp = fopen(fileName, "r");
        if(!fp)
        {
            printf("can not open %s\n", fileName);
            return;
        }

        int i = 1;
        char buffer[1024];
        while(fgets(buffer, 1024, fp))
        {
            if(i == line)
            {
                int len = strlen(buffer);
                if(buffer[len - 1] == '\r' || buffer[len - 1] == '\n')
                {
                    buffer[len - 1] = '\0';
                }
                printf("%s\n", buffer);
                break;
            }
            i++;
        }

        fclose(fp);
    }
}

void CDebugHelper::printBacktrace(lua_State* pState)
{
    int depth = getStackDepth(pState);
    for (int i = 0; i < depth; ++i)
    {
        printFrame(pState, i);
    }
}

void CDebugHelper::printSource(lua_State* pState, int beginLine, int endLine)
{
    lua_Debug ar;
    if(!lua_getstack(pState, m_curFrame, &ar))
    {
        printf("no source info\n");
        return;
    }

    lua_getinfo(pState, "nSl", &ar);
    int line = ar.currentline;
    if (line == -1)
    {
        printf("No Source Info\n");
        return;
    }

    if (beginLine >= endLine)
    {
        beginLine = line - 5;
        endLine = line + 5;
    }

    if (beginLine < 0)
    {
        beginLine = 0;
    }

    const char* source =ar.source;
    if(source[0] == '@')
    {
        const char* fileName = ++source;
        FILE* fp = fopen(fileName, "r");
        if(!fp)
        {
            printf("can not open %s\n", fileName);
            return;
        }

        int i = 1;
        char buffer[1024];
        while(fgets(buffer, 1024, fp))
        {
            if(i >= beginLine and i <= endLine)
            {
                int len = strlen(buffer);
                if(buffer[len - 1] == '\r' || buffer[len - 1] == '\n')
                {
                    buffer[len - 1] = '\0';
                }
                printf("%04d:%s\n", i, buffer);
            }
            if (i > endLine)
            {
                break;
            }
            i++;
        }

        fclose(fp);
    }

}

void CDebugHelper::printFrame(lua_State* pState, int frame)
{
    lua_Debug ar;
    if(!lua_getstack(pState, frame, &ar))
    {
        return;
    }

    lua_getinfo(pState, "nSl", &ar);

    char buffer[1024];

    char* p = buffer;

    int len = sprintf(p, "%d ", frame);
    p += len;

    if(ar.name)
    {
        len = sprintf(p, "%s ", ar.name);
        p += len;
    }
    else
    {
        if(ar.what)
        {
            len = sprintf(p, "%s ", ar.what);
            p += len;
        }
    }

    if (ar.source)
    {
        len = sprintf(p, "%s", ar.source);
        p += len;
    }

    sprintf(p, ":%d", ar.currentline);

    printf("%s\n", buffer);
}

void CDebugHelper::printValue(lua_State* pState, const char* value)
{
    char buffer[1024];
    int top = lua_gettop(pState);

    char sep[] = ".[]";
    char* next = NULL;
    char* last = NULL;

    bool found = false;

    strcpy(buffer, value);

    lua_Debug ar;
    lua_getstack(pState, m_curFrame, &ar);

    next = strtok_r(buffer, sep, &last);

    do
    {
        for(int i = 1;;i++)
        {
            const char* key = lua_getlocal(pState, &ar, i);
            if(!key)
            {
                break;
            }

            if(strcmp(key, next) == 0)
            {
                found = true;
                break;
            }
            else
            {
                lua_pop(pState, 1);
            }
        }

        if (found)
        {
            break;
        }

        lua_getinfo(pState, "f", &ar);
        int funcIndex = lua_gettop(pState);
        for (int i = 1; ; i++)
        {
            const char* key = lua_getupvalue(pState, funcIndex, i);
            if (!key)
            {
                break;
            }

            if (strcmp(key, next) == 0)
            {
                found = true;
                break;
            }
            else
            {
                lua_pop(pState, 1);
            }
        }

        if (found)
        {
            break;
        }

        lua_getglobal(pState, next);
        if (lua_isnil(pState, -1))
        {
            lua_pop(pState, 1);
        }
        else
        {
            found = true;
        }
    }while(0);

    if (!found)
    {
        printf("nil value\n");
    }
    else
    {
        while(true)
        {
            if (lua_isnil(pState, -1))
            {
                printf("nil value\n");
                break;
            }
            next = strtok_r(NULL, sep, &last);
            if(!next)
            {
                printStackValue(pState, -1);
                break;
            }
            else
            {
                bool isNumber = true;
                int len = strlen(next);
                for (int i = 0; i < len; ++i)
                {
                    if (!isdigit(next[i]))
                    {
                        isNumber = false;
                        break;
                    }
                }

                if (isNumber)
                {
                    lua_pushnumber(pState, atoi(next));
                }
                else
                {
                    if(next[0] == '"' && next[len - 1] == '"')
                    {
                        next[len - 1] = '\0';
                        lua_pushstring(pState, next + 1);
                    }
                    else
                    {
                        lua_pushstring(pState, next);                        
                    }
                }

                lua_gettable(pState, -2);
                continue;
            }
        }
    }

    lua_settop(pState, top);
}

void CDebugHelper::printStackValue(lua_State* pState, int index)
{
    int type = lua_type(pState, index);
    switch(type)
    {
        case LUA_TNUMBER:
                printf("%g\n", lua_tonumber(pState, index));
                break;
        case LUA_TSTRING:
                printf("%s\n", lua_tostring(pState, index));
                break;
        case LUA_TBOOLEAN:
                printf("%d\n", int(lua_toboolean(pState, index)));
                break;
        case LUA_TTABLE:
                printTable(pState, index);
                break;
        case LUA_TTHREAD:
        case LUA_TFUNCTION:
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
                printf("%s:%p\n", lua_typename(pState, type), lua_topointer(pState, -1));
                break;
        default:
                printf("nil value\n");
                break;
    }
}

void CDebugHelper::printTable(lua_State* pState, int index)
{
    int top = lua_gettop(pState);

    lua_pushvalue(pState, index);

    fprintf(stdout, "{\n");

    lua_pushnil(pState);

    while(lua_next(pState, -2))
    {
        fprintf(stdout, "\t");
        int type = lua_type(pState, -2);
        switch(type)
        {
            case LUA_TNUMBER:
                fprintf(stdout, "%g", lua_tonumber(pState, -2));
                break;
            case LUA_TBOOLEAN:
                fprintf(stdout, "%d", int(lua_toboolean(pState, -2)));
                break;
            case LUA_TSTRING:
                fprintf(stdout, "%s", lua_tostring(pState, -2));
                break;
            default:
                fprintf(stdout, "%s:%p", lua_typename(pState, type), lua_topointer(pState, -2));
                break;
        }

        fprintf(stdout, "\t\t=\t");

        type = lua_type(pState, -1);
        switch(type)
        {
        case LUA_TNUMBER:
            fprintf(stdout, "%g", lua_tonumber(pState, -1));
            break;
        case LUA_TBOOLEAN:
            fprintf(stdout, "%d", int(lua_toboolean(pState, -1)));
            break;
        case LUA_TSTRING:
            fprintf(stdout, "%s", lua_tostring(pState, -1));
            break;
        default:
            fprintf(stdout, "%s:%p", lua_typename(pState, type), lua_topointer(pState, -1));
            break;
        }

        fprintf(stdout, "\n");
 
        lua_pop(pState, 1);
    }
            
    fprintf(stdout, "}\n");
 
    lua_settop(pState, top);
}


