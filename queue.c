
/*!

\file queue.c
\brief HTTP requests queue implementation file
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016

*/

#include <http.h>
#include <windows.h>

/*!

\brief Create the queue
\param[out] queue Queue handle
\param[in] name Queue name
\return Error code

*/
DWORD queue_create(PHANDLE queue, PCWSTR name)
{
    static HTTPAPI_VERSION http_api_version = HTTPAPI_VERSION_2;

    return HttpCreateRequestQueue(http_api_version, name, NULL, 0, queue);
}

/*!

\brief Shutdown the queue
\param[in] queue Queue handle
\return Error code

*/
DWORD queue_shutdown(HANDLE queue)
{
    return HttpShutdownRequestQueue(queue);
}

/*!

\brief Close the queue
\param[in] queue Queue handle
\return Error code

*/
DWORD queue_close(HANDLE queue)
{
    return HttpCloseRequestQueue(queue);
}

/*!

\brief Set queue enabled state (active\inactive)
\param[in] queue Queue handle
\param[in] state Queue state
\return Error code

*/
DWORD queue_set_state(HANDLE queue, HTTP_ENABLED_STATE state)
{
    HTTP_STATE_INFO si = { 0 };
    si.Flags.Present = (HttpEnabledStateInactive == state ? 1 : 0);
    si.State = state;
    return HttpSetRequestQueueProperty(queue, HttpServerStateProperty, (PVOID) &si, sizeof(si), 0, NULL);
}

/*!

\brief Set queue verbosity
\param[in] queue Queue handle
\param[in] verbosity Queue verbosity
\return Error code

*/
DWORD queue_set_503_verbosity(HANDLE queue, HTTP_503_RESPONSE_VERBOSITY verbosity)
{
    DWORD value = (DWORD) verbosity;
    return HttpSetRequestQueueProperty(queue, HttpServer503VerbosityProperty, (PVOID) &value, sizeof(DWORD), 0, NULL);
}

/*!

\brief Receive HTTP request
\param[in] queue Queue handle
\param[out] req Request buffer
\param[in] size Buffer length
\param[in] flags How to copy request body
\param[out] recevied Bytes received
\return Error code

*/
DWORD queue_get_request(HANDLE queue, PHTTP_REQUEST req, ULONG size, ULONG flags, PULONG received)
{
    return HttpReceiveHttpRequest(queue, HTTP_NULL_ID, flags, req, size, received, NULL);
}

/*!

\brief Send HTTP response
\param[in] queue Queue handle
\param[in] id Request ID
\param[in] res Response buffer
\param[in] flags How to send a response
\param[out] sended Bytes sended
\return Error code

*/
DWORD queue_put_response(HANDLE queue, HTTP_REQUEST_ID id, PHTTP_RESPONSE res, ULONG flags, PULONG sended)
{
    return HttpSendHttpResponse(queue, id, flags, res, NULL, sended, NULL, 0, NULL, NULL);
}
