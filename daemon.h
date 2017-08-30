
/*!

\file daemon.h
\brief HTTP daemon header file
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016

*/

#ifndef __TIMETABLE_DAEMON_H__
#define __TIMETABLE_DAEMON_H__

#include <windows.h>

DWORD daemon_start(PCWSTR);
DWORD daemon_stop(VOID);

#endif /* __TIMETABLE_DAEMON_H__ */
