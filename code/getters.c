#include "types.h"
Enclave SM_SECTION get_enclave(SM* sm, int id){
        for(int i =0; i<sm->enclave_idx;i++){
            if(sm->enclaves[i].id == id){
                return sm->enclaves[i];
            }
        }
        return sm->enclave_error;
    }

Buffer SM_SECTION get_buffer(SM* sm,int id){
        for(int i =0; i<sm->buffer_idx;i++){
            if(sm->buffers[i].id == id){
                return sm->buffers[i];
            }
        }
        return sm->buffer_error;
    }

SP SM_SECTION get_sp(SM* sm,int id){
        for(int i =0; i<sm->SP_list_idx;i++){
            if(sm->SP_list[i].id == id){
                return sm->SP_list[i];
            }
        }
        return sm->sp_error;
    }