
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
    auto_removed_folder(const boost::filesystem::path& p) :
        p(p),
        sweeper([&]()
                {
                    boost::filesystem::create_directory(p);
                },
                [this]()
                {
                    boost::filesystem::remove_all(this->p);
                }) { }
};

#pragma GCC diagnostic pop

