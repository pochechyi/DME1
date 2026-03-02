#include "Station.h"

Station::Station()
{
	name = new char[MaxName];
	name[0] = '\0';
}


// Деструктор
Station::~Station() {
	delete[] name;
}

// Конструктор копирования
Station::Station(const Station& s) {
	coordinates = s.coordinates;
	name = new char[strlen(s.name) + 1];
	strcpy_s(name, strlen(s.name) + 1, s.name);
}

// Конструктор с координатами (double, double)
Station::Station(double x1, double y1, const char* c) : coordinates(x1, y1) {
	name = new char[strlen(c) + 1];
	strcpy_s(name, strlen(c) + 1, c);
}

// Конструктор с точкой Point
Station::Station(Point p, const char* c) : coordinates(p) {
	name = new char[strlen(c) + 1];
	strcpy_s(name, strlen(c) + 1, c);
}

// Сеттер для имени
void Station::setName(const char* n) {
	if (n == nullptr) {
		name[0] = '\0';
		return;
	}
	delete[] name;
	name = new char[strlen(n) + 1];
	strcpy_s(name, strlen(n) + 1, n);
}

// Геттер для имени - ИСПРАВЛЕНО: добавлен const
const char* Station::getName() const {
	return name;
}

// Сеттер для координат
void Station::setCoordinates(const Point& p) {
	coordinates = p;
}

// Геттер для координат
Point Station::getCoordinates() const {
	return coordinates;
}

// Вывод информации
void Station::print() const {
	printf("Станция: %s, ", name);
	coordinates.print();
}

// Ввод информации
void Station::scan() {
	char buffer[MaxName];
	printf("Введите название станции: ");
	scanf_s("%s", buffer);

	delete[] name;
	name = new char[strlen(buffer) + 1];
	strcpy_s(name, strlen(buffer) + 1, buffer);

	coordinates.scan();
}

// Оператор присваивания
Station& Station::operator=(const Station& s) {
	if (this != &s) {
		coordinates = s.coordinates;

		delete[] name;
		name = new char[strlen(s.name) + 1];
		strcpy_s(name, strlen(s.name) + 1, s.name);
	}
	return *this;
}