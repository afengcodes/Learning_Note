/*
RationalNumber a(1);
RationalNumber b(2,4);
auto c = a+b;
auto c = a.Add(b);
std::cout << a.ToString(); // 1
std::cout << b; // 1/2
std::cout << c; // 3/2
*/


// 美团二面

// - 设计有理数类
//     - 重载+
//     - 重载<<
//     - 注意分母为0 程序要处理异常啊 assert
//     - gcd函数

[C++有理数类设计](https://blog.csdn.net/weixin_43912687/article/details/110673715)



有理数类的功能就比C语言的有理数数据类型功能强多了，主要就是运算符重载。

有理数类，能实现以m/n形式的输入输出，支持加减乘除运算；
支持加赋，减赋，乘赋，除赋，判等，大于，小于运算；
也支持和普通double类型混合进行以上这些运算；
运算结果均已化简；
注意由于内部分子分母均是int型，乘除法容易溢出。
设计除法的除数为0时，会报错退出。


.h

```c++  
#ifndef RATIONAL_H
#define RATIONAL_H
#include <iostream>

using namespace std;

class Rational 
{
private:
   int fenzi, fenmu;
   void simply();//化简
   bool negative() const;//判断是否是负数
public:
   Rational(int numerator = 0, int denominator = 1);//构造函数
   Rational(const double n);//类型转换构造函数
   Rational(const Rational & ra);//复制构造函数
   //~Rational();没有动态分配，默认析构就行

   Rational & operator =(const Rational & ra);
   Rational & operator =(const double & n);//使有理数类对象能被double类型赋值

   //以下20个函数使有理数类对象能和double类型混合加减乘除和赋值
   Rational operator +(const Rational & ra);
   Rational operator -(const Rational & ra);
   Rational operator *(const Rational & ra);
   Rational operator /(const Rational & ra);
   Rational operator +(const double & n);
   Rational operator -(const double & n);
   Rational operator *(const double & n);
   Rational operator /(const double & n);
   friend Rational operator +(const double & n, const Rational & ra);
   friend Rational operator -(const double & n, const Rational & ra);
   friend Rational operator *(const double & n, const Rational & ra);
   friend Rational operator /(const double & n, const Rational & ra);
   Rational & operator +=(const Rational & ra);
   Rational & operator -=(const Rational & ra);
   Rational & operator *=(const Rational & ra);
   Rational & operator /=(const Rational & ra);
   Rational & operator +=(const double & n);
   Rational & operator -=(const double & n);
   Rational & operator *=(const double & n);
   Rational & operator /=(const double & n);

   int GetFenzi() { return fenzi;  }
   int GetFenmu() { return fenmu;  }
   void Set(int numerator, int denominator) { fenzi = numerator; fenmu = denominator; }

   //重载强制类型转换运算符，使得凡是应该出现double型的地方，均可以由有理数类对象替代。比如，可以使得
   //有理数类对象和普通double型比较大小成为可能
   operator double();

   //支持以m/n形式的有理数类对象输入输出,负号会显示在前面。
    friend ostream & operator <<(ostream & os, const Rational & ra);
    friend istream & operator >>(istream & is, Rational & ra);
};

#endif
```

.cpp

```c++  
#include <iostream>
#include <string>
#include <cstdlib>
#include "rational_oop.h"

using namespace std;

// 即
// int gcd(int a, int b)  // 欧几里得算法
// {
//     return b ? gcd(b, a % b) : a;
// }


void Rational::simply()//辗转相除法求最大公约数化简
{
    int a = fenzi, b = fenmu;

    while (b){
        int mod = a % b;
        a = b;
        b = mod;
    }
    
    fenzi /= a;
    fenmu /= a;
}

bool Rational::negative() const
{
    return (fenzi < 0 || fenmu < 0);
}

Rational::Rational(int numerator, int denominator)
{
    fenzi = numerator;
    fenmu = denominator;
    simply();//确保任意路径得到的有理数类对象均是最简形式
}

Rational::Rational(const double n)
{
    double numerator = n, denominator = 1;

    while ((int)numerator != numerator) {
        numerator *= 10;
        denominator *= 10;
    }

    fenzi = numerator, fenmu = denominator;
    simply();//确保任意路径得到的有理数类对象均是最简形式
}

Rational::Rational(const Rational & ra): fenzi(ra.fenzi), fenmu(ra.fenmu) {}

Rational & Rational::operator =(const Rational & ra)
{
    fenzi = ra.fenzi, fenmu = ra.fenmu;
    return *this;
}

Rational & Rational::operator =(const double & n)
{
    Rational tmp(n);
    *this = tmp;
    return *this;
}

Rational Rational::operator +(const Rational & ra)
{
    int numerator  = fenzi * ra.fenmu + ra.fenzi * fenmu;
    int denominator = fenmu * ra.fenmu;

    return Rational(numerator, denominator);
}

Rational Rational::operator -(const Rational & ra)
{
    int numerator  = fenzi * ra.fenmu - ra.fenzi * fenmu;
    int denominator = fenmu * ra.fenmu;

    return Rational(numerator, denominator);
}

Rational Rational::operator *(const Rational & ra)
{
    int numerator  = fenzi * ra.fenzi;
    int denominator = fenmu * ra.fenmu;

    return Rational(numerator, denominator);
}

Rational Rational::operator /(const Rational & ra)
{
    if (ra.fenzi == 0) {
        cout << "除数为0了！" << endl;
        exit(1);
    }

    int numerator  = fenzi * ra.fenmu;
    int denominator = fenmu * ra.fenzi;

    return Rational(numerator, denominator);
}

Rational Rational::operator +(const double & n)
{
    Rational tmp(n);
    return *this + tmp;
}

Rational Rational::operator -(const double & n)
{
    Rational tmp(n);
    return *this - tmp;
}

Rational Rational::operator *(const double & n)
{
    Rational tmp(n);
    return *this * tmp;
}

Rational Rational::operator /(const double & n)
{
    Rational tmp(n);
    return *this / tmp;
}

Rational operator +(const double & n, const Rational & ra)
{
    Rational tmp(n);
    return tmp + ra;
}

Rational operator -(const double & n, const Rational & ra)
{
    Rational tmp(n);
    return tmp - ra;
}

Rational operator *(const double & n, const Rational & ra)
{
    Rational tmp(n);
    return tmp * ra;
}

Rational operator /(const double & n, const Rational & ra)
{
    Rational tmp(n);
    return tmp / ra;
}

Rational & Rational::operator +=(const Rational & ra)
{
    Rational tmp =  *this + ra;
    *this = tmp;
    return *this;
}

Rational & Rational::operator -=(const Rational & ra)
{
    Rational tmp =  *this - ra;
    *this = tmp;
    return *this;
}

Rational & Rational::operator *=(const Rational & ra)
{
    Rational tmp = *this * ra;
    *this = tmp;
    return *this;
}

Rational & Rational::operator /=(const Rational & ra)
{
    Rational tmp =  *this / ra;
    *this = tmp;
    return *this;
}

Rational & Rational::operator +=(const double & ra)
{
    Rational tmp(ra);
    *this += tmp;
    return *this;
}

Rational & Rational::operator -=(const double & ra)
{
    Rational tmp(ra);
    *this -= tmp;
    return *this;
}

Rational & Rational::operator *=(const double & ra)
{
    Rational tmp(ra);
    *this *= tmp;
    return *this;
}

Rational & Rational::operator /=(const double & ra)
{
    Rational tmp(ra);
    *this /= tmp;
    return *this;
}

Rational::operator double()
{
    return (double) fenzi / fenmu;
}

ostream & operator <<(ostream & os, const Rational & ra)
{
    if (ra.fenzi == 0)
        os << 0;
    else if (ra.negative())
        os << "-" << abs(ra.fenzi) << "/" << abs(ra.fenmu); //如果是负数，符号写在前面。
    else
        os << ra.fenzi << "/" << ra.fenmu;
    return os;
}

istream & operator >>(istream & is, Rational & ra)
{
    string s;
    is >> s;

    int pos = s.find("/", 0);

    string sTmp = s.substr(0, pos);
    ra.fenzi = atoi(sTmp.c_str());//读入分子
    sTmp = s.substr(pos+1, s.length() - pos - 1);
    ra.fenmu = atoi(sTmp.c_str());//读入分母

    return is;
}

```


main.cpp

```c++  
#include <iostream>
#include "rational_oop.h"

using namespace std;

int main(void)
{
    cout << "该程序用来测试有理数类:" << endl;
    cout << "由于内部用int型装分子分母，所以输入的数不要太大，计算乘除容易溢出：" << endl << endl;

    Rational ra1, ra2(8.8);
    cout << "请以m/n的形式输入一个有理数ra1：";
    cin >> ra1;
    Rational ra3 = ra1;
    cout << "这里还用8.8为参数构造了ra2，用ra1为参数初始化ra3.现在：" << endl;
    cout << "ra1 = " << ra1 << " ra2 = " << ra2 << " ra3 = " << ra3 << ";" << endl << endl;

    ra1 = ra2;
    ra2 = 66.6;
    cout << "这里让ra1 = ra2, ra2 = 66.6。"<< "现在：" << endl;
    cout << "ra1 = " << ra1 << " ra2 = " << ra2 << " ra3 = " << ra3 << ";" << endl << endl;

    cout << "ra2 + ra3 = " << (ra2 + ra3) << endl;
    cout << "ra2 - ra3 = " << (ra2 - ra3) << endl;
    cout << "ra2 * ra3 = " << (ra2 * ra3) << endl;
    cout << "ra2 / ra3 = " << (ra2 / ra3) << endl << endl;

    cout << "ra2 + 34.55 = " << (ra2 + 34.55) << endl;
    cout << "ra2 - 7.9 = " << (ra2 - 7.9) << endl;
    cout << "ra2 * 3.6 = " << (ra2 * 3.6) << endl;
    cout << "ra2 / 2.5 = " << (ra2 / 2.5) << endl << endl;

    cout << "5.0 + ra2 = " << (5.0 + ra2) << endl;
    cout << "88.88 - ra2 = " << (88.88 - ra2) << endl;
    cout << "5.6 * ra2 = " << (5.6 * ra2) << endl;
    cout << "102.4 / ra2 = " << (102.4 / ra2) << endl << endl;

    cout << "现在" ;
    cout << "ra1 = " << ra1 << " ra2 = " << ra2 << " ra3 = " << ra3 << ";" << endl << endl;
    ra2 += 6.0;
    cout << "ra2 += 6.0, ra2 = " << ra2 << endl;
    ra2 -= 6.0;
    cout << "ra2 -= 6.0, ra2 = " << ra2 << endl;
    ra2 *= 6.0;
    cout << "ra2 *= 6.0, ra2 = " << ra2 << endl;
    ra2 /= 6.0;
    cout << "ra2 /= 6.0, ra2 = " << ra2 << endl<< endl;

    ra2 += ra3;
    cout << "ra2 += ra3, ra2 = " << ra2 << endl;
    ra2 -= ra3;
    cout << "ra2 -= ra3, ra2 = " << ra2 << endl;
    ra2 *= ra3;
    cout << "ra2 *= ra3, ra2 = " << ra2 << endl;
    ra2 /= ra3;
    cout << "ra2 /= ra3, ra2 = " << ra2 << endl << endl;

    cout << "现在" ;
    cout << "ra1 = " << ra1 << " ra2 = " << ra2 << " ra3 = " << ra3 << ";" << endl << endl;
    cout << "我现在double出一个变量n。赋初值为9.1;" << endl;
    double n = 9.1;
    n += ra2;
    cout << "n += ra2, n = " << n << endl;
    n -= ra2;
    cout << "n -= ra2, n = " << n << endl;
    n *= ra2;
    cout << "n *= ra2, n = " << n << endl;
    n /= ra2;
    cout << "n /= ra2, n = " << n << endl << endl;

    cout << "现在" ;
    cout << "ra1 = " << ra1 << " ra2 = " << ra2 << " ra3 = " << ra3 << ";" << endl << endl;

    cout << "ra1 > ra2, 结果为" << (ra1 > ra2) << endl;
    cout << "ra1 != ra2, 结果为" << (ra1 != ra2) << endl;
    cout << "ra1 == ra1, 结果为" << (ra1 == ra1) << endl;
    cout << "ra1 > 8, 结果为" << (ra1 > 8) << endl;
    cout << "88 > ra2, 结果为" << (88 > ra2) << endl;
    cout << "8.8 == ra1, 结果为" << (8.8 == ra1) << endl;

    return 0;
}
```