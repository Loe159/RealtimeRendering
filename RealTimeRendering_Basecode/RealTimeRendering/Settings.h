#pragma once

#ifdef _WIN32
#  include <SDL.h>
#  include <SDL_image.h>
#else
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_image.h>
#endif

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#ifdef _MSC_VER
#  define INLINE inline
#else
#  define INLINE static inline
#endif

/// @brief Initialise la SDL.
/// @return EXIT_SUCCESS ou EXIT_FAILURE.
int Settings_InitSDL();

/// @brief Quitte la SDL.
void Settings_QuitSDL();

#ifdef _MSC_VER
#  define strdup(str) _strdup(str)
#else
INLINE void strcpy_s(char *dest, int destSize, const char *src)
{
    strcpy(dest, src);
}

INLINE void fopen_s(FILE **pFile, const char *filename, const char *mode)
{
    *pFile = fopen(filename, mode);
}

INLINE void strcat_s(char *strDestination, size_t numberOfElements, const char *strSource)
{
    strcat(strDestination, strSource);
}

INLINE char *strtok_s(char *str, const char *delimiters, char **context)
{
    return strtok_r(str, delimiters, context);
}
#endif
