
#include "../../include/cauver/curveexporter.h"
//#include <QFile>
//#include <QTextStream>
#include <fstream>
#include <string>

// 舊有方法：單線存檔
bool CurveExporter::toCSV(const std::vector<Point>& curve,
                          const std::string& filename,
                          const std::string& xLabel,
                          const std::string& yLabel)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    file << xLabel << "," << yLabel << "\n";

    for (const auto& p : curve) {
        file << p.x << "," << p.y << "\n";
    }


    file.close();
    return true;
}

bool CurveExporter::toTXT(const std::vector<Point>& curve,
                          const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    for (const auto& p : curve) {
        file << p.x << "\t" << p.y << "\n";
    }

    file.close();
    return true;
}

bool CurveExporter::toCSVWithMetadata(const std::vector<Point>& curve,
                                      const std::string& filename,
                                      const std::string& metadata,
                                      const std::string& xLabel,
                                      const std::string& yLabel)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    file << "# " << metadata << "\n";
    file << xLabel << "," << yLabel << "\n";

    for (const auto& p : curve) {
        file << p.x << "," << p.y << "\n";
    }

    file.close();
    return true;
}


// 新增方法：處理多條線並列的專業格式 (已移除 gm, rds)
std::string CurveExporter::formatProfessionalCSV(const MultiCurveBundle& bundle) {
    std::string out = "";

    // 1. 寫入標頭註解 (Metadata)
    out += "# " + bundle.title + "\n";
    out += "# " + bundle.metadata + "\n";
    out += "# ------------------------------------------------\n";

    // 2. 寫入欄位標題 (橫向展開)
    // 第一欄固定為 X 軸基準 (例如 Vds(V))
    out += bundle.xAxisLabel + "(V)";
    for (const std::string& label : bundle.curveLabels) {
        // 現在每條線只輸出一個數據欄位 (例如 Id)
        out += "," + label + "_Id(A)";
    }
    out += "\n";

    // 3. 寫入數據列
    if (bundle.data.empty()) {
        return out;
    }

    // 以第一條線的點數為基準進行循環
    size_t rowCount = bundle.data[0].size();
    for (size_t i = 0; i < rowCount; ++i) {
        // 寫入 X 軸數值 (取第一條線的 x)
        out += std::to_string(bundle.data[0][i].x);

        // 並列寫入所有曲線在該 X 點下的 Y 值 (Id)
        for (size_t j = 0; j < bundle.data.size(); ++j) {
            if (i < bundle.data[j].size()) {
                // 使用 Point 結構取代 ExtendedPoint
                const Point& pt = bundle.data[j][i];

                // 只輸出 y 值 (Id)
                out += "," + std::to_string(pt.y);
            }
            else {
                // 如果曲線長度不一，補 N/A
                out += ",N/A";
            }
        }
        out += "\n";
    }
    return out;
}