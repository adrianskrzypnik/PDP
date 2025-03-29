#include <iostream>
#include <vector>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>

class DiningPhilosophers {
private:
    static const int NUM_PHILOSOPHERS = 5;
    std::vector<HANDLE> forks;
    CRITICAL_SECTION printLock;

    static DWORD WINAPI philosopherFunction(LPVOID param);

    void printMessage(const std::string& message) {
        EnterCriticalSection(&printLock);
        std::cout << message << std::endl;
        std::cout.flush();
        LeaveCriticalSection(&printLock);
    }

    int getRandom(int min, int max) {
        return min + rand() % (max - min + 1);
    }

    void think(int id) {
        printMessage("Philosopher " + std::to_string(id) + " is thinking...");
        Sleep(getRandom(500, 1000));
    }

    void eat(int id) {
        printMessage("Philosopher " + std::to_string(id) + " is eating...");
        Sleep(getRandom(300, 800));
    }

    void runPhilosopher(int id) {
        printMessage("Philosopher " + std::to_string(id) + " is starting");

        int leftFork = id;
        int rightFork = (id + 1) % NUM_PHILOSOPHERS;

        if (id % 2 == 0) {
            std::swap(leftFork, rightFork);
        }

        for (int iterations = 0; iterations < 3; iterations++) {
            think(id);

            printMessage("Philosopher " + std::to_string(id) + " is trying to pick up fork " + std::to_string(leftFork));
            WaitForSingleObject(forks[leftFork], INFINITE);
            printMessage("Philosopher " + std::to_string(id) + " picked up fork " + std::to_string(leftFork));

            Sleep(50);

            printMessage("Philosopher " + std::to_string(id) + " is trying to pick up fork " + std::to_string(rightFork));
            WaitForSingleObject(forks[rightFork], INFINITE);
            printMessage("Philosopher " + std::to_string(id) + " picked up fork " + std::to_string(rightFork));

            eat(id);

            ReleaseMutex(forks[rightFork]);
            printMessage("Philosopher " + std::to_string(id) + " put down fork " + std::to_string(rightFork));

            ReleaseMutex(forks[leftFork]);
            printMessage("Philosopher " + std::to_string(id) + " put down fork " + std::to_string(leftFork));
        }

        printMessage("Philosopher " + std::to_string(id) + " is finishing");
    }

public:
    DiningPhilosophers() {
        std::cout << "Initializing the Dining Philosophers problem..." << std::endl;

        srand(static_cast<unsigned int>(time(nullptr)));
        InitializeCriticalSection(&printLock);

        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            HANDLE fork = CreateMutex(NULL, FALSE, NULL);
            forks.push_back(fork);
        }

        std::cout << "Initialization completed. Created " << forks.size() << " forks." << std::endl;
    }

    ~DiningPhilosophers() {
        DeleteCriticalSection(&printLock);
        for (size_t i = 0; i < forks.size(); i++) {
            CloseHandle(forks[i]);
        }
        std::cout << "Resources released." << std::endl;
    }

    void start() {
        std::cout << "Starting philosophers..." << std::endl;

        HANDLE philosophers[NUM_PHILOSOPHERS];
        DWORD threadIds[NUM_PHILOSOPHERS];

        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            std::pair<DiningPhilosophers*, int>* param = new std::pair<DiningPhilosophers*, int>(this, i);
            philosophers[i] = CreateThread(
                NULL,
                0,
                philosopherFunction,
                param,
                0,
                &threadIds[i]
            );

            if (philosophers[i] == NULL) {
                std::cout << "Error creating thread: " << GetLastError() << std::endl;
            } else {
                std::cout << "Created thread for philosopher " << i << std::endl;
            }
        }

        std::cout << "Waiting for philosophers to finish..." << std::endl;

        WaitForMultipleObjects(NUM_PHILOSOPHERS, philosophers, TRUE, INFINITE);

        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            CloseHandle(philosophers[i]);
        }

        std::cout << "All philosopher threads have finished." << std::endl;
    }
};

DWORD WINAPI DiningPhilosophers::philosopherFunction(LPVOID param) {
    auto pair = static_cast<std::pair<DiningPhilosophers*, int>*>(param);
    DiningPhilosophers* instance = pair->first;
    int id = pair->second;
    delete pair;

    instance->runPhilosopher(id);
    return 0;
}

int main() {
    std::cout << "Dining Philosophers Problem" << std::endl;
    std::cout << "----------------------------" << std::endl;
    std::cout.flush();

    {
        DiningPhilosophers problem;
        problem.start();
    }

    std::cout << "Program successfully finished." << std::endl;
    return 0;
}
