
#include <stdarg.h>
#include <stdlib.h>

#include "debug.h"

#ifdef HAVE_LTTNG
#include "lttng-server-tp.h"
#endif

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
    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);
    char buf[1+vsnprintf(NULL, 0, fmt, args1)];
    va_end(args1);
    vsnprintf(buf, sizeof buf, fmt, args2);
    va_end(args2);
#ifdef HAVE_LTTNG
    lttng_ust_tracepoint(wine, server, "server", buf);
#endif
    fprintf( stderr, "[%s:%d:%s %s] ", file, line, func, buf);
}

void _trace_raw(const char *fmt, ...)
{
    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);
    char buf[1+vsnprintf(NULL, 0, fmt, args1)];
    va_end(args1);
    vsnprintf(buf, sizeof buf, fmt, args2);
    va_end(args2);

#ifdef HAVE_LTTNG
    lttng_ust_tracepoint(wine, server, "server", buf);
#endif

    fprintf( stderr, "%s", buf);
}
