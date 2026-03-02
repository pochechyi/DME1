#include "SearchMission.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

// Конструктор
SearchMission::SearchMission()
    : target(0, 0), maxSteps(100), currentStep(0), targetFound(false)
{
    foundByHelicopter = new char[256];
    foundByHelicopter[0] = '\0';
}

// Деструктор
SearchMission::~SearchMission()
{
    delete[] foundByHelicopter;
}

// Конструктор копирования
SearchMission::SearchMission(const SearchMission& s)
    : target(s.target),
    maxSteps(s.maxSteps),
    currentStep(s.currentStep),
    targetFound(s.targetFound)
{
    foundByHelicopter = new char[strlen(s.foundByHelicopter) + 1];
    strcpy_s(foundByHelicopter, strlen(s.foundByHelicopter) + 1, s.foundByHelicopter);
}

// Загрузка станций из файла
bool SearchMission::loadStations(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return false;
    }

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string name;
        double x, y;

        if (iss >> name >> x >> y) {
            if (std::isfinite(x) && std::isfinite(y)) {
                stations.push_back(Station(Point(x, y), name.c_str()));
                printf("Загружена станция: %s (%.2lf, %.2lf)\n", name.c_str(), x, y);
            }
            else {
                printf("Предупреждение: строка %d содержит некорректные координаты\n", lineNumber);
            }
        }
        else {
            printf("Предупреждение: строка %d имеет неверный формат\n", lineNumber);
        }
    }

    file.close();
    printf("Загружено %zu станций\n", stations.size());
    return true;
}

// Загрузка вертолётов из файла
bool SearchMission::loadHelicopters(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return false;
    }

    std::string line;
    int lineNumber = 0;
    int loadedCount = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string id, stationName;
        double x, y, radius, speed;

        if (iss >> id >> x >> y >> radius >> speed >> stationName) {
            if (!std::isfinite(x) || !std::isfinite(y) ||
                !std::isfinite(radius) || !std::isfinite(speed)) {
                printf("Предупреждение: строка %d содержит некорректные числа\n", lineNumber);
                continue;
            }

            Station* baseStation = findStationByName(stationName.c_str());
            if (baseStation == nullptr) {
                printf("Предупреждение: строка %d - станция '%s' не найдена, "
                    "вертолёт будет без базы\n", lineNumber, stationName.c_str());
            }

            Point pos(x, y);
            helicopters.push_back(Helicopter(id.c_str(), pos, radius, baseStation, speed));
            loadedCount++;

            printf("Загружен вертолёт: %s на базе %s\n",
                id.c_str(), baseStation ? stationName.c_str() : "неизвестна");
        }
        else {
            printf("Предупреждение: строка %d имеет неверный формат\n", lineNumber);
        }
    }

    file.close();
    printf("Загружено %d вертолётов\n", loadedCount);
    return true;
}

// Загрузка цели из файла
bool SearchMission::loadTarget(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return false;
    }

    double x, y;
    if (file >> x >> y) {
        target = Point(x, y);
        printf("Загружена цель: (%.2lf, %.2lf)\n", x, y);
        file.close();
        return true;
    }

    printf("Ошибка: неверный формат файла цели\n");
    file.close();
    return false;
}

// Поиск станции по имени
Station* SearchMission::findStationByName(const char* name)
{
    if (name == nullptr) return nullptr;

    for (auto& station : stations) {
        if (strcmp(station.getName(), name) == 0) {
            return &station;
        }
    }

    return nullptr;
}

// Сеттеры
void SearchMission::setTarget(const Point& newTarget)
{
    target = newTarget;
}

void SearchMission::setMaxSteps(int steps)
{
    maxSteps = steps;
}

// Запуск поиска
void SearchMission::run()
{
    printf("\n=== ЗАПУСК ПОИСКОВОЙ МИССИИ ===\n");
    printf("Цель: ");
    target.print();
    printf("Максимальное число шагов: %d\n", maxSteps);

    currentStep = 1;
    targetFound = false;

    // Начальная карта
    printf("\n📍 НАЧАЛЬНАЯ СИТУАЦИЯ:\n");
    drawMap();

    while (currentStep <= maxSteps && !targetFound) {
        printf("\n=== ШАГ %d ===\n", currentStep);
        makeStep();
        drawMap();

        if (!targetFound) {
            printf("Шаг %d/%d: поиск продолжается... Нажмите Enter", currentStep, maxSteps);
            getchar();
            currentStep++;
        }
    }

    printResult();
}

// Один шаг поиска
void SearchMission::makeStep()
{
    // Движение вертолётов
    for (auto& heli : helicopters) {
        Point oldPos = heli.getPosition();
        heli.moveTowards(target);
        Point newPos = heli.getPosition();
        printf("  %s: (%.1f, %.1f) -> (%.1f, %.1f)\n",
            heli.getId(), oldPos.x, oldPos.y, newPos.x, newPos.y);
    }

    // Проверка обнаружения
    checkDetection();
}

