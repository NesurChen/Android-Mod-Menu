#ifndef UTILS
#define UTILS
using namespace std;
#include <jni.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>
#include <sstream>
#include "Logger.h"


typedef unsigned long DWORD;
static uintptr_t libBase;

bool libLoaded = false;

DWORD findLibrary(const char *library) {
    char filename[0xFF] = {0},
            buffer[1024] = {0};
    FILE *fp = NULL;
    DWORD address = 0;

    sprintf(filename, OBFUSCATE("/proc/self/maps"));

    fp = fopen(filename, OBFUSCATE("rt"));
    if (fp == NULL) {
        perror(OBFUSCATE("fopen"));
        goto done;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strstr(buffer, library)) {
            address = (DWORD) strtoul(buffer, NULL, 16);
            goto done;
        }
    }

    done:

    if (fp) {
        fclose(fp);
    }

    return address;
}

DWORD getAbsoluteAddress(const char *libraryName, DWORD relativeAddr) {
    libBase = findLibrary(libraryName);
    if (libBase == 0)
        return 0;
    return (reinterpret_cast<DWORD>(libBase + relativeAddr));
}


jboolean isGameLibLoaded(JNIEnv *env, jobject thiz) {
    return libLoaded;
}

bool isLibraryLoaded(const char *libraryName) {
    //libLoaded = true;
    char line[512] = {0};
    FILE *fp = fopen(OBFUSCATE("/proc/self/maps"), OBFUSCATE("rt"));
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            std::string a = line;
            if (strstr(line, libraryName)) {
                libLoaded = true;
                return true;
            }
        }
        fclose(fp);
    }
    return false;
}

uintptr_t string2Offset(const char *c) {
    int base = 16;
    // See if this function catches all possibilities.
    // If it doesn't, the function would have to be amended
    // whenever you add a combination of architecture and
    // compiler that is not yet addressed.
    static_assert(sizeof(uintptr_t) == sizeof(unsigned long)
                  || sizeof(uintptr_t) == sizeof(unsigned long long),
                  "Please add string to handle conversion for this architecture.");

    // Now choose the correct function ...
    if (sizeof(uintptr_t) == sizeof(unsigned long)) {
        return strtoul(c, nullptr, base);
    }

    // All other options exhausted, sizeof(uintptr_t) == sizeof(unsigned long long))
    return strtoull(c, nullptr, base);
}

namespace ToastLength {
    inline const int LENGTH_LONG = 1;
    inline const int LENGTH_SHORT = 0;
}
std::string WString2String(const std::wstring& ws)
{
    std::string strLocale = setlocale(LC_ALL, "");
    const wchar_t* wchSrc = ws.c_str();
    size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
    char *chDest = new char[nDestSize];
    memset(chDest, 0, nDestSize);
    wcstombs(chDest, wchSrc, nDestSize);
    std::string strResult = chDest;
    delete[]chDest;
    setlocale(LC_ALL, strLocale.c_str());
    return strResult;
}
std::wstring String2WString(const std::string& s)
{
    std::string strLocale = setlocale(LC_ALL, "");
    const char* chSrc = s.c_str();
    size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
    wchar_t* wchDest = new wchar_t[nDestSize];
    wmemset(wchDest, 0, nDestSize);
    mbstowcs(wchDest, chSrc, nDestSize);
    std::wstring wstrResult = wchDest;
    delete[]wchDest;
    setlocale(LC_ALL, strLocale.c_str());
    return wstrResult;
}
wstring String2Hex(wstring str, wstring separator = L"")
{
    const wstring hex = L"0123456789ABCDEF";
    wstringstream ss;
    for (wstring::size_type i = 0; i < str.size(); ++i)
    {

        ss << hex[(unsigned char)str[i] >> 4] <<
           hex[(unsigned char)str[i] & 0xf] <<
           hex[str[i] >> 12] <<
           hex[(str[i] & 0xf00)>>8] <<
           separator;
    }
    return ss.str();
}
wstring Hex2String(wstring str)
{
    wstring hex = str;
    long len = hex.length();
    wstring newString;
    for (long i = 0; i < len; i += 4)
    {

        wstring byte = hex.substr(i, 4);
        int n= wcstol(byte.c_str(), NULL, 16);
        char* p = (char*)&n;
        *(p + 1) = *p + *(p + 1);
        *p = *(p + 1)- *p;
        *(p + 1) = *(p + 1) - *p;
        wchar_t chr = (wchar_t)(int)n;
        newString.push_back(chr);
    }
    return newString;
}
string Number2Hex(uint8_t n){
    uint8_t  x;
    string s;
    string t = "0123456789ABCDEF";
    while(n != 0){
        x = n % 16;
        // 将 n % 16 转换为字符逆序存入 s
        s = t[x] + s;
        n = n / 16;
    }

    if(s == ""){
        return "";
    }else{
        return  s;
    }
}
string readUtf16String(int* nameStr){
    string result;
    uint8_t name;
    if(nameStr == 0){
        return "";
    }
    for(int i = 0; ; i++){
        name = *(int*)((uint64_t)nameStr + 0xC + i);
        if(name == 0){
            break;
        }
        result += Number2Hex(name);
    }
    wstring ret = Hex2String(String2WString(result));
    return WString2String(ret);
}


#endif