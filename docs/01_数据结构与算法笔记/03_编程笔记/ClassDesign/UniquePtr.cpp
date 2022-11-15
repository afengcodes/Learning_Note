/*
 * Filename: d:\学习笔记\01_数据结构与算法笔记\02_编程笔记\ClassDesign\UniquePtr.cpp
 * Path: d:\学习笔记
 * Created Date: Friday, September 9th 2022, 8:06:09 pm
 * Author: Afeng
 * 
 * Copyright (c) 2022 Afengcodes Inc.
 */

// ————————————————
// 版权声明：本文为CSDN博主「吃米饭」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
// 原文链接：https://blog.csdn.net/Cdreamfly/article/details/123338341



// 唯一指针
// 管理指针的存储，提供有限的垃圾回收工具，与内置指针相比几乎没有开销（取决于所使用的删除程序）。

// 这些对象具有获取指针所有权的能力：一旦它们获得所有权，它们就会通过在某个时候负责删除指向的对象来管理指向的对象。

// unique_ptr对象在它们本身被销毁时，或者一旦它们的值通过赋值操作或显式调用unique_ptr::reset而改变，就会自动删除它们所管理的对象（使用删除器）。

// unique_ptr对象唯一地拥有其指针：任何其他工具都不应负责删除该对象，因此任何其他托管指针都不应指向其托管对象，因为一旦它们必须这样做，unique_ptr对象就会删除其托管对象，而不考虑其他指针是否仍指向同一对象， 从而将指向那里的任何其他指针保留为指向无效位置。

// unique_ptr对象有两个组件：

// 存储的指针：指向它所管理的对象的指针。这是在构造时设置的，可以通过赋值操作或调用成员重置进行更改，并且可以单独访问以使用成员获取或释放进行读取。
// 存储的删除程序：一个可调用对象，它采用与存储指针类型相同的参数，并被调用以删除托管对象。它在施工时设置，可以通过分配操作进行更改，并且可以使用成员get_deleter单独访问。
// unique_ptr对象通过运算符 * 和 ->（对于单个对象）或运算符 []（对于数组对象）提供对其托管对象的访问来复制有限的指针功能。出于安全原因，它们不支持指针算术，仅支持移动分配（禁用复制分配）

// 成员函数


// 	（构造函数）构造unique_ptr（公共成员函数）
// 	（析构函数）销毁unique_ptr（公共成员函数）
// 	operator=	unique_ptr赋值（公共成员函数）
// 	get	获取指针（公共成员函数）
// 	get_deleter	获取存储的删除程序（公共成员函数）
// 	operator bool	检查是否不为空（公共成员函数）
// 	release	释放指针（公共成员函数）
// 	reset	重置指针（公共成员函数）
// 	swap	交换内容（公共成员函数）

// 非专用版本（单个对象）独占：
// 	operator*	取消引用对象（公共成员函数）
// 	operator->	取消引用对象成员（公共成员函数）

// 专用于具有运行时长度的数组的版本：
// 	operator[]	偏移访问（公共成员函数）

// 非成员函数重载
// 	swap	交换unique_ptr对象的内容（函数模板）
// 	relational operators	关系运算符 ==， ！=， <， <=， >， >= （函数模板）


#include <utility>
#include <stddef.h>

template <typename T>
class Unique_ptr
{
public:
    constexpr Unique_ptr() noexcept = default;
    constexpr Unique_ptr(nullptr_t) noexcept : Unique_ptr() {}
    explicit Unique_ptr(T *ptr) noexcept : ptr_{ptr} {}
    Unique_ptr(const Unique_ptr &) = delete; // unique_ptr的特性不允许拷贝
    Unique_ptr(Unique_ptr &&rhx) noexcept : ptr_{rhx.release()} {}

    ~Unique_ptr() noexcept
    {
        delete ptr_;
    }

    Unique_ptr &operator=(const Unique_ptr &) = delete; // unique_ptr的特性不允许拷贝
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
        return std::exchange(ptr_, nullptr); //返回当前指针指向地址，并置当前指针为空
    }
    T *get() const noexcept
    {
        return ptr_;
    }
    void reset(T *ptr) noexcept
    {
        delete std::exchange(ptr_, ptr); //释放当前指针指向地址内存并传入新的的地址内存
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
————————————————
版权声明：本文为CSDN博主「吃米饭」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/Cdreamfly/article/details/123338341