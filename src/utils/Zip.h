#include <iostream>
#include <functional>
#include <filesystem>
#include <fstream>
#include <zip.h>

namespace fs = std::filesystem;

inline bool ExtractZip(
	const std::string& zipPath,
	const std::string& outDir,
	std::function<void(const std::string&)> progressCallback)
{
	int err = 0;
	zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY, &err);
	if (!archive) {
		std::string msg = "Failed to open zip: " + zipPath;
		std::cerr << msg << std::endl;
		progressCallback(msg);
		return false;
	}

	zip_int64_t num_entries = zip_get_num_entries(archive, 0);
	for (zip_uint64_t i = 0; i < num_entries; i++) {
		struct zip_stat st;
		zip_stat_init(&st);
		if (zip_stat_index(archive, i, 0, &st) == 0) {
			zip_file_t* file = zip_fopen_index(archive, i, 0);
			if (!file) continue;

			fs::path outPath = fs::path(outDir) / st.name;

			// 如果是目录
			if (st.name[strlen(st.name) - 1] == '/') {
				fs::create_directories(outPath);
				zip_fclose(file);
				progressCallback("Created directory: " + outPath.string());
				continue;
			}

			fs::create_directories(outPath.parent_path());

			std::vector<char> buffer(st.size);
			zip_fread(file, buffer.data(), st.size);
			zip_fclose(file);

			std::ofstream outFile(outPath, std::ios::binary);
			outFile.write(buffer.data(), st.size);
			outFile.close();

			// 回调报告当前文件
			progressCallback("Extracted file: " + outPath.string());
		}
	}

	zip_close(archive);
	progressCallback("Extraction finished.");
	return true;
}
