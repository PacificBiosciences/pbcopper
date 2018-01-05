#ifndef PBCOPPER_STREAM_STREAM_H
#define PBCOPPER_STREAM_STREAM_H

#include <functional>

#include "pbcopper/Config.h"

namespace PacBio {
namespace Stream {

/// \brief Sink function type for data streams.
///
/// A sink function takes a data value and does some 'final' action on it, such
/// as printing.
///
/// Example:
/// \code{.cpp}
///     auto printToConsole = [](const int x) { std::cout << x; };
/// \endcode
///
template<typename T>
using Sink = std::function<void (const T&)>;

/// \brief Source function type for data streams.
///
/// A source function writes data to the sink provided.
///
/// Example:
/// \code{.cpp}
///     auto intList = [](Sink<std::int> out)
///     {
///         const std::vector<int> v = { 1,2,3,4,5 };
///         for (auto e : v)
///             out(e);
///     };
/// \endcode
///
template<typename T>
using Source = std::function<void (Sink<T>)>;

/// \brief Source function type for data streams.
///
/// A transform function takes a value, (likely) does some modification and
/// writes to the sink provided. Input & output types are allowed to differ.
///
/// Example:
/// \code{.cpp}
///     auto splitLines = [](Sink<std::string> out, std::string s)
///     {
///         auto stream = std::stringstream{ s };
///         auto token  = std::string{ };
///         while (std::getline(stream, token)
///             out(token);
///     };
///
///     auto intToString = [](Sink<std::string> out, int x)
///     {
///         out( std::to_string(x) );
///     };
/// \endcode
///
template<typename T, typename U>
using Transform = std::function<void (Sink<U>, T)>;

/// \brief This method connects a Source to a Sink, in that order.
///
/// \param source   data source
/// \param sink     data sink
///
template<typename T>
void sourceToSink(Source<T> source, Sink<T> sink)
{
    source(sink);
}

/// \brief This method connects a Transform to a Sink, in that order.
///
/// \param transform    data transform
/// \param sink         data sink
///
/// \returns a Sink function, which can transform data before the final action
///
template<typename T, typename U>
Sink<T> transformToSink (Transform<T,U> transform, Sink<U> sink)
{
    return [transform, sink](const T& value) {
        transform(sink, value);
    };
}

/// \brief This method connects a Source to a Transform, in that order.
///
/// \param source       data source
/// \param transform    data transform
///
/// \returns a Source function, which provides transformed data
///
template<typename T, typename U>
Source<U> sourceToTransform (Transform<T,U> transform, Source<T> source)
{
    return [transform, source](Sink<U> sink)
    {
        auto intermediateSink = [transform, sink](const T& value)
        {
            transform(sink, value);
        };
        source(intermediateSink);
    };
}

/// \brief This method connects a Transform to another Transform.
///
/// \param t1   upstream data transform
/// \param t2   downstream data transform
///
/// \returns a Transform function representing the composite operation
///
template<typename T, typename Intermediate, typename U>
Transform<T,U> transformToTransform(Transform<T, Intermediate> t1,
                                    Transform<Intermediate, U> t2)
{
    return [t1, t2](Sink<U> finalSink, T originalValue)
    {
        auto intermediateSink = [t2,finalSink](const Intermediate& value)
        {
            t2(finalSink, value);
        };
        t1(intermediateSink, originalValue);
    };
}

/// \brief Overloaded stream operator for chaining (source >> sink).
///
/// \param source   data source
/// \param sink     data sink
///
/// \sa sourceToSink
///
template<typename T, typename U>
void operator>>(Source<T> source, Sink<U> sink)
{
    sourceToSink(source, sink);
}

/// \brief Overloaded stream operator for chaining (source >> transform).
///
/// \param source       data source
/// \param transform    data transform
///
/// \returns a Source function, which provides transformed data
/// \sa sourceToTransform
///
template<typename T, typename U>
Source<U> operator>>(Source<T> source, Transform<T,U> transform)
{
    return sourceToTransform(transform, source);
}

/// \brief Overloaded stream operator for chaining (transform >> sink).
///
/// \param transform    data transform
/// \param sink         data sink
///
/// \returns a Sink function, which can transform data before the final action
/// \sa transformToSink
///
template<typename T, typename U>
Sink<T> operator>>(Transform<T,U> transform, Sink<U> sink)
{
    return transformToSink(transform, sink);
}

/// \brief Overloaded stream operator for chaining (transform >> transform).
///
/// \param t1   upstream data transform
/// \param t2   downstream data transform
///
/// \returns a Transform function representing the composite operation
/// \sa transformToTransform
///
template<typename T, typename Intermediate, typename U>
Transform<T,U> operator>>(Transform<T, Intermediate> t1,
                          Transform<Intermediate, U> t2)
{
    return transformToTransform(t1, t2);
}

} // namespace Stream
} // namespace PacBio

#endif // PBCOPPER_CLI_FXNCLI_H
