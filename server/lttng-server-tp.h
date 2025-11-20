
#undef LTTNG_UST_TRACEPOINT_PROVIDER
#define LTTNG_UST_TRACEPOINT_PROVIDER wine

#undef LTTNG_UST_TRACEPOINT_INCLUDE
#define LTTNG_UST_TRACEPOINT_INCLUDE "./lttng-server-tp.h"

#if !defined(LTTNG_SERVER_TP_H) || defined(LTTNG_UST_TRACEPOINT_HEADER_MULTI_READ)
#define LTTNG_SERVER_TP_H

#include <lttng/tracepoint.h>

/* Wineserver events */
LTTNG_UST_TRACEPOINT_EVENT(
    wine,
    server,
    LTTNG_UST_TP_ARGS(
        const char *, event,
        const char *, details
    ),
    LTTNG_UST_TP_FIELDS(
        ctf_string(event, event)
        ctf_string(details, details)
    )
)

#endif /* LTTNG_SERVER_TP_H */

#include <lttng/tracepoint-event.h>
