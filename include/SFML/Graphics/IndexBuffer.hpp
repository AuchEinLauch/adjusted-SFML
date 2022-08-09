//NOTE THAT THIS FILE IS NOT IN THE OFFICIAL SFML LIBRARY AND IS BASED ON THE indexBuffer.hpp FILE

#pragma once

////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2022 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/GlResource.hpp>
#include <cstddef>


namespace sf
{
    class RenderTarget;
    

    class SFML_GRAPHICS_API IndexBuffer : private GlResource
    {
    public:

        ////////////////////////////////////////////////////////////
        /// \brief Usage specifiers
        ///
        /// If data is going to be updated once or more every frame,
        /// set the usage to Stream. If data is going to be set once
        /// and used for a long time without being modified, set the
        /// usage to Static. For everything else Dynamic should be a
        /// good compromise.
        ///
        ////////////////////////////////////////////////////////////
        enum Usage
        {
            Stream,  //!< Constantly changing data
            Dynamic, //!< Occasionally changing data
            Static   //!< Rarely changing data
        };

        ////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        /// Creates an empty index buffer.
        ///
        ////////////////////////////////////////////////////////////
        IndexBuffer();

        ////////////////////////////////////////////////////////////
        /// \brief Construct a IndexBuffer with a specific usage specifier
        ///
        /// Creates an empty index buffer and sets its usage to \p usage.
        ///
        /// \param usage Usage specifier
        ///
        ////////////////////////////////////////////////////////////
        explicit IndexBuffer(Usage usage);

        ////////////////////////////////////////////////////////////
        /// \brief Copy constructor
        ///
        /// \param copy instance to copy
        ///
        ////////////////////////////////////////////////////////////
        IndexBuffer(const IndexBuffer& copy);

        ////////////////////////////////////////////////////////////
        /// \brief Destructor
        ///
        ////////////////////////////////////////////////////////////
        ~IndexBuffer();

        ////////////////////////////////////////////////////////////
        /// \brief Create the index buffer
        ///
        /// Creates the index buffer and allocates enough graphics
        /// memory to hold \p indexCount indices. Any previously
        /// allocated memory is freed in the process.
        ///
        /// In order to deallocate previously allocated memory pass 0
        /// as \p indexCount. Don't forget to recreate with a non-zero
        /// value when graphics memory should be allocated again.
        ///
        /// \param indexCount Number of indices worth of memory to allocate
        ///
        /// \return True if creation was successful
        ///
        ////////////////////////////////////////////////////////////
        [[nodiscard]] bool create(std::size_t indexCount);

        ////////////////////////////////////////////////////////////
        /// \brief Return the index count
        ///
        /// \return Number of indices in the index buffer
        ///
        ////////////////////////////////////////////////////////////
        std::size_t getIndexCount() const;

        ////////////////////////////////////////////////////////////
        /// \brief Update the whole buffer from an array of indices
        ///
        /// The \a index array is assumed to have the same size as
        /// the \a created buffer.
        ///
        /// No additional check is performed on the size of the index
        /// array, passing invalid arguments will lead to undefined
        /// behavior.
        ///
        /// This function does nothing if \a indices is null or if the
        /// buffer was not previously created.
        ///
        /// \param indices Array of indices to copy to the buffer
        ///
        /// \return True if the update was successful
        ///
        ////////////////////////////////////////////////////////////
        [[nodiscard]] bool update(const uint32_t* indices);

        ////////////////////////////////////////////////////////////
        /// \brief Update a part of the buffer from an array of indices
        ///
        /// \p offset is specified as the number of indices to skip
        /// from the beginning of the buffer.
        ///
        /// If \p offset is 0 and \p indexCount is equal to the size of
        /// the currently created buffer, its whole contents are replaced.
        ///
        /// If \p offset is 0 and \p indexCount is greater than the
        /// size of the currently created buffer, a new buffer is created
        /// containing the index data.
        ///
        /// If \p offset is 0 and \p indexCount is less than the size of
        /// the currently created buffer, only the corresponding region
        /// is updated.
        ///
        /// If \p offset is not 0 and \p offset + \p indexCount is greater
        /// than the size of the currently created buffer, the update fails.
        ///
        /// No additional check is performed on the size of the index
        /// array, passing invalid arguments will lead to undefined
        /// behavior.
        ///
        /// \param indices    Array of indices to copy to the buffer
        /// \param indexCount Number of indices to copy
        /// \param offset      Offset in the buffer to copy to
        ///
        /// \return True if the update was successful
        ///
        ////////////////////////////////////////////////////////////
        [[nodiscard]] bool update(const uint32_t* indices, std::size_t indexCount, unsigned int offset);

        ////////////////////////////////////////////////////////////
        /// \brief Copy the contents of another buffer into this buffer
        ///
        /// \param indexBuffer index buffer whose contents to copy into this index buffer
        ///
        /// \return True if the copy was successful
        ///
        ////////////////////////////////////////////////////////////
        [[nodiscard]] bool update(const IndexBuffer& indexBuffer);

        ////////////////////////////////////////////////////////////
        /// \brief Overload of assignment operator
        ///
        /// \param right Instance to assign
        ///
        /// \return Reference to self
        ///
        ////////////////////////////////////////////////////////////
        IndexBuffer& operator =(const IndexBuffer& right);

