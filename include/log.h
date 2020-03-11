#pragma once
/**
 * Logging levels and logging helper macros.
 * Consult documentation on how to use this.
 */

#include <stdio.h>

#ifdef LOG_ALL
#define TRACE_ENABLE
#define DEBUG_ENABLE
#define INFO_ENABLE
#define WARN_ENABLE
#endif

#ifdef TRACE_ENABLE
#define TRACE(...) printf("%-6s [%-20s:%-4d]  ", "TRACE", __BASE_FILE__, __LINE__); printf(__VA_ARGS__); printf("\n")
#else
#define TRACE(...)
#endif

#ifdef DEBUG_ENABLE
#define DEBUG(...) printf("%-6s [%-20s:%-4d]  ", "DEBUG", __BASE_FILE__, __LINE__); printf(__VA_ARGS__); printf("\n")
#else
#define DEBUG(...)
#endif

#ifdef INFO_ENABLE
#define INFO(...) printf("%-6s [%-20s:%-4d]  ", "INFO", __BASE_FILE__, __LINE__); printf(__VA_ARGS__); printf("\n")
#else
#define INFO(...)
#endif

#ifdef WARN_ENABLE
#define WARN(...) printf("%-6s [%-20s:%-4d]  ", "WARN", __BASE_FILE__, __LINE__); printf(__VA_ARGS__); printf("\n")
#else
#define WARN(...)
#endif

#define ERROR(...) fprintf(stderr, "ERROR  "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")

#define FATAL(...) fprintf(stderr, "FATAL  "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")