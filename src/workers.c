#include "include.h"

void init_list(JobList* job_list) {
    Node* head = (Node*)MemAlloc(sizeof(Node));
    Node* tail = (Node*)MemAlloc(sizeof(Node));

    head->next = tail;
    tail->next = NULL;

    job_list->head = head;
    job_list->tail = tail;

    return;
}

void push_job(JobList* job_list, ThreadStruct* thread_struct) {
    Node* new_node = (Node*)MemAlloc(sizeof(Node));
    //new_node->thread_struct = thread_struct;

    if(job_list->head->next == job_list->tail) {    // first real node to be added
        new_node->thread_struct = thread_struct;
        job_list->head->next = new_node;
        new_node->next = job_list->tail;
        return;
    }

    new_node = job_list->head->next;
    while(true) {
        if(new_node->next == job_list->tail) {
            new_node->thread_struct = thread_struct;
            break;
        }
        new_node = new_node->next;
    }
    TraceLog(LOG_WARNING, "pushing job!");
    return;
}

//FIFO
ThreadStruct* pop_job(JobList* job_list) {
    if(job_list->head->next == job_list->tail) {
        TraceLog(LOG_WARNING, "popped job, where none exist");
        return NULL;
    } 


    Node* node = (Node*)MemAlloc(sizeof(Node));
    node = job_list->head->next;
    //set next node to node following the one we popped
    job_list->head->next = node->next;

    return node->thread_struct;
}

void cleanup_jobs(JobList* job_list) {
    Node* temp = (Node*)MemAlloc(sizeof(Node));
    temp = job_list->head;

    while(temp != job_list->tail) {
        Node* next = temp->next;
        free(temp);
        temp = next;
    }

    free(job_list->head);
    free(job_list->tail);

    return;
}
