#include <windows.h>

#define hwndApp (*(HWND*)(0x699600))
#define gbRemoteOn (*(int*)(0x69954C))
#define insideProcessMessage (*(int*)(0x6995B8))

static inline void VideoNextFrame()
{
    ((void (* __fastcall)(void))0x5976E0)();
}

static inline void PollSound()
{
    ((void (* __fastcall)(void))0x4ED490)();
}

void __fastcall Process1WindowsMessage()
{
    struct tagMSG Msg;
    BOOL anyMessage = FALSE;

    insideProcessMessage = 1;
    while (1)
    {
        while (PeekMessageA(&Msg, 0, 0, 0, 1u))
        {
            anyMessage = TRUE;
            TranslateMessage(&Msg);
            DispatchMessageA(&Msg);
        }
        if (!IsIconic(hwndApp) || gbRemoteOn)
            break;
        do
        {
            if (GetMessageA(&Msg, 0, 0, 0))
            {
                anyMessage = TRUE;
                TranslateMessage(&Msg);
                DispatchMessageA(&Msg);
            }
        }
        while (IsIconic(hwndApp) && !gbRemoteOn);
    }

    if (!anyMessage)
        MsgWaitForMultipleObjectsEx(0, nullptr, 1, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

    VideoNextFrame();
    insideProcessMessage = 0;
}

static void PumpGameMessages(DWORD timeout_ms)
{
    MSG msg;

    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    if (IsIconic(hwndApp) && !gbRemoteOn)
    {
        while (IsIconic(hwndApp) && !gbRemoteOn)
        {
            if (GetMessageA(&msg, nullptr, 0, 0) > 0)
            {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
        }
        return;
    }

    if (timeout_ms > 0)
    {
        MsgWaitForMultipleObjectsEx(
            0,              // no handles
            nullptr,
            timeout_ms,     // wait this long
            QS_ALLINPUT,    // wake on any message
            MWMO_INPUTAVAILABLE
        );
    }

    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void __fastcall GameTime__Delay(DWORD time)
{
    DWORD sum = time + timeGetTime();

    while ((int)(timeGetTime() - sum) < 0)
    {
        insideProcessMessage = 1;

        DWORD remaining = sum - timeGetTime();
        PumpGameMessages(remaining);

        VideoNextFrame();
        insideProcessMessage = 0;

        PollSound();
    }
}

void __fastcall GameTime__DelayTil(DWORD time)
{
    DWORD t = time;

    while ((int)(timeGetTime() - t) < 0)
    {
        insideProcessMessage = 1;

        DWORD remaining = t - timeGetTime();
        PumpGameMessages(remaining);

        VideoNextFrame();
        insideProcessMessage = 0;

        PollSound();
    }
}
