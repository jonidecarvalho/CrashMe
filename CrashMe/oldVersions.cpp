/*

    //v1.2
    #define UNICODE
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <stdexcept>

    static const wchar_t* kWndClass = L"E06D7363DemoWnd";
    static const int ID_BTN_CRASH = 1001;

    static LONG CALLBACK VectoredHandler(PEXCEPTION_POINTERS p)
    {
        if (p && p->ExceptionRecord &&
            p->ExceptionRecord->ExceptionCode == 0xE06D7363)
        {
            // Mimic “no WER, but exit code matches exception”
            ExitProcess(0xE06D7363);
        }
        return EXCEPTION_CONTINUE_SEARCH;
    }


    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CREATE:
        {
            // Create a single button
            CreateWindowW(
                L"BUTTON",
                L"0xE06D7363",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                20, 20, 160, 40,
                hwnd,
                (HMENU)(INT_PTR)ID_BTN_CRASH,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                nullptr
            );
            return 0;
        }

        case WM_COMMAND:
        {
            const int id = LOWORD(wParam);
            const int code = HIWORD(wParam);

            // Only crash when the user clicks our button
            if (id == ID_BTN_CRASH && code == BN_CLICKED)
            {
                // Unhandled C++ exception => becomes 0xE06D7363 (MSVC EH exception)
                //throw std::runtime_error("User clicked the 0xE06D7363 crash button");
                RaiseException(0xE06D7363, EXCEPTION_NONCONTINUABLE, 0, nullptr);
            }
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
    {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = kWndClass;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

        if (!RegisterClassExW(&wc))
            return 1;


        // Optional: suppress crash UI (not required if we handle + ExitProcess)
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);

        // Catch exceptions early, then force clean exit with same code
        AddVectoredExceptionHandler(1, VectoredHandler);


        HWND hwnd = CreateWindowExW(
            0,
            kWndClass,
            L"Click button to throw 0xE06D7363",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 420, 180,
            nullptr, nullptr, hInstance, nullptr
        );

        if (!hwnd)
            return 1;

        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);

        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        return 0;
    }


    //v1.1
    #define UNICODE
    #define _UNICODE
    #include <windows.h>
    #include <stdexcept>

    static const wchar_t* kWndClass = L"E06D7363ReproWnd";
    static const int kButtonId = 1001;

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CREATE:
        {
            // Create the button
            CreateWindowW(
                L"BUTTON",
                L"0xE06D7363",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                20, 20, 180, 40,
                hwnd,
                (HMENU)kButtonId,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                nullptr
            );
            return 0;
        }

        case WM_COMMAND:
        {
            if (LOWORD(wParam) == kButtonId)
            {
                // Option A: Raise the exact exception code seen in your stack
                // This is the MSVC C++ exception code (commonly seen as 0xE06D7363)
                RaiseException(0xE06D7363, 0, 0, nullptr);

                // Option B: Throw a normal C++ exception (also results in 0xE06D7363 under MSVC)
                // throw std::runtime_error("Repro C++ exception");
            }
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nCmdShow)
    {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInst;
        wc.lpszClassName = kWndClass;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

        if (!RegisterClassExW(&wc))
            return 1;

        HWND hwnd = CreateWindowExW(
            0,
            kWndClass,
            L"Exception Repro - 0xE06D7363",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 420, 180,
            nullptr, nullptr, hInst, nullptr
        );

        if (!hwnd)
            return 2;

        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);

        // Standard message loop (PeekMessage/Dispatch style)
        MSG m{};
        while (GetMessageW(&m, nullptr, 0, 0) > 0)
        {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }

        MSG m{};
        while (true)
        {
            while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE))
            {
                if (m.message == WM_QUIT) return (int)m.wParam;
                TranslateMessage(&m);
                DispatchMessageW(&m);
            }
            Sleep(1);
        }


        return (int)m.wParam;
    }
*/