#include <FastLED.h>
#include "ImpulseList.h"

ImpulseList::ImpulseList() {
    head = NULL;
    tail = NULL;
}

void ImpulseList::add(unsigned long timestamp, float power, impulseType type) {
    // make a new node
    Impulse* imp = new Impulse;
    imp->timestamp = timestamp;
    imp->power = power;
    imp->type = type;
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

// Removes impulses from the front of the list up to the supplied time
// Returns the number of impulses removed from the list 
int ImpulseList::purgeBefore(unsigned long cutoffTime) {
    // Serial.println("Purging before " + String(cutoffTime) + "; head is "+(head==NULL ? "-" : String(head->timestamp)));
    int purgedCount = 0;
    Impulse *cur = head;
    while ((cur != NULL) && (cur->timestamp < cutoffTime)) {
        // Serial.println("...now purging impulse.timestamp: "+String(cur->timestamp));
        delete cur;
        cur = cur->next;
        purgedCount++;
    }

    // May set it to null if we walked off the end; this is correct
    head = cur;
    if (head == NULL) {
        tail = NULL;
    }

    return purgedCount;
}

int ImpulseList::count() {
    int count = 0;
    Impulse *cur = head;
    while (cur != NULL) {
        cur = cur->next;
        count++;
    }
    return count;
}