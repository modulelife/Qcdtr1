#ifndef QCD_DEBUG_H
#define QCD_DEBUG_H
#include <iostream>
#include <qcd_api.h>

#if defined(_DEBUG) || !defined(NDEBUG)

#define __QCD_ASSERT(bool_expr) if(!(bool_expr)) __debugbreak()
//**don't pass in multiple statements: QcDebug(...;...;);
//**don't use it in clauses: if(condition) QcDebug(...);
//**always use it individually in a scope: if(condition) { QcDebug(...); }
#define QcDebug(single_statement) qcdClearError();\
        single_statement;\
        __QCD_ASSERT(__QcdLogCall(#single_statement, __FILE__, __LINE__))

#else

#define QCD_ASSERT(bool_expr)
#define QcDebug(single_statement) single_statement

#define QCD_DEBUG_BEGIN
#define QCD_DEBUG_END

#endif // if DEBUG
#define Qdb QcDebug

inline bool __QcdLogCall(const char* Statement, const char* File, int Line)
{
    if (qcd_enum error = qcdGetError(); error != QCD_NO_ERROR)
    {
#ifdef QT_CORE_LIB
        qDebug() << "[Qcd Error] (" << error << ") " << "line " << Line << ": " << Statement << "; " << File;
#else
        std::cerr << "[Qcd Error] (" << error << ") " << "line " << Line << ": " << Statement << "; " << File << std::endl;
#endif
        return false;
    }
    return true;
}

#endif // QCD_DEBUG_H