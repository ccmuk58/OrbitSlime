#include "TimerDisplay.h"

#include <string>

TimerDisplay::TimerDisplay(Mesh* mesh, float width, float height, float gap)
    : DigitTextureDisplay(mesh, width, height, gap)
{
}

void TimerDisplay::Reset()
{
    // 게임 재시작 때 누적 시간과 화면 표시를 모두 처음 상태로 돌린다.
    elapsedSeconds = 0.0f;
    lastShownSecond = -1;
    SetText("0");
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

    int shownSecond = (int)elapsedSeconds;

    // 초가 바뀔 때만 메쉬를 다시 만든다. 매 프레임 만들면 불필요하게 비용이 든다.
    if (shownSecond != lastShownSecond)
    {
        lastShownSecond = shownSecond;
        SetText(std::to_string(shownSecond));
    }
}
