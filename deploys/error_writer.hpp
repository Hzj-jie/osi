
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <initializer_list>
using namespace std;

class ierror_writer
{
public:
    virtual void write(const string& s);
};

class console_error_writer : public ierror_writer
{
public:
    virtual void write(const string& s)
    {
        cout << s;
    }
};

class file_error_writer : public ierror_writer
{
private:
    ofstream writer;
public:
    virtual void write(const string& s)
    {
        if(writer) writer << s;
    }

    file_error_writer(const string& file) :
        writer(filename) { }

    file_error_writer(file_error_writer&& other) :
        writer(move(other.writer)) { }

    ~file_error_writer()
    {
        writer.close();
    }
};

class error_type_selected_error_writer : public ierror_writer
{
private:
    ierror_writer impl;
    bool selected[error_type::last - error_type::first - 1];
public:
    error_type_selected_error_writer(const ierror_writer& impl,
                                     const initializer_list<error_type>& selected) :
        impl(impl),
        selected({ false })
    {
        for(auto it = selected.begin(); it != selected.end(); it++)
        {
            if(*it > error_type::first &&
               *it < error_type::last)
                selected[*it - 1] = true;
        }
    }

    virtual void write(const string& s)
    {
        if(!s.empty())
        {
            using namespace error_handle;
            if(selected[char_to_error_type(s[0])])
                impl.write(s);
        }
    }
};

