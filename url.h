
/*!

\file url.h
\brief URL utility header file
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016

*/

#ifndef __TIMETABLE_URL_H__
#define __TIMETABLE_URL_H__

#include <windows.h>
#include <wininet.h>

URL_COMPONENTS url_parse(PCTSTR);
VOID url_free(URL_COMPONENTS*);

#endif /* __TIMETABLE_URL_H__ */
