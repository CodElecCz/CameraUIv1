#pragma once

#ifdef CLVLIB_EXPORTS
#define CLV_API __declspec(dllexport)
#else
#define CLV_API __declspec(dllimport)
#endif