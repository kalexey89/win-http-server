
/*!

\file daemon.c
\brief HTTP daemon implementation file
\author Aleksey Kuznetsov
\version 2.0
\date 21.06.2016

*/

#include <http.h>
#include <windows.h>
#include <wininet.h>
#include <shlwapi.h>
#include <tchar.h>

#include "session.h"
#include "urlgroup.h"
#include "queue.h"
#include "router.h"
#include "url.h"

#define DAEMON_WAIT_FOR_STOP_TIMEOUT    10000   // 10 sec
#define DAEMON_DEFAULT_REQUEST_SIZE     1024    // 1 Kb

static HTTPAPI_VERSION daemon_http_api_version = HTTPAPI_VERSION_2;
static DWORD daemon_http_flags = HTTP_INITIALIZE_CONFIG | HTTP_INITIALIZE_SERVER;

static HANDLE daemon_stop_event = NULL;
static HANDLE daemon_done_event = NULL;

static HTTP_SERVER_SESSION_ID daemon_session_id = HTTP_NULL_ID;
static HTTP_URL_GROUP_ID daemon_url_group_id = HTTP_NULL_ID;
static HTTP_URL_CONTEXT daemon_url_context = (HTTP_URL_CONTEXT) 0;

static HANDLE daemon_queue_handle = NULL;
static PCWSTR daemon_queue_name = L"Timetable";

static DWORD CALLBACK daemon_thread(PVOID);

static DWORD daemon_get_request(HANDLE, PHTTP_REQUEST*, PULONG);
static DWORD daemon_process_request(HTTP_REQUEST_ID, HTTP_VERB, URL_COMPONENTS* uc);
/* static DWORD daemon_transmite_file(PCTSTR); */

/*!

\brief Start the daemon
\param[in] url URL to listen on
\return Error code

*/
DWORD daemon_start(PCWSTR url)
{
    // Initialize HTTP server API
    DWORD error = HttpInitialize(daemon_http_api_version, daemon_http_flags, NULL);
    if (ERROR_SUCCESS != error) return error;

    // Create and setup server session
    error = session_create(&daemon_session_id);
    if (ERROR_SUCCESS != error) return error;

    // Create and setup url group
    error = urlgroup_create(&daemon_url_group_id, daemon_session_id);
    if (ERROR_SUCCESS != error) return error;

    error = urlgroup_add_url(daemon_url_group_id, url, daemon_url_context);
    if (ERROR_SUCCESS != error) return error;

    // Create and setup request queue
    error = queue_create(&daemon_queue_handle, daemon_queue_name);
    if (ERROR_SUCCESS != error) return error;

    error = queue_set_503_verbosity(daemon_queue_handle, Http503ResponseVerbosityLimited);
    if (ERROR_SUCCESS != error) return error;

    // Bind url group to request queue
    error = urlgroup_bind(daemon_url_group_id, daemon_queue_handle);
    if (ERROR_SUCCESS != error) return error;

    // Create events
    daemon_stop_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == daemon_stop_event) return GetLastError();

    daemon_done_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == daemon_done_event) return GetLastError();

    // Start worker thread
    HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) daemon_thread, NULL, 0, NULL);
    if (NULL == thread) return GetLastError();

    return ERROR_SUCCESS;
}

/*!

\brief Stop the daemon
\return Error code

*/
DWORD daemon_stop(VOID)
{
    DWORD error = ERROR_SUCCESS;

    // Shutdown request queue
    queue_shutdown(daemon_queue_handle);

    // Wait for threads exit
    SetEvent(daemon_stop_event);
    WaitForSingleObject(daemon_done_event, DAEMON_WAIT_FOR_STOP_TIMEOUT);

    // Close request queue
    error = queue_close(daemon_queue_handle);
    if (ERROR_SUCCESS != error) return error;

    // Clear and close url group
    error = urlgroup_clear(daemon_url_group_id);
    if (ERROR_SUCCESS != error) return error;

    error = urlgroup_close(daemon_url_group_id);
    if (ERROR_SUCCESS != error) return error;

    // Close server session
    error = session_close(daemon_session_id);
    if (ERROR_SUCCESS != error) return error;

    // Uninitialize HTTP server API
    error = HttpTerminate(daemon_http_flags, NULL);
    if (ERROR_SUCCESS != error) return error;

    return ERROR_SUCCESS;
}

/*!

\brief Worker thread of daemon
\return Error code

 */
static DWORD CALLBACK daemon_thread(PVOID param)
{
    // Create heap for each thread for faster memory allocation/deallocation
    HANDLE heap = HeapCreate(0, 0x010000, 0);

    // Process requests while stop event not present
    while (WAIT_OBJECT_0 != WaitForSingleObject(daemon_stop_event, 0))
    {
        DWORD error = ERROR_SUCCESS;

        PHTTP_REQUEST req = NULL;
        PULONG received = 0;

        error = daemon_get_request(heap, &req, received);
        if (ERROR_SUCCESS != error) {}

        HTTP_REQUEST_ID id = req->RequestId;
        HTTP_VERB verb = req->Verb;
        URL_COMPONENTS uc = url_parse(req->CookedUrl.pFullUrl);

        daemon_process_request(id, verb, &uc);

        HeapFree(heap, HEAP_NO_SERIALIZE, req);
    }

    HeapDestroy(heap);
    SetEvent(daemon_done_event);

    return EXIT_SUCCESS;
}

