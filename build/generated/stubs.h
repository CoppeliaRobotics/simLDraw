#ifndef STUBS_H__INCLUDED
#define STUBS_H__INCLUDED


#ifndef SIM_STUBS_GEN_USE_SIM_LIB_VERSION
#define SIM_STUBS_GEN_USE_SIM_LIB_VERSION 1
#endif // SIM_STUBS_GEN_USE_SIM_LIB_VERSION

#if SIM_STUBS_GEN_USE_SIM_LIB_VERSION == 1
#include <simLib/simExp.h>
#include <simLib/simTypes.h>
#elif SIM_STUBS_GEN_USE_SIM_LIB_VERSION == 2
#include <simLib-2/simExp.h>
#include <simLib-2/simTypes.h>
#else
#error "The requested version of simLib is not supported by simStubsGen"
#endif // SIM_STUBS_GEN_USE_SIM_LIB_VERSION

#include <simStubsGen/cpp/common.h>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>

class FuncTracer
{
    int l_;
    std::string f_;
public:
    FuncTracer(const std::string &f, int l = sim_verbosity_trace);
    ~FuncTracer();
};

#ifndef __FUNC__
#ifdef __PRETTY_FUNCTION__
#define __FUNC__ __PRETTY_FUNCTION__
#else
#define __FUNC__ __func__
#endif
#endif // __FUNC__

#define TRACE_FUNC FuncTracer __funcTracer__##__LINE__((boost::format("%s:%d:%s:") % __FILE__ % __LINE__ % __FUNC__).str())

#ifdef QT_COMPIL
#include <QThread>

extern Qt::HANDLE UI_THREAD;
extern Qt::HANDLE SIM_THREAD;

std::string threadNickname();
void uiThread();
void simThread();

#define ASSERT_THREAD(ID) \
    if(UI_THREAD == NULL) {\
        sim::addLog(sim_verbosity_debug, "warning: cannot check ASSERT_THREAD(" #ID ") because global variable UI_THREAD is not set yet.");\
    } else if(strcmp(#ID, "UI") == 0) {\
        if(QThread::currentThreadId() != UI_THREAD) {\
            sim::addLog(sim_verbosity_errors, "%s:%d %s should be called from UI thread", __FILE__, __LINE__, __FUNC__);\
            exit(1);\
        }\
    } else if(strcmp(#ID, "!UI") == 0) {\
        if(QThread::currentThreadId() == UI_THREAD) {\
            sim::addLog(sim_verbosity_errors, "%s:%d %s should NOT be called from UI thread", __FILE__, __LINE__, __FUNC__);\
            exit(1);\
        }\
    } else {\
        sim::addLog(sim_verbosity_debug, "warning: cannot check ASSERT_THREAD(" #ID "). Can check only UI and !UI.");\
    }
#endif // QT_COMPIL


bool registerScriptStuff();

struct import__in
{
    SScriptCallBack _;
    std::string filePath;

    import__in();
};

struct import__out
{
    std::vector< int > handles;

    import__out();
};

void import__callback(SScriptCallBack *p);
SIM_DLLEXPORT void simLDraw_import_(import__in *in, import__out *out);

// following functions must be implemented in the plugin

void import_(SScriptCallBack *p, const char *cmd, import__in *in, import__out *out);

#endif // STUBS_H__INCLUDED
