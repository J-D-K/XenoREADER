#include "XenoBuffer.h"

#include <stdlib.h>

void XenoBuffer_Free(XenoBuffer *buffer)
{
    if (!buffer) { return; }

    if (buffer->data) { free(buffer->data); }

    free(buffer);
}