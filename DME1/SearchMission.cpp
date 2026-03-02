#include "SearchMission.h"
#include <fstream>
#include <sstream>

// Конструктор
SearchMission::SearchMission()
    : target(0, 0), maxSteps(MaxSteps), currentStep(0), targetFound(false)
{
    foundByHelicopter = new char[MAX_ID_LENGTH];
    foundByHelicopter[0] = '\0';
}

SearchMission::~SearchMission()
{
    delete[]foundByHelicopter;
}

SearchMission::SearchMission(const SearchMission& s):target(s.target), maxSteps(s.maxSteps), currentStep(s.currentStep), targetFound(s.targetFound)
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

        // Пропускаем пустые строки
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string name;
        double x, y;

        if (iss >> name >> x >> y) {
            // Проверка на корректность координат (не NaN, не бесконечность)
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

// Поиск станции по имени
Station* SearchMission::findStationByName(const char* name)
{
    if (name == nullptr) return nullptr;

    for (auto& station : stations) {
        if (strcmp(station.getName(), name) == 0) {
            return &station;  // возвращаем указатель на найденную станцию
        }
    }

    return nullptr;  // станция не найдена
}

// Загрузка вертолётов из файла (пока заглушка)
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

        // Формат: ID X Y Radius Speed StationName
        if (iss >> id >> x >> y >> radius >> speed >> stationName) {
            // Проверяем корректность данных
            if (!std::isfinite(x) || !std::isfinite(y) ||
                !std::isfinite(radius) || !std::isfinite(speed)) {
                printf("Предупреждение: строка %d содержит некорректные числа\n", lineNumber);
                continue;
            }

            // Ищем станцию базирования
            Station* baseStation = findStationByName(stationName.c_str());
            if (baseStation == nullptr) {
                printf("Предупреждение: строка %d - станция '%s' не найдена, "
                    "вертолёт будет без базы\n", lineNumber, stationName.c_str());
            }

            // Создаём вертолёт
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

// Остальные методы (заглушки)
void SearchMission::setTarget(const Point& newTarget)
{
    target = newTarget;
}

void SearchMission::setMaxSteps(int steps)
{
    maxSteps = steps;
}

void SearchMission::run()
{
    printf("Запуск поисковой миссии...\n");
    printf("Цель: ");
    target.print();
    printf("Максимальное число шагов: %d\n", maxSteps);

    // Сбрасываем счётчик
    currentStep = 1;
    targetFound = false;

    // Цикл поиска
    while (currentStep < maxSteps && !targetFound) {
        makeStep();  // делаем шаг

        if (targetFound) {
            // Если нашли, выводим статус с правильным номером шага
            printf("Шаг %d/%d: ЦЕЛЬ ОБНАРУЖЕНА!\n", currentStep, maxSteps);
        }
        else {
            printStatus();  // обычный статус
        }

        if (!targetFound) {
            currentStep++;  // увеличиваем счётчик ТОЛЬКО если не нашли
        }
    }

    printResult();
}
void SearchMission::makeStep()
{
    // Двигаем все вертолёты к цели
    for (auto& heli : helicopters) {
        heli.moveTowards(target);
    }

    // Проверяем обнаружение цели
    for (const auto& heli : helicopters) {
        if (heli.detect(target)) {
            targetFound = true;
            strcpy_s(foundByHelicopter, 256, heli.getId());

            // Для отладки - покажем расстояние
            Point pos = heli.getPosition();
            double dist = pos.get_distance(target.x, target.y);
            printf("  >>> Вертолёт %s обнаружил цель с расстояния %.2lf!\n",
                heli.getId(), dist);
            break;
        }
    }
}
void SearchMission::printStatus() const
{
    if (targetFound) {
        printf("Шаг %d/%d: ЦЕЛЬ ОБНАРУЖЕНА!\n", currentStep, maxSteps);
        return;
    }

    // Показываем прогресс каждые 5 шагов
    if (currentStep % 5 == 0 || currentStep == maxSteps) {
        printf("Шаг %d/%d: поиск... ", currentStep, maxSteps);

        // Найти ближайший вертолёт
        double minDist = 1e9;
        for (const auto& heli : helicopters) {
            double dist = heli.getPosition().get_distance(target.x, target.y);
            if (dist < minDist) {
                minDist = dist;
            }
        }

        // Прогресс-бар
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
void SearchMission::printResult() const
{
    printf("\n========== РЕЗУЛЬТАТ ПОИСКА ==========\n");
    if (targetFound) {
        printf("✓ Цель обнаружена на шаге %d\n", currentStep);  // уже правильно
        printf("  Вертолёт: %s\n", foundByHelicopter);
        printf("  Координаты цели: ");
        target.print();
    }
    else {
        printf("✗ Цель НЕ обнаружена за %d шагов\n", maxSteps);
    }
    printf("======================================\n");
}

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
        fprintf(file, "  - %s: (%.2lf, %.2lf)\n",
            station.getName(), pos.x, pos.y);
    }

    fprintf(file, "\n=== ВЕРТОЛЁТЫ ===\n");
    for (const auto& heli : helicopters) {
        Point pos = heli.getPosition();
        const char* baseName = "нет базы";
        if (heli.getBase() != nullptr) {
            baseName = heli.getBase()->getName();
        }
        fprintf(file, "  - %s: (%.2lf, %.2lf), радиус=%.1lf, база=%s\n",
            heli.getId(), pos.x, pos.y,
            heli.getDetectionRadius(), baseName);
    }

    fprintf(file, "\n=== РЕЗУЛЬТАТ ===\n");
    fprintf(file, "Шагов: %d/%d\n", currentStep, maxSteps);
    fprintf(file, "Статус: %s\n", targetFound ? "НАЙДЕН" : "НЕ НАЙДЕН");

    if (targetFound) {
        Point pos = target;
        fprintf(file, "Найден вертолётом: %s\n", foundByHelicopter);
        fprintf(file, "На шаге: %d\n", currentStep);
    }

    fprintf(file, "\n========================================\n");
    fclose(file);

    printf("Лог сохранён в %s\n", filename);
    return true;
}