# System Performance Monitor

A Windows console application that monitors system performance metrics and analyzes disk usage. Built with C++ using Windows API.

## 📋 Overview

This application provides real-time system monitoring capabilities with a user-friendly console interface. It tracks CPU usage, memory consumption, disk space, and system uptime while offering folder size analysis and customizable color themes.

## ✨ Features

### 1. **System Performance Scanning**
- **CPU Usage**: Real-time CPU utilization percentage
- **Memory Usage**: Current RAM usage in MB with total available
- **Disk Usage**: Storage consumption for C: drive in GB
- **System Uptime**: Time elapsed since last boot (days, hours, minutes, seconds)
- **Health Summary**: Automatic warnings when system resources exceed safe thresholds

### 2. **Folder Usage Analysis**
- Scans C:\ drive for top space-consuming folders
- Visual progress bar during scanning
- Bar chart representation of top 5 largest folders
- Displays folder sizes in MB for easy comparison

### 3. **Customizable Themes**
- Four color schemes: Red, Blue, Green, and Default
- Theme persistence (saved to `theme.cfg` file)
- Applied across entire application interface

## 🛠️ Technical Details

### Technologies Used
- **Language**: C++17
- **Platform**: Windows (requires Windows API)
- **Libraries**: 
  - `windows.h` - Windows API functions
  - `<filesystem>` - File system operations (C++17)
  - Standard C++ libraries

### Key Components

#### Performance Monitoring
```cpp
getCPUUsage()        // Measures CPU load over 100ms interval
getMemoryUsage()     // Queries physical memory statistics
getDiskUsage()       // Retrieves disk space information
getSystemUptime()    // Calculates time since system boot
```

#### Folder Analysis
```cpp
getFolderSize()      // Recursively calculates folder size
displayFolderUsage() // Scans and displays top folders
showFolderProgressBar() // Visual feedback during scanning
```

#### Theme Management
```cpp
setConsoleColor()    // Changes console text color
saveTheme()          // Persists theme to file
loadTheme()          // Loads saved theme on startup
```

### Compilation

**Using g++ (MinGW-w64):**
```bash
g++ -std=c++17 -o SystemMonitor.exe SystemMonitor.cpp
```

**Using Visual Studio:**
1. Create a new Console App project
2. Set C++ Language Standard to ISO C++17
3. Add the source code
4. Build the solution (Ctrl+Shift+B)

### Running the Application
```bash
SystemMonitor.exe
```

## 📖 Usage Guide

### Main Menu Options

#### Option 1: Scan System
Performs a comprehensive system health check:
1. Displays animated progress bar
2. Shows CPU usage percentage
3. Reports memory usage (used/total)
4. Displays disk usage (used/total)
5. Shows system uptime
6. Provides health summary with warnings if:
   - CPU usage > 85%
   - Memory usage > 90%
   - Disk usage > 90%

**Example Output:**
```
CPU Usage: 42.5%
Memory Usage: 8192 MB / 16384 MB
Disk Usage (C:\): 256 GB / 512 GB
System Uptime: 2d 5h 23m 45s

===== SYSTEM HEALTH SUMMARY =====
[GOOD] Your system is running smoothly.
```

#### Option 2: Show Folder Usage
Analyzes C:\ drive folder sizes:
1. Scans all top-level folders
2. Shows progress bar during scan
3. Displays top 5 largest folders with visual bars
4. Sizes shown in MB

**Example Output:**
```
Scanning folders: [##########################] 100%

Top Folder Usage (Approx.):
Users [##################################################] 102400 MB
Program Files [#################################] 71680 MB
Windows [#########################] 51200 MB
ProgramData [##########] 20480 MB
```

#### Option 3: Change Theme
Customize console colors:
1. Choose from 4 color options
2. Theme applies immediately
3. Saved automatically for future sessions

#### Option 0: Exit
Closes the application gracefully.

## 🔍 How It Works

### CPU Usage Calculation
The application samples system times twice (100ms apart) and calculates CPU usage using this formula:
```
CPU Usage = (1 - Idle Time Difference / Total Time Difference) × 100%
```

### Memory Monitoring
Uses `GlobalMemoryStatusEx()` to query:
- Total physical memory installed
- Available physical memory
- Used memory (calculated as Total - Available)

### Disk Space Analysis
Leverages `GetDiskFreeSpaceEx()` to retrieve:
- Total disk capacity
- Free space available
- Used space (calculated as Total - Free)

### Folder Size Calculation
Recursively traverses directories using C++17 filesystem:
- Iterates through all files in subdirectories
- Accumulates file sizes
- Handles permission errors gracefully
- Skips inaccessible files/folders

### Theme Persistence
- Saves theme choice as integer to `theme.cfg`
- Loads theme on application startup
- Uses Windows Console API to set text colors

## ⚠️ Important Notes

### Permissions
- Some folders require administrator privileges
- Running without admin may skip protected directories
- Disk operations are read-only (safe to use)

### Performance Considerations
- Folder scanning can be slow for large directories
- CPU measurement takes 100ms per sample
- Recursive folder traversal may take time on full drives

### Limitations
- **Windows-only**: Uses Windows API extensively
- **C: Drive Default**: Folder analysis targets C:\ only
- **Single Drive**: Disk monitoring shows C:\ drive
- **Top 5 Limit**: Only displays 5 largest folders

## 🐛 Error Handling

The application handles various errors:
- **Permission Denied**: Skips inaccessible folders during scan
- **Invalid Drive**: Shows error message if drive unavailable
- **File Access Errors**: Continues scan, ignoring problematic files
- **Invalid Menu Input**: Prompts user to try again

### Code Modifications
```cpp
// Monitor different drive
getDiskUsage("D:\\");

// Change folder limit from 5 to 10
if (count++ >= 10) break;

// Adjust critical thresholds
if(cpuUsage > 80) critical = true;  // Change from 85%
```

## 📚 Learning Resources

### Concepts Demonstrated
- **Windows API Programming**: System information retrieval
- **File System Operations**: C++17 filesystem library
- **Error Handling**: Try-catch blocks and permission checks
- **Data Structures**: Vectors, pairs, sorting algorithms
- **Console Manipulation**: Colors, progress bars, screen clearing
- **File I/O**: Configuration persistence
- **Time Calculations**: Converting system ticks to readable format


## 📄 License

This is educational software provided as-is for learning purposes.

