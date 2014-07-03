
#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <string>

static std::string uuid_str()
{
    using namespace std;
    using namespace boost;
    using namespace boost::uuids;
    return lexical_cast<string>(random_generator()());
}

