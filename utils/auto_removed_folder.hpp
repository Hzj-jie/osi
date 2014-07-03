
#pragma once
#include "sweeper.hpp"
#include <boost/filesystem.hpp>

class auto_removed_folder : private sweeper
{
private:
    const path p;
public:
    auto_removed_folder(const path& p) :
        p(p),
        sweeper([this]()
                {
                    boost::filesystem::create_directory(p);
                },
                [this]()
                {
                    boost::filesystem::remove_all(p);
                }) { }
};

