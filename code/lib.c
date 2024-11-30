#include "lib.h"
#include "types.h"
#include "shared_lib.h"

SHARED_LIB(".sm",)

void SM_SECTION print_mem(void * start, void * end, char * str){
    print(str);
    print(" : start :");
    print_address(start);
    print(", end : ");
    print_address(end);
    print("\n\r");
}


void SM_SECTION print_space(SM * sm){
    print("Space layout : \n\r");
    int i =0;
    while(sm->free_list[i].start != (void*)-1)
    {
        Space s = sm->free_list[i++];
        print_mem(s.start,s.end,"Mem");
    }
    print("======================\n\r");
}
