
/*!

\file session.h
\brief HTTP session header file
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016

*/

#ifndef __TIMETABLE_SESSION_H__
#define __TIMETABLE_SESSION_H__

#include <windows.h>

DWORD session_create(PHTTP_SERVER_SESSION_ID);
DWORD session_close(HTTP_SERVER_SESSION_ID);

DWORD session_set_state(HTTP_SERVER_SESSION_ID, HTTP_ENABLED_STATE);
DWORD session_set_timeouts(HTTP_SERVER_SESSION_ID, PHTTP_TIMEOUT_LIMIT_INFO);

#endif /* __TIMETABLE_SESSION_H__ */
