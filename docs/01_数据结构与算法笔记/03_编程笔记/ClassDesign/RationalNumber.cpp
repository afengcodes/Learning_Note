
/*
RationalNumber a(1);
RationalNumber b(2,4);
auto c = a+b;
auto c = a.Add(b);
std::cout << a.ToString(); // 1
std::cout << b; // 1/2
std::cout << c; // 3/2
*/


// ���Ŷ���

// - �����������
//     - ����+
//     - ����<<
//     - ע���ĸΪ0 ����Ҫ�����쳣�� assert
//     - gcd����


// [C/C++ assert()�����÷��ܽ�](https://www.cnblogs.com/lvchaoshun/p/7816288.html)
// [��Ͷ���-C++���������](https://blog.csdn.net/lyxawdn/article/details/123502824)
// [������ C++ ���������](https://blog.csdn.net/qq_27278957/article/details/85269245?spm=1001.2101.3001.6650.4&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EOPENSEARCH%7ERate-4.pc_relevant_default&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EOPENSEARCH%7ERate-4.pc_relevant_default&utm_relevant_index=9)

/******************************************************  assert   ****************************************/
// assert���ԭ�Ͷ�����<assert.h>�У�����������������������ش�������ֹ����ִ�С�

// ԭ�Ͷ��壺
// #include <assert.h>
// void assert( int expression );
// ��assert���������ȼ�����ʽ expression �������ֵΪ�٣���Ϊ0����
// ��ô������stderr��ӡһ��������Ϣ��Ȼ��ͨ������ abort ����ֹ�������С�
// 2. �÷��ܽ���ע�����

// ��1���ں�����ʼ�����鴫������ĺϷ����磺

// int resetBufferSize(int nNewSize)
// {
//����:�ı仺������С,
//����:nNewSize �������³���
//����ֵ:��������ǰ���� 
//˵��:����ԭ��Ϣ���ݲ���     nNewSize<=0��ʾ���������
// assert(nNewSize >= 0);
// assert(nNewSize <= MAX_BUFFER_SIZE);
// ...
// }
// ���ƴ���


// 2ÿ��assertֻ����һ����������Ϊͬʱ����������ʱ��
// �������ʧ�ܣ��޷�ֱ�۵��ж����ĸ�����ʧ�ܣ��磺
// ���ã�
// assert(nOffset>=0 && nOffset+nSize<=m_nInfomationSize);
// �ã�

// assert(nOffset >= 0);
// assert(nOffset+nSize <= m_nInfomationSize);
// ��3������ʹ�øı价������䣬��Ϊassertֻ��DEBUG����Ч��
// �����ô������ʹ�ó�������������ʱ�������⣬�磺

// ��������

// assert(i++ < 100);
// ����������Ϊ�������������ִ��֮ǰi=100��
// ��ô�������Ͳ���ִ�У���ôi++���������û��ִ�С�

// ������ȷ��

//  assert(i < 100);
//  i++;
// ��4��assert�ͺ�������Ӧ��һ�У����γ��߼����Ӿ��ϵ�һ�¸С�

// ��5���еĵط���assert���ܴ����������ˡ�

//     assert�����������Զ��׼��Ĵ���ģ������Ǵ����쳣�ġ�
//     ������쳣�ǲ�һ���ģ������ǲ�Ӧ�ó��ֵģ��쳣�ǲ��ɱ���ġ�
//     c�����쳣����ͨ�������ж����������������и��Ե��쳣������ơ�

//     һ���ǳ��򵥵�ʹ��assert�Ĺ��ɾ��ǣ��ڷ������ߺ������ʼʹ�ã�
//     ����ڷ������м�ʹ������Ҫ���ؿ����Ƿ���Ӧ�õġ�
//     �������ʼ��û��ʼһ�����ܹ��̣���һ�����ܹ���ִ���г��ֵ����⼸�������쳣��



/******************************************************  + / << ������   ****************************************/


ostream & operator<< (ostream &cout , A &p)
{
	cout<<"m_A = "<<p.m_A <<"m_B = "<<p.m_B;
	return cout;
}


//ǰ�õ���
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

// ���õ���
MyInteger operator++(int)
{
	MyInteger temp = *this;
	m_Num++;
	return temp;
}






// ����ʱ�İ汾!��!����!!!!!!!    

// ��Ҫѧϰgcd  assert   
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