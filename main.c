
/*!

\file main.c
\brief Application entry point
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016

*/

#include <windows.h>
#include <tchar.h>

#include "service.h"

/*!

\brief Entry point
\param[in] argc Arguments count
\param[in] argv Arguments array
\return Application exit code

*/
INT _tmain(INT argc, TCHAR* argv[])
{
    return (TRUE == service_run(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE);
}
