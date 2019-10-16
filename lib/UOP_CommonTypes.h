#ifndef __UOP_COMMON__
#define __UOP_COMMON__
typedef enum
{
    UOP_OK = 0U,
    UOP_RUNNING = 1U,
    UOP_BUSY = 2U,
    UOP_ERROR = 3U,
    UOP_WRITE_TIMEOUT = 4U,
    UOP_READ_TIMEOUT = 5U,
    UOP_CHECKSUM_ERROR = 6U,
} UOP_StateType;
#endif