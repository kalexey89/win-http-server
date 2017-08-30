
/*!

\file urlgroup.c
\brief URL group implementation file
\author Aleksey Kuznetsov
\version 1.0
\date 14.03.2016

*/

#include <http.h>
#include <windows.h>

/*!

\brief Create the URL group
\param[out] url_group_id URL group ID
\param[in] session_id HTTP session ID
\return Error code

*/
DWORD urlgroup_create(PHTTP_URL_GROUP_ID url_group_id, HTTP_SERVER_SESSION_ID session_id)
{
    return HttpCreateUrlGroup(session_id, url_group_id, 0);
}

/*!

\brief Close the URL group
\param[in] url_group_id URL group ID
\return Error code

*/
DWORD urlgroup_close(HTTP_URL_GROUP_ID url_group_id)
{
    return HttpCloseUrlGroup(url_group_id);
}

/*!

\brief Bind URL group to request queue
\param[in] url_group_id URL group ID
\param[in] queue Queue handle
\return Error code

*/
DWORD urlgroup_bind(HTTP_URL_GROUP_ID url_group_id, HANDLE queue)
{
    HTTP_BINDING_INFO bi = { 0 };
    bi.Flags.Present = 1;
    bi.RequestQueueHandle = queue;
    return HttpSetUrlGroupProperty(url_group_id, HttpServerBindingProperty, (PVOID) &bi, (DWORD) sizeof(bi));
}

/*!

\brief Add URL to group
\param[in] url_group_id URL group ID
\param[in] url URL string
\param[in] context URL context
\return Error code

*/
DWORD urlgroup_add_url(HTTP_URL_GROUP_ID url_group_id, PCWSTR url, HTTP_URL_CONTEXT context)
{
    return HttpAddUrlToUrlGroup(url_group_id, url, context, 0);
}

/*!

\brief Remove URL from group
\param[in] url_group_id URL group ID
\param[in] url URL string
\return Error code

*/
DWORD urlgroup_remove_url(HTTP_URL_GROUP_ID url_group_id, PCWSTR url)
{
    return HttpRemoveUrlFromUrlGroup(url_group_id, url, 0);
}

/*!

\brief Clear the URL group
\param[in] url_group_id URL group ID
\return Error code

*/
DWORD urlgroup_clear(HTTP_URL_GROUP_ID url_group_id)
{
    return HttpRemoveUrlFromUrlGroup(url_group_id, NULL, HTTP_URL_FLAG_REMOVE_ALL);
}

/*!

\brief Set group state
\param[in] url_group_id URL group ID
\param[in] state URL group state
\return Error code

*/
DWORD urlgroup_set_state(HTTP_URL_GROUP_ID url_group_id, HTTP_ENABLED_STATE state)
{
    HTTP_STATE_INFO si = { 0 };
    si.Flags.Present = (HttpEnabledStateInactive == state ? 1 : 0);
    si.State = state;
    return HttpSetUrlGroupProperty(url_group_id, HttpServerStateProperty, (PVOID) &si, (DWORD) sizeof(si));
}

/*!

\brief Set group timeout params
\param[in] url_group_id URL group ID
\param[in] li Timeout params
\return Error code

*/
DWORD urlgroup_set_timeouts(HTTP_URL_GROUP_ID url_group_id, PHTTP_TIMEOUT_LIMIT_INFO li)
{
    return HttpSetUrlGroupProperty(url_group_id, HttpServerTimeoutsProperty, (PVOID) li, (DWORD) sizeof(*li));
}
