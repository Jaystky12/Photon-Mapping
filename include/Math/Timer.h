#ifndef TIMER_H
#define TIMER_H
using namespace std;

/*-------------------------------------------------------------------
 * Timer struct to measure the execution time of a certain scope
 * Static usage for best performance:
 *
 * Timer timer = { "tag" };
 *
 * ------------------------------------------------------------------*/
struct Timer
{
    chrono::time_point<chrono::high_resolution_clock> start, end;
    chrono::duration<float> duration;
    string tag = "Timer";

    Timer()
    {
        start = chrono::high_resolution_clock::now();
    }
    Timer(string Tag)
    {
        tag = Tag;
        start = chrono::high_resolution_clock::now(); 
    }
    

    ~Timer()
    {
        end = chrono::high_resolution_clock::now();
        duration = end - start;

        int sec = (int)duration.count();
        double ms = duration.count() * 1000.0f;

        printf("\033[32m%s stopped after %10ds\t(%.3f ms)\033[0m\n",tag.c_str(), sec, ms);
    }
};
#endif