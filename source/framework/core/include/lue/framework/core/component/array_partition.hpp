#pragma once
#include "lue/framework/core/component/server/array_partition.hpp"
// #include "lue/framework/core/serialize/array_partition_definition.hpp"


namespace lue {

/*!
    @brief      Class for array partition component client instances
    @param      .
    @return     .
    @exception  .

    Each instance is associated with a single array partition component
    server instance.
*/
template<
    typename Element,
    std::size_t rank>
class ArrayPartition:
    public hpx::components::client_base<
        ArrayPartition<Element, rank>,
        server::ArrayPartition<Element, rank>>

{

public:

    using Server = server::ArrayPartition<Element, rank>;

    using Data = typename Server::Data;

    using Base =
        hpx::components::client_base<ArrayPartition<Element, rank>, Server>;

    using Shape = typename Data::Shape;

                   ArrayPartition      ();

                   ArrayPartition      (hpx::id_type const& id);

                   ArrayPartition      (hpx::future<hpx::id_type>&&
                                            component_id);

                   ArrayPartition      (hpx::future<ArrayPartition>&&
                                            component);

                   ArrayPartition      (hpx::id_type const& locality_id,
                                        Shape const& shape);

                   ArrayPartition      (hpx::id_type const& locality_id,
                                        Shape const& shape,
                                        Element value);

                   ArrayPartition      (hpx::id_type component_id,
                                        Data const& data);

                   ArrayPartition      (ArrayPartition const&)=default;

                   ArrayPartition      (ArrayPartition&&)=default;

                   ~ArrayPartition     ()=default;

    ArrayPartition& operator=          (ArrayPartition const&)=default;

    ArrayPartition& operator=          (ArrayPartition&&)=default;

    // std::shared_ptr<Server> component  () const;

    hpx::future<Data> data             () const;

    hpx::future<void> fill             (Element value);

    hpx::future<Shape> shape           () const;

    hpx::future<void> resize           (Shape const& shape);

private:

};


template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition():

    Base{}

{
}


template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type const& id):

    Base{id}

{
}


/*!
    @brief      Construct an instance based on an existing and possibly
                remote component with ID @a component_id
    @param      component_id ID of component
*/
template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::future<hpx::id_type>&& component_id):

    Base{std::move(component_id)}

{
}


/*!
    @brief      Construct an instance based on an existing @a component
    @param      component A future to a component

    A partition already holds a future to the id of the referenced object.
    Unwrapping accesses this inner future.
*/
template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::future<ArrayPartition>&& component):

    Base{std::move(component)}

{
}


template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type const& locality_id,
    Shape const& shape):

    Base{hpx::new_<Server>(locality_id, shape)}

{
}


template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type const& locality_id,
    Shape const& shape,
    Element const value):

    Base{hpx::new_<Server>(locality_id, shape, value)}

{
}


/*!
    @brief      Construct an instance on the same locality as an existing
                component's id and initial @a data
    @param      component_id ID of an existing component
    @param      data Initial data
*/
template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    hpx::id_type component_id,
    Data const& data):

    Base{hpx::new_<Server>(hpx::colocated(component_id), data)}

{
}


// template<
//     typename Value,
//     typename Data>
// ArrayPartition<Value, Data>::ArrayPartition(
//     ArrayPartition const& other):
// 
//     Base{other}
// 
// {
// }


// template<
//     typename Value,
//     typename Data>
// ArrayPartition<Value, Data>::ArrayPartition(
//     ArrayPartition&& other):
// 
//     Base{std::move(other)}
// 
// {
// }


// template<
//     typename Value,
//     typename Data>
// ArrayPartition<Value, Data>& ArrayPartition<Value, Data>::operator=(
//     ArrayPartition const& other)
// {
//     Base::operator=(other);
// 
//     return *this;
// }


// template<
//     typename Value,
//     typename Data>
// ArrayPartition<Value, Data>& ArrayPartition<Value, Data>::operator=(
//     ArrayPartition&& other)
// {
//     Base::operator=(std::move(other));
// 
//     return *this;
// }


// /*!
//     @brief      Return a pointer to the underlying component server instance
// */
// template<
//     typename Value,
//     typename Data>
// std::shared_ptr<typename ArrayPartition<Value, Data>::Server>
//     ArrayPartition<Value, Data>::component() const
// {
//     // this->get_id() identifies the server instance
//     return hpx::get_ptr<Server>(this->get_id()).get();
// }


/*!
    @brief      Return underlying data

    Although a future to a copy is returned, the underlying Data instance
    is an ArrayPartitionedData, which contains a shared pointer to
    the actual multidimensional array. These are cheap to copy (but
    possibly surprising to use).
*/
template<
    typename Element,
    std::size_t rank>
hpx::future<typename ArrayPartition<Element, rank>::Data>
    ArrayPartition<Element, rank>::data() const
{
    typename Server::DataAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id());
}


template<
    typename Element,
    std::size_t rank>
hpx::future<typename ArrayPartition<Element, rank>::Shape>
    ArrayPartition<Element, rank>::shape() const
{
    typename Server::ShapeAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id());
}


/*!
    @brief      Asynchronously fill the partition with @a value
*/
template<
    typename Element,
    std::size_t rank>
hpx::future<void> ArrayPartition<Element, rank>::fill(
    Element value)
{
    typename Server::FillAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id(), value);
}


/*!
    @brief      Asynchronously resize the partition with @a value
*/
template<
    typename Element,
    std::size_t rank>
hpx::future<void> ArrayPartition<Element, rank>::resize(
    Shape const& shape)
{
    typename Server::ResizeAction action;

    // this->get_id() identifies the server instance
    return hpx::async(action, this->get_id(), shape);
}


template<
    typename Element,
    std::size_t rank>
class ArrayPartitionTypeTraits<ArrayPartition<Element, rank>>
{

private:

    // Use template parameters to create Partition type

    using Partition = ArrayPartition<Element, rank>;

public:

    // Only use Partition, not the template parameters

    using ServerType = typename Partition::Server;
    using DataType = typename Partition::Data;
    using ShapeType = typename Partition::Shape;

    template<
        typename ElementType>
    using DataTemplate =
        typename ArrayPartitionDataTypeTraits<DataType>::
            template DataTemplate<ElementType>;

    template<
        typename ElementType>
    using PartitionTemplate = ArrayPartition<ElementType, rank>;

};

}  // namespace lue