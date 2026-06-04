#include "ScoreManager.h"
#include "Logger.h"


int ScoreManager::slimeHitCount = 0;
int ScoreManager::planetHitCount = 0;

void ScoreManager::PrintScore()
{
    Logger::LogFormat("[Score] slime hits: %d, planet hits: %d", slimeHitCount, planetHitCount);
}