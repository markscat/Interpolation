#include "../include/MonteCarloNoise.h"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace MonteCarloNoise {

    // 全域靜態隨機產生器（確保不同時間點的一致性）
    static std::mt19937& getGlobalRng() {
        static std::mt19937 rng(
            static_cast<unsigned int>(
                std::chrono::steady_clock::now().time_since_epoch().count()
                )
        );
        return rng;
    }

    NoiseSimulator::NoiseSimulator(unsigned int seed)
        : m_noiseType(1)
        , m_noiseParam1(0.05)
        , m_noiseParam2(0.0)
        , m_constantOffset(0.0)
        , m_hasWaveform(false) {

        if (seed == 0) {
            seed = static_cast<unsigned int>(
                std::chrono::steady_clock::now().time_since_epoch().count()
                );
        }
        m_rng.seed(seed);
    }

    void NoiseSimulator::setWaveform(WaveformFunction waveform) {
        m_waveform = waveform;
        m_hasWaveform = true;
    }

    void NoiseSimulator::setNoiseType(const std::string& type, double level) {
        m_noiseType = 0;
        if (type == "additive") {
            m_noiseParam1 = level;
        }
        else if (type == "multiplicative") {
            m_noiseParam1 = level;
        }
        else if (type == "mixed") {
            m_noiseParam1 = level;
            m_noiseParam2 = 0.0;
        }
        else {
            throw std::invalid_argument("Unknown noise type: " + type);
        }
    }

    void NoiseSimulator::setAdditiveNoise(double stddev) {
        m_noiseType = 0;
        m_noiseParam1 = stddev;
    }

    void NoiseSimulator::setMultiplicativeNoise(double ratio) {
        m_noiseType = 1;
        m_noiseParam1 = ratio;
    }

    void NoiseSimulator::setMixedNoise(double multiplicativeRatio, double additiveStddev) {
        m_noiseType = 2;
        m_noiseParam1 = multiplicativeRatio;
        m_noiseParam2 = additiveStddev;
    }

    void NoiseSimulator::setConstantOffset(double offset) {
        m_constantOffset = offset;
    }

    double NoiseSimulator::addNoise(double value) {
        double result = value;

        if (m_noiseType == 0) {
            // 加法雜訊：value + N(0, stddev)
            std::normal_distribution<> dist(0.0, m_noiseParam1);
            result = value + dist(m_rng);
        }
        else if (m_noiseType == 1)   {
            // 乘法雜訊：value * (1 + uniform(-ratio, ratio))
            std::uniform_real_distribution<> dist(-m_noiseParam1, m_noiseParam1);
            result = value * (1.0 + dist(m_rng));
        }
        else if (m_noiseType == 2) {
            // 混合雜訊：value * (1 + uniform(-ratio, ratio)) + N(0, stddev)
            std::uniform_real_distribution<> multDist(-m_noiseParam1, m_noiseParam1);
            std::normal_distribution<> addDist(0.0, m_noiseParam2);
            result = value * (1.0 + multDist(m_rng)) + addDist(m_rng);
        }

        // 加上常數偏移
        result += m_constantOffset;

        return result;
    }

    NoiseStatistics NoiseSimulator::simulateAtTime(double time, int numSamples) {
        if (!m_hasWaveform) {
            throw std::runtime_error("No waveform function set");
        }

        std::vector<double> samples;
        samples.reserve(numSamples);

        // 計算理想值
        double ideal = m_waveform(time);

        for (int i = 0; i < numSamples; ++i) {
            samples.push_back(addNoise(ideal));
        }

        // 計算統計
        NoiseStatistics stats;
        stats.sampleCount = numSamples;

        double sum = 0.0;
        double minVal = samples[0];
        double maxVal = samples[0];

        for (size_t i = 0; i < samples.size(); ++i) {
            sum += samples[i];
            if (samples[i] < minVal) minVal = samples[i];
            if (samples[i] > maxVal) maxVal = samples[i];
        }

            
        // 平均值
        //stats.mean = std::accumulate(samples.begin(), samples.end(), 0.0) / numSamples;
        stats.mean = sum / static_cast<double>(samples.size());
        // 最小值、最大值
        //stats.min = *std::min_element(samples.begin(), samples.end());
        //stats.max = *std::max_element(samples.begin(), samples.end());
        stats.min = minVal;
        stats.max = maxVal;
        
        // 變異數和標準差
        
        double sq_sum = 0.0;
        for (size_t i = 0; i < samples.size(); ++i) {
            double diff = samples[i] - stats.mean;
            sq_sum += diff * diff;
        }

        /*
        for (double v : samples) {
            double diff = v - stats.mean;
            sq_sum += diff * diff;
        }*/

        stats.var = sq_sum / numSamples;
        stats.stddev = std::sqrt(stats.var);

        return stats;
    }

    std::vector<NoiseStatistics> NoiseSimulator::simulateAtTimes(const std::vector<double>& times, int numSamples) {
        std::vector<NoiseStatistics> results;
        results.reserve(times.size());


        for (size_t i = 0; i < times.size(); ++i) {
            results.push_back(simulateAtTime(times[i], numSamples));
        }

        /*
        for (double t : times) {
            results.push_back(simulateAtTime(t, numSamples));
        }*/

        return results;
    }

    std::vector<double> NoiseSimulator::getSamplesAtTime(double time, int numSamples) {
        if (!m_hasWaveform) {
            throw std::runtime_error("No waveform function set");
        }

        std::vector<double> samples;
        //samples.reserve(numSamples);
        samples.reserve(static_cast<size_t>(numSamples));


        double ideal = m_waveform(time);

        for (int i = 0; i < numSamples; ++i) {
            samples.push_back(addNoise(ideal));
        }

        return samples;
    }

    void NoiseSimulator::clear() {
        // 沒有需要清除的內部狀態
    }

    void NoiseSimulator::setSeed(unsigned int seed) {
        m_rng.seed(seed);
    }

    // 便捷函數實作
    NoiseStatistics computeWaveformNoise(
        WaveformFunction waveform,
        double time,
        int numSamples,
        double noiseRatio) {

        NoiseSimulator sim;
        sim.setWaveform(waveform);
        sim.setMultiplicativeNoise(noiseRatio);
        return sim.simulateAtTime(time, numSamples);
    }

    std::vector<NoiseStatistics> computeWaveformNoiseSeries(
        WaveformFunction waveform,
        const std::vector<double>& times,
        int numSamples,
        double noiseRatio) {

        NoiseSimulator sim;
        sim.setWaveform(waveform);
        sim.setMultiplicativeNoise(noiseRatio);
        return sim.simulateAtTimes(times, numSamples);
    }

} // namespace MonteCarloNoise