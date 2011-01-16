#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "aygshell.h"

#define TRACE(msg, ...) _tprintf(_T("%S:%d:") _T(msg)  _T("\n"), __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define CHECK(val) if (!(val)) _tprintf(_T("%S:%d: \" #val \": Check failed\n"), __FUNCTION__, __LINE__)

WINSHELLAPI BOOL SHCreateMenuBar(SHMENUBARINFO *pmb)
{
    if (pmb->dwFlags & SHCMBF_EMPTYBAR) {
        TRACE("SHCMBF_EMPTYBAR");
        return TRUE;
    } else if (pmb->dwFlags & SHCMBF_HMENU) {
        TRACE("SHCMBF_HMENU");
        HMENU hMenu = LoadMenu(pmb->hInstRes, MAKEINTRESOURCE(pmb->nToolBarId));
        CHECK(SetMenu(pmb->hwndParent, hMenu));
        // Resize tightly-packed pocketpc window to add room for menu
        RECT rc;
        CHECK(GetWindowRect(pmb->hwndParent, &rc));
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top + GetSystemMetrics(SM_CYMENU);
        CHECK(SetWindowPos(pmb->hwndParent, NULL, 0, 0, width, height, SWP_NOMOVE|SWP_NOZORDER));
        return TRUE;
    } else {
        _tprintf(_T("%s: UNHANDLED: dwFlags=0x%x\n"), __FUNCTION__, pmb->dwFlags);
        return FALSE;
    }
}

static TCHAR *describe_hwnd(HWND h)
{
  static TCHAR buf[200];
  CHECK(GetClassName(h, buf, 200));
  return buf;
}

TCHAR PROP_OLD_WNDPROC[] = _T("ayg_wndproc");

static LRESULT CALLBACK tapandhold_wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
//    TRACE("hWnd=%s(%x) uMsg=%d", describe_hwnd(hWnd), hWnd, uMsg);
    if (uMsg == WM_CONTEXTMENU) {
        TRACE("WM_CONTEXTMENU for hwnd=%s(%x) x,y=%x", describe_hwnd((HWND)wParam), wParam, lParam);
#if 0
        POINT p;
        p.x = GET_X_LPARAM(lParam);
        p.y = GET_Y_LPARAM(lParam);
        if (ScreenToClient(hWnd, &p)) {
            PostMessage(/*hWnd*/wParam, WM_LBUTTONDOWN, MK_LBUTTON, POINTTOPOINTS(p));
        }
#endif
        NMRGINFO info = {0};
        info.hdr.hwndFrom = (HWND)wParam;
        info.hdr.idFrom = GetDlgCtrlID((HWND)wParam);
        info.hdr.code = GN_CONTEXTMENU;
        info.ptAction.x = GET_X_LPARAM(lParam);
        info.ptAction.y = GET_Y_LPARAM(lParam);
//        wprintf(L"%d,%d\n", info.ptAction.x, info.ptAction.y);
        SendMessage(GetParent((HWND)wParam), WM_NOTIFY, 0, (LPARAM)&info);
        return 0;
    }
    WNDPROC old_wndproc = (WNDPROC)GetProp(hWnd, PROP_OLD_WNDPROC);
    return CallWindowProc(old_wndproc, hWnd, uMsg, wParam, lParam);
}

BOOL SHInitDialog(SHINITDLGINFO *di)
{
    /*
    Subclass dialog to translate WM_CONTEXTMENU to WinCE-specific GN_CONTEXTMENU notification.
    */
    TRACE("hDlg=%x", di->hDlg);
//    LONG old_wndproc = SetWindowLong(di->hDlg, GWL_WNDPROC, (LONG)tapandhold_wndproc);
    LONG old_wndproc = SetWindowLong(di->hDlg, DWL_DLGPROC, (LONG)tapandhold_wndproc);
    SetProp(di->hDlg, PROP_OLD_WNDPROC, (HANDLE)old_wndproc);
    TRACE("Old WndProc=%x", old_wndproc);
    return TRUE;
}
