#ifndef QCD_DEBUG_H
#define QCD_DEBUG_H

#if defined(__cplusplus)
#include <iostream>
#else
#include <stdio.h>
#endif

#if defined(_DEBUG) || !defined(NDEBUG)
#include <qcd_api.h>

#if defined(_MSC_VER)
#define _QCD_DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define _QCD_DEBUG_BREAK() __builtin_trap()
#else
#define _QCD_DEBUG_BREAK() (*(volatile int*)0 = 0)
#endif

#define _QCD_ASSERT(bool_expr) if(!(bool_expr)) _QCD_DEBUG_BREAK()

/**
* ************************** Qcd Debug Tool **************************
* -- similar to GLCall invented by the Cherno
* [!] don't pass in multiple statements: QcDebug(...;...;);
* [!] don't use it in clauses: if(condition) QcDebug(...);
* [!] always use it individually in a scope: if(condition) { QcDebug(...); }
*/
#define QcDebug(single_statement) qcdClearError(); \
        single_statement; \
        _QCD_ASSERT(__QcdLogCall(#single_statement, __FILE__, __LINE__))

#if defined(__cplusplus)
inline bool __QcdLogCall(const char* Statement, const char* File, int Line) {
    if (qcd_enum error = qcdGetError(); error != QCD_NO_ERROR) {
        std::cerr << "[Qcd Error] (" << error << ") "
            << "line " << Line << ": " << Statement << ";\n[at] " << File << "\n"
            << qcdWhatError(error);
        return false;
    }
    return true;
}
#else
static int __QcdLogCall(const char* Statement, const char* File, int Line) {
    qcd_enum error = qcdGetError();
    if (error != QCD_NO_ERROR) {
        printf("[Qcd Error] (%d) line %d: %s;\n[at] %s\n%s",
            error, Line, Statement, File, qcdWhatError(error));
        return 0; // false
    }
    return 1; // true
}
#endif

#else
#define QcDebug(single_statement) single_statement
#endif // if DEBUG

#define Qdb QcDebug

#endif // QCD_DEBUG_H