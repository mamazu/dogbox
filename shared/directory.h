#include <filesystem>
#include <map>
#include <variant>
#include <vector>

namespace dogbox
{
    struct regular_file
    {
        std::vector<std::byte> content;
    };

    struct directory_entry;

    struct directory
    {
        std::map<std::string, directory_entry> entries;
    };

    struct directory_entry
    {
        std::variant<regular_file, directory> content;
    };

    regular_file scan_regular_file(std::filesystem::path const &input);
    directory scan_directory(std::filesystem::path const &input);
}
