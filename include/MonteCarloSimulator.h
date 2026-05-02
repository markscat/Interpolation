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

namespace MonteCarloHelper {
    // 這些是「非 template」的輔助函式，實作放在 .cpp
    double computeMean(const std::vector<double>& values);
    double computeStdDev(const std::vector<double>& values, double mean);
    double findMin(const std::vector<double>& values);
    double findMax(const std::vector<double>& values);

/*    void saveToCSV(const std::string& filename,
        const std::vector<double>& outputs,
        const std::vector<bool>& specs);*/
}

template<typename InputType, typename OutputType>
struct SimulationRecord {
    InputType input;
    OutputType output;
    bool withinSpec;
};

template<typename OutputType>
struct Statistics {
    double mean = 0.0;
    double stddev = 0.0;
    double min = 0.0;
    double max = 0.0;
    double yield = 0.0;
};

template<typename InputType, typename OutputType>
class MonteCarloSimulator {
public:
    MonteCarloSimulator();
    ~MonteCarloSimulator();

    void setParameterSampler(const std::string& paramName,
        std::function<double()> sampler);
    void setModel(std::function<OutputType(const InputType&)> model);
    void setSpecLimit(std::function<bool(const OutputType&)> spec);
    void setSeed(unsigned int seed);
    void run(int numSimulations);
    void clear();

    const std::vector<SimulationRecord<InputType, OutputType>>& getResults() const;
    const Statistics<OutputType>& getStatistics() const;

    InputType generateRandomInput() {
        InputType input;
        return input;
    }

    //void printStatistics() const;
    //void exportToCSV(const std::string& filename) const;

private:
    void computeStatistics() {
        
    if (m_results.empty()) return;

    std::vector<double> values;
    values.reserve(m_results.size());
    for (const auto& rec : m_results) {
        values.push_back(toDouble(rec.output));
    }

    m_statistics.mean = MonteCarloHelper::computeMean(values);
    m_statistics.stddev = MonteCarloHelper::computeStdDev(values, m_statistics.mean);
    m_statistics.min = MonteCarloHelper::findMin(values);
    m_statistics.max = MonteCarloHelper::findMax(values);

    int passed = 0;
    for (const auto& rec : m_results) {
        if (rec.withinSpec) passed++;
    }
    m_statistics.yield = static_cast<double>(passed) / m_results.size();
    }

    // 輔助函式：把 OutputType 轉成 double（使用者需要特化這個）
    double toDouble(const OutputType& value) const {
        return static_cast<double>(value);
    }

    std::map<std::string, std::function<double()>> m_samplers;
    std::function<OutputType(const InputType&)> m_model;
    std::function<bool(const OutputType&)> m_spec;
    std::vector<SimulationRecord<InputType, OutputType>> m_results;
    Statistics<OutputType> m_statistics;
    std::mt19937 m_rng;
    bool m_hasModel = false;
    bool m_hasSpec = false;
}

// ============================================
// Template 實作（必須放在 .h，因為 C++ 的限制）
// ============================================

template<typename InputType, typename OutputType>
MonteCarloSimulator<InputType, OutputType>::MonteCarloSimulator() {
    unsigned int seed = static_cast<unsigned int>(
        std::chrono::steady_clock::now().time_since_epoch().count()
        );
    m_rng.seed(seed);
}

template<typename InputType, typename OutputType>
MonteCarloSimulator<InputType, OutputType>::~MonteCarloSimulator() {}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::setParameterSampler(
    const std::string& paramName, std::function<double()> sampler) {
    m_samplers[paramName] = sampler;
}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::setModel(
    std::function<OutputType(const InputType&)> model) {
    m_model = model;
    m_hasModel = true;
}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::setSpecLimit(
    std::function<bool(const OutputType&)> spec) {
    m_spec = spec;
    m_hasSpec = true;
}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::setSeed(unsigned int seed) {
    m_rng.seed(seed);
}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::run(int numSimulations) {
    if (!m_hasModel) {
        std::cerr << "錯誤：請先使用 setModel() 設定系統模型\n";
        //'cerr': 並非 'std' 的成員
        return;
    }

    clear();
    m_results.reserve(numSimulations);

    for (int i = 0; i < numSimulations; ++i) {
        InputType input = generateRandomInput();
        OutputType output = m_model(input);
        bool withinSpec = m_hasSpec ? m_spec(output) : true;
        m_results.push_back({ input, output, withinSpec });
    }

    computeStatistics();
}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::clear() {
    m_results.clear();
}

template<typename InputType, typename OutputType>
const std::vector<SimulationRecord<InputType, OutputType>>&
MonteCarloSimulator<InputType, OutputType>::getResults() const {
    return m_results;
}

template<typename InputType, typename OutputType>
const Statistics<OutputType>&
MonteCarloSimulator<InputType, OutputType>::getStatistics() const {
    return m_statistics;
}

template<typename InputType, typename OutputType>

/*
void MonteCarloSimulator<InputType, OutputType>::printStatistics() const {
    std::cout << "\n========== 蒙地卡罗模拟统计 ==========\n";
    std::cout << "模拟次数: " << m_results.size() << "\n";
    std::cout << "平均值:   " << m_statistics.mean << "\n";
    std::cout << "标准差:   " << m_statistics.stddev << "\n";
    std::cout << "最小值:   " << m_statistics.min << "\n";
    std::cout << "最大值:   " << m_statistics.max << "\n";
    std::cout << "良率:     " << (m_statistics.yield * 100) << "%\n";
    std::cout << "=====================================\n";
}*/

/*
template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::exportToCSV(
    const std::string& filename) const {

    std::vector<double> outputs;
    std::vector<bool> specs;
    outputs.reserve(m_results.size());
    specs.reserve(m_results.size());

    for (const auto& rec : m_results) {
        outputs.push_back(toDouble(rec.output));
        specs.push_back(rec.withinSpec);
    }

    MonteCarloHelper::saveToCSV(filename, outputs, specs);
}*/
/*
template<typename InputType, typename OutputType>
inline InputType MonteCarloSimulator<InputType, OutputType>::generateRandomInput() {
    InputType input;
    return input;
}*/


#endif