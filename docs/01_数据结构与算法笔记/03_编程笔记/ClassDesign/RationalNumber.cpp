
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


// [C/C++ assert()函数用法总结](https://www.cnblogs.com/lvchaoshun/p/7816288.html)
// [类和对象-C++运算符重载](https://blog.csdn.net/lyxawdn/article/details/123502824)
// [★★★★★ C++ 运算符重载](https://blog.csdn.net/qq_27278957/article/details/85269245?spm=1001.2101.3001.6650.4&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EOPENSEARCH%7ERate-4.pc_relevant_default&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EOPENSEARCH%7ERate-4.pc_relevant_default&utm_relevant_index=9)

/******************************************************  assert   ****************************************/
// assert宏的原型定义在<assert.h>中，其作用是如果它的条件返回错误，则终止程序执行。

// 原型定义：
// #include <assert.h>
// void assert( int expression );
// 　assert的作用是先计算表达式 expression ，如果其值为假（即为0），
// 那么它先向stderr打印一条出错信息，然后通过调用 abort 来终止程序运行。
// 2. 用法总结与注意事项：

// 　1）在函数开始处检验传入参数的合法性如：

// int resetBufferSize(int nNewSize)
// {
//功能:改变缓冲区大小,
//参数:nNewSize 缓冲区新长度
//返回值:缓冲区当前长度 
//说明:保持原信息内容不变     nNewSize<=0表示清除缓冲区
// assert(nNewSize >= 0);
// assert(nNewSize <= MAX_BUFFER_SIZE);
// ...
// }
// 复制代码


// 2每个assert只检验一个条件，因为同时检验多个条件时，
// 如果断言失败，无法直观的判断是哪个条件失败，如：
// 不好：
// assert(nOffset>=0 && nOffset+nSize<=m_nInfomationSize);
// 好：

// assert(nOffset >= 0);
// assert(nOffset+nSize <= m_nInfomationSize);
// 　3）不能使用改变环境的语句，因为assert只在DEBUG个生效，
// 如果这么做，会使用程序在真正运行时遇到问题，如：

// 　　错误：

// assert(i++ < 100);
// 　　这是因为如果出错，比如在执行之前i=100，
// 那么这条语句就不会执行，那么i++这条命令就没有执行。

// 　　正确：

//  assert(i < 100);
//  i++;
// 　4）assert和后面的语句应空一行，以形成逻辑和视觉上的一致感。

// 　5）有的地方，assert不能代替条件过滤。

//     assert是用来避免显而易见的错误的，而不是处理异常的。
//     错误和异常是不一样的，错误是不应该出现的，异常是不可避免的。
//     c语言异常可以通过条件判断来处理，其它语言有各自的异常处理机制。

//     一个非常简单的使用assert的规律就是，在方法或者函数的最开始使用，
//     如果在方法的中间使用则需要慎重考虑是否是应该的。
//     方法的最开始还没开始一个功能过程，在一个功能过程执行中出现的问题几乎都是异常。



/******************************************************  + / << 等重载   ****************************************/


ostream & operator<< (ostream &cout , A &p)
{
	cout<<"m_A = "<<p.m_A <<"m_B = "<<p.m_B;
	return cout;
}


//前置递增
MyInteger& operator++()
{
	m_Num++;
	return *this;
}
void test01()
{
	MyInteger myint;
	cout << myint++ << endl;
}

// 后置递增
MyInteger operator++(int)
{
	MyInteger temp = *this;
	m_Num++;
	return temp;
}






// 面试时的版本!！!！！!!!!!!!    

// 需要学习gcd  assert   
#include <iostream>
// #include <
using namespace std;
int gcd(int a, int b){
    return b ? gcd(b, a % b) : a;
}

class RationalNumber {
public:
    RationalNumber(int a):fz(a),fm(1){}
    RationalNumber(int a, int b):fz(a), fm(b){
        if(b == 0) {
            cout << " Error " << endl;
        }
    }
    void add(const RationalNumber& rhs){
        this->fz = this->fz * rhs.fm + this->fm * rhs.fz;
        this->fm = this->fm * rhs.fm;
        diff();
    }
    RationalNumber Add(const RationalNumber& rhs){
        RationalNumber rn(1);
        rn.fz = this->fz * rhs.fm + this->fm * rhs.fz;
        rn.fm = this->fm * rhs.fm;
        rn.diff();
        return rn;
    }
    void ToString(){
        diff();
        if(this->fm == 1) cout << this->fz << endl;
        else{
            cout << this->fz << "/" << this->fm << endl;
        }
    }
    void diff(){
        int g = gcd(this->fz, this->fm);
//         cout <<" g" << g << endl;
        this->fz /= g;
        this->fm /= g;
    }
    
private:
    int fz, fm; 
};

// RationalNumber& operator + (const RationalNumber& lhs,const RationalNumber& rhs){
        
//  }


#include <iostream>
using namespace std;
int main() {
    //int a;
    //cin >> a;
    RationalNumber test(1);
    
    RationalNumber test_b(2,4);
    test_b.ToString();
    test.add(test_b); 
    test.ToString();
    
    RationalNumber c = test.Add(test_b);
    c.ToString();
    // test
    RationalNumber test_1(0);
    RationalNumber test_2(10,3);
    RationalNumber test_3(1,9);
    test_1 = test_2.Add(test_3);
    test_1.ToString();
    RationalNumber test_zero(2,0);
}