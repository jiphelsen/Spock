#include <stdio.h>

#define BUFFERS_START 10
#define END 100
#define free_list_SZ 1000

struct Space;

typedef struct Space {
    void* start;
    void* end;
} Space;

typedef struct
{
    Space free_list[free_list_SZ];
} SM;


int resrv_mem(SM* sm, void * start,void * end){
        Space * free_list = sm->free_list;
        if(end <= start ){
            printf("Error : illegal space structure.\n");
            return -1;
        }
        if(end >  (void*)  END ){
            printf("Error : you cannot reserve space beyond the buffer zone.\n");
            return -1;
        }
        if(start <  (void*)  BUFFERS_START){
            printf("Error : you cannot reserve space outside the buffer zone.\n");
            return -1;
        }
        if(free_list[0].start == (void*)-1){
            printf("Error : no free space found.\n");
            return -1;
        }
        Space s = free_list[0];
        Space prev =s;
        //first item delete
        if(s.start == start && s.end == end){
            if(sm->free_list[1].start == (void*) -1){
                sm->free_list[0].start = (void*) -1;
                return 1;
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
                return 1;
            }
            
            if(s.start == start && end < s.end){
                sm->free_list[i].start = end;
                return 1;
            }
            if(s.start < start && end == s.end){
                sm->free_list[i].end = start;
                return 1;
            }
            if(s.start < start && end <s.end){
                
                Space new_space;
                new_space.start = end;
                new_space.end = s.end;
                for(int j =i+1;sm->free_list[j-1].start != (void*)-1;j++){
                sm->free_list[j+1] = sm->free_list[j];}
                sm->free_list[i+1] = new_space;
                sm->free_list[i].end =start;
                return 1;
            }
            i++;
        }
        printf("Error : no free space found.\n");
        return -1;
    }

