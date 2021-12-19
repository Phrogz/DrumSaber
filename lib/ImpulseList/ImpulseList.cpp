#include "ImpulseList.h"
#define NULL 0

ImpulseList::ImpulseList() {
    head = NULL;
    tail = NULL;
}

void ImpulseList::add(unsigned long timestamp, float power) {
    // make a new node
    Impulse* imp = new Impulse;
    imp->timestamp = timestamp;
    imp->power = power;
    imp->next = NULL;

    // If list is empty, make the new node, the head
    if (head == NULL) {
        head = imp;
        tail = imp;
    } else {
        tail->next = imp;
        tail = imp;
    }
}
    
void ImpulseList::remove(unsigned long timestamp) {
    Impulse *cur = head;
    Impulse *prev = NULL;
    while (cur != NULL) {
        if (cur->timestamp == timestamp) {
            if (head == cur) {
                head = cur->next;
            }
            if (tail == cur) {
                tail = prev;
            }
            if (prev != NULL) {
                prev->next = cur->next;
            }
            delete cur;
            break; // FIXME: maybe remove this, in case multiple have the same timestamp
        }
        prev = cur;
        cur = cur->next;
    }
}