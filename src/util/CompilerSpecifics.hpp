#ifndef YADAW_SRC_UTIL_COMPILERSPECIFICS
#define YADAW_SRC_UTIL_COMPILERSPECIFICS

#ifdef(_MSC_VER)
#define ALWAYS_INLINE __force_inline
#elifdef(__GNUC__)
#define ALWAYS_INLINE __attribute__(always_inline)
#else
#define ALWAYS_INLINE
#warning "Macro ALWAYS_INLINE does NOT do what it implies right now."
#endif

#endif //YADAW_SRC_UTIL_COMPILERSPECIFICS