//free_mem is trusted this is not idiot proofed!!!!!
int free_mem(SM* sm, void * start,void * end){
    // alright I lied, there is some idiot proofing hope this makes verifatication easier
    if(end <= start ){
            printf("Error : illegal space structure.\n");
            return -1;
    }
    if(end >  (void*)  END ){
        printf("Error : you cannot free space beyond the buffer zone.\n");
        return -1;
    }
    if(start <  (void*)  BUFFERS_START){
        printf("Error : you cannot free space outside the buffer zone.\n");
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

void print_space(SM * sm){
    printf("Space layout : \n");
    int i =0;
    while(sm->free_list[i].start != (void*)-1)
    {
        Space s = sm->free_list[i++];
        printf("start : %p ,  end : %p  \n",s.start,s.end);
    }
    printf("======================\n");
}

void main(){
    SM sm;
    Space init_space;
    init_space.start = (void*)  BUFFERS_START;
    init_space.end = (void*)  END;

    Space terminator;
    terminator.start = (void*) -1;
    //reset
    sm.free_list[0] = init_space;
    sm.free_list[1] = terminator;

    //error tests
    // Illegal space struct
    resrv_mem(&sm,(void *) 50,(void *) 30);
    // end overflow
    resrv_mem(&sm,(void *) 20,(void *) 101);
    // start underflow
    resrv_mem(&sm,(void *) 5,(void *) 20);

    //space list is emptied
    resrv_mem(&sm,(void *) 10,(void *) 100);
    printf("All memory is reserved should give Error no free space.\n");
    resrv_mem(&sm,(void *) 20,(void *) 30);
    //reset
    sm.free_list[0] = init_space;
    sm.free_list[1] = terminator;
    
    //check for move first pointer
    init_space.start = (void *)20;
    init_space.end =(void *) 40;
    Space second_space;
    second_space.start =(void *) 50;
    second_space.end = (void *) 55;
    sm.free_list[0] = init_space;
    sm.free_list[1] = second_space;
    sm.free_list[2] = terminator;
    resrv_mem(&sm,(void *) 20,(void *) 40);

    //should give an error no free space found
    printf("Trying to double reserve memory should give Error no free space.\n");
    resrv_mem(&sm,(void *) 20,(void *) 40);
    printf("Should have 1 entry!\n");
    print_space(&sm);
    
    
    // test entry equal second entry : 
    resrv_mem(&sm,(void *) 50,(void *) 55);
    printf("Should be an empty list!\n");
    print_space(&sm);
    


    //setting up test
    init_space.start = (void*)  BUFFERS_START;
    init_space.end = (void*)  END;
    sm.free_list[0] = init_space;
    sm.free_list[1].start = (void*) -1;

    //testing taking memory from the start
    printf("Should print start : 0x1a ,  end : 0x64 only.\n");
    resrv_mem(&sm,(void *) 10,(void *) 25);
    resrv_mem(&sm,(void *) 25,(void *) 26);
    print_space(&sm);
    //testing taking memory from the end
    resrv_mem(&sm,(void *) 99,(void *) 100);
    resrv_mem(&sm,(void *) 98,(void *) 99);
    //testing is secure
    printf("Should give 2 no space errors.\n");
    resrv_mem(&sm,(void *) 25,(void *) 98);
    resrv_mem(&sm,(void *) 26,(void *) 99);
    printf("Should print start : 0x1a ,  end : 0x62 only.\n");
    print_space(&sm);

    //taking memory from the middle   
    resrv_mem(&sm,(void *) 40,(void *) 50);
    resrv_mem(&sm,(void *) 50,(void *) 51);
    resrv_mem(&sm,(void *) 35,(void *) 40);
    resrv_mem(&sm, (void *) 60 , (void *) 80);
    printf("Should print 3 entries : \n");
    printf("start : 0x1a ,  end : 0x23 \nstart : 0x33 ,  end : 0x3c \nstart : 0x50 ,  end : 0x62 \n");
    print_space(&sm);
    printf("Should give no space error.\n");
    resrv_mem(&sm, (void*) 78, (void*) 93 );

    // I absolutely insist that this is a seperate section and the call is not used in the previous section.
    // This may ease testing but it is a  violation against the very principle of torough testing as two
    // functions whoms behaviour is not yet tested is tested at once. Resrv_mem is already tested so it can be used.
    printf("======================\n \n");
    printf("Here starts the free test!\n");
    init_space.start = (void*)  BUFFERS_START;
    init_space.end = (void*)  END;
    //reset
    sm.free_list[0] = init_space;
    sm.free_list[1] = terminator;
    // testing freeing all space
    resrv_mem(&sm,(void *) 10,(void *) 100);
    free_mem(&sm,(void *) 10,(void *) 100);
    printf("All should be free : \n");
    print_space(&sm);

    // test free after and before 
    resrv_mem(&sm,(void *) 30,(void *) 90);
    resrv_mem(&sm,(void *) 10,(void *) 20);
    free_mem(&sm,(void *) 10, (void *) 20);
    free_mem(&sm,(void * )80, (void *)90);
    printf("Should print start : 0xa ,  end : 0x1e and start : 0x50 ,  end : 0x64 \n");
    print_space(&sm);


    // test end free :
    // reset
    sm.free_list[0] = init_space;
    sm.free_list[1] = terminator;
    resrv_mem(&sm,(void *) 10,(void *) 100);
    free_mem(&sm,(void *) 90, (void *)100);
    printf("Should print start : 0x5a ,  end : 0x64 \n");
    print_space(&sm);

    printf("Merge test\n");
    // merge test
    // reset
    init_space.start = (void*)  BUFFERS_START;
    init_space.end = (void*)  END;
    terminator.start = (void*) -1;
    sm.free_list[0] = init_space;
    sm.free_list[1] = terminator;

    //merging at the end
    print_space(&sm);
    resrv_mem(&sm,(void *) 40,(void *) 50);
    free_mem(&sm,(void *) 40, (void *)50);
    printf("Should print start : 0xa ,  end : 0x64 \n");
    print_space(&sm);

    



    return;
}