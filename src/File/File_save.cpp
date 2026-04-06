//#include"../include/File/File_save.h"
#include "../../include/File/File_save.h" 
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string File_save::generateTimestampFilename(const std::string& prefix,const std::string& ext) {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm timeinfo;
#ifdef _WIN32
    localtime_s(&timeinfo, &now);
#else
    localtime_r(&now, &timeinfo);
#endif

    std::stringstream ss;
    ss << prefix << std::put_time(&timeinfo, "%Y%m%d_%H%M%S") << ext;
    return ss.str();
}

bool File_save::writeFile(const std::string& filename, const std::string& content) {
    // writeFile 預設用覆蓋模式（比較直覺）
    return overwriteFile(filename, content);
}

bool File_save::appendFile(const std::string& filename, const std::string& content) {
    if (filename.empty()) return false;

    std::ofstream fout(filename, std::ios::app);
    if (!fout.is_open()) return false;

    fout << content;
    // 如果內容沒有換行，自動加一個
    if (!content.empty() && content.back() != '\n') {
        fout << '\n';
    }

    return true;
}

bool File_save::overwriteFile(const std::string& filename, const std::string& content) {
    if (filename.empty()) return false;

    std::ofstream fout(filename, std::ios::trunc);
    if (!fout.is_open()) return false;

    fout << content;
    if (!content.empty() && content.back() != '\n') {
        fout << '\n';
    }

    return true;
}


