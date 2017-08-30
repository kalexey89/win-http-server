
/*!

\file session.c
\brief HTTP session implementation file
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016

*/

#include <http.h>
#include <windows.h>
#include <tchar.h>

/*!

\brief Create the session
\param[out] session_id Session ID
\return Error code

*/
DWORD session_create(PHTTP_SERVER_SESSION_ID session_id)
{
    static HTTPAPI_VERSION http_api_version = HTTPAPI_VERSION_2;

    return HttpCreateServerSession(http_api_version, session_id, 0);
}


/*!

\brief Close the session
\param[in] session_id Session ID
\return Error code

*/
DWORD session_close(HTTP_SERVER_SESSION_ID session_id)
{
    return HttpCloseServerSession(session_id);
}

/*!

\brief Set session state
\param[in] session_id Session ID
\param[in] state Session state
\return Error code

*/
DWORD session_set_state(HTTP_SERVER_SESSION_ID session_id, HTTP_ENABLED_STATE state)
{
    HTTP_STATE_INFO si = { 0 };
    si.Flags.Present = (HttpEnabledStateInactive == state ? 1 : 0);
    si.State = state;
    return HttpSetServerSessionProperty(session_id, HttpServerStateProperty, (PVOID) &si, (DWORD) sizeof(si));
}

/*!

\brief Set session timeouts
\param[in] session_id Session ID
\param[id] li Timeout params
\return Error code

*/
DWORD session_set_timeouts(HTTP_SERVER_SESSION_ID session_id, PHTTP_TIMEOUT_LIMIT_INFO li)
{
    return HttpSetServerSessionProperty(session_id, HttpServerTimeoutsProperty, (PVOID) li, (DWORD) sizeof(*li));
}
