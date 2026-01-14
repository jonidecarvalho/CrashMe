
#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static const wchar_t* kCls = L"CrashMe_GetMessageHook_Demo";
static const UINT WM_CRASHME = WM_APP + 0x73;

static HHOOK g_hHook = nullptr;

LRESULT CALLBACK GetMsgHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (code >= 0 && lParam)
    {
        MSG* pMsg = reinterpret_cast<MSG*>(lParam);

        // Trigger the exception while the thread is in Get/PeekMessage flow.
        if (pMsg->message == WM_CRASHME)
        {

            // Avoid any WER UI (optional)
            SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

            __try
            {
                // Keep the same exception code and most of the same stack shape
                RaiseException(0xE06D7363, 0, 0, nullptr);
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                // Make it look like a "controlled termination" with the same code,
                // so WER isn't the post-mortem debugger.
                ExitProcess(0xE06D7363);
            }

        }
    }

    return CallNextHookEx(g_hHook, code, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        CreateWindowExW(0, L"BUTTON", L"0xE06D7363",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 20, 160, 32, hwnd, (HMENU)1, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), nullptr);

        // Edit control increases chance MSCTF/TSF is involved (hook chain resemblance).
        CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"focus me",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            20, 70, 260, 24, hwnd, (HMENU)2, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), nullptr);

        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
        {
            // Post a thread message so the WH_GETMESSAGE hook sees it inside PeekMessage.
            PostThreadMessageW(GetCurrentThreadId(), WM_CRASHME, 0, 0);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nCmdShow)
{
    // Install a thread hook (same thread only) – no DLL needed.
    g_hHook = SetWindowsHookExW(WH_GETMESSAGE, GetMsgHookProc, nullptr, GetCurrentThreadId());

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = kCls;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&wc))
        return 1;

    HWND hwnd = CreateWindowExW(0, kCls, L"CrashMe - GetMessage Hook",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 340, 170,
        nullptr, nullptr, hInst, nullptr);

    if (!hwnd) return 1;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Use PeekMessage explicitly (closer to your original: NtUserPeekMessage/xxxRealInternalGetMessage).
    MSG m{};
    while (true)
    {
        while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE))
        {
            if (m.message == WM_QUIT)
                goto done;

            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
        Sleep(1);
    }

done:
    if (g_hHook) UnhookWindowsHookEx(g_hHook);
    return (int)m.wParam;
}
