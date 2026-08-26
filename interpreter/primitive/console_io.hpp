#pragma once
#include <iostream>
#include <sstream>
#include <memory>
#include <string>

namespace primitive
{
    class console_io
    {
    private:
        std::istream* in_{&std::cin};
        std::ostream* out_{&std::cout};
        std::ostream* err_{&std::cerr};

    public:
        console_io() = default;
        console_io(std::istream* in, std::ostream* out, std::ostream* err)
            : in_(in ? in : &std::cin), out_(out ? out : &std::cout), err_(err ? err : &std::cerr) {}

        std::istream& in() { return *in_; }
        std::ostream& out() { return *out_; }
        std::ostream& err() { return *err_; }

        void redirect_input(std::istream* in = nullptr) { in_ = in ? in : &std::cin; }
        void redirect_output(std::ostream* out = nullptr) { out_ = out ? out : &std::cout; }
        void redirect_error(std::ostream* err = nullptr) { err_ = err ? err : &std::cerr; }

        class test_wrapper
        {
        private:
            std::unique_ptr<std::istringstream> in_stream_;
            std::ostringstream out_stream_;
            std::ostringstream err_stream_;
            std::shared_ptr<console_io> io_;

        public:
            test_wrapper()
            {
                io_ = std::make_shared<console_io>(nullptr, &out_stream_, &err_stream_);
            }

            explicit test_wrapper(const std::string& input)
            {
                in_stream_ = std::make_unique<std::istringstream>(input);
                io_ = std::make_shared<console_io>(in_stream_.get(), &out_stream_, &err_stream_);
            }

            std::shared_ptr<console_io> io() const { return io_; }
            std::string output() const { return out_stream_.str(); }
            std::string error() const { return err_stream_.str(); }
        };
    };
}
