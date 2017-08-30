
/*!

\file urlgroup.h
\brief URL group header file
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016


*/
#ifndef __TIMETABLE_URLGROUP_H__
#define __TIMETABLE_URLGROUP_H__

#include <windows.h>

DWORD urlgroup_create(PHTTP_URL_GROUP_ID, HTTP_SERVER_SESSION_ID);
DWORD urlgroup_close(HTTP_URL_GROUP_ID);

DWORD urlgroup_bind(HTTP_URL_GROUP_ID, HANDLE);

DWORD urlgroup_add_url(HTTP_URL_GROUP_ID, PCWSTR, HTTP_URL_CONTEXT);
DWORD urlgroup_remove_url(HTTP_URL_GROUP_ID, PCWSTR);
DWORD urlgroup_clear(HTTP_URL_GROUP_ID);

DWORD urlgroup_set_state(HTTP_URL_GROUP_ID, HTTP_ENABLED_STATE);
DWORD urlgroup_set_timeouts(HTTP_URL_GROUP_ID, PHTTP_TIMEOUT_LIMIT_INFO);

#endif /* __TIMETABLE_URLGROUP_H__ */
