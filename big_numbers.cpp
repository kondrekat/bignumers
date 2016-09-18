#include <iostream>
#include "big_numbers.h"
#include <string.h>

BigNumbers::BigNumbers(){
	p20();
}

BigNumbers::~BigNumbers(){
	FreeMem();
}

BigNumbers::BigNumbers(char * str){
	if(str != NULL) {
		int n = 0;
		char * tmp_str;
		while(str[n] == '0') n++;
		int i = n;
		//проверяем валидность числа
		while(((str[i] >= '0')&&(str[i] <= '9'))||((str[i] >= 'A')&&(str[i] <= 'F'))||((str[i] >= 'a')&&(str[i] <= 'f'))) 
			i++;
		if (str[i] != '\0')
			throw "error";
		//Тут мы можем быть уверенны что число корректно
		p20();
		tmp_str = str + n;
		n = strlen(tmp_str);
		//захватываем памяти сколько надо
		AllocMem((((n - 1)<0)?0:(n-1))/(BASE_16Q) + 1);

		//обнуляем число
		for(i = 0; al + i <= ah; i++)
		{
			al[i] = 0;
		}
		BASE tmp_BASE;
		for(i = n-1; i >= 0; i--){
			//Напишем получение инта из чара
			if((tmp_str[i] >= '0') && (tmp_str[i] <= '9'))  
				tmp_BASE = tmp_str[i] -  '0';
			else if((tmp_str[i] >= 'a') && (tmp_str[i] <= 'f')) 
				tmp_BASE = tmp_str[i] - 'a' + 10;
			else tmp_BASE = tmp_str[i] - 'A' + 10;
			//Вычисялем номер в массиве коэффициентов
			//вычисялем смещение внутри базы
			//пишем присваивание.
			al[(n - 1 - i) / BASE_16Q] |= tmp_BASE << (((n - 1 - i) % BASE_16Q) * 4);
		}
	}
}

BigNumbers& BigNumbers::operator = (const BigNumbers& src){
	if (this != &src){
	FreeMem();
  	AllocMem(src.GetLen());
  	Copy(src, *this);
	}
  	return *this;
}

int BigNumbers::GetLen() const {
	return (al == NULL)?0:(ar - al + 1);
}

void BigNumbers::Copy(const BigNumbers& src, const BigNumbers& dst){
	BASE *s, *d;
	if(src.al != NULL)
		for(s = src.al, d = dst.al; (s <= src.ar) &&  (d <= dst.ar); d++, s++) *d = *s;
}

std::ostream& operator << (std::ostream & out, const BigNumbers & src) {
	if (src.al == NULL) out << '0';
	else {
		char c = '0';
		int j = BASE_SIZE;
		unsigned short tmp;
		BASE * i = src.ar;
		//Пропускаем незначащие нули в начале числа. 
		//4 - к-во бит, которое занимает один 16ричный разряд
		while((j >= 4) && (c == '0')){
			j -= 4;
			tmp = ((* i) >> j) % 16;
			c = (tmp < 10)?('0' + tmp):('A' + tmp - 10);
		}
		out << c;
		while(i >= src.al) {
			j -= 4;
			if(j < 0) {
				j += BASE_SIZE;
				i--;
				if(i >= src.al){
					tmp = ((* i) >> j) % 16;
					c = (tmp < 10)?('0' + tmp):('A' + tmp - 10);
					out << c;
				}
			}
			else {
				tmp = ((* i) >> j) % 16;
				c = (tmp < 10)?('0' + tmp):('A' + tmp - 10);
				out << c;
			}
		}
	}
	return out;
}

std::istream& operator >> (std::istream & in, BigNumbers & src) {
	src.FreeMem();
	std::string tmp_str;
	in >> tmp_str;
	char *s = new char[tmp_str.size()+1];
	strcpy(s, tmp_str.c_str());
    		
	src = BigNumbers(s);

}

BigNumbers BigNumbers::operator + (const BigNumbers& b) const{
	BigNumbers res;
	D_BASE tmp = 0;
	int a_size = GetLen(), b_size = b.GetLen();
	if (a_size < b_size)
		res.AllocMem(b_size+1);
	else res.AllocMem(a_size+1);
	BASE *aw = al, *bw = b.al, *rw = res.al;
	for (; (aw <= ar) && (bw <= b.ar); aw++, bw++, rw++){
		tmp += *aw + *bw;
		*rw = (BASE)tmp;
		tmp >>= BASE_SIZE;
	}
	for (; aw <= ar; aw++, rw++){
		tmp += *aw;
		*rw = (BASE)tmp;
		tmp >>= BASE_SIZE;
	}
	for (; bw <= b.ar; bw++, rw++){
		tmp += *bw;
		*rw = (BASE)tmp;
		tmp >>= BASE_SIZE;
	}
	*rw = (BASE)tmp;
	if(*res.ah == 0)
		res.ar = res.ah-1;
		
	return res;
}

