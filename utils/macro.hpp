
#ifdef __PASTE_RAW
    __PASTE_RAW redefined
#endif
#define __PASTE_RAW(a, b) a ## b

#ifdef PASTE_RAW
    PASTE_RAW redefined
#endif
#define PASTE_RAW(a, b) __PASTE_RAW(a, b)

#ifdef random_variable_name
    random_variable_name redefined
#endif
#define random_variable_name(x) PASTE_RAW(x, __LINE__)