// Проверка обнаружения цели
void SearchMission::checkDetection()
{
    for (const auto& heli : helicopters) {
        if (heli.detect(target)) {
            targetFound = true;
            strcpy_s(foundByHelicopter, 256, heli.getId());

            Point pos = heli.getPosition();
            double dist = pos.get_distance(target.x, target.y);
            printf("\n🎯 ВЕРТОЛЁТ %s ОБНАРУЖИЛ ЦЕЛЬ! (расстояние: %.2f)\n",
                heli.getId(), dist);
            break;
        }
    }
}

void SearchMission::drawMap() const
{
    const int MIN = -15;
    const int MAX = 15;

    printf("\n  КАРТА ПОИСКА\n");
    printf("  +");
    for (int x = MIN; x <= MAX; x++) printf("-");
    printf("+\n");

    for (int y = MAX; y >= MIN; y--) {
        printf("  |");
        for (int x = MIN; x <= MAX; x++) {
            char c = '.';

            // Цель
            if (abs(x - target.x) < 0.5 && abs(y - target.y) < 0.5) {
                c = 'X';
            }

            // Вертолёты
            for (const auto& heli : helicopters) {
                Point pos = heli.getPosition();
                if (abs(x - pos.x) < 0.5 && abs(y - pos.y) < 0.5) {
                    c = heli.detect(target) ? '!' : heli.getId()[0];
                    break;
                }
            }

            // Станции
            for (const auto& station : stations) {
                Point pos = station.getCoordinates();
                if (abs(x - pos.x) < 0.5 && abs(y - pos.y) < 0.5) {
                    c = 'S';
                    break;
                }
            }

            printf("%c", c);
        }
        printf("| %3d\n", y);
    }

    printf("  +");
    for (int x = MIN; x <= MAX; x++) printf("-");
    printf("+\n");
    printf("    ");
    for (int x = MIN; x <= MAX; x += 5) {
        printf("%-5d", x);
    }
    printf("\n");
    printf("  X-цель, S-станция, !-ОБНАРУЖИЛ, буквы-вертолёты\n");
}

// Печать статуса
void SearchMission::printStatus() const
{
    if (targetFound) {
        printf("Шаг %d/%d: ЦЕЛЬ ОБНАРУЖЕНА!\n", currentStep, maxSteps);
        return;
    }

    if (currentStep % 5 == 0 || currentStep == maxSteps) {
        printf("Шаг %d/%d: поиск... ", currentStep, maxSteps);

        double minDist = 1e9;
        for (const auto& heli : helicopters) {
            double dist = heli.getPosition().get_distance(target.x, target.y);
            if (dist < minDist) minDist = dist;
        }

        int progress = (int)((1.0 - minDist / 25.0) * 20);
        if (progress < 0) progress = 0;
        if (progress > 20) progress = 20;

        printf("[");
        for (int i = 0; i < 20; i++) {
            printf("%c", i < progress ? '#' : '.');
        }
        printf("] %.1lf до цели\n", minDist);
    }
}

// Печать результата
void SearchMission::printResult() const
{
    printf("\n========== РЕЗУЛЬТАТ ПОИСКА ==========\n");
    if (targetFound) {
        printf("✓ Цель обнаружена на шаге %d\n", currentStep);
        printf("  Вертолёт: %s\n", foundByHelicopter);
        printf("  Координаты цели: ");
        target.print();
    }
    else {
        printf("✗ Цель НЕ обнаружена за %d шагов\n", maxSteps);
    }
    printf("======================================\n");
}

// Сохранение лога
bool SearchMission::saveLog(const char* filename) const
{
    FILE* file = nullptr;
    if (fopen_s(&file, filename, "w") != 0) return false;

    fprintf(file, "========================================\n");
    fprintf(file, "      ОТЧЁТ О ПОИСКОВОЙ МИССИИ        \n");
    fprintf(file, "========================================\n\n");

    fprintf(file, "=== ЦЕЛЬ ===\n");
    fprintf(file, "Координаты: (%.2lf, %.2lf)\n\n", target.x, target.y);

    fprintf(file, "=== СТАНЦИИ ===\n");
    for (const auto& station : stations) {
        Point pos = station.getCoordinates();
        fprintf(file, "  - %s: (%.2lf, %.2lf)\n", station.getName(), pos.x, pos.y);
    }

    fprintf(file, "\n=== ВЕРТОЛЁТЫ ===\n");
    for (const auto& heli : helicopters) {
        Point pos = heli.getPosition();
        const char* baseName = heli.getBase() ? heli.getBase()->getName() : "нет базы";
        fprintf(file, "  - %s: (%.2lf, %.2lf), радиус=%.1lf, база=%s\n",
            heli.getId(), pos.x, pos.y, heli.getDetectionRadius(), baseName);
    }

    fprintf(file, "\n=== РЕЗУЛЬТАТ ===\n");
    fprintf(file, "Шагов: %d/%d\n", currentStep, maxSteps);
    fprintf(file, "Статус: %s\n", targetFound ? "НАЙДЕН" : "НЕ НАЙДЕН");

    if (targetFound) {
        fprintf(file, "Найден вертолётом: %s\n", foundByHelicopter);
        fprintf(file, "На шаге: %d\n", currentStep);
    }

    fprintf(file, "\n========================================\n");
    fclose(file);

    printf("Лог сохранён в %s\n", filename);
    return true;
}