
#ifdef _SINGLETON_FUNC
    _SINGLETON_FUNC redefined
#endif
#define _SINGLETON_FUNC(DEC, T, name, init) \
    DEC static T& name() { \
        DEC static T i init; \
        return i; }
#ifdef CONST_SINGLETON_FUNC
    CONST_SINGLETON_FUNC redefined
#endif
#define CONST_SINGLETON_FUNC(T, name, init) _SINGLETON_FUNC(const, T, name, init)
#ifdef CONST_DEFAULT_SINGLETON_FUNC
    CONST_DEFAULT_SINGLETON_FUNC redefined
#endif
#define CONST_DEFAULT_SINGLETON_FUNC(T, name) CONST_SINGLETON_FUNC(T, name,)
#ifdef SINGLETON_FUNC
    SINGLETON_FUNC redefined
#endif
#define SINGLETON_FUNC(T, name, init) _SINGLETON_FUNC(, T, name, init)
#ifdef DEFAULT_SINGLETON_FUNC
    DEFAULT_SINGLETON_FUNC redefined
#endif
#define DEFAULT_SINGLETON_FUNC(T, name) SINGLETON_FUNC(T, name,)

#ifdef CONST_STATIC_STRING
    CONST_STATIC_STRING redefined
#endif
#define CONST_STATIC_STRING(name, value) CONST_SINGLETON_FUNC(std::string, name, (#value))
#define CONST_STATIC_STRING_EXP(name, value) CONST_SINGLETON_FUNC(std::string, name, (value))

#ifdef _SINGLETON
    _SINGLETON redefined
#endif
#define _SINGLETON(DEC, T) \
    private: \
        T(const T&) = delete; \
        void operator=(const T&) = delete; \
    public: \
        _SINGLETON_FUNC(DEC, T, instance,);

#ifdef CONST_SINGLETON
    CONST_SINGLETON redefined
#endif
#define CONST_SINGLETON(T) _SINGLETON(const, T)
#ifdef SINGLETON
    SINGLETON redefined
#endif
#define SINGLETON(T) _SINGLETON(, T)

