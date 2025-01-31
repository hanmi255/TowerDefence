#pragma once
// 
/**
 * @brief 通用单例模式管理器模板类
 * @tparam T 需要实现单例模式的具体类型
 *
 * 该类提供了一个线程不安全的单例模式实现。
 * 使用时将具体类型作为模板参数传入即可获得单例功能。
 */
template <typename T>
class Manager
{
public:
    /**
     * @brief 获取单例实例
     * @return T* 返回类型T的单例指针
     *
     * 如果单例未创建则创建新实例，否则返回已存在的实例
     */
    static T* instance()
    {
        if (!manager)
            manager = new T();
        return manager;
    }

private:
    static T* manager;                               // 静态成员变量，存储单例实例指针

protected:
    Manager() = default;                             // 默认构造函数，protected防止外部创建实例
    ~Manager() = default;                            // 默认析构函数
    Manager(const Manager&) = delete;                // 删除拷贝构造函数，确保单例
    Manager& operator=(const Manager&) = delete;     // 删除赋值运算符，确保单例
};

/**
 * @brief 静态成员变量初始化
 */
template <typename T>
T* Manager<T>::manager = nullptr;
