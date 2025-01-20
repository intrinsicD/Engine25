//
// Created by alex on 20.01.25.
//

#ifndef ENGINE25_ASSETIO_H
#define ENGINE25_ASSETIO_H

#include <string>

namespace Bcg {
    class AssetIo {
    public:
        explicit AssetIo(std::string filename) : m_filename(filename) {}

        virtual ~AssetIo() = default;

        virtual bool can_load_file() = 0;

    protected:
        std::string m_filename;
    };
}

#endif //ENGINE25_ASSETIO_H
