#include <windows.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

enum ConsoleColor {
    DEFAULT = 7,
    RED = 12,
    GREEN = 10,
    BLUE = 9
};

void saveTheme(ConsoleColor color) {
    std::ofstream out("theme.cfg");
    if (out.is_open()) {
        out << (int)color;
        out.close();
    }
}

ConsoleColor loadTheme() {
    std::ifstream in("theme.cfg");
    int color = DEFAULT;
    if (in.is_open()) {
        in >> color;
        in.close();
    }
    return (ConsoleColor)color;
}

void setConsoleColor(ConsoleColor color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

unsigned long long fileTimeToULL(const FILETIME& ft) {
    return ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}

double getCPUUsage() {
    FILETIME idleTime1, kernelTime1, userTime1;
    FILETIME idleTime2, kernelTime2, userTime2;

    GetSystemTimes(&idleTime1, &kernelTime1, &userTime1);
    Sleep(100);
    GetSystemTimes(&idleTime2, &kernelTime2, &userTime2);

    unsigned long long idle1 = fileTimeToULL(idleTime1);
    unsigned long long idle2 = fileTimeToULL(idleTime2);

    unsigned long long kernel1 = fileTimeToULL(kernelTime1);
    unsigned long long kernel2 = fileTimeToULL(kernelTime2);

    unsigned long long user1 = fileTimeToULL(userTime1);
    unsigned long long user2 = fileTimeToULL(userTime2);

    unsigned long long total1 = kernel1 + user1;
    unsigned long long total2 = kernel2 + user2;

    unsigned long long totalDiff = total2 - total1;
    unsigned long long idleDiff = idle2 - idle1;

    return (1.0 - (double)idleDiff / totalDiff) * 100.0;
}

void getMemoryUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG totalPhys = memInfo.ullTotalPhys;
    DWORDLONG availPhys = memInfo.ullAvailPhys;
    DWORDLONG usedPhys = totalPhys - availPhys;

    cout << "Memory Usage: "
         << usedPhys / (1024 * 1024) << " MB / "
         << totalPhys / (1024 * 1024) << " MB\n";
}

