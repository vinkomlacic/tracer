#pragma once
/**
 * Logging levels and logging helper macros.
 * Consult documentation on how to use this.
 */

#include "console.h"

#ifdef LOG_ALL
#define TRACE_ENABLE
#define DEBUG_ENABLE
#define INFO_ENABLE
#define WARN_ENABLE
#endif

#ifdef TRACE_ENABLE
#define TRACE(...) trace(__BASE_FILE__, __LINE__, __VA_ARGS__)
#else
#define TRACE(...)
#endif

#ifdef DEBUG_ENABLE
#define DEBUG(...) debug(__BASE_FILE__, __LINE__, __VA_ARGS__)
#else
#define DEBUG(...)
#endif

#ifdef INFO_ENABLE
#define INFO(...) info(__BASE_FILE__, __LINE__, __VA_ARGS__)
#else
#define INFO(...)
#endif

#ifdef WARN_ENABLE
#define WARN(...) warn(__BASE_FILE__, __LINE__, __VA_ARGS__)
#else
#define WARN(...)
#endif

#define ERROR(...) error(__BASE_FILE__, __LINE__, __VA_ARGS__)
