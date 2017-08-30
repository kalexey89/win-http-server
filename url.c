
/*!

\file url.c
\brief URL utility implementation file
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016

*/

#include <windows.h>
#include <wininet.h>
#include <tchar.h>


/*!

\brief Parse the URL
\param[in] url URL string
\return Parsed URL

*/
URL_COMPONENTS url_parse(PCTSTR url)
{
    HANDLE heap = GetProcessHeap();

    URL_COMPONENTS uc = { 0 };
    uc.dwStructSize = sizeof(URL_COMPONENTS);
    uc.dwSchemeLength = INTERNET_MAX_SCHEME_LENGTH;
    uc.nScheme = INTERNET_SCHEME_UNKNOWN;
    uc.lpszScheme = (PTSTR) HeapAlloc(heap, HEAP_ZERO_MEMORY, INTERNET_MAX_SCHEME_LENGTH);
    uc.dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;
    uc.lpszUserName = (PTSTR) HeapAlloc(heap, HEAP_ZERO_MEMORY, INTERNET_MAX_USER_NAME_LENGTH);
    uc.dwPasswordLength = INTERNET_MAX_PASSWORD_LENGTH;
    uc.lpszPassword =(PTSTR)  HeapAlloc(heap, HEAP_ZERO_MEMORY, INTERNET_MAX_PASSWORD_LENGTH);
    uc.dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;
    uc.lpszHostName = (PTSTR) HeapAlloc(heap, HEAP_ZERO_MEMORY, INTERNET_MAX_HOST_NAME_LENGTH);
    uc.nPort = INTERNET_DEFAULT_HTTP_PORT;
    uc.dwUrlPathLength = INTERNET_MAX_PATH_LENGTH;
    uc.lpszUrlPath = (PTSTR) HeapAlloc(heap, HEAP_ZERO_MEMORY, INTERNET_MAX_PATH_LENGTH);
    uc.dwExtraInfoLength = INTERNET_MAX_PATH_LENGTH;
    uc.lpszExtraInfo = (PTSTR) HeapAlloc(heap, HEAP_ZERO_MEMORY, INTERNET_MAX_PATH_LENGTH);
    InternetCrackUrl(url, lstrlen(url), ICU_DECODE, &uc);

    return uc;
}

/*!

\brief Release URL buffer memory
\param[in] uc URL buffer

*/
VOID url_free(URL_COMPONENTS* uc)
{
    HANDLE heap = GetProcessHeap();

    HeapFree(heap, 0, uc->lpszScheme);
    uc->lpszScheme = NULL;

    HeapFree(heap, 0, uc->lpszUserName);
    uc->lpszUserName = NULL;

    HeapFree(heap, 0, uc->lpszPassword);
    uc->lpszPassword = NULL;

    HeapFree(heap, 0, uc->lpszHostName);
    uc->lpszHostName = NULL;

    HeapFree(heap, 0, uc->lpszUrlPath);
    uc->lpszUrlPath = NULL;

    HeapFree(heap, 0, uc->lpszExtraInfo);
    uc->lpszExtraInfo = NULL;
}
