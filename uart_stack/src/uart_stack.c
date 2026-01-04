


#include "common.h"
#include "uart_stack.h"
#include "uart.h"
#include "utils.h"
my_uart_t uart_inst;
bool uart_stack_init(uart_stack_config_t *config, queue_t *queue)
{
    if(config)
    {
        uart_init(config->uart,config->baudrate,true);
        uart_inst.port = config->uart;
        uart_inst.queue =queue;
        uart_inst.recv_active_q = NULL;
        uart_inst.recv_done_q = NULL;
        uart_inst.send_active_q = NULL;
        uart_inst.send_free_q = NULL;
        uart_inst.recv_free_q = NULL;
        for(int i=0;i < RECV_Q_SIZE - 1; i++)
        {
            uart_inst.recv_ctxs[i].next = &uart_inst.recv_ctxs[i+1];
        }
        uart_inst.recv_ctxs[RECV_Q_SIZE - 1].next = NULL;
        uart_inst.recv_free_q = &uart_inst.recv_ctxs[0];

        for(int i=0;i < SEND_Q_SIZE - 1; i++)
        {
            uart_inst.send_ctxs[i].next = &uart_inst.send_ctxs[i+1];
        }
        uart_inst.send_ctxs[SEND_Q_SIZE - 1].next = NULL;
        uart_inst.send_free_q = &uart_inst.send_ctxs[0];
        return 1;

    }
    return 0;
}


void send_isotp_frame(isotp_frame_t *frame)
{
    uint16_t total_frame_len = get_frame_len(frame);
    uint8_t buffer[sizeof(uart_packet_t)+ total_frame_len];
    uart_packet_t *packet = (uart_packet_t *)buffer;
    packet->SOE = 0x7E;
    packet->header = 0x12;
    packet->crc = 0x34;
    packet->payload_len = total_frame_len;
    packet->payload[0] = frame->PCI;
    packet->payload[1] = frame->data_len;
    if(frame->PCI == ISOTP_FRAME_TYPE_SF)
        mycopy(packet->payload + 2,frame->data,frame->data_len);
    else
        mycopy(packet->payload + 2,frame->data,ISOTP_MAX_CF_DATA_LENGTH); 
    uart_send_data(uart_inst.port , (uint8_t *)packet , sizeof(buffer));


}


static void recv_rx(my_uart_t *inst)
{
    uint8_t buffer[MAX_UART_PKT_SIZE];
    uart_packet_t *pkt = (uart_packet_t *)buffer;
    if(queue_dequeue(inst->queue , (uint8_t *)pkt) != 0)
    {
       return;
    }

    if(pkt->SOE != 0x7E)
    {
        printf("Corrupted pkt");
        return;
    }


    // printf("Found valid packet in Q\n");
    isotp_frame_t frame;
    frame.PCI = pkt->payload[0];
    frame.data_len = pkt->payload[1];
    frame.data = pkt->payload + 2;
    isotp_ctx_t *ctx;
    if(frame.PCI == ISOTP_FRAME_TYPE_SF || frame.PCI == ISOTP_FRAME_TYPE_FF)
    {
        // printf("SF/FF\n");
        ctx = GET_HEAD(inst->recv_free_q);
        
    }
    else{
        // printf("CF\n");

        ctx = GET_HEAD(inst->recv_active_q);
    }

    if(ctx == NULL){
        printf("No memory available to revcieve\n");
        return;
    }
    switch(frame.PCI)
    {
        case ISOTP_FRAME_TYPE_SF:{
            if(frame.data_len > ISOTP_MAX_SF_DATA_LENGTH)
            {
                printf("Invalid single frame length . Got %d\n",frame.data_len);
                break;
            }
            ctx->isotp_state = ISOTP_STATE_RX_DONE;
            ctx->recv_len = frame.data_len;
            ctx->data_len = frame.data_len;
            mycopy(ctx->data,frame.data,ctx->data_len);
            REMOVE_HEAD(inst->recv_free_q);
            ctx->next = NULL;
            ADD_TO_TAIL(inst->recv_done_q,ctx);
            // printf("SF\n");
            break;
        }

        case ISOTP_FRAME_TYPE_FF:{
            if(frame.data_len > MAX_DATA_SIZE)
            {
                printf("Cant handle more than %d bytes of data",MAX_DATA_SIZE);
                break;
            }
            // uint16_t remaining = ctx->data_len - ctx->recv_len;
            // uint16_t to_copy = (remaining < ISOTP_MAX_CF_DATA_LENGTH)
            //        ? remaining
            //        : ISOTP_MAX_CF_DATA_LENGTH;
            ctx->isotp_state = ISOTP_STATE_WAIT_FOR_CF;
            ctx->recv_len = ISOTP_MAX_FF_DATA_LENGTH;
            ctx->data_len = frame.data_len;
            mycopy(ctx->data,frame.data,ISOTP_MAX_FF_DATA_LENGTH);
            REMOVE_HEAD(inst->recv_free_q);
            ctx->next = NULL;
            ADD_TO_TAIL(inst->recv_active_q,ctx);
            // isotp_frame_t fc;
            // fc.PCI = ISOTP_FRAME_TYPE_FC;
            // fc.data_len = 1;
            // uint8_t data[]={0xFC};
            // fc.data = data;
            // send_isotp_frame(&fc);
            // printf("FF\n");
            break;
        }

        case ISOTP_FRAME_TYPE_CF:{
            if(ctx->isotp_state == ISOTP_STATE_WAIT_FOR_CF)
            {
                // printf("Ctx Rx Len %d\n",ctx->recv_len);
                mycopy(ctx->data+ctx->recv_len,frame.data,ISOTP_MAX_CF_DATA_LENGTH);
                ctx->recv_len += ISOTP_MAX_CF_DATA_LENGTH;
                // printf("CF %d\n",frame.seq_num);
                if(ctx->recv_len >= ctx->data_len)
                {
                    ctx->isotp_state = ISOTP_STATE_RX_DONE;
                    // printf("All frames recieved\n");
                    REMOVE_HEAD(inst->recv_active_q);
                    ctx->next = NULL;
                    ADD_TO_TAIL(inst->recv_done_q,ctx);
                }
                else
                {
                    ctx->isotp_state = ISOTP_STATE_WAIT_FOR_CF;
                    
                }
            }
            else{
                printf("Invalid frame\n");
            }
            break;
        }

        case ISOTP_FRAME_TYPE_FC:{
    
            break;
        }

       




    }

}


