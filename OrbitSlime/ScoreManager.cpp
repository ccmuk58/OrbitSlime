#include "ScoreManager.h"

// 처음에 점수는 0점으로 초기화
int ScoreManager::slimeHitCount = 0;
int ScoreManager::planetHitCount = 0;

void ScoreManager::PrintScore()
{
    // 검은색 콘솔 창에 점수를 출력해 줌
    std::cout << "====================================\n";
    std::cout << "[슬라임] 피격 횟수 : " << slimeHitCount << " 회\n";
    std::cout << "[행성] 피격 횟수   : " << planetHitCount << " 회\n";
    std::cout << "====================================\n\n";
}