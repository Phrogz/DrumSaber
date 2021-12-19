struct Impulse {
    unsigned long timestamp; // in milliseconds
    float power; // range [0.0, 1.0]
    Impulse *next; 
};

class ImpulseList {
    public:
        Impulse* head;
        Impulse* tail;
        
        ImpulseList();
        void add(unsigned long timestamp, float power);
        void remove(unsigned long timestamp);
};
