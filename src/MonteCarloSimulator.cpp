#include "../include/MonteCarloSimulator.h"
#include <chrono>
#include <cmath>
#include <cstring>
#include <numeric>
#include <algorithm>
#include <iostream>

MonteCarloSimulator::MonteCarloSimulator() {
    unsigned int seed = static_cast<unsigned int>(
        std::chrono::steady_clock::now().time_since_epoch().count()
        );
    m_rng.seed(seed);
}

MonteCarloSimulator::~MonteCarloSimulator() {}

void MonteCarloSimulator::setParameterSampler(const std::string& paramName,
    std::function<double()> sampler) {
    m_samplers[paramName] = sampler;
}

/**
* void MonteCarloSimulator::setModel(std::function<double(void* input)> model) 
* 
* @brief 設定系統模型
* @param model 一個函式，輸入使用者的資料結構（void*），回傳模擬結果（double）
* @details 這個函式允許使用者定義一個系統模型，用於計算每次模擬的結果。
           使用者可以根據自己的需求來定義這個函式，例如：
* - 計算物理系統的輸出
* - 模擬金融模型的結果
* - 自訂複雜的計算邏輯
*/
void MonteCarloSimulator::setModel(std::function<double(void* input)> model) {
    m_model = model;
    m_hasModel = true;
}
/**
* void MonteCarloSimulator::setSpecLimit(std::function<bool(double result)> spec) 
* @brief 設定規格檢查
* @param spec 一個函式，輸入模擬結果（double），回傳是否合格（bool）
* @details 這個函式允許使用者定義一個規格檢查函式，用於評估每次模擬的結果是否符合特定標準。
           使用者可以根據自己的需求來定義這個函式，例如：
* - 檢查結果是否在某個範圍內
* - 檢查結果是否大於或小於某個閾值
* - 自訂複雜的邏輯判斷
*/
void MonteCarloSimulator::setSpecLimit(std::function<bool(double result)> spec) {
    m_spec = spec;
    m_hasSpec = true;
}

void MonteCarloSimulator::setSeed(unsigned int seed) {
    m_rng.seed(seed);
}

void MonteCarloSimulator::run(int numSimulations, size_t inputSize, void* defaultInput) {
    if (!m_hasModel) {
        return;
    }

    clear();
    m_results.reserve(numSimulations);
    if (m_hasSpec) {
        m_specResults.reserve(numSimulations);
    }

    // 準備一塊記憶體存放輸入資料
    void* input = malloc(inputSize);

    for (int i = 0; i < numSimulations; ++i) {
        // 複製預設輸入
        memcpy(input, defaultInput, inputSize);

        // 更新每個參數（將取樣值寫入 input 的各個欄位）
        // 注意：這需要使用者自行處理，因為我們不知道 input 的結構
        // 使用者的 model 內部會讀取 input 的欄位

        double result = m_model(input);
        m_results.push_back(result);

        if (m_hasSpec) {
            m_specResults.push_back(m_spec(result));
        }
    }

    free(input);
    computeStatistics();
}

void MonteCarloSimulator::clear() {
    m_results.clear();
    m_specResults.clear();
}

const std::vector<double>& MonteCarloSimulator::getResults() const {
    return m_results;
}

const std::vector<bool>& MonteCarloSimulator::getSpecResults() const {
    return m_specResults;
}

const Statistics& MonteCarloSimulator::getStatistics() const {
    return m_statistics;
}

void MonteCarloSimulator::computeStatistics() {
    if (m_results.empty()) return;

    double sum = std::accumulate(m_results.begin(), m_results.end(), 0.0);
    m_statistics.mean = sum / m_results.size();

    double sq_sum = 0.0;
    for (double v : m_results) {
        double diff = v - m_statistics.mean;
        sq_sum += diff * diff;
    }
    m_statistics.stddev = std::sqrt(sq_sum / m_results.size());

    m_statistics.min = *std::min_element(m_results.begin(), m_results.end());
    m_statistics.max = *std::max_element(m_results.begin(), m_results.end());

    if (m_hasSpec && !m_specResults.empty()) {
        int passed = std::count(m_specResults.begin(), m_specResults.end(), true);
        /*
1>D:\for work\temp\workshorp\PC\Qt\Interpolation\src\MonteCarloSimulator.cpp(125,20): warning C4244: '正在初始化': 將 '__int64' 轉換為 'int'，由於類型不同，可能導致資料遺失
        *
        */
        m_statistics.yield = static_cast<double>(passed) / m_specResults.size();
    }
    else {
        m_statistics.yield = 1.0;
    }
}
