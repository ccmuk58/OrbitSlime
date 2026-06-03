#include "ScoreManager.h"
#include "Logger.h"

// 처음에 점수는 0점으로 초기화
int ScoreManager::slimeHitCount = 0;
int ScoreManager::planetHitCount = 0;

void ScoreManager::PrintScore()
{
    Logger::LogFormat("[Score] slime hits: %d, planet hits: %d", slimeHitCount, planetHitCount);
}