void getDiskUsage(const char* drive = "C:\\") {
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;

    if (GetDiskFreeSpaceEx(drive, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        DWORDLONG used = totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart;

        cout << "Disk Usage (" << drive << "): "
             << used / (1024 * 1024 * 1024) << " GB / "
             << totalNumberOfBytes.QuadPart / (1024 * 1024 * 1024) << " GB\n";
    } else {
        cout << "Failed to get disk usage for drive " << drive << "\n";
    }
}

void getSystemUptime() {
    ULONGLONG ms = GetTickCount64();
    unsigned long long seconds = ms / 1000;
    unsigned long long minutes = seconds / 60;
    unsigned long long hours = minutes / 60;
    unsigned long long days = hours / 24;

    hours %= 24;
    minutes %= 60;
    seconds %= 60;

    cout << "System Uptime: "
         << days << "d " << hours << "h " << minutes << "m " << seconds << "s\n";
}

unsigned long long getFolderSize(const fs::path& folder) {
    unsigned long long size = 0;
    try {
        for (auto& entry : fs::recursive_directory_iterator(folder, fs::directory_options::skip_permission_denied)) {
            if (fs::is_regular_file(entry.path())) {
                try {
                    size += fs::file_size(entry.path());
                } catch (...) {
                    // ignore files can't access
                }
            }
        }
    } catch (...) {
        // ignore folders can't access
    }
    return size;
}

void showFolderProgressBar(int current, int total) {
    int barWidth = 50;
    double progress = (double)current / total;
    int pos = (int)(barWidth * progress);

    std::cout << "\rScanning folders: [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "#";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "%";
    std::cout.flush();
}

void displayFolderUsage(const std::string& folder) {
    std::vector<std::pair<std::string, unsigned long long>> subfolders;

    std::vector<fs::path> topFolders;
    for (auto& entry : fs::directory_iterator(folder)) {
        if (fs::is_directory(entry.path()))
            topFolders.push_back(entry.path());
    }

    int total = topFolders.size();
    int scanned = 0;

    for (auto& path : topFolders) {
        unsigned long long sz = getFolderSize(path);
        subfolders.push_back({path.filename().string(), sz});
        scanned++;
        showFolderProgressBar(scanned, total);
    }
    std::cout << std::endl;

    std::sort(subfolders.begin(), subfolders.end(), [](auto &a, auto &b){ return a.second > b.second; });

    std::cout << "\nTop Folder Usage (Approx.):\n";
    int count = 0;
    for (auto& f : subfolders) {
        if (count++ >= 5) break; 
        int barLength = (int)((double)f.second / subfolders[0].second * 50);
        std::cout << f.first << " [";
        for (int i = 0; i < barLength; i++) std::cout << "#";
        std::cout << "] " << f.second / (1024*1024) << " MB\n";
    }
}

void changeTheme(ConsoleColor &currentColor) {
    int choice;
    std::cout << "\nSelect Theme Color:\n";
    std::cout << "1. Red\n";
    std::cout << "2. Blue\n";
    std::cout << "3. Green\n";
    std::cout << "4. Default\n";
    std::cout << "Enter your choice: ";
    std::cin >> choice;

    switch (choice) {
        case 1: currentColor = RED; break;
        case 2: currentColor = BLUE; break;
        case 3: currentColor = GREEN; break;
        case 4: currentColor = DEFAULT; break;
        default: std::cout << "Invalid choice, keeping current theme.\n"; return;
    }

    setConsoleColor(currentColor);
    std::cout << "Theme changed successfully!\n";
}

void showProgressBar() {
    cout << "\nScanning System...\n\n[";
    for (int i = 0; i <= 50; i++) {
        cout << "#";
        Sleep(50);
    }
    cout << "] 100%\n\n";
}

void clearScreen() {
    system("cls");
}

void showMenu() {
    std::cout << "=====================================\n";
    std::cout << "     SYSTEM PERFORMANCE MONITOR       \n";
    std::cout << "=====================================\n";
    std::cout << "1. Scan System\n";
    std::cout << "2. Show Folder Usage\n";
    std::cout << "3. Change Theme\n";
    std::cout << "0. Exit\n";
    std::cout << "-------------------------------------\n";
    std::cout << "Enter your choice: ";
}

int main() {
    int choice;
    bool running = true;

    ConsoleColor currentColor = loadTheme();
    setConsoleColor(currentColor);

    while (running) {
        showMenu();
        cin >> choice;

        if (choice == 1) {
            setConsoleColor(currentColor);
            showProgressBar();

            double cpuUsage = getCPUUsage();
            cout << "CPU Usage: " << cpuUsage << "%\n";

            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            DWORDLONG totalPhys = memInfo.ullTotalPhys;
            DWORDLONG availPhys = memInfo.ullAvailPhys;
            DWORDLONG usedPhys = totalPhys - availPhys;
            double memoryUsagePercent = ((double)usedPhys / totalPhys) * 100;
            cout << "Memory Usage: "
                 << usedPhys / (1024 * 1024) << " MB / "
                 << totalPhys / (1024 * 1024) << " MB\n";

            ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
            GetDiskFreeSpaceEx("C:\\", &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
            DWORDLONG usedDisk = totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart;
            double diskUsagePercent = ((double)usedDisk / totalNumberOfBytes.QuadPart) * 100;
            cout << "Disk Usage (C:\\): "
                 << usedDisk / (1024 * 1024 * 1024) << " GB / "
                 << totalNumberOfBytes.QuadPart / (1024 * 1024 * 1024) << " GB\n";

            ULONGLONG ms = GetTickCount64();
            unsigned long long seconds = ms / 1000;
            unsigned long long minutes = seconds / 60;
            unsigned long long hours = minutes / 60;
            unsigned long long days = hours / 24;
            hours %= 24;
            minutes %= 60;
            seconds %= 60;
            cout << "System Uptime: "
                 << days << "d " << hours << "h " << minutes << "m " << seconds << "s\n";

            cout << "\n===== SYSTEM HEALTH SUMMARY =====\n";
            bool critical = false;
            if(cpuUsage > 85) critical = true;
            if(memoryUsagePercent > 90) critical = true;
            if(diskUsagePercent > 90) critical = true;

            if(critical)
                cout << "[CRITICAL] Your system is under heavy load!\n";
            else
                cout << "[GOOD] Your system is running smoothly.\n";

            cout << "\nPress Enter to return to menu...";
            cin.ignore();
            cin.get();
        }
        else if (choice == 2) {
            setConsoleColor(currentColor);
            std::string folder = "C:\\";

            if(fs::exists(folder) && fs::is_directory(folder)) {
                displayFolderUsage(folder);
            } else {
                std::cout << "Cannot access folder: " << folder << "\n";
            }

            std::cout << "\nPress Enter to return to menu...";
            std::cin.ignore();
            std::cin.get();
        }
        else if (choice == 3) {
            changeTheme(currentColor);
            saveTheme(currentColor);
        }
        else if (choice == 0) {
            running = false;
            cout << "\nExiting program. Goodbye!\n";
        }
        else {
            cout << "\nInvalid choice. Please try again.\n";
        }
    }

    return 0;
}
