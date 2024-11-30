extern void zero_mem(void * start, void * end);
#include "types.h"
#include "memory.h"
#include "lib.h"
void  SM_SECTION delete_enclave_buffer(SM* sm,int enclave_id){
        int disp = 0;
        for(int i=0;i<sm->enclave_buffer_idx;i++){
            if(sm->enclave_buffer[i].enclave_id == enclave_id){disp++;}
            else{sm->enclave_buffer[i-disp] = sm->enclave_buffer[i];}
        }
        sm->enclave_buffer_idx -=disp;
}
    int SM_SECTION delete_enclave(SM* sm,int id){
        int i=0;
        for(; i<sm->enclave_idx;i++){if(sm->enclaves[i].id == id){break;}}
        if(i == sm->enclave_idx){return -1;}
        for(;i+1<sm->enclave_idx;i++){ sm->enclaves[i] = sm->enclaves[i+1];}
        sm->enclave_idx--;
    }
    int SM_SECTION  delete_buffer(SM* sm,int id){
        int i=0;
        Buffer b_todel;
        for(; i<sm->buffer_idx;i++){
            if(sm->buffers[i].id == id){
                b_todel = sm->buffers[i];
            break;
            }
        }
        if(i == sm->buffer_idx){return -1;}
        for(;i+1<sm->buffer_idx;i++){ sm->buffers[i] = sm->buffers[i+1];}
        sm->buffer_idx--;
        free_mem(sm,b_todel.start,b_todel.end);
    }