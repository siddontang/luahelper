#ifndef debughelper_h
#define debughelper_h

struct lua_State;
struct lua_Debug;    

class CDebugHelper
{
public:
    static CDebugHelper& inst()
    {
        static CDebugHelper helper;
        return helper;
    }

    void regDebug(lua_State* pState);
    void unRegDebug(lua_State* pState);

    static int debug(lua_State* pState);
    static void debugHook(lua_State* pState, lua_Debug* ar);
    
private:
    CDebugHelper();
    ~CDebugHelper();

    void callHook(lua_State* pState, lua_Debug* ar);
    void noneHook(lua_State* state, lua_Debug* ar);
    void returnHook(lua_State* state, lua_Debug* ar);
    void nextHook(lua_State* state, lua_Debug* ar);
    void stepHook(lua_State* state, lua_Debug* ar);

    void setNoneHook(lua_State* state);
    void setReturnHook(lua_State* state);
    void setNextHook(lua_State* state);
    void setStepHook(lua_State* state);

    void run(lua_State* pState);
    int checkNoneHook();

    void help();
    int getStackDepth(lua_State* pState);
    
    void printLine(lua_State* pState);
    void printBacktrace(lua_State* pState);
    void printSource(lua_State* pState, int beginLine, int endLine);
    void printFrame(lua_State* pState, int frame);
    void printValue(lua_State* pState, const char* value);

    void printStackValue(lua_State* pState, int index);
    void printTable(lua_State* pState, int index);

private:
    typedef void (CDebugHelper::*hookFunc)(lua_State* state, lua_Debug* ar);

    hookFunc                m_hook;
    int                     m_curStackDepth;
    int                     m_curFrame;

    char                    m_lastCmd[1024];
    char                    m_curCmd[1024];
};


#endif