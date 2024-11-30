#include "types.h"
#include "lib.h"
#include <stddef.h>
#include <stdint.h>
void* SM_SECTION find_and_reserve_mem(SM* sm, int size){
        Space * free_list = sm->free_list;
        if(0 >= size ){
            print("Error : illegal space structure.\n\r");
            return (void*) -1;
        }
        if(free_list[0].start == (void*)-1){
            print("Error : no free space found.\n\r");
            return (void*)-1;
        }
        Space s = free_list[0];
        Space prev =s;

        //asuming there is an s! see guard clause above
            int i = 0;
        while (free_list[i].start != (void*)-1) {
            Space s = free_list[i];
            void *start = s.start;
            void *end = start + size;

            // Check if the current free space can fit the requested size
            if ((end <= s.end) && (start >= s.start)) {
                // Case 1: Exact match
                if (start == s.start && end == s.end) {
                    for (int j = i; free_list[j].start != (void*)-1; j++) {
                        free_list[j] = free_list[j+1];
                    }
                    return s.start;
                }

                // Case 2: Allocation at the beginning
                if (start == s.start && end < s.end) {
                    free_list[i].start = end;
                    return s.start;
                }

                // Case 3: Allocation at the end
                if (start > s.start && end == s.end) {
                    free_list[i].end = start;
                    return s.start;
                }

                // Case 4: Allocation in the middle, split space
                if (start > s.start && end < s.end) {
                    Space new_space;
                    new_space.start = end;
                    new_space.end = s.end;

                    // Shift elements to make room for the new space
                    for (int j = i + 1; free_list[j - 1].start != (void*)-1; j++) {
                        free_list[j + 1] = free_list[j];
                    }

                    free_list[i + 1] = new_space;
                    free_list[i].end = start;
                    return s.start;
                }
            }
            i++;
        }

        print("Error : no free space found.\n\r");
        return (void*) -1;
    }


void * SM_SECTION reserve_mem(SM* sm, void * start,void * end){
        Space * free_list = sm->free_list;
        if(end <= start ){
            print("Error : illegal space structure.\n\r");
            return (void *)-1;
        }
        if(end >  (void*)  END ){
            print("Error : you cannot reserve space beyond the buffer zone.\n\r");
            return (void *)-1;
        }
        if(start <  (void*)  BUFFERS_START){
            print("Error : you cannot reserve space outside the buffer zone.\n\r");
            return (void *)-1;
        }
        if(free_list[0].start == (void*)-1){
            print("Error : no free space found.\n\r");
            return (void *)-1;
        }
        Space s = free_list[0];
        Space prev =s;
        //first item delete
        if(s.start == start && s.end == end){
            if(sm->free_list[1].start == (void*) -1){
                sm->free_list[0].start = (void*) -1;
                return start;
            }
        }
        
        //asuming there is an s! see guard clause above
        
        
        int i=0;
        while(sm->free_list[i].start != (void*)-1)
        {   
            
            Space s = sm->free_list[i];
            if(s.start == start && end == s.end){
                for(int j =i;sm->free_list[j].start != (void*)-1;j++){
                sm->free_list[j] = sm->free_list[j+1];}
                return start;
            }
            
            if(s.start == start && end < s.end){
                sm->free_list[i].start = end;
                return start;
            }
            if(s.start < start && end == s.end){
                sm->free_list[i].end = start;
                return start;
            }
           
            if(s.start < start && end <s.end){
                
                Space new_space;
                new_space.start = end;
                new_space.end = s.end;
                for(int j =i+1;sm->free_list[j-1].start != (void*)-1;j++){
                sm->free_list[j+1] = sm->free_list[j];}
                sm->free_list[i+1] = new_space;
                sm->free_list[i].end =start;
                return start;
            }
            i++;
        }
}






//free_mem is trusted this is not idiot proofed!!!!!
int SM_SECTION free_mem(SM* sm, void * start,void * end){
    // alright I lied, there is some idiot proofing hope this makes verifatication easier
    if(end <= start ){
            print("Error : illegal space structure.\n\r");
            return -1;
    }
    if(end >  (void*)  END ){
        print("Error : you cannot free space beyond the buffer zone.\n\r");
        return -1;
    }
    if(start <  (void*)  BUFFERS_START){
        print("Error : you cannot free space outside the buffer zone.\n\r");
        return -1;
    }
    Space new_space;
    new_space.start=start;
    new_space.end=end;

    if(sm->free_list[0].start == (void*) -1){
        sm->free_list[0] =new_space;
        return 1;
    }

    int freed_space=0;
    int i = 0;
    for(;sm->free_list[i].start != (void*)-1;i++){
        Space s = sm->free_list[i];
        if(s.start == end){
            sm->free_list[i].start = start;
            freed_space=1;
            break;
        }
        if(s.end == start){
            sm->free_list[i].end = end;
            freed_space=1;
            break;
        }if(s.end == start){
            sm->free_list[i].end = end;
            freed_space=1;
            break;
        }
    }

    //merging spaces
    if(freed_space && sm->free_list[i].end == sm->free_list[i+1].start){
        sm->free_list[i].end = sm->free_list[i+1].end;
        for(int j=i+1; sm->free_list[j].start != (void *)-1;j++){
            sm->free_list[j] = sm->free_list[j+1];
        }
    }
    
    if(!freed_space && sm->free_list[i-1].end < start){
        sm->free_list[i+1] = sm->free_list[i];
        sm->free_list[i] = new_space;
        freed_space=1;
    }

    return freed_space;
}

void SM_SECTION mem_copy(void* dest, const void* src, int size,void*data_address, int relocatable) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    if(relocatable == 1){
        for (int i = 0; i < size-8; i++) {
            d[i] = s[i];
        }
        d[size] = data_address;
    }
    else{
        for (int i = 0; i < size-8; i++) {
            d[i] = s[i];
        }
        d[size-1] = dest;
        d[size] = data_address;
    }

}


