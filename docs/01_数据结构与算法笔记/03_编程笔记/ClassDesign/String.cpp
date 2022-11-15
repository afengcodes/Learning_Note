/*
 * Filename: d:\ѧϰ�ʼ�\01_���ݽṹ���㷨�ʼ�\02_��̱ʼ�\ClassDesign\String.cpp
 * Path: d:\ѧϰ�ʼ�
 * Created Date: Sunday, September 4th 2022, 12:47:09 pm
 * Author: Afeng
 * 
 * Copyright (c) 2022 Afengcodes Inc.
 */
�̶����


// 
#include<iostream>
#include<string.h>
using namespace std;
class String {
public:
 String(const char* str) {
  this->mstr = new char[strlen(str) + 1];
  strcpy_s(this->mstr, strlen(str) + 1, str);
  this->m_size = strlen(mstr);
  cout << "���ù��캯��" << endl;
 }
 String(const String& str) {
  if (this->mstr != NULL)
  {
   delete[] mstr;
   this->mstr = NULL;
  }
  mstr = new char[strlen(str.mstr) + 1];
  strcpy_s(this->mstr, strlen(str.mstr) + 1, str.mstr);
  this->m_size = strlen(mstr);
  cout << "���ÿ������캯��" << endl;
 }
 ~String()
 {
  if (this->mstr != NULL)
  {
   delete[] mstr;
   this->mstr = NULL;
  }
  cout << "������������" << endl;
 
 }
 friend ostream& operator<<(ostream& out, String& str);
 friend istream& operator>>(istream& in, String& str);
 String operator+(String &str)
 {
  
  int newsize = this->m_size + str.m_size + 1;
  char *temp = new char[newsize];
  memset(temp, 0, newsize);
  strcat(temp, this->mstr);
  strcat(temp, str.mstr);
  String newstring(temp);
  delete[] temp;
  return newstring;
 }
 String& operator+(const char* str)
 {
 /* char* tem = new char[this->m_size + strlen(str) + 1];
  memset(tem, 0, this->m_size + strlen(str) + 1);
  strcat(tem, this->mstr);
  strcat(tem, str);
  String mystring(tem);
  delete[] tem;
  return mystring;*/
  int newsize = this->m_size + strlen(str) + 1;
  char *temp = new char[newsize];
  memset(temp, 0, newsize);
  strcat(temp, this->mstr);
  strcat(temp, str);
  static String newstring(temp);
  if (temp != NULL)
  {
   delete[] temp;
   temp = NULL;
  }
  delete[] temp;
  return newstring;
  
 }
 bool operator==(String &str)
 {
  if (strcmp(this->mstr, str.mstr) == 0 && this->m_size == str.m_size)
  {
   return true;
  }
  return false;
 }
 
 bool operator==(const char* str)
 {
  if (strcmp(this->mstr, str) == 0 && this->m_size == strlen(str))
  {
   return true;
  }
  return false;
 }
 char& operator[](int index)
 {
 
  return this->mstr[index];
 
 }
 String& operator=(const char* str)
 {
  if (this->mstr != NULL)
  {
   delete[] this->mstr;
   this->mstr = NULL;
  }
  this->mstr = new char[strlen(str) + 1];
  strcpy_s(mstr, strlen(str) + 1, str);
  return *this;
 }
 String& operator=(String& str)
 {
  if (this->mstr != NULL)
  {
   delete[] this->mstr;
   this->mstr = NULL;
  }
  this->mstr = new char[strlen(str.mstr) + 1];
  strcpy_s(mstr, strlen(str.mstr) + 1, str.mstr);
  return *this;
 }
 
private:
 char* mstr;
 int m_size;
};
ostream& operator<<(ostream& out, String& str)
{
 out << str.mstr << endl;
 return out;
}
istream& operator>>(istream& in, String& str)
{
 if (str.mstr != NULL)
 {
  delete[] str.mstr;
  str.mstr = NULL;
 }
 char tem[1024];
 in >> tem;
 str.mstr = new char[strlen(tem) + 1];
 strcpy_s(str.mstr, strlen(tem) + 1, tem);
 str.m_size = strlen(str.mstr);
 return in;
}
void test()
{
 String str("hello World");
 cout << str << endl;
 //cout << "������ MyString �����ַ�����" << endl;
 //cin >> str;
 //cout << "�ַ���Ϊ�� " << str << endl;
 //����[]
 cout << "MyString �ĵ�һ���ַ�Ϊ��" << str[0] << endl;
 //���� =
 String str2 = "^_^";
 String str3 = "";
 str3 = "aaaa";
 str3 = str2;
 cout << "str2 = " << str2 << endl;
 cout << "str3 = " << str3 << endl;
 //���� +
 String str4 = "�Ұ�";
 String str5 = "�����";
 String str6 = str4 + str5;
 cout << str6 << endl;
 String str7 = str4 + "�찲��";
 cout << str7 << endl;
 ���� ==
 if (str6 == str7)
 {
  cout << "s6 �� s7 ���" << endl;
 }
 else
 {
  cout << "s6 �� s7 �����" << endl;
 }
}
int main()
{
 test();
 return 0;
}


