#include "isotp.h"
#include "uart_stack.h"
#include "utils.h"


uint16_t get_frame_len(isotp_frame_t *frame)
{
    switch(frame->PCI)
    {
        case ISOTP_FRAME_TYPE_SF:
            return frame->data_len + 2;
        case ISOTP_FRAME_TYPE_FF:
            return ISOTP_MAX_FF_DATA_LENGTH + 2;
        case ISOTP_FRAME_TYPE_CF:
            return ISOTP_MAX_FF_DATA_LENGTH + 2;
        default:
            return 0;
    }
}
