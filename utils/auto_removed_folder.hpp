
#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder"

#include "sweeper.hpp"
#include <boost/filesystem.hpp>

class auto_removed_folder : private sweeper
{
private:
    const boost::filesystem::path p;
public:
#define AUTO_REMOVED_FOLDER_CTOR \
            p(p), \
            sweeper([&]() { \
                        boost::filesystem::create_directory(p); }, \
                    [this]() { \
                        boost::filesystem::remove_all(this->p); }) { }
    auto_removed_folder(boost::filesystem::path&& p) :
        AUTO_REMOVED_FOLDER_CTOR;
    auto_removed_folder(const boost::filesystem::path& p) :
        AUTO_REMOVED_FOLDER_CTOR;
#undef AUTO_REMOVED_FOLDER_CTOR
};

#pragma GCC diagnostic pop