int BigNumbers::operator < (const BigNumbers& b) const{
	if(GetLen() < b.GetLen())
 		return 1;
	if(GetLen() > b.GetLen())
 		return 0;
	BASE *aw = ar, *bw = b.ar;
	for (; (aw >= al); aw--, bw--){
		if((*aw) < (*bw))
			return 1;
		else if ((*aw) > (*bw))
			return 0;
	}	
	return 0;
}

int BigNumbers::operator == (const BigNumbers& b) const{
	if((!(*this < b)) && (!(*this > b)))
 		return 1;
	return 0;
}

int BigNumbers::operator > (const BigNumbers& b) const{
	if (b < *this)
		return 1;
	return 0;

}
// BigNumbers a("1"), b, c;
//c = a - b;
BigNumbers BigNumbers::operator - (const BigNumbers& b) const{
	if((IsNull())||(b.IsNull()))
		throw "arguments error";
	if(*this < b)
		throw "sub error";
	BigNumbers result;
	if(*this == b) {
		result.AllocMem(1);
		return result;
	}
	result.AllocMem(GetLen());
	BASE *r = result.al;
	result.ar = r;
	int flag = 0;
	int res;
	BASE *aw = al, *bw = b.al;
	for(; bw <= b.ar; aw++, bw++, r++){
		res = *aw - *bw - flag;
		if(res < 0){
			flag = 1;
			*r = (BASE)((1 << BASE_SIZE) + res);
		}
		else{
			flag = 0;
			*r = res;
		}

		if (*r) result.ar = r;

	}
	for(; aw <= ar; aw++, r++){
		res = *aw - flag;
		if(res < 0){
			flag = 1;
			*r = (BASE)((1 << BASE_SIZE) + res);
		}
		else{
			flag = 0;
			*r = res;
		}
		if (*r) result.ar = r;
	}
	return result;
}


BigNumbers BigNumbers::MulBase(const BASE b) const{
	if(IsNull())
		throw "arguments error";
	BigNumbers result;
	D_BASE tmp = 0;
	result.AllocMem(GetLen()+1);
	BASE *aw = al, *r = result.al;
	for(;aw <= ar; aw++, r++){
		tmp = (tmp >> BASE_SIZE) + (*aw)*b;
		*r = (BASE)tmp;
	}
	tmp = (tmp >> BASE_SIZE);
	if (tmp) 
		*r = (BASE)tmp;
	else{
		while((!(*r)) && (r > result.al)) r--;
		result.ar = r;
	}
	return result;
}

BigNumbers BigNumbers::operator * (const BigNumbers& b) const{
	if((IsNull())||(b.IsNull()))
		throw "arguments error";
	BigNumbers result;
	int i, j;
	D_BASE tmp = 0;
	result.AllocMem(GetLen()+b.GetLen()+1);
	for (i = 0; i < GetLen(); i++){
		for (j = 0; j < b.GetLen(); j++){
			tmp = (tmp >> BASE_SIZE) + (*(al+i))*(*(b.al+j)) + *(result.al+j+i);
			*(result.al+i+j) = (BASE)tmp;
		}
		//std::cerr << *(result.al+i+j) << " " << (tmp >> BASE_SIZE) << std::endl;
		*(result.al+i+j) += (BASE)(tmp >> BASE_SIZE);		
		tmp = 0;
	}
	BASE * r = result.ah;
	for (; (!(*r)) && (r >= result.al); r--);
	result.ar = r;
	return result;
}

BigNumbers BigNumbers::DivBase(const BASE b, BASE * mod) const{
	if(IsNull())
		throw "arguments error";
	if(!b)
		throw "error: divide by 0";
	BigNumbers result;
	D_BASE tmp = 0;
	result.AllocMem(GetLen());
	for(int i = 0; i < GetLen(); i++){
		tmp = (tmp << BASE_SIZE) + *(ar-i); 
		*(result.ar-i) = tmp / b;
		tmp %= b;
	}
	BASE * r = result.ah;
	for (; (!(*r)) && (r >= result.al); r--);
	result.ar = r;
	*mod = tmp;
	return result;
}

