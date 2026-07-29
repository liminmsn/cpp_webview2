#include <zip.h>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

inline bool ExtractZip(const std::string& zipPath, const std::string& outDir) {
    int err = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY, &err);
    if (!archive) {
        std::cerr << "Failed to open zip: " << zipPath << std::endl;
        return false;
    }

    zip_int64_t num_entries = zip_get_num_entries(archive, 0);
    for (zip_uint64_t i = 0; i < num_entries; i++) {
        struct zip_stat st;
        zip_stat_init(&st);
        if (zip_stat_index(archive, i, 0, &st) == 0) {
            zip_file_t* file = zip_fopen_index(archive, i, 0);
            if (!file) continue;

            // 构造输出路径
            fs::path outPath = fs::path(outDir) / st.name;

            // 如果是目录，直接创建
            if (st.name[strlen(st.name) - 1] == '/') {
                fs::create_directories(outPath);
                zip_fclose(file);
                continue;
            }

            // 确保父目录存在
            fs::create_directories(outPath.parent_path());

            // 读取文件内容
            std::vector<char> buffer(st.size);
            zip_fread(file, buffer.data(), st.size);
            zip_fclose(file);

            // 写入磁盘
            std::ofstream outFile(outPath, std::ios::binary);
            outFile.write(buffer.data(), st.size);
            outFile.close();
        }
    }

    zip_close(archive);
    return true;
}

//int main() {
//    std::string zipPath = "E:/source/cpp/cpp_webview2/resources/zip/mysql-8.4.10-winx64.zip";
//    std::string outDir = "E:/source/cpp/cpp_webview2/resources/mysql";
//
//    if (ExtractZip(zipPath, outDir)) {
//        std::cout << "解压完成！" << std::endl;
//    }
//    else {
//        std::cout << "解压失败！" << std::endl;
//    }
//    return 0;
//}
