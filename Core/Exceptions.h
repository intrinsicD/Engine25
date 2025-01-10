//
// Created by alex on 10.01.25.
//

#ifndef ENGINE25_EXCEPTIONS_H
#define ENGINE25_EXCEPTIONS_H

#include <string>

namespace Bcg{
    enum class Exception{
        InvalidArgument,
        Solver,
        Allocation,
        Topology,
        IO,
        GL,
    };

    inline std::string GetErrorMessage(Exception e) {
        switch (e) {
            case Exception::InvalidArgument: return "Invalid Argument Exception";
            case Exception::Solver: return "Solver Exception";
            case Exception::Allocation: return "Allocation Exception";
            case Exception::Topology: return "Topology Exception";
            case Exception::IO: return "IO Exception";
            case Exception::GL: return "GL Exception";
            default: return "Invalid Exception Code";
        }
    }
}

#endif //ENGINE25_EXCEPTIONS_H
