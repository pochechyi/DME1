#include "Helicopter.h"


// Конструктор по умолчанию
Helicopter::Helicopter()
	: position(0, 0), speed(0), base(nullptr), detectionRadius(10.0)
{
	id = new char[MAX_ID_LENGTH];
	id[0] = '\0'; 
}

// Конструктор с параметрами
Helicopter::Helicopter(const char* id_str, Point pos, double radius, Station* base_ptr, double sp)
	: position(pos), speed(sp), base(base_ptr), detectionRadius(radius)
{
	id = new char[strlen(id_str) + 1];
	strcpy_s(id, strlen(id_str) + 1, id_str);
}

// Конструктор копирования
Helicopter::Helicopter(const Helicopter& other) :position(other.position), speed(other.speed),base(other.base),
detectionRadius(other.detectionRadius){
	id = new char[strlen(other.id) + 1];
	strcpy_s(id, strlen(other.id) + 1, other.id);
}

// Деструктор
Helicopter::~Helicopter()
{
	delete[] id;
}

// Оператор присваивания
Helicopter& Helicopter::operator=(const Helicopter& other)
{
	if (this != &other)
	{
		// Копируем простые поля
		position = other.position;
		speed = other.speed;
		base = other.base;
		detectionRadius = other.detectionRadius;

		// Копируем строку id
		delete[] id;
		id = new char[strlen(other.id) + 1];
		strcpy_s(id, strlen(other.id) + 1, other.id);
	}
	return *this;
}

// Геттеры
const char* Helicopter::getId() const
{
	return id;
}

Point Helicopter::getPosition() const
{
	return position;
}

double Helicopter::getSpeed() const
{
	return speed;
}

Station* Helicopter::getBase() const
{
	return base;
}

double Helicopter::getDetectionRadius() const
{
	return detectionRadius;
}

// Сеттеры
void Helicopter::setId(const char* new_id)
{
	delete[] id; 

	if (new_id == nullptr || strlen(new_id) == 0)
	{
		id = new char[1];
		id[0] = '\0';
		return;
	}

	id = new char[strlen(new_id) + 1];
	strcpy_s(id, strlen(new_id) + 1, new_id);
}

void Helicopter::setPosition(const Point& new_pos)
{
	position = new_pos;
}

void Helicopter::setSpeed(double new_speed)
{
	speed = new_speed;
}

void Helicopter::setBase(Station* new_base)
{
	base = new_base;
}

void Helicopter::setDetectionRadius(double new_radius)
{
	detectionRadius = new_radius;
}

// Метод move (пока заглушка)
void Helicopter::move()
{
	// Случайное движение
	double dx = ((rand() % 200) - 100) / 100.0;  // от -1.0 до 1.0
	double dy = ((rand() % 200) - 100) / 100.0;  // от -1.0 до 1.0

	double newX = position.x + dx * speed;
	double newY = position.y + dy * speed;

	position.refresh(newX, newY);
}

// Метод detect - проверяет, видит ли вертолёт цель
bool Helicopter::detect(const Point& target) const
{
	// Вычисляем расстояние от вертолёта до цели
	double distance = position.get_distance(target.x, target.y);///////////////////////////////////////////

	// Если расстояние меньше радиуса обнаружения - цель видна
	return distance <= detectionRadius;
}

// Метод print - выводит информацию о вертолёте
void Helicopter::print() const
{
	printf("========== ВЕРТОЛЁТ ==========\n");
	printf("ID: %s\n", id);
	printf("Позиция: ");
	position.print();
	printf("Скорость: %.2lf\n", speed);
	printf("Радиус обнаружения: %.2lf\n", detectionRadius);

	if (base != nullptr)
	{
		printf("Базируется на: %s\n", base->getName());
	}
	else
	{
		printf("Базируется: не привязан к станции\n");
	}
	printf("==============================\n");
}










void Helicopter::moveTowards(const Point& target)
{
	// Вычисляем направление к цели
	double dx = target.x - position.x;
	double dy = target.y - position.y;

	// Вычисляем расстояние до цели
	double distance = sqrt(dx * dx + dy * dy);

	if (distance > speed) {
		// Двигаемся с максимальной скоростью в направлении цели
		double ratio = speed / distance;
		position.x += dx * ratio;
		position.y += dy * ratio;
	}
	else {
		// Мы уже близко к цели - просто перемещаемся на цель
		position = target;
	}

	// Не даём вертолёту улететь за пределы разумной области
	// Можно добавить ограничения, если нужно
}