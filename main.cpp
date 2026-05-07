//#include "./include/MonteCarloSimulator.h"
#include "./include/MonteCarloNoise.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 定義各種波形函數
double sineWave(double time) {
    return std::sin(2 * M_PI * time);  // 1Hz 正弦波
}

double squareWave(double time) {
    double period = 1.0;  // 1Hz
    double t_mod = std::fmod(time, period);
    return (t_mod < period / 2) ? 1.0 : -1.0;
}

double triangleWave(double time) {
    double period = 1.0;
    double t_mod = std::fmod(time, period);
    double p = t_mod / period;
    if (p < 0.25) return 4 * p;
    if (p < 0.75) return 2 - 4 * p;
    return 4 * p - 4;
}

double sawtoothWave(double time) {
    double period = 1.0;
    double t_mod = std::fmod(time, period);
    return 2 * (t_mod / period) - 1;
}

int main() {
    using namespace MonteCarloNoise;
    
    // 範例 1: 正弦波 + 5% 雜訊
    std::cout << "=== 範例 1: 正弦波 + 5% 雜訊 ===\n";
    NoiseSimulator sim;
    /*E0020 識別項 "NoiseSimulator" 未定義
    */
    sim.setWaveform(sineWave);
    sim.setMultiplicativeNoise(0.05);  // ±5%
    
    auto stats = sim.simulateAtTime(0.25, 10000);  // 在 t=0.25 時取 10000 個樣本
    std::cout << "理想值: " << sineWave(0.25) << "\n";
    std::cout << "平均值: " << stats.mean << "\n";
    std::cout << "標準差: " << stats.stddev << "\n";
    std::cout << "範圍: [" << stats.min << ", " << stats.max << "]\n\n";
    
    // 範例 2: 方波 + 10% 雜訊，多個時間點
    std::cout << "=== 範例 2: 方波 + 10% 雜訊 ===\n";
    sim.setWaveform(squareWave);
    sim.setMultiplicativeNoise(0.10);
    
    std::vector<double> times;
    for (int i = 0; i <= 100; ++i) {
        times.push_back(i * 0.01);  // 0 到 1 秒
    }
    
    auto results = sim.simulateAtTimes(times, 1000);
    
    // 輸出部分結果
    for (size_t i = 0; i < results.size(); i += 10) {
        std::cout << "t=" << std::fixed << std::setprecision(2) << times[i]
                  << " 理想=" << std::setw(5) << squareWave(times[i])
                  << " 平均=" << std::setw(6) << std::setprecision(4) << results[i].mean
                  << " ±" << std::setw(6) << results[i].stddev << "\n";
    }
    std::cout << "\n";
    
    // 範例 3: 使用混合雜訊（乘法 + 加法）
    std::cout << "=== 範例 3: 三角波 + 混合雜訊 ===\n";
    NoiseSimulator sim2;
    sim2.setWaveform(triangleWave);
    sim2.setMixedNoise(0.05, 0.02);  // ±5% 乘性 + 標準差 0.02 加性
    
    auto stats2 = sim2.simulateAtTime(0.3, 5000);
    std::cout << "理想值: " << triangleWave(0.3) << "\n";
    std::cout << "平均值: " << stats2.mean << "\n";
    std::cout << "標準差: " << stats2.stddev << "\n\n";
    
    // 範例 4: 使用便捷函數（快速取得統計）
    std::cout << "=== 範例 4: 便捷函數 ===\n";
    auto quickStats = computeWaveformNoise(sineWave, 0.5, 1000, 0.03);
    /*
    * 識別項 "computeWaveformNoise" 未定義
    */
    std::cout << "sin(0.5*2π) + 3% noise: 平均=" << quickStats.mean 
              << ", 標準差=" << quickStats.stddev << "\n\n";
    
    // 範例 5: 如果需要輸出 CSV 檔案（由 main 決定）
    std::cout << "=== 範例 5: 輸出 CSV 檔案（可選）===\n";
    
    NoiseSimulator csvSim;
    csvSim.setWaveform(sineWave);
    csvSim.setMultiplicativeNoise(0.10);
    
    std::vector<double> csvTimes;
    for (double t = 0; t <= 2.0; t += 0.01) {
        csvTimes.push_back(t);
    }
    
    auto csvResults = csvSim.simulateAtTimes(csvTimes, 1000);
    
    std::ofstream file("waveform_noise.csv");
    file << "time,ideal,mean,lower,upper\n";
    file << std::fixed << std::setprecision(6);
    
    for (size_t i = 0; i < csvTimes.size(); ++i) {
        double ideal = sineWave(csvTimes[i]);
        file << csvTimes[i] << ","
             << ideal << ","
             << csvResults[i].mean << ","
             << csvResults[i].mean - csvResults[i].stddev << ","
             << csvResults[i].mean + csvResults[i].stddev << "\n";
    }
    
    file.close();
    std::cout << "已輸出到 waveform_noise.csv\n";
    
    return 0;
}