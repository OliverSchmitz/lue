#pragma once
#include "lue/framework/core/define.hpp"
#include "lue/framework/core/type_traits.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/core/span.hpp"
#include <boost/container/vector.hpp>
#include <cassert>


namespace lue {
// namespace detail {

template<
    typename Element,
    Rank rank>
class Array
{

public:

    using Span = DynamicSpan<Element, rank>;

    using Shape = lue::Shape<lue::Index, rank>;

    using Elements = boost::container::vector<Element>;

    using Iterator = typename Elements::iterator;

    using ConstIterator = typename Elements::const_iterator;

                   Array               ()=default;

                   Array               (Shape const& shape);

                   Array               (Shape const& shape,
                                        Element value);

                   Array               (Shape const& shape,
                                        std::initializer_list<Element> values);

    template<
        typename InputIterator>
                   Array               (Shape const& shape,
                                        InputIterator begin,
                                        InputIterator end);

                   Array               (Array const&)=default;

                   Array               (Array&&)=default;

                   ~Array              ()=default;

    Array&         operator=           (Array const&)=default;

    Array&         operator=           (Array&&)=default;

    Shape const&   shape               () const;

    Iterator       begin               ();

    Iterator       end                 ();

    ConstIterator  begin               () const;

    ConstIterator  end                 () const;

    void           reshape             (Shape const& shape);

    Element const* data                () const;

    Element*       data                ();

    template<
        typename... Indxs>
    Element const& operator()(
        Indxs... idxs) const
    {
        return _span(idxs...);
    }

    template<
        typename... Indxs>
    Element& operator()(
        Indxs... idxs)
    {
        return _span(idxs...);
    }

    Element const  operator[]          (std::size_t idx) const;

    Element        operator[]          (std::size_t idx);

protected:

private:

    // Shape of array
    Shape          _shape;

    // 1D buffer with array elements
    Elements       _elements;

    // Span for converting nD indices to linear indices
    Span           _span;

};


template<
    typename Element,
    Rank rank>
Array<Element, rank>::Array(
    Shape const& shape):

    _shape{shape},
    _elements(lue::nr_elements(shape)),
    _span{_elements.data(), _shape}

{
}


template<
    typename Element,
    Rank rank>
Array<Element, rank>::Array(
    Shape const& shape,
    Element const value):

    _shape{shape},
    _elements(lue::nr_elements(shape), value),
    _span{_elements.data(), _shape}

{
}


template<
    typename Element,
    Rank rank>
template<
    typename InputIterator>
Array<Element, rank>::Array(
    Shape const& shape,
    InputIterator begin,
    InputIterator end):

    _shape{shape},
    _elements(lue::nr_elements(shape)),
    _span{_elements.data(), _shape}

{
    std::move(begin, end, _elements.begin());

    assert(static_cast<Size>(_elements.size()) == lue::nr_elements(_shape));
}


template<
    typename Element,
    Rank rank>
Array<Element, rank>::Array(
    Shape const& shape,
    std::initializer_list<Element> values):

    _shape{shape},
    _elements(lue::nr_elements(shape)),
    _span{_elements.data(), _shape}

{
    assert(
        std::distance(values.begin(), values.end()) ==
        lue::nr_elements(_shape));

    std::move(values.begin(), values.end(), _elements.begin());

    assert(static_cast<Size>(_elements.size()) == lue::nr_elements(_shape));
}


template<
    typename Element,
    Rank rank>
typename Array<Element, rank>::Shape const& Array<Element, rank>::shape() const
{
    return _shape;
}


template<
    typename Element,
    Rank rank>
typename Array<Element, rank>::Iterator Array<Element, rank>::begin()
{
    return _elements.begin();
}


template<
    typename Element,
    Rank rank>
typename Array<Element, rank>::ConstIterator Array<Element, rank>::begin() const
{
    return _elements.begin();
}


template<
    typename Element,
    Rank rank>
typename Array<Element, rank>::Iterator Array<Element, rank>::end()
{
    return _elements.end();
}


template<
    typename Element,
    Rank rank>
typename Array<Element, rank>::ConstIterator Array<Element, rank>::end() const
{
    return _elements.end();
}


template<
    typename Element,
    Rank rank>
Element const* Array<Element, rank>::data() const
{
    return _elements.data();
}


template<
    typename Element,
    Rank rank>
Element* Array<Element, rank>::data()
{
    return _elements.data();
}


template<
    typename Element,
    Rank rank>
void Array<Element, rank>::reshape(
    Shape const& shape)
{
    if(_shape != shape) {
        _elements.resize(lue::nr_elements(shape));
        _shape = shape;
        _span = Span{_elements.data(), _shape};
    }
}

// } // namespace detail


// template<
//     typename Element,
//     Rank rank>
// class Array:
//     public detail::Array<Element, rank>
// {
// 
// public:
// 
//     using detail::Array<Element, rank>::Array;
// 
//     Element const  operator[]          (std::size_t idx) const;
// 
//     Element        operator[]          (std::size_t idx);
// 
// };


template<
    typename Element,
    Rank rank>
Element const Array<Element, rank>::operator[](
    std::size_t const idx) const
{
    return this->data()[idx];
}


template<
    typename Element,
    Rank rank>
Element Array<Element, rank>::operator[](
    std::size_t const idx)
{
    return this->data()[idx];
}


// template<
//     typename Element>
// class Array<Element, 1>:
//     public detail::Array<Element, 1>
// {
// 
// public:
// 
//     using detail::Array<Element, 1>::Array;
// 
//     Element&       operator[]          (Index idx);
// 
//     Element const& operator[]          (Index idx) const;
// 
// };
// 
// 
// template<
//     typename Element>
// Element const& Array<Element, 1>::operator[](
//     Index const idx) const
// {
//     return this->data()[idx];
// }
// 
// 
// template<
//     typename Element>
// Element& Array<Element, 1>::operator[](
//     Index const idx)
// {
//     return this->data()[idx];
// }


namespace detail {

template<
    typename E,
    Rank r>
class ArrayTraits<lue::Array<E, r>>
{

public:

    using Element = E;

    constexpr static Rank rank = r;

    using Shape = typename lue::Array<E, r>::Shape;

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
inline auto const& shape(
    Array<Element, rank> const& array)
{
    return array.shape();
}


template<
    typename Element,
    Rank rank>
inline auto nr_elements(
    Array<Element, rank> const& array)
{
    return nr_elements(shape(array));
}

}  // namespace lue
