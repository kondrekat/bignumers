#include <iostream>
typedef unsigned short BASE;
#define BASE_SIZE (sizeof(BASE) << 3) //Размер базы в битах
#define BASE_16Q (BASE_SIZE/4) //Количество 16-ричных разрядов в 1м элементе типа BASE
typedef unsigned long D_BASE;
typedef unsigned long long DD_BASE;

class BigNumbers{

public:
	//Конструктор по умолчанию
	BigNumbers();

	//Конструктор. Аргумент - строка, содержащая большое число
	BigNumbers(char * str);

	//Деструктор
	~BigNumbers();	

	//Оператор <<
	friend std::ostream& operator << (std::ostream & out, const BigNumbers & src);

	//Оператор >>
	friend std::istream& operator >> (std::istream & in, BigNumbers & src);

	//Преобразование в 16ричную систему счисления
	//char to16(const short a);

	//Если выделена память, то освобождает её
	void FreeMem();
	int GetLen() const; 

	//Оператор присваивания. Присваетвает *this значение src
	BigNumbers& operator = (const BigNumbers& src);
	
	//Сложение. Суммирует *this и b. Возвращает сумму
	BigNumbers operator + (const BigNumbers& b) const;

	//Оператор меньше. Сравнивает *this и b. 
	//Если *this < b, возвращает 1, иначе 0
	int operator < (const BigNumbers& b) const;

	//Оператор равно. Сравнивает *this и b. 
	//Если *this == b, возвращает 1, иначе 0
	int operator == (const BigNumbers& b) const;

	//Оператор больше. Сравнивает *this и b. 
	//Если *this > b, возвращает 1, иначе 0
	int operator > (const BigNumbers& b) const;
	
	//Вычитание. Вычитает из *this b, если b больше *this - ошибка. 
	//Возвращает разность.
	BigNumbers operator - (const BigNumbers& b) const;

	BigNumbers operator * (const BigNumbers& b) const;

	BigNumbers MulBase(BASE b) const;

	BigNumbers DivBase(BASE b, BASE * mod) const;

private:
	//Обнуляет все указатели (pointer to null)
	void p20();

	//Выделяет память под число длинной Len баз
	//Заполняет память нулями	
	void AllocMem(int Len);

	//Делает преобразование из string в char*
	//char * Convert(const std::string & s);

	//Указатели: на младший разряд - *al, 
	//реально существующий старший разряд - *ar, 
	//старший разряд для которого выделена память - *ah
	BASE *al, *ar, *ah;
	
	//Копирует из src в dst
	void Copy(const BigNumbers& src, const BigNumbers& dst);

	//Возвращяет true, если хотя бы 1 из указателей равен NULL
	//Иначе возвращает false
	bool IsNull() const;
};
