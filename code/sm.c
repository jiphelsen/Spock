#include "types.h"
#include "enclave.h"
#include "getters.h"
#include "delete.h"
#include "memory.h"
#include "lib.h"
#include "hacl-c/Hacl_HMAC_SHA2_256.h"


// extern
extern int r_enclave(void * text_start,void * text_end,void*data_start, void * data_end,int id,int perms,int arg1,int arg2);
extern int r_untrusted();
extern void enclave1(int*test);
extern void enclave2(int*test);
extern void untrusted_boot();
extern void set_untrusted_pmp();
extern int  get_resp_byte(int nb);
extern void set_resp_byte(int nb, int byte);
extern void zero_mem(void * start, void * end);



int  SM_SECTION sm_call(int call_number, int enclave_id,Arg regsa2a3,Arg regsa4a5, Arg arga6a7,Arg arg1,Arg arg2, Arg arg3, Arg arg4, Arg arg5, Arg arg6) {
    
    static SM sm;

    // aead :
    // encrypt : Key, Message, Data, -1 => Cypher, MAC
    // decrypt : Key, Cypher, Data, MAC => Message
    int* aead(int key, void * M_start, int M_size,void * D_start, int D_size,int MAC){return M_start;};
    int mac(int key, void * D_start, int D_size){return 42;} // mac: Key, Data => MAC

    // change perm to perms todel
    int buffer_init( void * start, int size, Perm  perm, int enclave_id){
        
        if(size < 0){
            print("Error : illegal buffer size.");
            return -1;
        }
        if(sm.buffer_idx >= MAX_BUFFERS){
            print("Error : number of buffers cannot exceed MAX_BUFFERS : ");
            print_int(MAX_BUFFERS);
            print(".\n\r");
            return -1;
        }
        if(sm.enclave_buffer_idx >= MAX_ENCLAVE_BUFFER){
            print("Error : enclave buffer cross-table overflow.\n\r");
            return -1;
        }
        if((perm.rwx > 3) || (perm.rwx <= 0)){
            print("Error : illegal permissions.\n\r");
            return -1;
        }
        
        int reallocatable = start == (void *)-1;
        if(reallocatable){
            start = find_and_reserve_mem(&sm,size);
        }else{
            start =  reserve_mem(&sm,start,(void*)((char*)start + size));
        }

        if(start == (void*) -1){
            print("Error : could not allocate sufficient buffer space (overlap).\n\r");
            return -1;
        }

        
        Buffer b;
        b.start = start;
        b.end = (void*)((char*)start + size) ;
        b.id = sm.buffer_id_c++;
        sm.buffers[sm.buffer_idx++] = b;

        BufferEnclave be;
        be.enclave_id = enclave_id;
        be.buffer_id = b.id;
        be.perm = perm;
        be.is_owner = 1;
        sm.enclave_buffer[sm.enclave_buffer_idx++] = be;
        return b.id;
    } 

    int unprotect(int id){
        //unprotect buffer!! todo
        print("unprotect \n\r"); //todel
        // this is if the untrusted os does an access violation
        Enclave e = get_enclave( &sm,id);
        if(e.id ==-1){
            print("The eclave you are trying to unprotect does not exist.\n\r");
            return -1;
        }
        if(e.data_id != -1){
            delete_buffer(&sm,e.data_id);
        }
        int text_section_deleted = delete_buffer(&sm,e.text_id);
        int enclave_deleted = delete_enclave(&sm,id);
        delete_enclave_buffer(&sm, id);

        return 1;
    }

    // encrypt : Key, Message, Data, -1 => Cypher, MAC
    // decrypt : Key, Cypher, Data, MAC => Message
    // void* aead(int key, void * M_start, int M_size,void * D_start, int D_size,int MAC){return M_start;};

    int run_enclave(int id,int nb1, int nb2){
        Enclave e = get_enclave( &sm,id);
        if(e.id == sm.enclave_error.id){
            print("Error : attempted to run unprotected or uninitialized enclave.\n\r");
            return -1;
        }
        Buffer to_run = get_buffer(&sm,e.text_id);
        if(to_run.id == -1){
            print("Error : no text-section buffer found.\n\r");
            return -1;
        }
        void * data_start = (void *)-1;
        void * data_end = (void *)-1;
        if(e.data_id != -1){
            Buffer data_section = get_buffer(&sm,e.data_id);
            if(to_run.id == -1){
                print("Error : no data-section buffer found.\n\r");
                return -1;
            }
            data_start = data_section.start;
            data_end = data_section.end;
        }

        // decrypting the input 
        int MAC = 12; // todel
        int data = 124;
        int key = e.key;
        int decyrpted_nb1 = nb1;
        // 8 is the size of a c int
        int * decrypted_address1 = aead(key,&decyrpted_nb1,8,&data,8,MAC);
        nb1 = *decrypted_address1;

        int decyrpted_nb2 = nb2;
        int * decrypted_address2 = aead(key,&decyrpted_nb2,8,&data,8,MAC);
        nb2 = *decrypted_address2;
        int to_unprotect = r_enclave(to_run.start,to_run.end,data_start,data_end,e.id,0b101,nb1,nb2);
        if(to_unprotect == -1){return 1;}
        unprotect(to_unprotect);
        return -1;
    } 

    int add_SP(void * SP_start, int SP_size){
        SP new_SP;
        //todo add real SP test. under here is an example
        if(sm.enclave_idx == MAX_ENCLAVES || sm.buffer_idx == MAX_BUFFERS || sm.enclave_buffer_idx == MAX_ENCLAVE_BUFFER){
            print("Request refused.\n\r");
            return -1;
        }
        new_SP.id = sm.SP_id_c++;
        hmac(new_SP.key, sm.K_N, KEY_LEN, &new_SP.id, sizeof new_SP.id);
            sm.SP_list[sm.SP_list_idx++]=new_SP;
            return new_SP.id;
        }
        
    int enclave_init(void * copy_start,void * text_start , int text_sz, void * data_start, int data_sz,int sp_id) {
        if(copy_start <  (void*) END){
            print("Error : illegal copying operation.\n\r");
            return -1;
        }
        if(sm.enclave_idx >= MAX_ENCLAVES){
            print("Error : number of enclaves cannot exceed MAX_ENCLAVES : ");
            print_int(MAX_ENCLAVES);
            print(".\n\r");
            return -1;
        }
        if(text_sz < 4){
            print("Error : size must at least be 4. \n\r");
            return -1;
        }
        
        Perm rx_perm;
        rx_perm.rwx = 0b01; // x is indicated through the text_id
        rx_perm.hash = 0; // todel placeholder
        Enclave e;
        e.id = sm.enclave_id_c++;
        int reallocatable = text_start == (void *) -1;

        text_sz = text_sz+8;
        
        if(reallocatable){
            e.text_id = buffer_init((void *) -1,text_sz,rx_perm,e.id);
            if(e.text_id == -1){
                print("Error : no memory available.\n\r");
                return -1;
            }
            text_start = get_buffer(&sm,e.text_id).start;
        }else{
            e.text_id = buffer_init(text_start,text_sz,rx_perm,e.id);
            if(e.text_id == -1){
                print("Error : no memory available.\n\r");
                return -1;
            }
        }
       

        void *data_address = (void *) -1;
        e.data_id = -1;
        if(data_sz > 0){
            //reserving data section 
            Perm rw_perm;
            rw_perm.rwx = 0b11;
            rw_perm.hash = 0; // todell place holder
            int data_id;
            if(reallocatable){
             data_id = buffer_init((void *)-1,data_sz,rw_perm,e.id);
            }else{
                data_id = buffer_init(data_start,data_sz,rw_perm,e.id);
            }
            void* data_start = get_buffer(&sm,data_id).start;
            void * data_end =  get_buffer(&sm,data_id).end;
            if(data_id == -1){
                unprotect(e.id);
                return -1;
            }
            zero_mem(data_start,data_end);
            e.data_id = data_id;
            data_address = data_start;
        }
        mem_copy(text_start, copy_start, text_sz-8,data_address,reallocatable);
        SP  sp = get_sp(&sm,sp_id);
        if(reallocatable){
            hmac(e.key, sp.key,KEY_LEN, text_start, text_sz-8);
        }else{
            hmac(e.key, sp.key,KEY_LEN, text_start, text_sz);
        }
        if(e.text_id == -1){return -1;}
        sm.enclaves[sm.enclave_idx++] = e;
        return e.id;
    }  

    void encrypt_response(){
        // for now hard coded to the first 6
        for(int i = 0;i< 7; i++){
            int to_encrypt = get_resp_byte(i*8);
            int encrypted = to_encrypt; //todo add encryption
            set_resp_byte(i*8,encrypted);
        }
    }
    /*
    For calls see ABI
    */
    // checking if call is allowed
    
    if(enclave_id > 0 && call_number <20){
        unprotect(enclave_id);
        return 0;
    }
    
    // initiliazation is only allowed once 
    if(call_number == 0 && sm.is_initialized != 1){
        //indices
        sm.is_initialized = 1;
        sm.enclave_id_c = 1; 
        sm.buffer_id_c = 1;
        sm.SP_id_c =1;
        sm.enclave_idx=0;
        sm.buffer_idx=0;
        sm.enclave_buffer_idx=0;
        sm.SP_list_idx = 0;
        //initiating free space linked list
        Space init_space;
        init_space.start = (void*)  BUFFERS_START;
        init_space.end = (void*)  END;
        Space terminator;
        terminator.start = (void*) -1;
        //reset
        sm.free_list[0] = init_space;
        sm.free_list[1] = terminator;

        //hard coded for now
        uint8_t K_N[KEY_LEN] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
        };
        memcpy(sm.K_N,K_N, sizeof K_N);

        sm.enclave_error.id = -1;
        sm.enclave_error.text_id = -1;
        sm.buffer_error.id = -1;
        sm.buffer_error.start = (void*) -1;
        sm.buffer_error.end = (void *) -1;
        sm.sp_error.id = -1;
        r_untrusted();
        // returning 1 to shut the compiler the f* up, this has no meaning just a fix
        return 1;
    }
    // sp_init
    if(call_number == 1){
        void * data_start = arg1.addr;
        int data_size = arg1.nb;
        int SP_key = add_SP((void*)data_start,data_size);
        return SP_key;
    }
    // realloc enclave_init
    if(call_number == 2){
        int id = enclave_init(arg1.addr,(void*)-1,arg2.nb,(void*)-1,arg3.nb,arg4.nb);
        return id;
    }
    // fixed enclave_init
    if(call_number == 3){
        int id = enclave_init(arg1.addr,arg2.addr,arg3.nb,arg4.addr,arg5.nb,arg6.nb);
        
        return id;
    }

    // run_enclave
    if(call_number == 4){
        int id = arg1.nb;
        int status = run_enclave(id,arg2.nb,arg3.nb);
        encrypt_response();
        set_untrusted_pmp();
        return status;
    }

    // attest_enclave
    if(call_number == 5){
        int id = arg1.nb;
        uint8_t nonce = (uint8_t) arg2.nb;
        Enclave e = get_enclave(&sm,id);
        uint8_t mac[MAC_LEN] = {0};
        hmac(mac, e.key, KEY_LEN, &nonce, sizeof nonce);
        print("attestation : ");\
        for(int i = 0;i< 7; i++){
            print_int(mac[i]);
            set_resp_byte(i*8,mac[i]);
        }
        return 1;
    }


    // unprotect
    if(call_number ==20){
        return unprotect(arg1.nb);
    }
    // dummy return
    return 1;
}

