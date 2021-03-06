#pragma once
#include"../engine/Config.hpp"
#include<iostream>
#include<typeinfo>
#pragma warning(disable : 4127)
#undef max
#undef min
//Configures
#if  defined(_MSC_VER)
#define VC_COMPILER
#endif

//Macro functions
#ifdef VC_COMPILER
#define MULTI_LINE_MACRO_BEGIN do {  
#define MULTI_LINE_MACRO_END \
} while (false)
#else
#define MULTI_LINE_MACRO_BEGIN do {  
#define MULTI_LINE_MACRO_END } while (false)
#endif

#define CODE_FILENAME __FILE__
#define CODE_LINENUM __LINE__

#define aSAFE_RELEASE(x) \
MULTI_LINE_MACRO_BEGIN\
	if (!!(x)){ (x)->Release(); \
	aDBG(CODE_FILENAME << '(' << CODE_LINENUM << ") : \n\'" << typeid(x).name() << "\' Object address at[" << (x) << "] released."); \
	(x) = nullptr;}\
MULTI_LINE_MACRO_END

#define aSAFE_DELETE(x) \
MULTI_LINE_MACRO_BEGIN\
	if (!!x){\
		delete (x);\
		aDBG(CODE_FILENAME << '(' << CODE_LINENUM << ") : \n\'" << typeid(x).name() << "\' Object address at[" << unsigned int(x) << "] deleted."); \
		(x) = nullptr;\
	}\
MULTI_LINE_MACRO_END

#define aSAFE_DELETE_ARRAY(x) \
MULTI_LINE_MACRO_BEGIN\
	if (!!x){\
		delete [] (x); \
		aDBG(CODE_FILENAME << '(' << CODE_LINENUM << ") : \n\'" << typeid(x).name() << "\' Object address at[" << unsigned int(x) << "] deleted."); \
		(x) = nullptr; \
	}\
MULTI_LINE_MACRO_END


#define aRETURN_ON_FAIL(x) if(FAILED(x)){ return E_FAIL;}
#define aOffsetof(s,x) ((std::size_t)&reinterpret_cast<const volatile char&>(((s*)nullptr)->x))
#define aRADIAN_TO_ANGLE(x) (x * 180.0 / (aPI))
#define aANGLE_TO_RADIAN(x) (x * aPI / 180.0)

//Debug
#ifdef OPEN_DEBUG_OPTION
#define aDBG_OLD_LINE(x) \
MULTI_LINE_MACRO_BEGIN\
	std::cout << "" << x << " "; \
MULTI_LINE_MACRO_END
#define aDBG_NEW_LINE(x) \
MULTI_LINE_MACRO_BEGIN\
	std::cout << "" << x << std::endl;\
MULTI_LINE_MACRO_END
#define aDBG(x)  aDBG_NEW_LINE(x)
#else
#define aDBG_OLD_LINE(x)
#define aDBG_NEW_LINE(x)
#define aDBG(x)
#endif
#define aTHROW_ON(expr,exception,msg) do{throw exception(msg);}while(0)

//Language
#define aNOEXCEPT  throw()
#define aCONSTEXPR 
#define aSTL_OUT_OF_RANGE(index,container)  (((index) >= (container).size())||((index) < 0))

//Common literals
#define aPI 3.141592653589793
