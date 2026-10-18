#include <Windows.h>
#include <crtdbg.h>

#include "Logger.h"

namespace
{
LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* exceptionPointers)
{
    const DWORD exceptionCode = exceptionPointers != nullptr && exceptionPointers->ExceptionRecord != nullptr
        ? exceptionPointers->ExceptionRecord->ExceptionCode
        : 0;

    logtrace::Write(L"UnhandledException: START code=0x%08lx", exceptionCode);
    logtrace::Write(L"UnhandledException: END");
    return EXCEPTION_EXECUTE_HANDLER;
}

int __cdecl CrtReportHook(int reportType, wchar_t* message, int* returnValue)
{
    logtrace::Write(L"CRT report: type=%d message=%s", reportType, message != nullptr ? message : L"(null)");
    if (returnValue != nullptr)
    {
        *returnValue = 0;
    }
    return 1;
}

void ConfigureNonInteractiveErrorHandling()
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    _set_abort_behavior(0, _CALL_REPORTFAULT);
    _set_error_mode(_OUT_TO_STDERR);
    _CrtSetReportHookW2(_CRT_RPTHOOK_INSTALL, CrtReportHook);
    SetUnhandledExceptionFilter(UnhandledExceptionHandler);
}

void OnExternalLibraryMessage(const wchar_t* message)
{
    if (message != nullptr)
    {
        logtrace::Write(L"DebugReportW: %s", message);
    }
}
}

void ConfigureErrorHandling()
{
    ConfigureNonInteractiveErrorHandling();

    // 外部ライブラリの初期化時に、OnExternalLibraryMessage を通知コールバックとして登録する。
    // ライブラリ固有の登録APIは、実際に使用するライブラリの仕様に合わせて実装する。
}

/*
使用上の注意:
- このファイルは設計確認用のサンプルであり、単独ではビルドしない。
- アプリケーションコードから ASSERT、DebugReportW、abort、MessageBox を呼び出さない。
- 外部ライブラリの通知コールバックから logtrace::Write へ転送する。
- Logger の初期化前に通知される可能性がある場合は、別途フォールバック経路を用意する。
*/
