#ifndef OPENGL_OPENGLEXCEPTION_HPP
#define OPENGL_OPENGLEXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace OpenGL
{

/**
 * \brief This class represent the OpenGL Exception.
 *
 * This class is used for throwing an exception about OpenGL content. It is
 * derived from std::runtime_error.
 */
class Exception : public std::runtime_error
{
public:
    /**
     * \brief Initializes a new instance of the OpenGL Exception class with
     * specified \a message.
     *
     * \param message specified exception message.
     */
    explicit Exception(const char *message);
    /**
     * \overload
     *
     * \param message specified exception message.
     */
    explicit Exception(const std::string &message);
};

} // namespace OpenGL

#endif // SAMPLECODE_OPENGL_OPENGLEXCEPTION_HPP_
