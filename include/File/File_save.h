// File_save.h
#ifndef FILE_SAVE_H
#define FILE_SAVE_H

#include <string>
#include <vector>

/**
* @file File_save.h
* @brief File_save 類別，提供檔案寫入功能
* @brief File_save 類別提供了生成時間戳檔名和寫入字串到檔案的功能。使用者可以選擇附加模式或覆蓋模式來寫入檔案，這些方法可以用於保存特性曲線、工作點資訊或其他相關數據。
* @details
*   @ File_save 類別的主要功能包括：
*   @ - generateTimestampFilename() 方法：生成一個包含當前日期和時間的檔名，格式為 "Calc_YYYYMMDD_HHMMSS.txt"，方便用於保存特性曲線或其他數據，並且可以通過 prefix 參數自定義檔名前綴。
*   @ - writeFile() 方法：將指定的字串內容寫入到指定的檔案中，這個方法是最通用的版本，可以選擇附加模式或覆蓋模式。
*   @ - appendFile() 方法：以附加模式將字串內容寫入到指定的檔案中，如果檔案不存在則會創建新檔案。
*   @ - overwriteFile() 方法：以覆蓋模式將字串內容寫入到指定的檔案中，如果檔案不存在則會創建新檔案。
* 
* @author Markscat
* @copyright Copyright (c) 2024 Markscat. All rights reserved.
* 本專案使用 GPLv3 授權，詳情請見 LICENSE 檔案。
*
* @since 2024-06
* */
class File_save {
public:
    // 產生時間戳檔名（維持原功能）
    /**
    * @brief generateTimestampFilename(const std::string& prefix = "Calc_",const std::string& ext= "log") static method
	* @brief 生成一個包含當前日期和時間的檔名，格式為 "Calc_YYYYMMDD_HHMMSS.txt"
	* @param prefix 檔名前綴，默認為 "Calc_"
	* @return std::string 生成的檔名
	* @details 這個方法生成一個包含當前日期和時間的檔名，格式為 "Calc_YYYYMMDD_HHMMSS.txt"，其中 "YYYYMMDD" 是當前的年月日，"HHMMSS" 是當前的時分秒。
               使用者可以通過 prefix 參數自定義檔名前綴，默認為 "Calc_"。這個方法非常適合用於保存特性曲線或其他數據，因為它可以確保每次保存的檔名都是唯一的，並且包含了保存的時間資訊，方便後續的管理和識別。
    */
    static std::string generateTimestampFilename(const std::string& prefix = "Calc_",const std::string& ext= "log");

    // 寫入字串到檔案（最通用的版本）
    /**
	* @brief writeFile(const std::string& filename, const std::string& content, bool append = true) static method
	* @brief 將指定的字串內容寫入到指定的檔案中
	* @param filename 輸出檔案的名稱
	* @param content 要寫入檔案的字串內容
	* @param append 是否以附加模式寫入，默認為 true（如果為 false 則以覆蓋模式寫入）
	* @return bool 寫入是否成功
    * @details 這個方法將指定的字串內容寫入到指定的檔案中，使用者可以選擇以附加模式（默認）或覆蓋模式來寫入檔案。如果 append 參數為 true，則內容會被附加到檔案末尾；
               如果 append 參數為 false，則檔案會被覆蓋，原有內容將被刪除。這個方法是最通用的版本，可以根據需要選擇寫入模式，
               適用於保存特性曲線、工作點資訊或其他相關數據。使用者可以通過這個方法來靈活地管理檔案內容，以滿足不同的保存需求。
    */
    static bool writeFile(const std::string& filename, const std::string& content);

    // 附加模式寫入（預設）
    /**
	* @brief appendFile(const std::string& filename, const std::string& content) static method
	* @brief 以附加模式將字串內容寫入到指定的檔案中，如果檔案不存在則會創建新檔案
	* @param filename 輸出檔案的名稱
	* @param content 要寫入檔案的字串內容
	* @return bool 寫入是否成功
    * @details 這個方法以附加模式將字串內容寫入到指定的檔案中，如果檔案不存在則會創建新檔案。使用者可以通過這個方法來將新的內容添加到現有的檔案末尾，而不會刪除原有的內容。
			   這對於保存特性曲線或其他數據非常有用，因為使用者可以多次調用這個方法來逐步添加數據，而不需要擔心覆蓋之前的內容。使用者可以通過這個方法來靈活地管理檔案內容，以滿足不同的保存需求。
    */
    static bool appendFile(const std::string& filename, const std::string& content);

    // 覆蓋模式寫入
    /**
	* @brief overwriteFile(const std::string& filename, const std::string& content) static method
	* @brief 以覆蓋模式將字串內容寫入到指定的檔案中，如果檔案不存在則會創建新檔案
	* @param filename 輸出檔案的名稱
	* @param content 要寫入檔案的字串內容
	* @return bool 寫入是否成功
	* @details 這個方法以覆蓋模式將字串內容寫入到指定的檔案中，如果檔案不存在則會創建新檔案。使用者可以通過這個方法來將新的內容寫入檔案，並且會刪除原有的內容。
			   這對於保存特性曲線或其他數據非常有用，當使用者希望每次保存都覆蓋之前的內容時，可以使用這個方法。使用者可以通過這個方法來靈活地管理檔案內容，以滿足不同的保存需求。
    */
    static bool overwriteFile(const std::string& filename, const std::string& content);


};

#endif
