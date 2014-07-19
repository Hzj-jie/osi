
#pragma once

#include "sweeper.hpp"
#include <boost/filesystem.hpp>

class auto_removed_folder : private sweeper
{
public:
    auto_removed_folder(const boost::filesystem::path& p) :
        sweeper([&]()
                {
                    boost::filesystem::create_directories(p);
                },
                [=]()
                {
                    boost::filesystem::remove_all(p);
                }) { }

    template <typename T>
    auto_removed_folder(T&& s) :
        auto_removed_folder((const boost::filesystem::path&)boost::filesystem::path(s)) { }
};
