#pragma once

#include <filesystem>
#include <string_view>

inline std::filesystem::path ippn_test_data_root()
{
    return std::filesystem::path{ IPPON_TEST_DATA_DIR };
}

inline std::filesystem::path ippn_test_data_path(std::string_view relative)
{
    return ippn_test_data_root() / std::filesystem::path(relative);
}

inline std::string ippn_test_data_string(std::string_view relative)
{
    return ippn_test_data_path(relative).string();
}
