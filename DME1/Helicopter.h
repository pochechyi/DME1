#pragma once
#include "Station.h"


const int MAX_ID_LENGTH = 256;  // максимальная длина идентификатора

class Helicopter
{
	Point position;
	double speed;
	char* id;
	Station* base;
	double detectionRadius;

public:
	// Конструкторы
	Helicopter();
	Helicopter(const char* id_str, Point pos, double radius, Station* base_ptr, double sp = 0);
	Helicopter(const Helicopter& other);

	// Деструктор
	~Helicopter();

	// Оператор присваивания
	Helicopter& operator=(const Helicopter& other);

	// Геттеры
	const char* getId() const;
	Point getPosition() const;
	double getSpeed() const;
	Station* getBase() const;
	double getDetectionRadius() const;

	// Сеттеры
	void setId(const char* new_id);
	void setPosition(const Point& new_pos);
	void setSpeed(double new_speed);
	void setBase(Station* new_base);
	void setDetectionRadius(double new_radius);

	// Основные методы
	void move();  // пока заглушка
	bool detect(const Point& target) const;
	void print() const;
	void moveTowards(const Point& target);
};
