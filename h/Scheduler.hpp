#ifndef PROJECT_BASE_V1_1_SCHEDULER_HPP
#define PROJECT_BASE_V1_1_SCHEDULER_HPP

static const int MAX_NUMBER_OF_THREADS = 30;
class _thread;

class Scheduler
{
private:
    _thread* red[MAX_NUMBER_OF_THREADS]; //lista od 20 pokazivaca na strukture TCB
    int first; //prvi koji se uzima
    int last; //poslednji dodat

    ~Scheduler() = default;
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

public:

    Scheduler()
    {
        first = -1;
        last = -1;
    }

    static Scheduler* getInstance();

    _thread *get(); //vraca TCB* niti koja je na redu da se izvrsava
    void put(_thread *tcb); //stavlja TCB* niti na kraj reda

};


#endif //PROJECT_BASE_V1_1_SCHEDULER_HPP