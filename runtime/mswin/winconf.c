#include <runtime/mswin/winconf.h>
#include <runtime/mswin/winapi.h>
#include <runtime/mswin/winutil.h>
#include <runtime/log.h>
#include <runtime/buffer.h>
#include <runtime/main.h>
#include <runtime/string.h>
#include <runtime/mem.h>

static WCHAR* g_configFileW;

#ifndef RUNTIME_PLATFORM_MSWIN_WIN64
static char* g_configFileA;
#endif

/********************************************************************************************************************/

int WinGetPrivateProfileInt(const char* section, const char* option, int defValue)
{
  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (g_configFileA)
        return GetPrivateProfileIntA(section, option, defValue, g_configFileA);
  #endif

    {
        const WCHAR* wsec, *wopt;
        char stmp[128], otmp[128];
        Buf sbuf, obuf;
        int value;

        BufInit(&sbuf, stmp, sizeof(stmp));
        BufInit(&obuf, otmp, sizeof(otmp));
        BufMultiByteToWideChar(&sbuf, section);
        BufMultiByteToWideChar(&obuf, option);
        wsec = BufGetUtf16(&sbuf);
        wopt = BufGetUtf16(&obuf);

        if (wsec && wopt)
            value = GetPrivateProfileIntW(wsec, wopt, defValue, g_configFileW);
        else {
            LogError("%s() failed.", "WinGetPrivateProfileInt");
            value = defValue;
        }

        BufFree(&sbuf);
        BufFree(&obuf);

        return value;
    }
}

bool WinGetPrivateProfileString(Buf* buf, const char* section, const char* option, const char* defValue)
{
    const WCHAR* wsec = NULL, *wopt = NULL, *wdef = NULL;
    char stmp[128], otmp[128], dtmp[128], vtmp[128];
    Buf sbuf, obuf, dbuf, vbuf;
    DWORD nSize, ret;

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (!g_configFileA)
  #endif
    {
        BufInit(&sbuf, stmp, sizeof(stmp));
        BufInit(&obuf, otmp, sizeof(otmp));
        BufInit(&dbuf, dtmp, sizeof(dtmp));
        BufMultiByteToWideChar(&sbuf, section);
        BufMultiByteToWideChar(&obuf, option);
        wsec = BufGetUtf16(&sbuf);
        wopt = BufGetUtf16(&obuf);

        if (!wsec || !wopt) {
          fail:
            LogError("%s() failed.", "WinGetPrivateProfileString");
            BufFree(&sbuf);
            BufFree(&obuf);
            BufFree(&dbuf);
            return false;
        }

        if (!defValue)
            wdef = NULL;
        else {
            BufMultiByteToWideChar(&dbuf, defValue);
            wdef = (WCHAR*)BufGetUtf16(&dbuf);
            if (!wdef)
                goto fail;
        }
    }

    BufInit(&vbuf, vtmp, sizeof(vtmp));
  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (g_configFileA) {
        nSize = (DWORD)BufGetCapacity(buf);
        if (nSize == 0)
            nSize = 16;
    } else
  #endif
        nSize = (DWORD)BufGetCapacityUtf16(&vbuf);

    for (;;) {
      #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
        if (g_configFileA) {
            char* dst = BufReserve(buf, nSize);
            if (!dst)
                goto fail2;

            ret = GetPrivateProfileStringA(section, option, defValue, dst, nSize, g_configFileA);
            if (ret < nSize - 1) {
                BufCommit(buf, ret);
                buf->hasNul = 1; /* WinAPI already put a NULL terminator there */
                break;
            }
        } else
      #endif
        {
            WCHAR* dst = BufReserveUtf16(&vbuf, nSize);
            if (!dst) {
              fail2:
                BufFree(&vbuf);
                goto fail;
            }

            ret = GetPrivateProfileStringW(wsec, wopt, wdef, dst, nSize, g_configFileW);
            if (ret < nSize - 1) {
                if (!BufUtf16ToUtf8N(buf, dst, ret))
                    goto fail2;
                break;
            }
        }

        nSize *= 2;
    }

    BufFree(&vbuf);
    BufFree(&sbuf);
    BufFree(&obuf);
    BufFree(&dbuf);

    return true;
}

/********************************************************************************************************************/

bool WinOpenConfigFile(void)
{
    const WCHAR* wfile;
    char tmp[1024];
    size_t len;
    Buf buf;

    const char* p = g_argv[0], *slash = p;
    while (*p) {
        if (*p == '/' || *p == '\\')
            slash = p + 1;
        ++p;
    }

    BufInit(&buf, tmp, sizeof(tmp));
    BufUtf8ToUtf16N(&buf, g_argv[0], (size_t)(slash - g_argv[0]));
    BufUtf8ToUtf16(&buf, "settings.ini");
    wfile = BufGetUtf16N(&buf, &len);
    if (!wfile) {
      err1:
        BufFree(&buf);
        return false;
    }

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (!g_isWinNT) {
        const char* afile;
        char tmp2[1024];
        size_t len2;
        Buf buf2;

        BufInit(&buf2, tmp2, sizeof(tmp2));
        BufWideCharToMultiByte(&buf2, CP_ACP, wfile);
        afile = BufGetCStrN(&buf2, &len2);
        if (!afile) {
          err2:
            BufFree(&buf2);
            goto err1;
        }

        g_configFileA = StrDupN(afile, len2);
        if (!g_configFileA)
            goto err2;

        BufFree(&buf2);
    } else
  #endif
    {
        g_configFileW = StrDupUtf16(wfile);
        if (!g_configFileW)
            goto err1;
    }

    BufFree(&buf);
    return true;
}

void WinCloseConfigFile(void)
{
    if (g_configFileW) {
        MemFree(g_configFileW);
        g_configFileW = NULL;
    }

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (g_configFileA) {
        MemFree(g_configFileA);
        g_configFileA = NULL;
    }
  #endif
}
