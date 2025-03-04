#pragma once

#ifndef MRK_SINGLETON
#define MRK_SINGLETON(x) friend class Mrk::Singleton<x>;
#endif // !MRK_SINGLETON

#ifndef MRK_INSTANCE_REF
#define MRK_INSTANCE_REF auto& instance = Instance()
#endif // !MRK_INSTANCE_REF


namespace Mrk
{
    template<typename T>
    class Singleton
    {
    public:
        Singleton(T&&) = delete;
        Singleton(const T&) = delete;
        void operator= (const T&) = delete;
        T* operator &() = delete;
        ~Singleton() = default;

    protected:
        Singleton() = default;
        static inline T& Instance()
        {
            static T instance;
            return instance;
        }
    };
}