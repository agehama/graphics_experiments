#pragma once

#include <string>
#pragma once

#include <fstream>
#include <regex>

#ifdef OUTPUT_LOG

#define M_FileName		(strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define M_FileNameW		(wcsrchr(__FILEW__, L'\\') ? wcsrchr(__FILEW__, L'\\') + 1 : __FILEW__)

#define FileDesc		(std::wstring(M_FileName) + L"(" + std::to_wstring(__LINE__) + L") : ")
#define FileDescW		(std::wstring(M_FileNameW) + L"(" + std::to_wstring(__LINE__) + L") : ")

#define ErrorTagW	(L"[ERROR] |> ")
#define LogTagW		(L"[LOG]   |> ")

inline std::string GetLogFileName()
{
    return "log.txt";
}

inline void Log(const std::wstring& str)
{
    std::wofstream ofs(GetLogFileName(), std::ios::app);

    std::wregex regex(L"\n");
    ofs << std::regex_replace(str, regex, L"\n        |> ") << L"\n";
    ofs.close();
}

inline void ClearLog()
{
    std::wofstream ofs(GetLogFileName());
    ofs.close();
}

#define ErrorLog(message) (Log(std::wstring() + ErrorTagW + FileDescW + message))
#define DebugLog(message) (Log(std::wstring() + LogTagW + FileDescW  + message))

#else

inline void Log(const std::wstring& str) {}
inline void ClearLog() {}

#define ErrorLog(message)
#define DebugLog(message)

#endif
