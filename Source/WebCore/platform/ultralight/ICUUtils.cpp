#include "config.h"
#include "ICUUtils.h"
#include <unicode/putil.h>
#include <unicode/udata.h>

#define ENABLE_ICU_TRACING 0

#if ENABLE_ICU_TRACING
#include <iostream>
#include <unicode/brkiter.h>
#include <unicode/errorcode.h>
#include <unicode/localpointer.h>
#include <unicode/utrace.h>

static void U_CALLCONV traceData(
    const void* context,
    int32_t fnNumber,
    int32_t level,
    const char* fmt,
    va_list args)
{
    char buf[1000];
    const char* fnName;

    fnName = utrace_functionName(fnNumber);
    utrace_vformat(buf, sizeof(buf), 0, fmt, args);
    printf("%s %s\n", fnName, buf);
}
#endif

void setICUDataDirectory(const char* dir)
{
#if ENABLE_ICU_TRACING
    const void* context = nullptr;
    utrace_setFunctions(context, nullptr, nullptr, traceData);
    utrace_setLevel(UTRACE_VERBOSE);
#endif
    u_setDataDirectory(dir);
}

void setICUData(const void* data)
{
#if ENABLE_ICU_TRACING
    const void* context = nullptr;
    utrace_setFunctions(context, nullptr, nullptr, traceData);
    utrace_setLevel(UTRACE_VERBOSE);
#endif
    UErrorCode status = U_ZERO_ERROR;
    udata_setCommonData(data, &status);
}