static void send_tx(my_uart_t *inst)
{
    isotp_ctx_t *ctx = GET_HEAD(inst->send_active_q);
    if(ctx == NULL) return;
    switch(ctx->isotp_state)
    {
        case ISOTP_STATE_TX_PENDING:{
            isotp_frame_t  frame;
            if(ctx->data_len > ISOTP_MAX_SF_DATA_LENGTH)
            {
                frame.PCI = ISOTP_FRAME_TYPE_FF;
                frame.data_len = ctx->data_len;
                frame.data = ctx->data;
                ctx->isotp_state = ISOTP_STATE_WAIT_FOR_FC;
                ctx->sent_len = ISOTP_MAX_FF_DATA_LENGTH;
                // printf("Sent FF\n");

            }
            else
            {
                frame.PCI = ISOTP_FRAME_TYPE_SF;
                frame.data_len = ctx->data_len;
                frame.data = ctx->data;
                ctx->isotp_state = ISOTP_STATE_TX_DONE;
                ctx->sent_len = ctx->data_len;
                // printf("Sent SF\n");

            }
            
            send_isotp_frame(&frame);
            break;
        }
        case ISOTP_STATE_WAIT_FOR_FC:{
            bool fc_recieved=true;
            if(fc_recieved)
            {
                ctx->isotp_state = ISOTP_STATE_SEND_CF;
            }
            break;
        }
        case ISOTP_STATE_SEND_CF:{
            static int i=1;
            if(i < 3)
            {
                isotp_frame_t frame;
                frame.PCI = ISOTP_FRAME_TYPE_CF;
                frame.seq_num = i;
                frame.data = ctx->data + ctx->sent_len;
                
                ctx->sent_len += ISOTP_MAX_FF_DATA_LENGTH;
                send_isotp_frame(&frame);
                
                // printf("Sent CF %d \n",i);
                i++;
                if(ctx->sent_len >= ctx->data_len)
                {
                    ctx->isotp_state = ISOTP_STATE_TX_DONE;
                    i=1;
                    // printf("Sent all CFs\n");

                }
                
            }
            else{
                ctx->isotp_state = ISOTP_STATE_WAIT_FOR_FC;
                i=1;
            }
            break;
        }
        case ISOTP_STATE_TX_DONE:{
            REMOVE_HEAD(inst->send_active_q);
            ctx->next = NULL;
            ADD_TO_TAIL(inst->send_free_q,ctx);
            // printf("Memory freed\n");
            break;
        }
        default:
            break;
    }

    
}

void uart_stack_task()
{
    recv_rx(&uart_inst);
    send_tx(&uart_inst);
}



int uart_stack_send_msg(uint8_t *data,uint8_t len)
{
    isotp_ctx_t *ctx = GET_HEAD(uart_inst.send_free_q);
    if(ctx != NULL)
    {
      
        ctx->isotp_state = ISOTP_STATE_TX_PENDING;
        mycopy(ctx->data,data,len);
        ctx->data_len = len;
        ctx->sent_len = 0;
        // Add it to send_active_q
        REMOVE_HEAD(uart_inst.send_free_q);
        ADD_TO_TAIL(uart_inst.send_active_q,ctx);
        ctx->next = NULL;
        // printf("Added msg\n");
        
    }
    else{
        printf("No memory\n");
    }
    return 0;
}


int uart_stack_get_msg(uint8_t *dest_buff,uint8_t *msg_len)
{
    isotp_ctx_t *ctx = GET_HEAD(uart_inst.recv_done_q);
    if(ctx)
    {
        mycopy(dest_buff,ctx->data,ctx->data_len);
        *msg_len = ctx->data_len;
        REMOVE_HEAD(uart_inst.recv_done_q);
        ctx->next=NULL;
        ADD_TO_TAIL(uart_inst.recv_free_q,ctx);
        return 0;
    }
    return -1;
}