/*!

\brief Wait for HTTP request
\param[in] heap Memory heap
\param[out] req HTTP request
\param[out] received Bytes received
\return Error code

*/
static DWORD daemon_get_request(HANDLE heap, PHTTP_REQUEST* req, PULONG received)
{
    ULONG size = (sizeof(HTTP_REQUEST) + DAEMON_DEFAULT_REQUEST_SIZE);

    *req = (PHTTP_REQUEST) HeapAlloc(heap, HEAP_ZERO_MEMORY, size);
    if (NULL == *req) return ERROR_NOT_ENOUGH_MEMORY;

    while (TRUE)
    {
        CONST DWORD error = queue_get_request(daemon_queue_handle, *req, size,
                                              HTTP_RECEIVE_REQUEST_FLAG_COPY_BODY, received);
        // Request data readed successfully?
        if (ERROR_SUCCESS == error)
        {
            break;
        }

        // Need more memory for process the request?
        if (ERROR_MORE_DATA == error)
        {
            size = *received;
            *req = (PHTTP_REQUEST) HeapReAlloc(heap, HEAP_ZERO_MEMORY, *req, size);
            if (NULL == *req) return ERROR_NOT_ENOUGH_MEMORY;

            continue;
        }

        // Else - report to event log and exit
        // \todo report to event log
        break;
    }

    return ERROR_SUCCESS;
}

/*!

\brief Process HTTP request
\param[in] id Request ID
\param[in] verb Request Verb (GET, POST, PUT & etc)
\param[in] uc Request URL
\return Error code

*/
static DWORD daemon_process_request(HTTP_REQUEST_ID id, HTTP_VERB verb, URL_COMPONENTS* uc)
{
    HTTP_DATA_CHUNK ck = { 0 };
    HTTP_RESPONSE res = { 0 };

    if (0 == wcsncmp(uc->lpszUrlPath, L"/group/get/", wcslen(uc->lpszUrlPath)))
    {
        ck.DataChunkType = HttpDataChunkFromMemory;
        ck.FromMemory.pBuffer = (PVOID) uc->lpszUrlPath;
        ck.FromMemory.BufferLength = (ULONG) uc->dwUrlPathLength * 2;

        res.StatusCode = 200;
        res.pReason = (PCSTR) "OK";
        res.ReasonLength = (USHORT) lstrlenA("OK");
        res.Headers.KnownHeaders[HttpHeaderContentType].pRawValue = (PCSTR) "text/plain";
        res.Headers.KnownHeaders[HttpHeaderContentType].RawValueLength = (USHORT) lstrlenA("text/plain");
        res.pEntityChunks = &ck;
        res.EntityChunkCount = 1;

        queue_put_response(daemon_queue_handle, id, &res, HTTP_SEND_RESPONSE_FLAG_DISCONNECT, NULL);
    }
    else if (0 == wcsncmp(uc->lpszUrlPath, L"/put/", wcslen(uc->lpszUrlPath)))
    {
        if (verb == HttpVerbPUT)
        {
            ck.DataChunkType = HttpDataChunkFromMemory;
            ck.FromMemory.pBuffer = (PVOID) "PUT!";
            ck.FromMemory.BufferLength = (ULONG) lstrlenA("PUT!");

            res.StatusCode = 200;
            res.pReason = (PCSTR) "OK";
            res.ReasonLength = (USHORT) lstrlenA("OK");
            res.Headers.KnownHeaders[HttpHeaderContentType].pRawValue = (PCSTR) "text/plain";
            res.Headers.KnownHeaders[HttpHeaderContentType].RawValueLength = (USHORT) lstrlenA("text/plain");
            res.pEntityChunks = &ck;
            res.EntityChunkCount = 1;

            queue_put_response(daemon_queue_handle, id, &res, HTTP_SEND_RESPONSE_FLAG_DISCONNECT, NULL);
        }
    }
    // Default request handler
    else
    {
        HANDLE file = CreateFile(_T("C:\\Users\\kuznetsov\\Desktop\\page.html"), GENERIC_READ, FILE_SHARE_READ,
                                 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        ck.DataChunkType = HttpDataChunkFromFileHandle;
        ck.FromFileHandle.FileHandle = file;
        ck.FromFileHandle.ByteRange.StartingOffset.QuadPart = 0ull;
        ck.FromFileHandle.ByteRange.Length.QuadPart = HTTP_BYTE_RANGE_TO_EOF;

        res.StatusCode = 200;
        res.pReason = (PCSTR) "OK";
        res.ReasonLength = (USHORT) lstrlenA("OK");
        res.Headers.KnownHeaders[HttpHeaderContentType].pRawValue = (PCSTR) "text/html";
        res.Headers.KnownHeaders[HttpHeaderContentType].RawValueLength = (USHORT) lstrlenA("text/html");
        res.pEntityChunks = &ck;
        res.EntityChunkCount = 1;

        queue_put_response(daemon_queue_handle, id, &res, HTTP_SEND_RESPONSE_FLAG_DISCONNECT, NULL);

        CloseHandle(file);
    }

    return ERROR_SUCCESS;
}

