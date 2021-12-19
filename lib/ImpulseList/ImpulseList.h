typedef enum {HIT, BEAT} impulseType;

struct Impulse {
    unsigned long timestamp; // in milliseconds
    float power; // range [0.0, 1.0]
    impulseType type;
    Impulse *next; 
};

class ImpulseList {
    public:
        Impulse* head;
        Impulse* tail;
        
        ImpulseList();
        void add(unsigned long timestamp, float power, impulseType type);
        int purgeBefore(unsigned long timestamp);
        int count();
};
