#ifndef E
#define E COPTL_ERR
#endif
#ifndef W
#define W COPTL_WARN
#endif
#ifndef I
#define I COPTL_INFO
#endif

#ifdef __cplusplus

#define COPTL_F std::string("")
#ifndef F
#define F std::string("")
#endif

#define COPTL_F_DBG coptl_concatDbgInfoCpp(__func__, __LINE__, __FILE__)
#ifndef F_DBG
#define F_DBG coptl_concatDbgInfoCpp(__func__, __LINE__, __FILE__)
#endif

#define COPTL_F_DBG_C coptl_concatDbgInfoC(__func__, __LINE__, __FILE__)
#ifndef F_DBG_C 
#define F_DBG_C coptl_concatDbgInfoC(__func__, __LINE__, __FILE__)
#endif

#else

#define COPTL_F ""
#ifndef F
#define F ""
#endif

#define COPTL_F_DBG coptl_concatDbgInfoC(__func__, __LINE__, __FILE__)
#ifndef F_DBG
#define F_DBG coptl_concatDbgInfoC(__func__, __LINE__, __FILE__)
#endif

#endif

#ifndef logger
#define logger coptl_logger
#endif

