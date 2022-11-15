/*
 * Filename: d:\ѧϰ�ʼ�\01_���ݽṹ���㷨�ʼ�\02_��̱ʼ�\ClassDesign\UniquePtr.cpp
 * Path: d:\ѧϰ�ʼ�
 * Created Date: Friday, September 9th 2022, 8:06:09 pm
 * Author: Afeng
 * 
 * Copyright (c) 2022 Afengcodes Inc.
 */

// ��������������������������������
// ��Ȩ����������ΪCSDN���������׷�����ԭ�����£���ѭCC 4.0 BY-SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
// ԭ�����ӣ�https://blog.csdn.net/Cdreamfly/article/details/123338341



// Ψһָ��
// ����ָ��Ĵ洢���ṩ���޵��������չ��ߣ�������ָ����ȼ���û�п�����ȡ������ʹ�õ�ɾ�����򣩡�

// ��Щ������л�ȡָ������Ȩ��������һ�����ǻ������Ȩ�����Ǿͻ�ͨ����ĳ��ʱ����ɾ��ָ��Ķ���������ָ��Ķ���

// unique_ptr���������Ǳ�������ʱ������һ�����ǵ�ֵͨ����ֵ��������ʽ����unique_ptr::reset���ı䣬�ͻ��Զ�ɾ������������Ķ���ʹ��ɾ��������

// unique_ptr����Ψһ��ӵ����ָ�룺�κ��������߶���Ӧ����ɾ���ö�������κ������й�ָ�붼��Ӧָ�����йܶ�����Ϊһ�����Ǳ�����������unique_ptr����ͻ�ɾ�����йܶ��󣬶�����������ָ���Ƿ���ָ��ͬһ���� �Ӷ���ָ��������κ�����ָ�뱣��Ϊָ����Чλ�á�

// unique_ptr���������������

// �洢��ָ�룺ָ����������Ķ����ָ�롣�����ڹ���ʱ���õģ�����ͨ����ֵ��������ó�Ա���ý��и��ģ����ҿ��Ե���������ʹ�ó�Ա��ȡ���ͷŽ��ж�ȡ��
// �洢��ɾ������һ���ɵ��ö�����������洢ָ��������ͬ�Ĳ���������������ɾ���йܶ�������ʩ��ʱ���ã�����ͨ������������и��ģ����ҿ���ʹ�ó�Աget_deleter�������ʡ�
// unique_ptr����ͨ������� * �� ->�����ڵ������󣩻������ []��������������ṩ�����йܶ���ķ������������޵�ָ�빦�ܡ����ڰ�ȫԭ�����ǲ�֧��ָ����������֧���ƶ����䣨���ø��Ʒ��䣩

// ��Ա����


// 	�����캯��������unique_ptr��������Ա������
// 	����������������unique_ptr��������Ա������
// 	operator=	unique_ptr��ֵ��������Ա������
// 	get	��ȡָ�루������Ա������
// 	get_deleter	��ȡ�洢��ɾ�����򣨹�����Ա������
// 	operator bool	����Ƿ�Ϊ�գ�������Ա������
// 	release	�ͷ�ָ�루������Ա������
// 	reset	����ָ�루������Ա������
// 	swap	�������ݣ�������Ա������

// ��ר�ð汾���������󣩶�ռ��
// 	operator*	ȡ�����ö��󣨹�����Ա������
// 	operator->	ȡ�����ö����Ա��������Ա������

// ר���ھ�������ʱ���ȵ�����İ汾��
// 	operator[]	ƫ�Ʒ��ʣ�������Ա������

// �ǳ�Ա��������
// 	swap	����unique_ptr��������ݣ�����ģ�壩
// 	relational operators	��ϵ����� ==�� ��=�� <�� <=�� >�� >= ������ģ�壩


#include <utility>
#include <stddef.h>

template <typename T>
class Unique_ptr
{
public:
    constexpr Unique_ptr() noexcept = default;
    constexpr Unique_ptr(nullptr_t) noexcept : Unique_ptr() {}
    explicit Unique_ptr(T *ptr) noexcept : ptr_{ptr} {}
    Unique_ptr(const Unique_ptr &) = delete; // unique_ptr�����Բ�������
    Unique_ptr(Unique_ptr &&rhx) noexcept : ptr_{rhx.release()} {}

    ~Unique_ptr() noexcept
    {
        delete ptr_;
    }

    Unique_ptr &operator=(const Unique_ptr &) = delete; // unique_ptr�����Բ�������
    constexpr Unique_ptr &operator=(nullptr_t)
    {
        this->reset();
        return *this;
    }
    Unique_ptr &operator=(Unique_ptr &&rhx) noexcept
    {
        this->reset(rhx.release());
        return *this;
    }

    T *release() noexcept
    {
        return std::exchange(ptr_, nullptr); //���ص�ǰָ��ָ���ַ�����õ�ǰָ��Ϊ��
    }
    T *get() const noexcept
    {
        return ptr_;
    }
    void reset(T *ptr) noexcept
    {
        delete std::exchange(ptr_, ptr); //�ͷŵ�ǰָ��ָ���ַ�ڴ沢�����µĵĵ�ַ�ڴ�
    }
    void swap(Unique_ptr &rhx) noexcept
    {
        std::swap(ptr_, rhx.ptr_);
    }

    T &operator*() const
    {
        return *ptr_;
    }
    T *operator->() const noexcept
    {
        return ptr_;
    }
    operator bool() const noexcept
    {
        return static_cast<bool>(ptr_);
    }

private:
    T *ptr_{nullptr};
};

template <typename T, typename... Args>
auto make_Unique(Args &&...args)
{
    return Unique_ptr<T>{new T(std::forward(args)...)};
}
#include <vector>

int main()
{
    Unique_ptr<std::vector<int>> ptr = make_Unique<std::vector<int>>();
    return 0;
}
��������������������������������
��Ȩ����������ΪCSDN���������׷�����ԭ�����£���ѭCC 4.0 BY-SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
ԭ�����ӣ�https://blog.csdn.net/Cdreamfly/article/details/123338341