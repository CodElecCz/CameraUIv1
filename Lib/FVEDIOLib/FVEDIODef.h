#pragma once

#ifdef FVEDIOLIB_EXPORTS
#define FVEDIO_API __declspec(dllexport)
#else
#define FVEDIO_API __declspec(dllimport)
#endif