#ifndef MONTECARLOSIMULATOR_H
#define MONTECARLOSIMULATOR_H

#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <string>
#include <random>
#include <chrono>
#include <numeric>
#include <cmath>
#include <algorithm> // 補上這個

namespace MonteCarloHelper {
    double computeMean(const std::vector<double>& values);
    double computeStdDev(const std::vector<double>& values, double mean);
    double findMin(const std::vector<double>& values);
    double findMax(const std::vector<double>& values);
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
    
    // 讓使用者決定如何產生隨機輸入
    void setInputGenerator(std::function<InputType()> gen) { m_inputGen = gen; }
    void setModel(std::function<OutputType(const InputType&)> model);
    void setSpecLimit(std::function<bool(const OutputType&)> spec);
    
    void run(int numSimulations);
    void clear();

    const std::vector<SimulationRecord<InputType, OutputType>>& getResults() const { return m_results; }
    const Statistics<OutputType>& getStatistics() const { return m_statistics; }

    // 提供一個虛擬函式或讓使用者可以覆寫轉換邏輯
    virtual double toDouble(const OutputType& value) const {
        return static_cast<double>(value);
    }

private:
    void computeStatistics();

    std::function<InputType()> m_inputGen; // 修改這裡
    std::function<OutputType(const InputType&)> m_model;
    std::function<bool(const OutputType&)> m_spec;
    std::vector<SimulationRecord<InputType, OutputType>> m_results;
    Statistics<OutputType> m_statistics;
    bool m_hasModel = false;
    bool m_hasSpec = false;
}; // <-- 修正：加上分號

// --- 模板實作部分 ---

template<typename InputType, typename OutputType>
MonteCarloSimulator<InputType, OutputType>::MonteCarloSimulator() {}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::setModel(std::function<OutputType(const InputType&)> model) {
    m_model = model;
    m_hasModel = true;
}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::setSpecLimit(std::function<bool(const OutputType&)> spec) {
    m_spec = spec;
    m_hasSpec = true;
}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::run(int numSimulations) {
    if (!m_hasModel || !m_inputGen) {
        std::cerr << "Error: Model or Input Generator not set!\n";
        return;
    }

    m_results.clear();
    m_results.reserve(numSimulations);

    for (int i = 0; i < numSimulations; ++i) {
        InputType input = m_inputGen(); // 使用產生器
        OutputType output = m_model(input);
        bool withinSpec = m_hasSpec ? m_spec(output) : true;
        m_results.push_back({ input, output, withinSpec });
    }
    computeStatistics();
}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::computeStatistics() {
    if (m_results.empty()) return;

    std::vector<double> values;
    values.reserve(m_results.size());
    int passed = 0;

    for (const auto& rec : m_results) {
        values.push_back(toDouble(rec.output));
        if (rec.withinSpec) passed++;
    }

    m_statistics.mean = MonteCarloHelper::computeMean(values);
    m_statistics.stddev = MonteCarloHelper::computeStdDev(values, m_statistics.mean);
    m_statistics.min = MonteCarloHelper::findMin(values);
    m_statistics.max = MonteCarloHelper::findMax(values);
    m_statistics.yield = static_cast<double>(passed) / m_results.size();
}

template<typename InputType, typename OutputType>
void MonteCarloSimulator<InputType, OutputType>::clear() {
    m_results.clear();
}

#endif