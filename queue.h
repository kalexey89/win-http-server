
/*!

\file queue.h
\brief HTTP requests queue header file
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016

*/

#ifndef __TIMETABLE_QUEUE_H__
#define __TIMETABLE_QUEUE_H__

#include <windows.h>

DWORD queue_create(PHANDLE, PCWSTR);
DWORD queue_shutdown(HANDLE);
DWORD queue_close(HANDLE);

DWORD queue_set_state(HANDLE, HTTP_ENABLED_STATE);
DWORD queue_set_503_verbosity(HANDLE, HTTP_503_RESPONSE_VERBOSITY);
DWORD queue_set_length(HANDLE, ULONG);

DWORD queue_get_request(HANDLE, PHTTP_REQUEST, ULONG, ULONG, PULONG);
DWORD queue_put_response(HANDLE, HTTP_REQUEST_ID, PHTTP_RESPONSE, ULONG, PULONG);

#endif /* __TIMETABLE_QUEUE_H__ */