// ��ϸ�汾
.h�ļ�
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <assert.h>
#include <string.h>
using std::ostream;
using std::istream;
namespace stringss
{
class string{
	friend ostream& operator<<(ostream& _cout, const stringss::string& s);
	friend istream& operator>>(istream& _cin, stringss::string& s);
public:
		typedef char* iterator;
		string(const char* str = "");
		string(const string& s);
		string& operator=(const string& s);
		~string();
 
		// iterator
		iterator begin();
		iterator end();
		// modify
 
		void push_back(char c);
		string& operator+=(char c);
		void append(const char* str);
		string& operator+=(const char* str);
		void clear();
		void swap(string& s);
		const char* c_str()const;
 
		// capacity
 
		size_t size()const;
		size_t capacity()const;
		bool empty()const;
		void resize(size_t n, char c = '\0');
		void reserve(size_t n);
 
		// access
 
		char& operator[](size_t index);
		const char& operator[](size_t index)const;
		// ����c��string�е�һ�γ��ֵ�λ��
		size_t find(char c, size_t pos = 0) const;
		// �����Ӵ�s��string�е�һ�γ��ֵ�λ��
		size_t find(const char* s, size_t pos = 0) const;
		// ��posλ���ϲ����ַ�c/�ַ���str�������ظ��ַ���λ��
    	string& insert(size_t pos, char c);
		string& insert(size_t pos, const char* str);
		// ɾ��posλ���ϵ�Ԫ�أ������ظ�Ԫ�ص���һ��λ��
		string& erase(size_t pos, size_t len = -1);
private:
	char* _str;
	size_t _size;
	size_t _capacity;
	};
	//relational operators
	bool operator<(const string & s1, const string & s2);
	bool operator<=(const string& s1, const string& s2);
	bool operator>(const string& s1, const string& s2);
	bool operator>=(const string& s1, const string& s2);
	bool operator==(const string& s1, const string& s2);
	bool operator!=(const string& s1, const string& s2);
};



.cpp�ļ�
#include "stringss.h"
namespace stringss {
 
	//���캯��
	string::string(const char* str)
		:_size(strlen(str))
		, _capacity(_size)
	{
		_str = new char[_capacity + 1];
		strcpy(_str, str);
	}
 
	//�������캯��
	string::string(const string& s)
		:_str(nullptr)
		, _size(0)
		, _capacity(_size)
	{
		string temp(s._str);
		swap(temp);
	}
	//����=���� �뿽�����첻ͬ���ǣ�����������ǹ��캯��������s1 = s2,s1���Ѿ����ڵġ�
	string& string::operator=(const string& s) {
		string temp(s);
		swap(temp);
		return *this;
	}
	//��������
	string::~string() {
		delete[] _str;
		_size = 0;
		_capacity = 0;
	}
	//������
	string::iterator string::begin() {
		return _str;
	}
	string::iterator string::end() {
		return _str + _size;
	}
	//modify
	void string::push_back(char c) {
		insert(_size - 1, c);
	}
	string& string::operator+=(char c) {
		push_back(c);
		return *this;
	}
	void string::append(const char* str) {
		insert(_size - 1, str);
	}
	string& string::operator+=(const char* str) {
		append(str);
		return *this;
	}
	void string::swap(string& s) {
		std::swap(_str, s._str);
		std::swap(_size, s._size);
		std::swap(_capacity, s._capacity);
	}
	const char* string::c_str()const {
		return _str;
	}
	//capacity
	size_t string::size()const {
		return _size;
	}
	size_t string::capacity()const {
		return _capacity;
	}
	bool string::empty()const {
		return _size == 0;
	}
	void string::resize(size_t n, char c) {
		if (n < _size) {
			memset(_str, c, n);
		}
		else {
			if (n > _capacity) {
				reserve(n);
			}
			memset(_str, c, n);
			_size = n;
			_str[_size] = '\0';
		}
	}
	void string::reserve(size_t n) {
		if (n > _capacity) {
			char* tmp = new char[n + 1];
			strcpy(tmp, _str);
			delete[] _str;
			_str = tmp;
			_capacity = n;
		}
	}
	//access
	char& string::operator[](size_t index) {
		assert(index < _size);
		return _str[index];
	}
	const 	char& string::operator[](size_t index)const {
		assert(index < _size);
		return _str[index];
	}
	//relational operators
	bool operator<(const string& s1, const string& s2)
	{
		return strcmp(s1.c_str(), s2.c_str()) < 0;
	}
 
