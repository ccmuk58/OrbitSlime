#include "TimerDisplay.h"

#include <cstdio>
#include <string>

TimerDisplay::TimerDisplay(Mesh* mesh, float width, float height, float gap)
    : BitmapTextRenderer(mesh, width, height, gap)
{
}

void TimerDisplay::Reset()
{
    // 게임 재시작 때 누적 시간과 화면 표시를 모두 처음 상태로 돌린다.
    elapsedSeconds = 0.0f;
    lastShownDecisecond = -1;
    SetText("00:00.0");
}

void TimerDisplay::SetRunning(bool running)
{
    // GameLoop가 현재 상태(Title/Playing/GameOver)에 맞춰 타이머 진행 여부를 알려준다.
    isRunning = running;
}

void TimerDisplay::Update(float dt)
{
    // Playing 상태일 때만 dt를 누적한다.
    if (isRunning)
    {
        elapsedSeconds += dt;
    }

    int shownDecisecond = (int)(elapsedSeconds * 10.0f);

    // 0.1초 단위가 바뀔 때만 메쉬를 다시 만든다.
    if (shownDecisecond != lastShownDecisecond)
    {
        lastShownDecisecond = shownDecisecond;

        int minutes = shownDecisecond / 600;
        int seconds = (shownDecisecond / 10) % 60;
        int deciseconds = shownDecisecond % 10;

        char text[16];
        sprintf_s(text, "%02d:%02d.%01d", minutes, seconds, deciseconds);
        SetText(text);
    }
}
