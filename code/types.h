#include "const.h"
/*
    This includes all the types required to run Spock and the constants via const.h
    const.h sets the const apart but only this file types.h should ever be imported by any other file.
*/
#ifndef SM_H
#define SM_H
    // types
    typedef struct {
        int id;
        void * start;
        void * end;
    } Buffer;

    typedef struct {
        int id;
        int text_id;
        int data_id;
        uint8_t key [KEY_LEN]; // K_{N,SP,SM} sm is software module here in sancus terms
    } Enclave;
    
    typedef struct 
    {
        int rwx;
        int hash;
    } Perm;
    
    typedef struct
    {   
        int enclave_id;
        int buffer_id;
        int is_owner; //bool
        Perm perm;
    } BufferEnclave;

    typedef struct 
    {
        uint8_t id;
        uint8_t key [KEY_LEN]; // K_{N,SP}
    }SP;

    typedef struct 
    {
        void* start;
        void* end;
    } Space;
    
    typedef struct 
    {
        int nb;
        void * addr;
    } Arg;
    
typedef struct
{
    int is_initialized;
    int enclave_id_c;
    int buffer_id_c;
    int SP_id_c;
    int enclave_idx;
    int buffer_idx;
    int enclave_buffer_idx;
    int SP_list_idx;
    // crypto
    uint8_t K_N[KEY_LEN];
    // lists
    Enclave enclaves [MAX_ENCLAVES];
    BufferEnclave enclave_buffer[MAX_ENCLAVE_BUFFER];
    SP SP_list[MAX_SP_LIST];
    Space free_list[SPACE_LIST_MAX];
    Buffer  buffers  [MAX_BUFFERS];
    // error
    Enclave enclave_error;
    Buffer buffer_error;
    SP sp_error;
} SM;
#endif