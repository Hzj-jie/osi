
#pragma once

#include "sweeper.hpp"
#include <boost/filesystem.hpp>

class auto_removed_folder : private sweeper
{
public:
#define AUTO_REMOVED_FOLDER_CTOR \
            sweeper([&]() { \
                        boost::filesystem::create_directories(p); }, \
                    [=]() { \
                        boost::filesystem::remove_all(p); }) { }
    auto_removed_folder(boost::filesystem::path&& p) :
        AUTO_REMOVED_FOLDER_CTOR;
    auto_removed_folder(const boost::filesystem::path& p) :
        AUTO_REMOVED_FOLDER_CTOR;
    template <typename T>
    auto_removed_folder(T&& s) :
        auto_removed_folder(boost::filesystem::path(s)) { }
#undef AUTO_REMOVED_FOLDER_CTOR
};
