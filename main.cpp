#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include <cstdlib>

using namespace std;

// Global variables
mutex mtx;
condition_variable cv;
bool allReady = false;

// Function prototypes
void doStuff(promise<int> && P, int id);
void runSimulation();
void this_sleep(int ms);

int main() {
    int N;
    cout << "Enter a non-negative integer: ";
    cin >> N;
    while (N < 0) {
        cout << "Please enter a non-negative integer: ";
        cin >> N;
    }

    // Read number of threads from file
    // Assuming this part is handled elsewhere in the program

    vector<thread> V;

    // Create threads
    for (int i = 0; i < N; ++i) {
        promise<int> P;
        future<int> F = P.get_future();
        V.push_back(thread(doStuff, std::move(P), i));

    }

    // Notify threads to start execution
    cout << "READY TO RELEASE THE THREADS" << endl;
    runSimulation();

    // Join threads
    for (thread & t : V) {
        t.join();
    }

    cout << "Program Execution is complete" << endl;

    return 0;
}

void doStuff(promise<int> && P, int id) {
    cout << "Thread " << id << " is now active" << endl;
    srand(time(0) + id);
    unique_lock<mutex> UL(mtx);

    cout << "Thread " << id << " is waiting" << endl;
    while (!allReady) {
        cv.wait(UL);
    }
    int sleepTime = rand() % 901 + 100; // Random sleep time between 100 and 1000 milliseconds

    cout << "Thread " << id << " is preparing to sleep for " << sleepTime << " milliseconds" << endl;
    this_sleep(sleepTime);
    int result = rand() % 100 + 1; // Random result between 1 and 100

    cout << "Thread " << id << " is returning the value " << result << endl;
    P.set_value(result);
}

void runSimulation() {
    unique_lock<mutex> UL(mtx);
    allReady = true;
    cv.notify_all();
}

void this_sleep(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

/**

Enter a non-negative integer: 5
Thread 0 is now active
        Thread 1 is now active
        Thread 3 is now active
        Thread 4 is now active
        Thread READY TO RELEASE THE THREADS
2 is now active
        Thread 1 is waiting
Thread 3 is waiting
Thread 0 is waiting
Thread 2 is waiting
Thread 4 is waiting
Thread 1 is preparing to sleep for 902 milliseconds
        Thread 1 is returning the value 19
Thread 3 is preparing to sleep for 333 milliseconds
        Thread 3 is returning the value 40
Thread 0 is preparing to sleep for 289 milliseconds
        Thread 0 is returning the value 41
Thread 2 is preparing to sleep for 110 milliseconds
        Thread 2 is returning the value 24
Thread 4 is preparing to sleep for 880 milliseconds
        Thread 4 is returning the value 20
Program Execution is complete

 *
 */


/**
Enter a non-negative integer: 3
Thread 0 is now active
READY TO RELEASE THE THREADS
Thread Thread 2 is now active1
 is now active
Thread 0 is waiting
Thread 1 is waiting
Thread 1 is preparing to sleep for 237 milliseconds
Thread 1 is returning the value 36
Thread 0 is preparing to sleep for 716 milliseconds
Thread 0 is returning the value 24
Thread 2 is waiting
Thread 2 is preparing to sleep for 717 milliseconds
Thread 2 is returning the value 81
Program Execution is complete
*/