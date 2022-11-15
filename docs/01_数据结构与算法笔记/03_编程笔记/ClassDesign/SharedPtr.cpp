/*
 * Filename: d:\ѧϰ�ʼ�\01_���ݽṹ���㷨�ʼ�\02_��̱ʼ�\ClassDesign\SharedPtr.cpp
 * Path: d:\ѧϰ�ʼ�
 * Created Date: Monday, September 5th 2022, 5:35:31 pm
 * Author: Afeng
 * 
 * Copyright (c) 2022 Afengcodes Inc.
 */

// reference :  https://blog.csdn.net/zsiming/article/details/125542007

һ��ʵ��ʱ��һЩ��Ҫ���ǵĵ�
1.���ڹ��캯�������Ƿ�����shared_ptr��Դ���룬������explicit�ؼ��֣���ֹԭʼָ����ʽ��ת��Ϊ����ָ�롣
2.�����ָ�����ʹ��Ĭ�Ϲ��캯��ʱ�����ü���Ϊ0��
3.������ֵ������Ҫ����Ը�ֵ�������
4.�ͷŶ�̬�ڴ�ʱʱ��ע������ָ���ǲ��ǿ�ָ�롣


��������ָ��shared_ptrʵ��



#include <iostream>
#include <memory>

using namespace std;

template<typename T>
class my_shared_ptr {
private:
	size_t* _useCnt;  
	T* _ptr;

public:
	// Ĭ�Ϲ��캯��
	my_shared_ptr() : _ptr(new T), _useCnt(new size_t(0)) {}

	// ���캯��
	explicit my_shared_ptr(T* p) : _ptr(p),  _useCnt(new size_t){
		if (p) {
			*_useCnt = 1;
		} else {
			*_useCnt = 0;
		}
	}

	// �������캯��
	my_shared_ptr(const my_shared_ptr& rhs) {
		_ptr = rhs._ptr; // ָ��ͬһ���ڴ�
		_useCnt = rhs._useCnt;
		(*_useCnt)++; // ���ü�������	
	}

	// ������ֵ������ע�����Ը�ֵ
	my_shared_ptr& operator= (const my_shared_ptr& rhs) {
		if (this != &rhs) {			// ����Ը�ֵ
			if (_ptr) {				// ��ֹ����ԭ��ָ���ָ�룬_useCnt>0˵��ȷʵ��ָ��ʵ���
				--(*_useCnt);		// �Լ�Ҫ��rhs�����ˣ����ü�����1
				// ���û�����ü����ˣ����Լ���գ��ͷŶ�̬�ڴ�
				if (*_useCnt == 0) {
					delete _useCnt;
					delete _ptr;
				}
				// ���Լ����rhs, ���ü�����һ
				_useCnt = rhs._useCnt;
				_ptr = rhs._ptr;
				++(*_useCnt);
			}
		}
		return *this;
	}


	// �õ�����
	int use_count() {
		return *_useCnt;
	}
	
	~my_shared_ptr() {
		if (!_ptr || (*_useCnt == 1)) { // ֻ�е�ǰָ��ָ��ջ���ֻ��һ��ָ�����ʱ�ͷ��ڴ�
			delete _useCnt;
			delete _ptr;
		} else {  // ����ֻ�������ü���
			--(*_useCnt);
		}
	}
};


int main() {

	// ��鹹�캯���Ƿ���������
	my_shared_ptr<int> p1(new int(1));
	cout << "p1 cnt:" << p1.use_count() << endl;       // 1

	// ��鿽�������Ƿ���������
	{
		my_shared_ptr<int> p2(p1);
		cout << "p2 cnt:" << p2.use_count() << endl;   // 2
		cout << "p1 cnt:" << p1.use_count() << endl;   // 2
	}
	
	// ������ü����Ƿ���������
	cout << "p1 cnt:" << p1.use_count() << endl;      // 1

	// ���Ĭ�ϳ�ʼ��ʱ�����Ƿ�����
	my_shared_ptr<int> p4;
	cout << "p4 cnt:" << p4.use_count() << endl;      // 0

	// ��鿽����ֵ�Ƿ���������
	p4 = p1;
	cout << "p1 cnt:" << p1.use_count() << " p4 cnt:" << p4.use_count() << endl;  // 2

	return 0;
}
