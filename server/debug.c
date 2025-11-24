
#include <stdarg.h>
#include <stdlib.h>

#include "debug.h"

/* die on a fatal error */
void fatal_error( const char *err, ... )
{
    va_list args;

    va_start( args, err );
    fprintf( stderr, "wineserver: " );
    vfprintf( stderr, err, args );
    va_end( args );
    exit(1);
}

void _trace(const char *file, int line, const char *func, const char *fmt, ...)
{
    va_list args;
    va_start( args, fmt);
    fprintf( stderr, "[%s:%d:%s] ", file, line, func );
    vfprintf( stderr, fmt, args);
    va_end( args );
}

void _trace_raw(const char *fmt, ...)
{
    va_list args;
    va_start( args, fmt);
    vfprintf( stderr, fmt, args);
    va_end( args );
}
