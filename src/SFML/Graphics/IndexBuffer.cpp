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
#include <SFML/Graphics/IndexBuffer.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/System/Err.hpp>
#include <mutex>
#include <utility>
#include <ostream>
#include <cstddef>
#include <cstring>

namespace
{
    // A nested named namespace is used here to allow unity builds of SFML.
    namespace VertexBufferImpl
    {
        std::recursive_mutex isAvailableMutex;

        GLenum usageToGlEnum(sf::IndexBuffer::Usage usage)
        {
            switch (usage)
            {
            case sf::IndexBuffer::Static:  return GLEXT_GL_STATIC_DRAW;
            case sf::IndexBuffer::Dynamic: return GLEXT_GL_DYNAMIC_DRAW;
            default:                        return GLEXT_GL_STREAM_DRAW;
            }
        }
    }
}


namespace sf
{
    ////////////////////////////////////////////////////////////
    IndexBuffer::IndexBuffer() :
        m_buffer(0),
        m_size(0),
        m_usage(Stream)
    {
    }

    ////////////////////////////////////////////////////////////
    IndexBuffer::IndexBuffer(IndexBuffer::Usage usage) :
        m_buffer(0),
        m_size(0),
        m_usage(usage)
    {
    }

    ////////////////////////////////////////////////////////////
    IndexBuffer::IndexBuffer(const IndexBuffer& copy) :
        m_buffer(0),
        m_size(0),
        m_usage(copy.m_usage)
    {
        if (copy.m_buffer && copy.m_size)
        {
            if (!create(copy.m_size))
            {
                err() << "Could not create index buffer for copying" << std::endl;
                return;
            }

            if (!update(copy))
                err() << "Could not copy index buffer" << std::endl;
        }
    }


    ////////////////////////////////////////////////////////////
    IndexBuffer::~IndexBuffer()
    {
        if (m_buffer)
        {
            TransientContextLock contextLock;

            glCheck(GLEXT_glDeleteBuffers(1, &m_buffer));
        }
    }


    ////////////////////////////////////////////////////////////
    bool IndexBuffer::create(std::size_t indexCount)
    {
        if (!isAvailable())
            return false;

        TransientContextLock contextLock;

        if (!m_buffer)
            glCheck(GLEXT_glGenBuffers(1, &m_buffer));

        if (!m_buffer)
        {
            err() << "Could not create index buffer, generation failed" << std::endl;
            return false;
        }

        glCheck(GLEXT_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer));
        glCheck(GLEXT_glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptrARB>(sizeof(GLuint) * indexCount), nullptr, VertexBufferImpl::usageToGlEnum(m_usage)));
        glCheck(GLEXT_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        m_size = indexCount;

        return true;
    }


    ////////////////////////////////////////////////////////////
    std::size_t IndexBuffer::getIndexCount() const
    {
        return m_size;
    }


    ////////////////////////////////////////////////////////////
    bool IndexBuffer::update(const GLuint* indices)
    {
        return update(indices, m_size, 0);
    }


    ////////////////////////////////////////////////////////////
    bool IndexBuffer::update(const GLuint* indices, std::size_t indexCount, unsigned int offset)
    {
        // Sanity checks
        if (!m_buffer)
            return false;

        if (!indices)
            return false;

        if (offset && (offset + indexCount > m_size))
            return false;

        TransientContextLock contextLock;

        glCheck(GLEXT_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer));

        // Check if we need to resize or orphan the buffer
        if (indexCount >= m_size)
        {
            glCheck(GLEXT_glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptrARB>(sizeof(GLuint) * indexCount), nullptr, VertexBufferImpl::usageToGlEnum(m_usage)));

            m_size = indexCount;
        }

        glCheck(GLEXT_glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLintptrARB>(sizeof(GLuint) * offset), static_cast<GLsizeiptrARB>(sizeof(GLuint) * indexCount), indices));

        glCheck(GLEXT_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        return true;
    }


    ////////////////////////////////////////////////////////////
    bool IndexBuffer::update([[maybe_unused]] const IndexBuffer& indexBuffer)
    {
#ifdef SFML_OPENGL_ES

        return false;

#else

        if (!m_buffer || !indexBuffer.m_buffer)
            return false;

        TransientContextLock contextLock;

        // Make sure that extensions are initialized
        sf::priv::ensureExtensionsInit();

        if (GLEXT_copy_buffer)
        {
            glCheck(GLEXT_glBindBuffer(GLEXT_GL_COPY_READ_BUFFER, indexBuffer.m_buffer));
            glCheck(GLEXT_glBindBuffer(GLEXT_GL_COPY_WRITE_BUFFER, m_buffer));

            glCheck(GLEXT_glCopyBufferSubData(GLEXT_GL_COPY_READ_BUFFER, GLEXT_GL_COPY_WRITE_BUFFER, 0, 0, static_cast<GLsizeiptr>(sizeof(GLuint) * indexBuffer.m_size)));

            glCheck(GLEXT_glBindBuffer(GLEXT_GL_COPY_WRITE_BUFFER, 0));
            glCheck(GLEXT_glBindBuffer(GLEXT_GL_COPY_READ_BUFFER, 0));

            return true;
        }

        glCheck(GLEXT_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer));
        glCheck(GLEXT_glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptrARB>(sizeof(GLuint) * indexBuffer.m_size), nullptr, VertexBufferImpl::usageToGlEnum(m_usage)));

        void* destination = nullptr;
        glCheck(destination = GLEXT_glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GLEXT_GL_WRITE_ONLY));

        glCheck(GLEXT_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.m_buffer));

        void* source = nullptr;
        glCheck(source = GLEXT_glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GLEXT_GL_READ_ONLY));

        std::memcpy(destination, source, sizeof(GLuint) * indexBuffer.m_size);

        GLboolean sourceResult = GL_FALSE;
        glCheck(sourceResult = GLEXT_glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));

        glCheck(GLEXT_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer));

        GLboolean destinationResult = GL_FALSE;
        glCheck(destinationResult = GLEXT_glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));

        glCheck(GLEXT_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        if ((sourceResult == GL_FALSE) || (destinationResult == GL_FALSE))
            return false;

        return true;

#endif // SFML_OPENGL_ES
    }


    ////////////////////////////////////////////////////////////
    IndexBuffer& IndexBuffer::operator =(const IndexBuffer& right)
    {
        IndexBuffer temp(right);

        swap(temp);

        return *this;
    }


    ////////////////////////////////////////////////////////////
    void IndexBuffer::swap(IndexBuffer& right)
    {
        std::swap(m_size, right.m_size);
        std::swap(m_buffer, right.m_buffer);
        std::swap(m_usage, right.m_usage);
    }


    ////////////////////////////////////////////////////////////
    unsigned int IndexBuffer::getNativeHandle() const
    {
        return m_buffer;
    }


    ////////////////////////////////////////////////////////////
    void IndexBuffer::bind(const IndexBuffer* indexBuffer)
    {
        if (!isAvailable())
            return;

        TransientContextLock lock;

        glCheck(GLEXT_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer ? indexBuffer->m_buffer : 0));
    }

    ////////////////////////////////////////////////////////////
    void IndexBuffer::setUsage(IndexBuffer::Usage usage)
    {
        m_usage = usage;
    }


    ////////////////////////////////////////////////////////////
    IndexBuffer::Usage IndexBuffer::getUsage() const
    {
        return m_usage;
    }


    ////////////////////////////////////////////////////////////
    bool IndexBuffer::isAvailable()
    {
        //TODO: add this function
        return true;
    }

} // namespace sf
