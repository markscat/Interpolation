
#include "include/Matrix.h"
#include "./include/MonteCarloSimulator.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <cmath>
#include <iomanip>



#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


// 定義正弦波的輸入參數
struct SineWaveInput {
    double amplitude;   // 振幅
    double frequency;   // 頻率 (Hz)
    double time;        // 時間點 (秒)
};

// 全域亂數產生器（讓 model 可以存取）
//std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
std::mt19937& rng = MonteCarloSimulator::getGlobalRng();

// 產生雜訊的函式
// noiseLevel: 雜訊強度 (例如 0.1 表示振幅的 ±10%)
double addNoise(double value, double noiseLevel) {
    std::uniform_real_distribution<> dist(-noiseLevel, noiseLevel);
    return value + value * dist(rng);  
}

// Model 函式：產生帶雜訊的正弦波
double sineWaveModel(void* input) {
    SineWaveInput* sw = static_cast<SineWaveInput*>(input);

    // 理想正弦值
    double ideal = sw->amplitude * std::sin(2 * M_PI * sw->frequency * sw->time);

    // 加入雜訊（雜訊強度可以根據需要調整，例如振幅的 ±5%）
    //double noisy = addNoise(ideal, 0.05);
    double noisy = addNoise(ideal, 0.50);  // ±50%
    //double noisy = addNoise(ideal, 0.01);  // ±1%

    return noisy;
}


//#define Matrix_
//#define _MonteCarloSimulator
#define _sinWave

#ifdef _MonteCarloSimulator

// 使用者定義自己的輸入結構
struct AmplifierInput {
    double R1;
    double R2;
    double Vcc;
};

// 全域亂數產生器（使用者自己掌控）
std::random_device rd;
std::mt19937 gen(rd());

// 使用者定義的 model
double myModel(void* input) {
    AmplifierInput* amp = static_cast<AmplifierInput*>(input);

    // 每次模擬，R1 和 R2 都在 ±5% 內隨機跳動
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> distR1(10000, 500);   // 10kΩ ±5%
    std::normal_distribution<> distR2(5000, 250);    // 5kΩ ±5%

    amp->R1 = distR1(gen);
    amp->R2 = distR2(gen);

    return -amp->R2 / amp->R1;
}
/*
double myModel(void* input) {
    AmplifierInput* amp = static_cast<AmplifierInput*>(input);

    // 在這裡加入隨機誤差（使用者自己決定怎麼做）
    // 也可以從全域變數讀取取樣值

    return -amp->R2 / amp->R1;  // 增益
}
*/
#endif // _MonteCarloSimulator


int main() {

#ifdef Matrix_
    // 準備兩組 5x5 的資料（這裡完全沒有迴圈，直接寫死）
    double dataA[] = { 12,45,78,23,56, 89,34,11,67,90, 22,55,88,33,66, 44,77,99,10,25, 15,35,45,65,75 };
    double dataB[] = { 98,76,54,32,10, 13,31,57,75,99, 24,42,68,86,11, 35,53,79,97,22, 46,64,80,02,33 };

    Matrix A(5, 5), B(5, 5);

    A.load(dataA); // 一次倒進去
    B.load(dataB); // 一次倒進去

    std::cout << "矩陣 A:\n"; A.print();
    std::cout << "\n矩陣 B:\n"; B.print();
    std::cout << "\n結果 A * B:\n"; A.multiply(B).print();


    /**
    _* MonteCarloSimulator
    */


	#elif defined(_MonteCarloSimulator)

    MonteCarloSimulator sim;

    // 設定 model
    sim.setModel(myModel);

    // 設定規格（選用）
    sim.setSpecLimit([](double gain) {
        return gain >= -0.55 && gain <= -0.45;
        });

    // 準備預設輸入
    AmplifierInput defaultInput = { 10000.0, 5000.0, 12.0 };

    // 執行模擬
    sim.run(20, sizeof(AmplifierInput), &defaultInput);



    // 印出每次的結果
    auto& results = sim.getResults();
    for (size_t i = 0; i < results.size(); ++i) {
        if(-0.55 <= results[i] && results[i] <= -0.45){
            std::cout << "模擬 " << i + 1 << ": 增益 = " << results[i] << " (合格)\n";
        }
        else{
            std::cout << "模擬 " << i + 1 << ": 增益 = " << results[i] << " (不合格)\n";
		}
    }


    // 輸出統計
    auto stats = sim.getStatistics();
    std::cout << "平均值: " << stats.mean << "\n";
    std::cout << "標準差: " << stats.stddev << "\n";
    std::cout << "良率: " << stats.yield * 100 << "%\n";

#elif defined(_sinWave)
    // 建立模擬器
    MonteCarloSimulator sim;
    sim.setModel(sineWaveModel);

    // 準備預設輸入（固定參數，時間會在迴圈中改變）
    SineWaveInput defaultInput = { 1.0, 1.0, 0.0 };

    // 設定要模擬的時間點（例如 0 到 1 秒，每 0.01 秒取一點）
    double startTime = 0.0;
    double endTime = 1.0;
    double step = 0.001;

    std::ofstream file("sine_wave_with_noise.csv");
    file << std::fixed << std::setprecision(6);
    file << "time,ideal,noisy_mean,noisy_lower,noisy_upper\n";

    // 對每一個時間點，執行蒙地卡羅模擬
    for (double t = startTime; t <= endTime; t += step) {
        // 更新時間
        defaultInput.time = t;

        // 跑一次模擬（只看第一次的結果）
        sim.run(1, sizeof(SineWaveInput), &defaultInput);
        double singleResult = sim.getResults()[0];

        // 執行 1000 次模擬（取得這個時間點的雜訊分佈）
        sim.run(1000, sizeof(SineWaveInput), &defaultInput);
        auto stats = sim.getStatistics();

        // 取得統計結果
        //auto stats = sim.getStatistics();
        double ideal = defaultInput.amplitude * std::sin(2 * M_PI * defaultInput.frequency * t);

        file << t << ","
            << ideal << ","
            << stats.mean << ","
            << stats.mean - stats.stddev << ","
            << stats.mean + stats.stddev << "\n";

        // 清除結果，準備下一個時間點
        sim.clear();
    }

    file.close();

    std::cout << "已輸出到 sine_wave_with_noise.csv\n";
    std::cout << "用 Excel 或其他軟體繪圖時：\n";
    std::cout << "  - ideal: 理想正弦波\n";
    std::cout << "  - noisy_mean: 受雜訊影響的平均值\n";
    std::cout << "  - noisy_lower / noisy_upper: 雜訊波動的範圍\n";


    /*
    std::mt19937 rng(42);  // 固定種子，結果可重現
    std::normal_distribution<> noise(0.0, 0.1);  // 平均值 0，標準差 0.1

    double f = 1.0;        // 頻率 1Hz
    double duration = 1.0; // 1 秒
    double dt = 0.01;      // 每 0.01 秒取一點

    std::vector<double> time, ideal, realistic;

    for (double t = 0; t <= duration; t += dt) {
        time.push_back(t);

        double ideal_val = sin(2 * M_PI * f * t);
        ideal.push_back(ideal_val);

        double realistic_val = ideal_val + noise(rng);
        realistic.push_back(realistic_val);
    }

    // 輸出到 CSV
    std::ofstream file("waveform.csv");
    file << "time,ideal,realistic\n";
    for (size_t i = 0; i < time.size(); ++i) {
        file << time[i] << "," << ideal[i] << "," << realistic[i] << "\n";
    }
    */

#endif

    return 0;
}
