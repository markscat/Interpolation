#ifndef MONTECARLOSIMULATOR_H
#define MONTECARLOSIMULATOR_H


#include <iostream>      // std::cerr, std::cout
#include <functional>    // std::function
#include <vector>        // std::vector
#include <map>           // std::map
#include <string>        // std::string
#include <random>        // std::mt19937
#include <chrono>        // std::chrono
#include <numeric>       // std::accumulate
#include <cmath>         // std::sqrt



// 統計結果
/**
*   struct Statistics
 * @brief 統計結果結構，包含平均值、標準差、最小值、最大值和合格率
 * @details 這個結構用於存儲 Monte Carlo 模擬的統計結果，包括：
 * @param mean 模擬結果的平均值
 * @param stddev 模擬結果的標準差
 * @param min 模擬結果的最小值
 * @param max 模擬結果的最大值
 */

struct Statistics {
    double mean = 0.0;
    double stddev = 0.0;
    double min = 0.0;
    double max = 0.0;
    double yield = 0.0;
};

class MonteCarloSimulator {
public:
    MonteCarloSimulator();
    ~MonteCarloSimulator();

    // 設定輸入參數的取樣器
    // paramName: 參數名稱
    // sampler: 回傳 double 的函式（例如亂數產生器）
    void setParameterSampler(const std::string& paramName,
        std::function<double()> sampler);

    // 設定系統模型（使用者完全掌控）
    // model: 輸入一個 void*（指向使用者的資料結構），輸出一個 double 結果
    void setModel(std::function<double(void* input)> model);

    // 設定規格檢查（選用）
    // spec: 輸入結果，回傳是否合格
    void setSpecLimit(std::function<bool(double result)> spec);

    // 設定隨機種子
    void setSeed(unsigned int seed);

    // 執行模擬
    // numSimulations: 模擬次數
    // inputSize: 輸入資料結構的大小（byte數），用於複製
    // defaultInput: 使用者的預設輸入結構（指標）
    void run(int numSimulations, size_t inputSize, void* defaultInput);

    // 取得所有模擬結果
    const std::vector<double>& getResults() const;

    // 取得所有合格標記
    const std::vector<bool>& getSpecResults() const;

    // 取得統計結果
    const Statistics& getStatistics() const;

    // 清除所有資料
    void clear();

    static std::mt19937& getGlobalRng() {
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        return rng;
    };

private:
    void computeStatistics();

    std::map<std::string, std::function<double()>> m_samplers;
    std::function<double(void* input)> m_model;
    std::function<bool(double result)> m_spec;

    std::vector<double> m_results;
    std::vector<bool> m_specResults;
    Statistics m_statistics;

    std::mt19937 m_rng;
    bool m_hasModel = false;
    bool m_hasSpec = false;
};

#endif


