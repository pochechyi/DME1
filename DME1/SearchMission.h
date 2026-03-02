#pragma once
#include <vector>
#include "Helicopter.h"

#define MaxSteps 100

class SearchMission
{
private:
    std::vector<Station> stations;      // все станции
    std::vector<Helicopter> helicopters; // все вертолёты
    Point target;                        // координаты пропавшего человека
    int maxSteps;                         // максимальное число шагов
    int currentStep;                       // текущий шаг
    bool targetFound;                      // найден ли человек
    char* foundByHelicopter;         // ID вертолёта, который нашёл

public:
    // Конструкторы
    SearchMission();
    ~SearchMission();
    SearchMission(const SearchMission&);

    // Загрузка данных из файлов
    bool loadStations(const char* filename);
    bool loadHelicopters(const char* filename);
    bool loadTarget(const char* filename);

    // Сохранение результатов
    bool saveLog(const char* filename) const;

    void menu();

    // Основные методы
    void setTarget(const Point& newTarget);
    void checkDetection();
    void setMaxSteps(int steps);
    void run();                              // запустить поиск
    void makeStep();                          // сделать один шаг
    bool isTargetFound() const { return targetFound; }

    // Вспомогательные методы
    void printStatus() const;                  // вывести текущее состояние
    void printResult() const;                  // вывести результат



    void drawMap() const;

private:
    // Вспомогательные методы для работы с файлами
    Station* findStationByName(const char* name);
};