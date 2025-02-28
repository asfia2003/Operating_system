#include <iostream>
#include <vector>
#include <windows.h>      
#include <chrono>        
#include <thread>        
#include <algorithm>    

using namespace std;

struct Task {
    int task_id;
    long start_time;
    int exec_time;
    long begin_time;
    long finish_time;
    long total_time;
    long idle_time;
    int extra_number;
};

long getCurrentTime(long start_time) {
    return chrono::duration_cast<chrono::seconds>(
        chrono::system_clock::now().time_since_epoch()).count() - start_time;
}

int main() {
    int count;
    cout << "Enter number of tasks: ";
    cin >> count;

    vector<Task> tasks;
    long program_start_time = chrono::duration_cast<chrono::seconds>(
        chrono::system_clock::now().time_since_epoch()).count();

    for (int i = 0; i < count; i++) {
        int exec_time, extra_number = -1;
        cout << "Enter execution time for Task " << i + 1 << " (in seconds): ";
        cin >> exec_time;

        if (i == 2) {  
            cout << "Enter number for Task 3: ";
            cin >> extra_number;
        }

        long start_time = getCurrentTime(program_start_time);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        string command = "cmd /c timeout /t " + to_string(exec_time) + " >nul";
        char cmd[256];
        strcpy(cmd, command.c_str());

        if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            cerr << "Error: Task creation failed!" << endl;
            return 1;
        }
  
        tasks.push_back({static_cast<int>(pi.dwProcessId), start_time, exec_time, 0, 0, 0, 0, extra_number});
    }

    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
        return a.start_time < b.start_time;
    });

    long system_time = 0;
    for (auto &task : tasks) {
        if (system_time < task.start_time) {
            system_time = task.start_time;
        }

        task.begin_time = system_time;
        this_thread::sleep_for(chrono::seconds(task.exec_time));

        task.finish_time = task.begin_time + task.exec_time;
        task.total_time = task.finish_time - task.start_time;
        task.idle_time = task.total_time - task.exec_time;
        system_time += task.exec_time;
    }

    cout << "\nTask Table (FCFS Scheduling):\n";
    cout << "---------------------------------------------------------------------------------------------------\n";
    cout << "ID\tStart Time (s)\tExec Time\tBegin Time\tFinish Time\tTotal Time\tIdle Time\tExtra Number\n";
    cout << "---------------------------------------------------------------------------------------------------\n";
    for (const auto& task : tasks) {
        cout << task.task_id << "\t" << task.start_time << "\t\t" << task.exec_time 
             << "\t\t" << task.begin_time << "\t\t" << task.finish_time
             << "\t\t" << task.total_time << "\t\t" << task.idle_time;

        if (task.task_id == tasks[2].task_id) {  
            cout << "\t\t" << task.extra_number;
        }
        
        cout << endl;
    }

    return 0;
}
