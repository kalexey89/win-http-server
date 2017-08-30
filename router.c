
#include <http.h>
#include <windows.h>
#include <tchar.h>

#include "queue.h"

/*
 * public functions
 */

DWORD router_route(HANDLE queue, PHTTP_REQUEST req)
{
    PCWSTR path = req->CookedUrl.pAbsPath;

    if (0 == wcsicmp(path, L"/group/get"))
    {
        HTTP_DATA_CHUNK ck = { 0 };
        ck.DataChunkType = HttpDataChunkFromMemory;
        ck.FromMemory.pBuffer = (PVOID) "GROUP GET";
        ck.FromMemory.BufferLength = (ULONG) lstrlenA("GROUP GET");

        HTTP_RESPONSE res = { 0 };
        res.StatusCode = 200;
        res.pReason = (PCSTR) "OK";
        res.ReasonLength = (USHORT) lstrlenA("OK");
        res.pEntityChunks = &ck;
        res.EntityChunkCount = 1;
        queue_put_response(queue, req->RequestId, &res, HTTP_SEND_RESPONSE_FLAG_DISCONNECT, NULL);
    }
    else
    {
        HANDLE file = CreateFile(_T("C:\\Users\\kuznetsov\\Desktop\\page.html"), GENERIC_READ, FILE_SHARE_READ,
                                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        HTTP_DATA_CHUNK ck = { 0 };
        ck.DataChunkType = HttpDataChunkFromFileHandle;
        ck.FromFileHandle.FileHandle = file;
        ck.FromFileHandle.ByteRange.StartingOffset.QuadPart = 0ull;
        ck.FromFileHandle.ByteRange.Length.QuadPart = HTTP_BYTE_RANGE_TO_EOF;

        HTTP_RESPONSE res = { 0 };
        res.StatusCode = 200;
        res.pReason = (PCSTR) "OK";
        res.ReasonLength = (USHORT) lstrlenA("OK");
        res.pEntityChunks = &ck;
        res.EntityChunkCount = 1;
        queue_put_response(queue, req->RequestId, &res, HTTP_SEND_RESPONSE_FLAG_DISCONNECT, NULL);

        CloseHandle(file);
    }

    return ERROR_SUCCESS;
}

