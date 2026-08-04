#pragma once
#include <cstdint>
#include <string_view>

namespace primitive
{
    enum class command_type : uint8_t
    {
        push = 0,
        pop,
        jump,
        cpc,
        mov,
        cp,
        add,
        sub,
        mul,
        div,
        ext,
        pow,
        jumpif,
        cpco,
        cpdbz,
        cpin,
        stop,
        equal,
        less,
        app,
        sapp,
        cut,
        cutl,
        interrupt_cmd,
        clr,
        scut,
        scutl,
        sizeof_cmd,
        empty,
        cmd_and,
        cmd_or,
        cmd_not,
        stst,
        rest,
        fadd,
        fsub,
        fmul,
        fdiv,
        fext,
        fpow,
        fequal,
        fless,
        lfs,
        rfs,
        alloc,
        dealloc,
        jmpr,
        unknown
    };

    inline std::string_view command_type_name(command_type t)
    {
        switch (t)
        {
            case command_type::push: return "push";
            case command_type::pop: return "pop";
            case command_type::jump: return "jump";
            case command_type::cpc: return "cpc";
            case command_type::mov: return "mov";
            case command_type::cp: return "cp";
            case command_type::add: return "add";
            case command_type::sub: return "sub";
            case command_type::mul: return "mul";
            case command_type::div: return "div";
            case command_type::ext: return "ext";
            case command_type::pow: return "pow";
            case command_type::jumpif: return "jumpif";
            case command_type::cpco: return "cpco";
            case command_type::cpdbz: return "cpdbz";
            case command_type::cpin: return "cpin";
            case command_type::stop: return "stop";
            case command_type::equal: return "equal";
            case command_type::less: return "less";
            case command_type::app: return "app";
            case command_type::sapp: return "sapp";
            case command_type::cut: return "cut";
            case command_type::cutl: return "cutl";
            case command_type::interrupt_cmd: return "int";
            case command_type::clr: return "clr";
            case command_type::scut: return "scut";
            case command_type::scutl: return "scutl";
            case command_type::sizeof_cmd: return "sizeof";
            case command_type::empty: return "empty";
            case command_type::cmd_and: return "and";
            case command_type::cmd_or: return "or";
            case command_type::cmd_not: return "not";
            case command_type::stst: return "stst";
            case command_type::rest: return "rest";
            case command_type::fadd: return "fadd";
            case command_type::fsub: return "fsub";
            case command_type::fmul: return "fmul";
            case command_type::fdiv: return "fdiv";
            case command_type::fext: return "fext";
            case command_type::fpow: return "fpow";
            case command_type::fequal: return "fequal";
            case command_type::fless: return "fless";
            case command_type::lfs: return "lfs";
            case command_type::rfs: return "rfs";
            case command_type::alloc: return "alloc";
            case command_type::dealloc: return "dealloc";
            case command_type::jmpr: return "jmpr";
            default: return "unknown";
        }
    }
}
