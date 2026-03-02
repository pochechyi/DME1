#include "SearchMission.h"
#include <iostream>
#include <ctime>

int main()
{
    // Инициализация генератора случайных чисел
    srand(time(nullptr));
    setlocale(LC_ALL, "RUSSIAN");

    SearchMission mission;

    // Загружаем данные
    printf("=== ЗАГРУЗКА ДАННЫХ ===\n");
    if (!mission.loadStations("stations.txt")) {
        printf("Ошибка загрузки станций!\n");
        return 1;
    }

    if (!mission.loadHelicopters("helicopters.txt")) {
        printf("Ошибка загрузки вертолётов!\n");
        return 1;
    }

    if (!mission.loadTarget("target.txt")) {
        printf("Ошибка загрузки цели!\n");
        return 1;
    }

    // Запускаем поиск
    printf("\n=== ЗАПУСК ПОИСКА ===\n");
    mission.setMaxSteps(100);
    mission.run();

    // Сохраняем лог
    mission.saveLog("log.txt");


    return 0;
}