        ////////////////////////////////////////////////////////////
        /// \brief Swap the contents of this index buffer with those of another
        ///
        /// \param right Instance to swap with
        ///
        ////////////////////////////////////////////////////////////
        void swap(IndexBuffer& right);

        ////////////////////////////////////////////////////////////
        /// \brief Get the underlying OpenGL handle of the index buffer.
        ///
        /// You shouldn't need to use this function, unless you have
        /// very specific stuff to implement that SFML doesn't support,
        /// or implement a temporary workaround until a bug is fixed.
        ///
        /// \return OpenGL handle of the index buffer or 0 if not yet created
        ///
        ////////////////////////////////////////////////////////////
        unsigned int getNativeHandle() const;

        ////////////////////////////////////////////////////////////
        /// \brief Set the usage specifier of this index buffer
        ///
        /// This function provides a hint about how this index buffer is
        /// going to be used in terms of data update frequency.
        ///
        /// After changing the usage specifier, the index buffer has
        /// to be updated with new data for the usage specifier to
        /// take effect.
        ///
        /// The default primitive type is sf::indexBuffer::Stream.
        ///
        /// \param usage Usage specifier
        ///
        ////////////////////////////////////////////////////////////
        void setUsage(Usage usage);

        ////////////////////////////////////////////////////////////
        /// \brief Get the usage specifier of this index buffer
        ///
        /// \return Usage specifier
        ///
        ////////////////////////////////////////////////////////////
        Usage getUsage() const;

        ////////////////////////////////////////////////////////////
        /// \brief Bind a index buffer for rendering
        ///
        /// This function is not part of the graphics API, it mustn't be
        /// used when drawing SFML entities. It must be used only if you
        /// mix sf::indexBuffer with OpenGL code.
        ///
        /// \code
        /// sf::indexBuffer vb1, vb2;
        /// ...
        /// sf::indexBuffer::bind(&vb1);
        /// // draw OpenGL stuff that use vb1...
        /// sf::indexBuffer::bind(&vb2);
        /// // draw OpenGL stuff that use vb2...
        /// sf::indexBuffer::bind(nullptr);
        /// // draw OpenGL stuff that use no index buffer...
        /// \endcode
        ///
        /// \param indexBuffer Pointer to the index buffer to bind, can be null to use no index buffer
        ///
        ////////////////////////////////////////////////////////////
        static void bind(const IndexBuffer* indexBuffer);

        ////////////////////////////////////////////////////////////
        /// \brief Tell whether or not the system supports index buffers
        ///
        /// This function should always be called before using
        /// the index buffer features. If it returns false, then
        /// any attempt to use sf::indexBuffer will fail.
        ///
        /// \return True if index buffers are supported, false otherwise
        ///
        ////////////////////////////////////////////////////////////
        static bool isAvailable();

    private:

        ////////////////////////////////////////////////////////////
        // Member data
        ////////////////////////////////////////////////////////////
        unsigned int  m_buffer;        //!< Internal buffer identifier
        std::size_t   m_size;          //!< Size in indexes of the currently allocated buffer
        Usage         m_usage;         //!< How this index buffer is to be used
    };

} // namespace sf


////////////////////////////////////////////////////////////
/// \class sf::indexBuffer
/// \ingroup graphics
///
/// sf::indexBuffer is a simple wrapper around a dynamic
/// buffer of indices and a primitives type.
///
/// Unlike sf::indexArray, the index data is stored in
/// graphics memory.
///
/// In situations where a large amount of index data would
/// have to be transferred from system memory to graphics memory
/// every frame, using sf::indexBuffer can help. By using a
/// sf::indexBuffer, data that has not been changed between frames
/// does not have to be re-transferred from system to graphics
/// memory as would be the case with sf::indexArray. If data transfer
/// is a bottleneck, this can lead to performance gains.
///
/// Using sf::indexBuffer, the user also has the ability to only modify
/// a portion of the buffer in graphics memory. This way, a large buffer
/// can be allocated at the start of the application and only the
/// applicable portions of it need to be updated during the course of
/// the application. This allows the user to take full control of data
/// transfers between system and graphics memory if they need to.
///
/// In special cases, the user can make use of multiple threads to update
/// index data in multiple distinct regions of the buffer simultaneously.
/// This might make sense when e.g. the position of multiple objects has to
/// be recalculated very frequently. The computation load can be spread
/// across multiple threads as long as there are no other data dependencies.
///
/// Simultaneous updates to the index buffer are not guaranteed to be
/// carried out by the driver in any specific order. Updating the same
/// region of the buffer from multiple threads will not cause undefined
/// behaviour, however the final state of the buffer will be unpredictable.
///
/// Simultaneous updates of distinct non-overlapping regions of the buffer
/// are also not guaranteed to complete in a specific order. However, in
/// this case the user can make sure to synchronize the writer threads at
/// well-defined points in their code. The driver will make sure that all
/// pending data transfers complete before the index buffer is sourced
/// by the rendering pipeline.
///
/// It inherits sf::Drawable, but unlike other drawables it
/// is not transformable.
///
/// Example:
/// \code
/// sf::index indices[15];
/// ...
/// sf::indexBuffer triangles(sf::Triangles);
/// triangles.create(15);
/// triangles.update(indices);
/// ...
/// window.draw(triangles);
/// \endcode
///
/// \see sf::index, sf::indexArray
///
////////////////////////////////////////////////////////////
