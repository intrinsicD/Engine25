//
// Created by alex on 20.01.25.
//

#ifndef ENGINE25_MESHIO_H
#define ENGINE25_MESHIO_H

#include "Mesh.h"
#include "AssetIo.h"
#include <memory>
#include <utility>

namespace Bcg {
    /**
     * @brief Abstract base class for mesh input/output operations.
     */
    class MeshIo : public AssetIo {
    public:
        struct WriteFlags {
            WriteFlags() = default;

            bool as_binary = false;
            bool with_normals = false;
            bool with_colors = false;
            bool with_tex_coords = false;
        };

        /**
        * @brief Constructs a MeshIo object with the given filename.
        * @param filename The name of the file to read/write.
        */
        explicit MeshIo(std::string filename) : AssetIo(std::move(filename)) {
        }

        /**
         * @brief Reads a mesh from the file.
         * @param mesh The mesh object to populate with data.
         * @return True if the mesh was successfully read, false otherwise.
         */
        virtual bool read(Mesh &mesh) = 0;

        /**
         * @brief Writes the given mesh to the file with specified flags.
         * @param mesh The mesh to write.
         * @param flags The flags to control the writing process.
         * @return True if the file was successfully written, false otherwise.
         */
        virtual bool write(const Mesh &mesh, const WriteFlags &flags) = 0;

        /**
         * @brief Checks if the file can be loaded.
         * @return True if the file can be loaded, false otherwise.
         */
        bool can_load_file() override = 0;
    };

    /**
     * @brief Class for reading and writing OFF mesh files.
     */
    class MeshIoOFF : public MeshIo {
    public:
        /**
         * @brief Constructs a MeshIoOFF object with the given filename.
         * @param filename The name of the OFF file to read/write.
         */
        explicit MeshIoOFF(std::string filename) : MeshIo(std::move(filename)) {
        }


        /**
         * @brief Reads a mesh from an OFF file.
         * @param mesh The mesh object to populate with data.
         * @return True if the mesh was successfully read, false otherwise.
         */
        bool read(Mesh &mesh) override;

        /**
         * @brief Writes the given mesh to an OFF file with specified flags.
         * @param mesh The mesh to write.
         * @param flags The flags to control the writing process.
         * @return True if the file was successfully written, false otherwise.
         */
        bool write(const Mesh &mesh, const WriteFlags &flags) override;

        /**
         * @brief Checks if the file can be loaded.
         * @return True if the file can be loaded, false otherwise.
         */
        bool can_load_file() override;
    };

    /**
     * @brief Class for reading and writing OBJ mesh files.
     */
    class MeshIoOBJ : public MeshIo {
    public:
        /**
         * @brief Constructs a MeshIoOBJ object with the given filename.
         * @param filename The name of the OBJ file to read/write.
         */
        explicit MeshIoOBJ(std::string filename) : MeshIo(std::move(filename)) {
        }

        /**
         * @brief Reads a mesh from an OBJ file.
         * @param mesh The mesh object to populate with data.
         * @return True if the mesh was successfully read, false otherwise.
         */
        bool read(Mesh &mesh) override;

        /**
         * @brief Writes the given mesh to an OBJ file with specified flags.
         * @param mesh The mesh to write.
         * @param flags The flags to control the writing process.
         * @return True if the file was successfully written, false otherwise.
         */
        bool write(const Mesh &mesh, const WriteFlags &flags) override;

        /**
         * @brief Checks if the file can be loaded.
         * @return True if the file can be loaded, false otherwise.
         */
        bool can_load_file() override;
    };

    /**
     * @brief Class for reading and writing STL mesh files.
     */
    class MeshIoSTL : public MeshIo {
    public:
        /**
         * @brief Constructs a MeshIoSTL object with the given filename.
         * @param filename The name of the STL file to read/write.
         */
        explicit MeshIoSTL(std::string filename) : MeshIo(std::move(filename)) {
        }

        /**
         * @brief Reads a mesh from an STL file.
         * @param mesh The mesh object to populate with data.
         * @return True if the mesh was successfully read, false otherwise.
         */
        bool read(Mesh &mesh) override;

        /**
         * @brief Writes the given mesh to an STL file with specified flags.
         * @param mesh The mesh to write.
         * @param flags The flags to control the writing process.
         * @return True if the file was successfully written, false otherwise.
         */
        bool write(const Mesh &mesh, const WriteFlags &flags) override;

        /**
         * @brief Checks if the file can be loaded.
         * @return True if the file can be loaded, false otherwise.
         */
        bool can_load_file() override;
    };

    /**
     * @brief Class for reading and writing PLY mesh files.
     */
    class MeshIoPLY : public MeshIo {
    public:
        /**
         * @brief Constructs a MeshIoPLY object with the given filename.
         * @param filename The name of the PLY file to read/write.
         */
        explicit MeshIoPLY(std::string filename) : MeshIo(std::move(filename)) {
        }

        /**
         * @brief Reads a mesh from a PLY file.
         * @param mesh The mesh object to populate with data.
         * @return True if the mesh was successfully read, false otherwise.
         */
        bool read(Mesh &mesh) override;

        /**
         * @brief Writes the given mesh to a PLY file with specified flags.
         * @param mesh The mesh to write.
         * @param flags The flags to control the writing process.
         * @return True if the file was successfully written, false otherwise.
         */
        bool write(const Mesh &mesh, const WriteFlags &flags) override;

        /**
         * @brief Checks if the file can be loaded.
         * @return True if the file can be loaded, false otherwise.
         */
        bool can_load_file() override;
    };

    class MeshIoPMP : public MeshIo {
    public:
        explicit MeshIoPMP(std::string filename) : MeshIo(std::move(filename)) {
        }

        bool read(Mesh &mesh) override;

        bool write(const Mesh &mesh, const WriteFlags &flags) override;

        bool can_load_file() override;
    };

    class MeshIoManager {
    public:
        explicit MeshIoManager(std::string filename);

        void add_io(std::shared_ptr<MeshIo> io);

        std::shared_ptr<MeshIo> get_io(const std::string &filename);

        bool read(Mesh &mesh);

        bool write(const Mesh &mesh, const MeshIo::WriteFlags &flags);

        bool can_load_file();

    private:
        std::string m_filename;
        std::vector<std::shared_ptr<MeshIo> > m_ios;
    };
}

#endif //ENGINE25_MESHIO_H
