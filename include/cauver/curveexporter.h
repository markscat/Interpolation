#ifndef CURVEEXPORTER_H
#define CURVEEXPORTER_H

/**
* @file curveexporter.h
* @brief CurveExporter 類別，提供將特性曲線輸出為 CSV 或 TXT 檔案的功能
* 
* @details
*   @ CurveExporter 類別提供了將 Transistor 的特性曲線（輸出特性和轉移特性）輸出為 CSV 或 TXT 檔案的功能。使用者可以選擇輸出格式，並且可以在 CSV 格式中加入中繼資料（註解）來描述曲線的參數和條件。
*   @ CurveExporter 的主要功能包括：
*   @ - toCSV() 方法：將特性曲線輸出為 CSV 檔案，包含 x 和 y 的標題行，方便在 Excel 或其他軟體中使用。
*   @ - toTXT() 方法：將特性曲線輸出為 TXT 檔案，格式簡單，適合用於純文字分析或其他需要純文字格式的場合。
*   @ - toCSVWithMetadata() 方法：將特性曲線輸出為 CSV 檔案，並在檔案開頭加入使用者提供的中繼資料（註解），這些註解可以描述曲線的參數、測試條件、日期等資訊，增加檔案的可讀性和資訊量。
*
*
* @author Markscat
* @copyright Copyright (c) 2024 Markscat. All rights reserved.
* 本專案使用 GPLv3 授權，詳情請見 LICENSE 檔案。
*
* @since 2024-06
* @see Transistor
* @see BJT
* */
#include "../Interpolation.h"
#include <string>
#include <vector>
#include <fstream>  // 標準 C++ 檔案操作

// 封裝多條曲線的資料包，用於匯出並列格式
struct MultiCurveBundle {
    std::string title;                          // 報表標題
    std::string metadata;                       // 額外註解
    std::string xAxisLabel;                     // X 軸標籤 (如 "Vds")
    std::vector<std::string> curveLabels;       // 每條曲線的標籤 (如 "Vgs=1V", "Vgs=2V")
    std::vector<std::vector<Point>> data;       // 每條曲線的點集
};


/*
// 提供 Point 定義給 curveexporter 使用
struct Point {
    double x;
    double y;
    Point() : x(0), y(0) {}
    Point(double x_, double y_) : x(x_), y(y_) {}
};
*/
// 其餘 DataSet、Interpolation 類別定義...


class CurveExporter
{

public:
    /**
    * @static bool toCSV(const std::vector<Point>& curve,
                      const std::string& filename,
                      const std::string& xLabel = "Vds",
					  const std::string& yLabel = "Id");
	* @brief 將特性曲線輸出為 CSV 檔案
	* @param curve 包含特性曲線點的向量，每個 Point 包含 x 和 y 的值
	* @param filename 輸出檔案的名稱，應該以 .csv 結尾
	* @param xLabel x 軸的標題，默認為 "Vds"
	* @param yLabel y 軸的標題，默認為 "Id"
	* @return bool 輸出是否成功
	* @details 這個方法將特性曲線輸出為 CSV 檔案，第一行包含 x 和 y 的標題，後續行包含每個 Point 的 x 和 y 值。
               使用者可以指定 x 和 y 軸的標題，以便在 Excel 或其他軟體中更清晰地識別數據的含義。輸出的 CSV 檔案可以直接用於數據分析、繪圖或其他需要表格格式的場合。
    */
    static bool toCSV(const std::vector<Point>& curve,
                      const std::string& filename,
                      const std::string& xLabel = "Vds",
                      const std::string& yLabel = "Id");

    /**
    * @static bool toTXT(const std::vector<Point>& curve,
                      const std::string& filename);
	* @brief 將特性曲線輸出為 TXT 檔案
	* @param curve 包含特性曲線點的向量，每個 Point 包含 x 和 y 的值
	* @param filename 輸出檔案的名稱，應該以 .txt 結尾
	* @return bool 輸出是否成功
    * @details 這個方法將特性曲線輸出為 TXT 檔案，每行包含一個 Point 的 x 和 y 值，格式為 "x: [x 值], y: [y 值]"。
			   這種格式適合用於純文字分析或其他需要純文字格式的場合。使用者可以通過閱讀 TXT 檔案來了解特性曲線的具體數據點，或者使用其他工具來解析和分析這些數據。

    */
    static bool toTXT(const std::vector<Point>& curve,
                      const std::string& filename);


    /**
    * @static bool toCSVWithMetadata(const std::vector<Point>& curve,
                                  const std::string& filename,
                                  const std::string& metadata,
                                  const std::string& xLabel = "Vds",
								  const std::string& yLabel = "Id");
	* @brief 將特性曲線輸出為 CSV 檔案，並加入中繼資料（註解）
	* @param curve 包含特性曲線點的向量，每個 Point 包含 x 和 y 的值
	* @param filename 輸出檔案的名稱，應該以 .csv 結尾
	* @param metadata 使用者提供的中繼資料（註解），這些註解會被寫入 CSV 檔案的開頭，通常以 # 開頭表示註解行
	* @param xLabel x 軸的標題，默認為 "Vds"
	* @param yLabel y 軸的標題，默認為 "Id"
	* @return bool 輸出是否成功
    * @details 這個方法將特性曲線輸出為 CSV 檔案，並在檔案開頭加入使用者提供的中繼資料（註解）。這些註解可以描述曲線的參數、測試條件、日期等資訊，增加檔案的可讀性和資訊量。
			   中繼資料行通常以 # 開頭，表示這些行是註解，不會被 Excel 或其他軟體當作數據行解析。使用者可以通過這個方法來生成包含豐富資訊的 CSV 檔案，方便後續的分析和記錄。
    */
    // 輸出為 CSV 並加上中繼資料（註解）
    static bool toCSVWithMetadata(const std::vector<Point>& curve,
                                  const std::string& filename,
                                  const std::string& metadata,
                                  const std::string& xLabel = "Vds",
                                  const std::string& yLabel = "Id");

    static std::string formatProfessionalCSV(const MultiCurveBundle& bundle);
    /*識別項 "MultiCurveBundle" 未定義
    */


};

#endif