	bool operator<=(const string& s1, const string& s2)
	{
		return s1 < s2 || s1 == s2;
	}
 
	bool operator>(const string& s1, const string& s2)
	{
		return !(s1 <= s2);
	}
 
	bool operator>=(const string& s1, const string& s2)
	{
		return s1 > s2 || s1 == s2;
	}
 
	bool operator==(const string& s1, const string& s2)
	{
		return strcmp(s1.c_str(), s2.c_str()) == 0;
	}
 
	bool operator!=(const string& s1, const string& s2)
	{
		return !(s1 == s2);
	}
	// ����c��string�е�һ�γ��ֵ�λ��
 
	size_t string::find(char c, size_t pos)const {
		assert(pos < _size);
		while (pos < _size) {
			if (_str[pos] == c) {
				return pos;
			}
			pos++;
		}
		return -1;
	}
 
	// �����Ӵ�s��string�е�һ�γ��ֵ�λ��
	size_t string::find(const  char* s, size_t pos)const {
		assert(pos < _size);
		size_t index = pos;
		size_t start = pos;
		while (index < _size) {
			if (_str[index] == s[0]) {
				start = index;
				size_t i = 0;
				while (start < _size && i < strlen(s)) {
					if (_str[index] == s[i]) {
						start++;
						i++;
					}
					else {
						break;
					}
				}
				if (i == strlen(s)) {
					return index;
				}
			}
			index++;
		}
		return -1;
	}
	// ��posλ���ϲ����ַ�c/�ַ���str�������ظ��ַ���λ��
	string& string::insert(size_t pos, char c){
		assert(pos <= _size);
		if (_size == _capacity){
			reserve(_capacity == 0 ? 4 : 2 * _capacity);
		}
		size_t end = _size + 1;//��ֹ�±�Խ��
		while (end > pos + 1){
			_str[end] = _str[end - 1];
			end--;
		}
		_str[end] = c;
		_size++;
		return *this;
	}
 
	string& string::insert(size_t pos, const char* str){
		assert(pos <= _size);
		int len = strlen(str);
		if (_size + len > _capacity){
			reserve(_size + len);
		}
		size_t end = _size + len;
		while (end > pos + len){
			_str[end] = _str[end - len];
			end--;
		}
		strncpy(_str + pos + 1, str, len);
		_size += len;
		return *this;
	}
 
	// ɾ��posλ���ϵ�Ԫ�أ������ظ�Ԫ�ص���һ��λ��
	string& string::erase(size_t pos, size_t len){
		assert(pos < _size);
		if (len == -1 || len + pos >= _size){
			_str[pos] = '\0';
			_size = pos;
		}
		else{
			for (int i = pos + len; i <= _size; i++)
			{
				_str[i - len] = _str[i];
			}
		}
		_size -= len;
		return *this;
	}
	ostream& operator<<(ostream& _cout, const stringss::string& s){
		for (size_t i = 0; i < s._size; i++){
			_cout << s[i];
		}
		return _cout;
	}
 
	istream& operator>>(istream& _cin, stringss::string& s){
		s.clear();
		char ch = _cin.get();
		while (ch != ' ' && ch != '\n'){
			s += ch;
			ch = _cin.get();
		}
		return _cin;
	}

��������������������������������
��Ȩ����������ΪCSDN��������֪�������֡���ԭ�����£���ѭCC 4.0 BY-SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
ԭ�����ӣ�https://blog.csdn.net/qeqe1212/article/details/124542679


















// �ֳ�д�Ľ��
```c++  

#include <cstring>

using namespace std;

class String{
private:
  size_t size;
  char* str;
public:
  String(const char* s){
    this->str = new char[strlen(s)];
    memcpy(str, s, sizeof s);
    this->size = strlen(this->str);
  }
  // ��ֵ
  String& operator = (const char* s){
    if(str != nullptr){
      delete[] str;
      this->size = 0;
    }

    this->str = new char[strlen(s)];
    memcpy(str, rhs,rhs.size);
    this->size = str.size();
    
    return 
  }

  // ����
  String(const String& rhs){
    if(str != nullptr){
      delete[] str;
      this->size = 0;
    }
    this->str = new char[rhs.size];
    memcpy(str, rhs,rhs.size);
    this->size = str.size();
  }
  
  ~String(){
    delete[] str;
    size = 0;
  }
  
};
```