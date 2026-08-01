#pragma once

#include "sweeper.hpp"
#include <filesystem>
#include <type_traits>

class auto_removed_folder : private sweeper
{
public:
    auto_removed_folder(const std::filesystem::path& p) :
        sweeper([&]()
                {
                    std::filesystem::create_directories(p);
                },
                [=]()
                {
                    std::filesystem::remove_all(p);
                }) { }

    template <typename T, typename = typename std::enable_if<!std::is_same<typename std::decay<T>::type, std::filesystem::path>::value>::type>
    auto_removed_folder(T&& s) :
        auto_removed_folder(std::filesystem::path(std::forward<T>(s))) { }
};
