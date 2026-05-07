#ifndef MONTECARLONOISE_H
#define MONTECARLONOISE_H

#include "../include/MonteCarloSimulator.h"
#include <vector>
#include <functional>
#include <random>
#include <string>

namespace MonteCarloNoise {

    /**
     * @brief 雜訊統計結果
     */
    struct NoiseStatistics {
        double mean;        // 平均值
        double stddev;      // 標準差
        double min;         // 最小值
        double max;         // 最大值
        double var;         // 變異數
        int sampleCount;    // 樣本數
    };

    /**
     * @brief 波形函數的簽名：輸入時間點，輸出波形值
     */
    using WaveformFunction = std::function<double(double time)>;

    /**
     * @brief 蒙地卡羅雜訊模擬器
     *
     * 用於對任何波形函數加入雜訊並進行統計分析
     */
    class NoiseSimulator {
    public:
        /**
         * @brief 建構子
         * @param seed 隨機種子（可選，預設使用時間種子）
         */
        explicit NoiseSimulator(unsigned int seed = 0);

        /**
         * @brief 設定基礎波形函數
         * @param waveform 波形函數 (time -> value)
         */
        void setWaveform(WaveformFunction waveform);

        /**
         * @brief 設定雜訊類型
         * @param type 雜訊類型 ("additive", "multiplicative", "mixed")
         * @param level 雜訊強度（標準差或比例）
         */
        void setNoiseType(const std::string& type, double level);

        /**
         * @brief 設定加法性雜訊 (value + noise)
         * @param stddev 雜訊的標準差
         */
        void setAdditiveNoise(double stddev);

        /**
         * @brief 設定乘法性雜訊 (value * (1 + noise))
         * @param ratio 雜訊比例（例如 0.05 表示 ±5%）
         */
        void setMultiplicativeNoise(double ratio);

        /**
         * @brief 設定混合雜訊 (value * (1 + noise1) + noise2)
         * @param multiplicativeRatio 乘法雜訊比例
         * @param additiveStddev 加法雜訊標準差
         */
        void setMixedNoise(double multiplicativeRatio, double additiveStddev);

        /**
         * @brief 設定常數偏移（可選）
         * @param offset 固定的偏移值
         */
        void setConstantOffset(double offset);

        /**
         * @brief 在單一時間點執行蒙地卡羅模擬
         * @param time 時間點
         * @param numSamples 樣本數
         * @return 統計結果
         */
        NoiseStatistics simulateAtTime(double time, int numSamples);

        /**
         * @brief 在多個時間點執行蒙地卡羅模擬
         * @param times 時間點陣列
         * @param numSamples 每個時間點的樣本數
         * @return 每個時間點的統計結果向量
         */
        std::vector<NoiseStatistics> simulateAtTimes(const std::vector<double>& times, int numSamples);

        /**
         * @brief 取得某個時間點的所有原始樣本值
         * @param time 時間點
         * @param numSamples 樣本數
         * @return 樣本值向量
         */
        std::vector<double> getSamplesAtTime(double time, int numSamples);

        /**
         * @brief 清除所有內部狀態
         */
        void clear();

        /**
         * @brief 設定隨機種子（用於可重現性）
         */
        void setSeed(unsigned int seed);

    private:
        /**
         * @brief 加入雜訊到數值
         */
        double addNoise(double value);

        WaveformFunction m_waveform;
        int m_noiseType;  // 0: additive, 1: multiplicative, 2: mixed
        //std::string m_noiseType;
        double m_noiseParam1;     // 乘法比例 或 加法標準差
        double m_noiseParam2;     // 混合模式的第二參數
        double m_constantOffset;

        std::mt19937 m_rng;
        //std::normal_distribution<> m_normalDist;
        //std::uniform_real_distribution<> m_uniformDist;

        bool m_hasWaveform;
    };

    /**
     * @brief 便捷函數：計算波形在特定時間點的雜訊統計
     *
     * @param waveform 波形函數
     * @param time 時間點
     * @param numSamples 樣本數
     * @param noiseRatio 雜訊比例（預設 0.05 = ±5%）
     * @return 統計結果
     */
    NoiseStatistics computeWaveformNoise(
        WaveformFunction waveform,
        double time,
        int numSamples,
        double noiseRatio = 0.05
    );

    /**
     * @brief 便捷函數：計算波形在多個時間點的雜訊統計
     *
     * @param waveform 波形函數
     * @param times 時間點陣列
     * @param numSamples 每個時間點的樣本數
     * @param noiseRatio 雜訊比例
     * @return 每個時間點的統計結果向量
     */
    std::vector<NoiseStatistics> computeWaveformNoiseSeries(
        WaveformFunction waveform,
        const std::vector<double>& times,
        int numSamples,
        double noiseRatio = 0.05
    );

} // namespace MonteCarloNoise

#endif // MONTECARLONOISE_H
