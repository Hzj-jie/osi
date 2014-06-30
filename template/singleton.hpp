
#ifdef _SINGLETON
    _SINGLETON redefined
#endif
#define _SINGLETON(DEC, T) \
    private: \
        T(const T&); \
        void operator=(const T&); \
    public: \
        static DEC T& instance() { \
            static DEC T i; \
            return i; }

#ifdef CONST_SINGLETON
    CONST_SINGLETON redefined
#endif
#define CONST_SINGLETON(T) _SINGLETON(const, T)
#ifdef SINGLETON
    SINGLETON redefined
#endif
#define SINGLETON(T) _SINGLETON(, T)