void BigNumbers::Div(const BigNumbers& arg2, BigNumbers * result) const{
	if((IsNull())||(arg2.IsNull()))
		throw "arguments error";
// Если *this < arg 2, то рез-т определён.
	int n = arg2.GetLen(), m = GetLen() - n, i, j, neg_step4_result_flag = 0;

//D1

	D_BASE b = 1 << BASE_SIZE, tmp, tmp2;
	BASE qApp = 0, d;

	tmp = arg2.ar;
	tmp ++;
	d = b / tmp;

	// Считаем новое значение u, временно используя v
	BigNumbers u, v = MulBase(d), u1, v1, q;
	q.AllocMem(m + 1);// (q индексируется с id=0)

	if(v.GetLen() == n + m){ // если размер значения u не увеличился, записываем u со старшим разрядом 0 (u индексируется с id=0)
		u.AllocMem(n + m + 1);
		*(u.ar) = 0;
		for(i = 1; i <= m + n; i++) (*(u.al + i) = *(v.al + i - 1));
	}
	else u = v; // иначе просто переписываем значение в u

	// Считаем новое значение v
	v = arg2.MulBase(d);

//D2
	for(j = 0; j <= m; j++){// цикл по j

//D3
		if(*(u.ar - j) == *(v.ar)) qApp = b - 1; // если u_0[j] == v_1[1]
		else{
			tmp = *(u.ar - j);// u_0[j]
			tmp <<= BASE_SIZE;// u_0[j] * b
			tmp += *(u.ar - j - 1);// u_0[j] * b + u_0[j + 1]
			qApp = tmp / *(v.ar);// цел((u_0[j] * b + u_0[j + 1]) / v_1[1])
		}

		// проверка D3
		do{
			tmp = (*(v.ar));// v_1[1]
			tmp *= qApp;// q^ * v_1[1]

			tmp2 = *(u.ar - j);// u_0[j]
			tmp2 <<= BASE_SIZE;// u_0[j] * b
			tmp2 += *(u.ar - j - 1);// u_0[j] * b + u_0[j + 1]
			tmp2 -= tmp;// (u_0[j] * b + u_0[j + 1]) - (q^ * v_1[1])
			tmp2 <<= BASE_SIZE;// ((u_0[j] * b + u_0[j + 1]) - (q^ * v_1[1])) * b
			tmp2 += *(u.ar - j - 2);// ((u_0[j] * b + u_0[j + 1]) - (q^ * v_1[1])) * b + u_0[j + 2]

			tmp = (*(v.ar - 1));// v_1[2]
			tmp *= qApp;// v_1[2] * q^

			if(tmp > tmp2) qApp--;
		while(tmp > tmp2);

//D4
		u1.AllocMem(n);
		for(i = 0; i < n; i++){
			*(u1.ar - i) = *(u.ar - i - j);// копируем изменяемые значения
		}
		v1 = v.MulBase(qApp);

		if(u1 < v1) neg_step4_result_flag = 1;

		if(neg_step4_result_flag){// нужно сделать заём из старшего разряда
			u1.AllocMem(n + 1);
			*(u1.ar) = 1;
			for(i = 1; i <= n; i++){//<<-- переприсваиваем значения с >1< (не 0) разряда
				*(u1.ar - i) = *(u.ar - i - j - 1);// копируем изменяемые значения
			}
		}

		u1 = u1 - v1;

//D5
		*(q.ar - j) = qApp - neg_step4_result_flag;
		if(neg_step4_result_flag){// был сделан заём
//D6
			u1 = u1 + v1;
		}
		if(u1.GetLen() > n) for(i = 0; i < n; i++) *(u.ar - i - j) = *(u1.ar - i - 1);
		else for(i = 0; i < n; i++) *(u.ar - i - j) = *(u1.ar - i);
		neg_step4_result_flag = 0;
	}
}

void BigNumbers::p20(){
	al = NULL;
	ar = NULL;
	ah = NULL;
}

void BigNumbers::FreeMem(){
	if (al != NULL)	
		delete [] al;
	p20();	
}

void BigNumbers::AllocMem(int Len){
	if(Len > 0){
		al = new BASE[Len];
		ar = al + Len - 1;
		ah = ar;
		for(int i = 0; al + i <= ah; i++)
		{
			al[i] = 0;
		}
	}
	else p20();
}

bool BigNumbers::IsNull() const{
	return ((al == NULL)||(ar == NULL)||(ah == NULL));
}

