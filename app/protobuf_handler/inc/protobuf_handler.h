

#ifndef PROTOBUF_HANDLER_H
#define PROTOBUF_HANDLER_H
#include "my_msgs.pb.h"

#include "common.h"

void process_protobuf_message(uint8_t * msg , uint8_t len);
void process_rx_msg();


#endif /* PROTOBUF_HANDLER_H */