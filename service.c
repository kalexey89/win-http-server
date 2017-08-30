
/*!

\file service.c
\brief timetable service implementation file
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016

*/

#include <windows.h>
#include <tchar.h>

#include "daemon.h"

static PCTSTR service_name = _T("Timetable");

static SERVICE_STATUS service_status = { 0 };
static SERVICE_STATUS_HANDLE service_handle = NULL;

static VOID WINAPI service_main(INT, TCHAR*[]);
static VOID WINAPI service_handler(DWORD);

static VOID service_start(INT, TCHAR*[]);
static VOID service_stop(VOID);
static VOID service_pause(VOID);
static VOID service_continue(VOID);

static VOID service_set_status(DWORD, DWORD);

/*!

\brief Run the service
\param[in] argc Arguments count
\param[in] argv Arguments array
\return Success?

*/
BOOL service_run(INT argc, TCHAR* argv[])
{
    SERVICE_TABLE_ENTRY service_table[1];
    service_table[0].lpServiceName = (PTSTR) service_name;
    service_table[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION) service_main;
    return StartServiceCtrlDispatcher(service_table);
}

/*!

\brief Service main function
\param[in] argc Arguments count
\param[in] argv Arguments array

*/
static VOID WINAPI service_main(INT argc, TCHAR* argv[])
{
    // Set service status information
    ZeroMemory(&service_status, sizeof(service_status));
    service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_status.dwControlsAccepted = 0;
    service_status.dwCurrentState = SERVICE_STOPPED;
    service_status.dwWin32ExitCode = ERROR_SUCCESS;
    service_status.dwServiceSpecificExitCode = ERROR_SUCCESS;
    service_status.dwCheckPoint = 0;
    service_status.dwWaitHint = 0;

    // Register service control handler
    service_handle = RegisterServiceCtrlHandler(service_name, (LPHANDLER_FUNCTION) service_handler);
    if (NULL == service_handle)
    {

    }

    service_start(argc, argv);

    // Wait before service stopped
    while (SERVICE_STOPPED != service_status.dwCurrentState)
    {
        Sleep(200);
    }
}

/*!

\brief Control code handler
\param[in] ctrl Control code

*/
static VOID WINAPI service_handler(DWORD ctrl)
{
    switch (ctrl)
    {
        case SERVICE_CONTROL_STOP: service_stop(); break;
        case SERVICE_CONTROL_PAUSE: service_pause(); break;
        case SERVICE_CONTROL_CONTINUE: service_continue(); break;
        default: break;
    }
}

/*!

\brief Service start handler
\param[in] argc Arguments count
\param[in] argv Arguments array

*/
static VOID service_start(INT argc, TCHAR* argv[])
{
    service_set_status(SERVICE_START_PENDING, ERROR_SUCCESS);

    daemon_start(L"http://+:8080/");

    service_set_status(SERVICE_RUNNING, ERROR_SUCCESS);
}

/*!

\brief Service stop handler

*/
static VOID service_stop(VOID)
{
    service_set_status(SERVICE_STOP_PENDING, ERROR_SUCCESS);

    daemon_stop();

    service_set_status(SERVICE_STOPPED, ERROR_SUCCESS);
}

/*!

\brief Service pause handler

*/
static VOID service_pause(VOID)
{
    service_set_status(SERVICE_PAUSE_PENDING, ERROR_SUCCESS);

    Sleep(5000);

    service_set_status(SERVICE_PAUSED, ERROR_SUCCESS);
}


/*!

\brief Service resume handler

*/
static VOID service_continue(VOID)
{
    service_set_status(SERVICE_CONTINUE_PENDING, ERROR_SUCCESS);

    Sleep(5000);

    service_set_status(SERVICE_RUNNING, ERROR_SUCCESS);
}

/*!

\brief Set service status
\param[in] state Service state
\param[in] error Error code

*/
static VOID service_set_status(DWORD state, DWORD error)
{
    CONST BOOL runned = ((SERVICE_RUNNING == state) || (SERVICE_PAUSED == state));
    CONST DWORD ctrls = (TRUE == runned ? SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE : 0);

    service_status.dwControlsAccepted = ctrls;
    service_status.dwCurrentState = state;
    service_status.dwWin32ExitCode = error;
    if (FALSE == SetServiceStatus(service_handle, &service_status))
    {
        // \todo Report to event log
    }
}
