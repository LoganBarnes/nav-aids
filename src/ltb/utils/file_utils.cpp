// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "file_utils.hpp"

namespace ltb::utils
{

// auto get_binary_file_contents(std::filesystem::path const& file_path) -> utils::Result<std::vector<char>> {
//     auto file = std::ifstream(file_path, std::ios::ate | std::ios::binary);
//
//     if (!file.is_open()) {
//         return LTB_MAKE_UNEXPECTED_ERROR("Failed to open file '{}'", file_path.string());
//     }
//
//     auto file_size = file.tellg();
//
//     auto buffer = std::vector<char>(static_cast<size_t>(file_size));
//
//     file.seekg(0);
//     file.read(buffer.data(), file_size);
//     file.close();
//
//     return buffer;
// }

} // namespace ltb::utils
