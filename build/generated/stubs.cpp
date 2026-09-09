
#ifndef SIM_STUBS_GEN_USE_SIM_LIB_VERSION
#define SIM_STUBS_GEN_USE_SIM_LIB_VERSION 1
#endif // SIM_STUBS_GEN_USE_SIM_LIB_VERSION

#if SIM_STUBS_GEN_USE_SIM_LIB_VERSION == 1
#include <simPlusPlus/Lib.h>
#elif SIM_STUBS_GEN_USE_SIM_LIB_VERSION == 2
#include <simPlusPlus-2/Lib.h>
#else
#error "The requested version of simLib is not supported by simStubsGen"
#endif // SIM_STUBS_GEN_USE_SIM_LIB_VERSION

#include <simStubsGen/cpp/common.h>

#include <optional>
#include <cstring>
#include <string>
#include <vector>
#include <set>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include "stubs.h"

FuncTracer::FuncTracer(const std::string &f, int l)
    : f_(f),
      l_(l)
{
    sim::addLog(l_, f_ + " [enter]");
}

FuncTracer::~FuncTracer()
{
    sim::addLog(l_, f_ + " [leave]");
}

#ifndef NDEBUG

template<typename... Arguments>
void addStubsDebugLog(const std::string &fmt, Arguments&&... args)
{
    if(sim::isStackDebugEnabled())
    {
        auto msg = sim::util::sprintf(fmt, std::forward<Arguments>(args)...);
        sim::addLog(sim_verbosity_debug, "STUBS DEBUG: %s", msg);
    }
}

static void addStubsDebugStackDump(sim::handle_t stackHandle)
{
    if(sim::isStackDebugEnabled())
        sim::debugStack(stackHandle);
}

#else // RELEASE
#define addStubsDebugLog(...)
#define addStubsDebugStackDump(x)
#endif

#ifdef QT_COMPIL

Qt::HANDLE UI_THREAD = NULL;
Qt::HANDLE SIM_THREAD = NULL;

std::string threadNickname()
{
    Qt::HANDLE h = QThread::currentThreadId();
    if(h == UI_THREAD) return "UI";
    if(h == SIM_THREAD) return "SIM";
    std::stringstream ss;
    ss << h;
    return ss.str();
}

void uiThread()
{
    Qt::HANDLE h = QThread::currentThreadId();
    if(UI_THREAD != NULL && UI_THREAD != h)
        sim::addLog(sim_verbosity_warnings, "UI thread has already been set");
    UI_THREAD = h;
}

void simThread()
{
    Qt::HANDLE h = QThread::currentThreadId();
    if(SIM_THREAD != NULL && SIM_THREAD != h)
        sim::addLog(sim_verbosity_warnings, "SIM thread has already been set");
    SIM_THREAD = h;
}

#endif // QT_COMPIL

void checkRuntimeVersion()
{
    int simVer = sim::programVersion();

    // version required by simStubsGen:
    int minVer = 4050107; // 4.5.1rev7
    if(simVer < minVer)
        throw sim::exception("requires at least %s (simStubsGen)", sim::versionString(minVer));

    // version required by plugin:
    if(simVer < SIM_REQUIRED_PROGRAM_VERSION_NB)
        throw sim::exception("requires at least %s", sim::versionString(SIM_REQUIRED_PROGRAM_VERSION_NB));

    // warn if the app older than the headers used to compile:
    if(simVer < SIM_PROGRAM_FULL_VERSION_NB)
        sim::addLog(sim_verbosity_warnings, "has been built for %s", sim::versionString(SIM_PROGRAM_FULL_VERSION_NB));
}

bool registerScriptStuff()
{
    try
    {
        checkRuntimeVersion();

        auto dbg = sim::getStringProperty(sim_handle_app, "namedParam.simStubsGen.debug", {});
        if(dbg && (*dbg == "true") || (*dbg == "1"))
            sim::enableStackDebug();

        try
        {
            // register varables from enums:
            // register commands:
            sim::registerScriptCallbackFunction("import", import__callback);
        }
        catch(std::exception &ex)
        {
            throw sim::exception("Initialization failed (registerScriptItems): %s", ex.what());
        }
    }
    catch(sim::exception& ex)
    {
        sim::addLog(sim_verbosity_errors, ex.what());
        return false;
    }
    return true;
}

import__in::import__in()
{
}

import__out::import__out()
{
}

void import__callback(SScriptCallBack *p)
{
    addStubsDebugLog("import__callback: reading input arguments...");
    addStubsDebugStackDump(p->stackID);

    const char *cmd = "simLDraw.import";

    import__in in_args;
    if(p)
    {
        std::memcpy(&in_args._, p, sizeof(SScriptCallBack));
    }
    import__out out_args;

    try
    {
        // check argument count

        int numArgs = sim::getStackSize(p->stackID);
        if(numArgs < 1)
            throw sim::exception("not enough arguments");
        if(numArgs > 1)
            throw sim::exception("too many arguments");

        // read input arguments from stack

        if(numArgs >= 1)
        {
            addStubsDebugLog("import__callback: reading input argument 1 \"filePath\" (std::string)...");
            try
            {
                sim::moveStackItemToTop(p->stackID, 0);
                readFromStack(TypeTag_string{}, p->stackID, &(in_args.filePath));
            }
            catch(std::exception &ex)
            {
                throw sim::exception("read in arg 1 (filePath): %s", ex.what());
            }
        }


        addStubsDebugLog("import__callback: stack content after reading input arguments:");
        addStubsDebugStackDump(p->stackID);

        addStubsDebugLog("import__callback: clearing stack content after reading input arguments");
        // clear stack
        sim::popStackItem(p->stackID, 0);


        addStubsDebugLog("import__callback: calling callback (import_)");
        simLDraw_import_(&in_args, &out_args);
    }
    catch(std::exception &ex)
    {
        sim::setLastError(ex.what());
    }

    try
    {
        addStubsDebugLog("import__callback: writing output arguments...");
        addStubsDebugStackDump(p->stackID);

        addStubsDebugLog("import__callback: clearing stack content before writing output arguments");
        // clear stack
        sim::popStackItem(p->stackID, 0);


        // write output arguments to stack

        addStubsDebugLog("import__callback: writing output argument 1 \"handles\" (std::vector< int >)...");
        try
        {
            writeToStack(TypeTag_table<TypeTag_int>{}, out_args.handles, p->stackID);
        }
        catch(std::exception &ex)
        {
            throw sim::exception("write out arg 1 (handles): %s", ex.what());
        }

        addStubsDebugLog("import__callback: stack content after writing output arguments:");
        addStubsDebugStackDump(p->stackID);
    }
    catch(std::exception &ex)
    {
        sim::setLastError(ex.what());
        // clear stack
        try { sim::popStackItem(p->stackID, 0); } catch(...) {}
    }

    addStubsDebugLog("import__callback: finished");
}

