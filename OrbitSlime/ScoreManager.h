#pragma once

// 게임의 점수와 타격 횟수를 전담해서 관리하는 매니저
class ScoreManager
{
public:
    static int slimeHitCount;   // 슬라임이 부딪힌 횟수
    static int planetHitCount;  // 행성이 부딪힌 횟수

    // 점수를 디버그 로그로 출력해주는 함수
    static void PrintScore();
};