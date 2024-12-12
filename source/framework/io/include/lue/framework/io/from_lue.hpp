#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/io/configure.hpp"  // LUE_USE_PARALLEL_IO
#include "lue/framework/io/io_strategy.hpp"
#include "lue/framework/io/util.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/data_model/hl/util.hpp"
#include "lue/data_model.hpp"
#ifdef LUE_USE_PARALLEL_IO
// Only available in case MPI is used in HPX
#include <hpx/mpi_base/mpi_environment.hpp>
#endif


/*!
    @file

    Thiѕ header contains the implementation of the various from_lue function overloads.

    The goal is to return an array as soon as possible, allowing the caller to create more work, while
    the read-tasks start the reading. The idea is that, as soon as partitions have been read, they can
    participate in computations, while other partitions are still being read.
*/


namespace lue {
    namespace detail {

        // template<typename Element, Rank rank>
        // auto read_block(
        //     data_model::Array const& array,
        //     hdf5::Dataset::TransferPropertyList const& transfer_property_list,
        //     hdf5::Hyperslab const& block_hyperslab,
        //     ShapeT<Array<Element, rank>> const& block_shape) -> Array<Element, rank>
        // {
        //     using Block = Array<Element, rank>;

        //     Block block{block_shape};
        //     hdf5::Datatype const memory_datatype{hdf5::native_datatype<Element>()};

        //     // Read values into the block
        //     array.read(memory_datatype, block_hyperslab, transfer_property_list, block.data());

        //     // TODO Use no-data policy
        //     // If no-data in the dataset, write no-data to the partition

        //     return block;
        // }


        // template<typename Partition>
        // void copy_partition_elements(
        //     Array<ElementT<Partition>, rank<Partition>> const& block,
        //     Offset<Index, rank<Partition>> const& block_offset,
        //     PartitionTuples<OffsetT<Partition>, ShapeT<Partition>, ElementT<Partition>> const&
        //         partition_tuples)
        // {
        //     // block_offset: Offset of the block within the array
        //     // hyperslab. It is required to be able to transpate partition
        //     // offsets, which are relative to the array hyperslab, to
        //     // offsets within the block read.

        //     using Offset = OffsetT<Partition>;
        //     using Slice = typename Array<ElementT<Partition>, rank<Partition>>::Slice;

        //     // Copy hyperslabs from block into the individual partitions
        //     for (auto& [partition_offset, partition_shape, partition_buffer] : partition_tuples)
        //     {
        //         // Select hyperslab corresponding to this partition from
        //         // block and copy the values to the partition's buffer

        //         // Offset of the partition relative to the block!
        //         Offset partition_block_offset{
        //             std::get<0>(partition_offset) - std::get<0>(block_offset),
        //             std::get<1>(partition_offset) - std::get<1>(block_offset)};

        //         Slice partition_row_slice{
        //             std::get<0>(partition_block_offset),
        //             std::get<0>(partition_block_offset) + std::get<0>(partition_shape)};
        //         Slice partition_col_slice{
        //             std::get<1>(partition_block_offset),
        //             std::get<1>(partition_block_offset) + std::get<1>(partition_shape)};

        //         copy(submdspan(block.span(), partition_row_slice, partition_col_slice), partition_buffer);
        //     }
        // }


        template<typename Policies, typename Partitions>
        auto read_partitions_constant(
            Policies const& policies,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,  // Only needed to offset block read from array
            data_model::ID const object_id,
            Partitions&& partitions) -> Partitions
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            // Open dataset. Configure for use of parallel I/O if necessary.
            hdf5::File::AccessPropertyList access_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            if (hpx::util::mpi_environment::enabled())
            {
                // // ::MPI_Comm communicator{hpx::util::mpi_environment::communicator()};
                // ::MPI_Comm communicator{MPI_COMM_WORLD};
                // ::MPI_Info info{MPI_INFO_NULL};
                // access_property_list.use_mpi_communicator(communicator, info);
            }
#endif

            auto const dataset{
                data_model::open_dataset(dataset_pathname, H5F_ACC_RDONLY, access_property_list)};

            // Open phenomenon
            auto const& phenomenon{dataset.phenomena()[phenomenon_name]};

            // Open property-set
            auto const& property_set{phenomenon.property_sets()[property_set_name]};

            // Open property
            lue_hpx_assert(property_set.properties().contains(property_name));
            lue_hpx_assert(
                property_set.properties().shape_per_object(property_name) ==
                data_model::ShapePerObject::different);
            lue_hpx_assert(
                property_set.properties().value_variability(property_name) ==
                data_model::ValueVariability::constant);
            using Properties = data_model::different_shape::Properties;
            auto const& property{property_set.properties().collection<Properties>()[property_name]};

            // Open value. Configure for use of parallel I/O if necessary.
            hdf5::Dataset::TransferPropertyList transfer_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            // if(hpx::util::mpi_environment::enabled())
            // {
            //     // Use collective I/O
            //     transfer_property_list.set_transfer_mode(::H5FD_MPIO_COLLECTIVE);
            // }
#endif

            // Iterate over partitions and read each partition's piece from the dataset
            auto const& value{property.value()};
            auto const array{value[object_id]};

            using Partition = typename Partitions::value_type;
            using Element = ElementT<Partition>;
            // Rank const rank{lue::rank<Partition>};


            // TODO Switch on some strategy

            // // -----------------------------------
            // // Read the whole block containing the partitions and distribute parts of it over the
            // partitions.
            // // This is faster than reading small blocks per individual partition. At the cost of reading
            // too
            // // much and having to allocate more memory.

            // // NOTE: We assume here that the partitions are clustered...

            // {
            //     using Block = Array<Element, rank>;

            //     auto [partition_tuples, block_offset, block_shape] = detail::partition_tuples(partitions);
            //     Block block{read_block<Element, rank>(
            //         array,
            //         transfer_property_list,
            //         block_hyperslab(array_hyperslab_start, block_offset, block_shape),
            //         block_shape)};
            //     copy_partition_elements<Partition>(block, block_offset, partition_tuples);
            // }
            // // -----------------------------------


            // -----------------------------------
            // Approach reading partitions individually

            // For each partition, spawn a task that will read from the dataset into the array. This task
            // returns a partition component client which becomes ready once the reading has finished. Use
            // continuations to serialize the reads.

            Partitions new_partitions(std::size(partitions));
            hdf5::Datatype const memory_datatype{hdf5::native_datatype<Element>()};

            for (std::size_t partition_idx = 0; partition_idx < std::size(partitions); ++partition_idx)
            {
                Partition const& partition{partitions[partition_idx]};
                lue_hpx_assert(partition.is_ready());

                auto read_partition = [policies,
                                       partition,
                                       array_hyperslab_start,
                                       array,
                                       memory_datatype,
                                       transfer_property_list]() -> Partition
                {
                    auto partition_ptr{detail::ready_component_ptr(partition)};
                    auto& partition_server{*partition_ptr};
                    Element* buffer{partition_server.data().data()};

                    array.read(
                        memory_datatype,
                        hyperslab(array_hyperslab_start, partition_server),
                        transfer_property_list,
                        buffer);

                    // TODO Use no-data policy
                    // If no-data in the dataset, write no-data to the partition

                    return partition;
                };

                if (partition_idx == 0)
                {
                    new_partitions[partition_idx] = hpx::async(read_partition);
                }
                else
                {
                    lue_hpx_assert(new_partitions[partition_idx - 1].valid());
                    new_partitions[partition_idx] = new_partitions[partition_idx - 1].then(
                        [read_partition]([[maybe_unused]] Partition const& previous_new_partition)
                        { return read_partition(); });
                }

                lue_hpx_assert(new_partitions[partition_idx].valid());
            }
            // -----------------------------------

            return new_partitions;
        }


        template<typename Policies, typename Partitions>
        auto read_partitions_variable(
            Policies const& policies,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,
            data_model::ID const object_id,
            Index const time_step_idx,
            Partitions&& partitions) -> Partitions
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            // Open dataset. Configure for use of parallel I/O if necessary.
            hdf5::File::AccessPropertyList access_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            // if(hpx::util::mpi_environment::enabled())
            // {
            //     // ::MPI_Comm communicator{hpx::util::mpi_environment::communicator()};
            //     ::MPI_Comm communicator{MPI_COMM_WORLD};
            //     ::MPI_Info info{MPI_INFO_NULL};
            //     access_property_list.use_mpi_communicator(communicator, info);
            // }
#endif

            auto const dataset{
                data_model::open_dataset(dataset_pathname, H5F_ACC_RDONLY, access_property_list)};

            // Open phenomenon
            auto const& phenomenon{dataset.phenomena()[phenomenon_name]};

            // Open property-set
            auto const& property_set{phenomenon.property_sets()[property_set_name]};

            // Open property
            lue_hpx_assert(property_set.properties().contains(property_name));
            lue_hpx_assert(
                property_set.properties().shape_per_object(property_name) ==
                data_model::ShapePerObject::different);
            lue_hpx_assert(
                property_set.properties().value_variability(property_name) ==
                data_model::ValueVariability::variable);
            lue_hpx_assert(
                property_set.properties().shape_variability(property_name) ==
                data_model::ShapeVariability::constant);
            using Properties = data_model::different_shape::constant_shape::Properties;
            auto const& property{property_set.properties().collection<Properties>()[property_name]};

            // Open value. Configure for use of parallel I/O if necessary.
            hdf5::Dataset::TransferPropertyList transfer_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            // if(hpx::util::mpi_environment::enabled())
            // {
            //     // Use collective I/O
            //     transfer_property_list.set_transfer_mode(::H5FD_MPIO_COLLECTIVE);
            // }
#endif

            // Iterate over partitions and read each partition's piece from the dataset
            auto const& value{property.value()};
            auto const array{value[object_id]};

            using Partition = typename Partitions::value_type;
            using Element = ElementT<Partition>;

            // TODO Switch on some strategy

            // // -----------------------------------
            // {
            //     using Block = Array<Element, rank>;

            //     auto [partition_tuples, block_offset, block_shape] = detail::partition_tuples(partitions);
            //     Block block{read_block<Element, rank>(
            //         array,
            //         transfer_property_list,
            //         block_hyperslab(array_hyperslab_start, time_step_idx, block_offset, block_shape),
            //         block_shape)};

            //     copy_partition_elements<Partition>(block, block_offset, partition_tuples);
            // }
            // // -----------------------------------

            // -----------------------------------
            // Approach reading partitions individually

            Partitions new_partitions(std::size(partitions));
            hdf5::Datatype const memory_datatype{hdf5::native_datatype<Element>()};

            for (std::size_t partition_idx = 0; partition_idx < std::size(partitions); ++partition_idx)
            {
                Partition const& partition{partitions[partition_idx]};
                lue_hpx_assert(partition.is_ready());

                auto read_partition = [policies,
                                       partition,
                                       array_hyperslab_start,
                                       time_step_idx,
                                       array,
                                       memory_datatype,
                                       transfer_property_list]() -> Partition
                {
                    auto partition_ptr{detail::ready_component_ptr(partition)};
                    auto& partition_server{*partition_ptr};
                    Element* buffer{partition_server.data().data()};

                    array.read(
                        memory_datatype,
                        hyperslab(array_hyperslab_start, partition_server, 0, time_step_idx),
                        transfer_property_list,
                        buffer);

                    // TODO Use no-data policy
                    // If no-data in the dataset, write no-data to the partition

                    return partition;
                };

                if (partition_idx == 0)
                {
                    new_partitions[partition_idx] = hpx::async(read_partition);
                }
                else
                {
                    lue_hpx_assert(new_partitions[partition_idx - 1].valid());
                    new_partitions[partition_idx] = new_partitions[partition_idx - 1].then(
                        [read_partition]([[maybe_unused]] Partition const& previous_new_partition)
                        { return read_partition(); });
                }

                lue_hpx_assert(new_partitions[partition_idx].valid());
            }
            // -----------------------------------

            return new_partitions;
        }


        template<typename Policies, typename Partitions>
        struct ReadPartitionsConstantAction:
            hpx::actions::make_action<
                decltype(&read_partitions_constant<Policies, Partitions>),
                &read_partitions_constant<Policies, Partitions>,
                ReadPartitionsConstantAction<Policies, Partitions>>::type
        {
        };


        template<typename Policies, typename Partitions>
        struct ReadPartitionsVariableAction:
            hpx::actions::make_action<
                decltype(&read_partitions_variable<Policies, Partitions>),
                &read_partitions_variable<Policies, Partitions>,
                ReadPartitionsVariableAction<Policies, Partitions>>::type
        {
        };


        template<typename Policies, Rank rank>
        [[nodiscard]] auto from_lue(
            Policies const& policies,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,
            data_model::ID const object_id,
            PartitionedArray<policy::OutputElementT<Policies>, rank>&& array)
            -> PartitionedArray<policy::OutputElementT<Policies>, rank>
        {
            // We can read from a dataset from multiple localities (processes) at the same time. We cannot
            // read from a dataset from multiple threads in a single process at the same time. We cannot
            // assume that the HDF5 library is thread safe. → Reads into partitions located in different
            // localities don't have to be serialized → Reads into partitions located in the same locality
            // must be serialized

            // 1. Group partitions per locality
            // 2. Per group, spawn a task which reads each partition, in turn

            using Element = policy::OutputElementT<Policies>;
            using Array = PartitionedArray<Element, rank>;
            using Partition = PartitionT<Array>;

            auto partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};
            lue_hpx_assert(partition_idxs_by_locality.size() <= hpx::find_all_localities().size());

            using Action = ReadPartitionsConstantAction<Policies, std::vector<Partition>>;
            Action action{};

            for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
            {
                std::vector<Partition> partitions{};
                partitions.resize(partition_idxs.size());

                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    partitions[idx++] = array.partitions()[partition_idx];
                }

                std::vector<hpx::future<Partition>> partition_fs = hpx::split_future<Partition>(
                    hpx::dataflow(
                        hpx::launch::async,
                        hpx::unwrapping(

                            [locality = locality,
                             action,
                             policies,
                             array_pathname,
                             array_hyperslab_start,
                             object_id](std::vector<Partition>&& partitions)
                            {
                                return action(
                                    locality,
                                    policies,
                                    array_pathname,
                                    array_hyperslab_start,
                                    object_id,
                                    std::move(partitions));
                            }),

                        hpx::when_all(partitions.begin(), partitions.end())),
                    std::size(partitions));

                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    // Replace the current partition with a new one which becomes ready once the reading has
                    // finished. Under the hood the new partition is the same and the current one, but this is
                    // not important here.
                    array.partitions()[partition_idx] = partition_fs[idx++].then(
                        [](auto&& partition_f)
                        {
                            lue_hpx_assert(partition_f.valid());
                            lue_hpx_assert(partition_f.is_ready());

                            Partition partition{partition_f.get()};

                            lue_hpx_assert(partition.valid());

                            return partition;
                        });
                }
            }

            for (auto const& partition : array.partitions())
            {
                lue_hpx_assert(partition.valid());
            }

            return array;
        }


        template<typename Policies, Rank rank>
        [[nodiscard]] auto from_lue(
            Policies const& policies,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,
            data_model::ID const object_id,
            Index const time_step_idx,
            PartitionedArray<policy::OutputElementT<Policies>, rank>&& array)
            -> PartitionedArray<policy::OutputElementT<Policies>, rank>
        {
            using Element = policy::OutputElementT<Policies>;
            using Array = PartitionedArray<Element, rank>;
            using Partition = PartitionT<Array>;

            auto partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};
            lue_hpx_assert(partition_idxs_by_locality.size() <= hpx::find_all_localities().size());

            using Action = ReadPartitionsVariableAction<Policies, std::vector<Partition>>;
            Action action{};

            for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
            {
                std::vector<Partition> partitions{};
                partitions.resize(partition_idxs.size());

                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    partitions[idx++] = array.partitions()[partition_idx];
                }

                std::vector<hpx::future<Partition>> partition_fs = hpx::split_future<Partition>(
                    hpx::dataflow(
                        hpx::launch::async,
                        hpx::unwrapping(

                            [locality = locality,
                             action,
                             policies,
                             array_pathname,
                             array_hyperslab_start,
                             object_id,
                             time_step_idx](std::vector<Partition>&& partitions)
                            {
                                return action(
                                    locality,
                                    policies,
                                    array_pathname,
                                    array_hyperslab_start,
                                    object_id,
                                    time_step_idx,
                                    std::move(partitions));
                            }),

                        hpx::when_all(partitions.begin(), partitions.end())),
                    std::size(partitions));

                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    // Replace the current partition with a new one which becomes ready once the reading has
                    // finished. Under the hood the new partition is the same and the current one, but this is
                    // not important here.
                    array.partitions()[partition_idx] = partition_fs[idx++].then(
                        [](auto&& partition_f)
                        {
                            lue_hpx_assert(partition_f.valid());
                            lue_hpx_assert(partition_f.is_ready());

                            Partition partition{partition_f.get()};

                            lue_hpx_assert(partition.valid());

                            return partition;
                        });
                }
            }

            for (auto const& partition : array.partitions())
            {
                lue_hpx_assert(partition.valid());
            }

            return array;
        }


        template<typename Shape>
        auto constant_array_shape(std::string const& array_pathname) -> Shape
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            namespace lh5 = lue::hdf5;
            namespace ldm = lue::data_model;

            using DatasetPtr = std::shared_ptr<ldm::Dataset>;
            using RasterView = ldm::constant::RasterView<DatasetPtr>;

            auto input_dataset_ptr =
                std::make_shared<ldm::Dataset>(ldm::open_dataset(dataset_pathname, H5F_ACC_RDONLY));

            lh5::Shape grid_shape{};

            // Open a view on the raster
            RasterView input_raster_view{input_dataset_ptr, phenomenon_name, property_set_name};

            grid_shape = lh5::Shape{input_raster_view.grid_shape()};

            lue_hpx_assert(rank<Shape> == 2);  // TODO(KDJ)

            return Shape{
                {static_cast<typename Shape::value_type>(grid_shape[0]),
                 static_cast<typename Shape::value_type>(grid_shape[1])}};
        }


        template<typename Shape>
        auto variable_array_shape(std::string const& array_pathname) -> Shape
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            namespace lh5 = lue::hdf5;
            namespace ldm = lue::data_model;

            using DatasetPtr = std::shared_ptr<ldm::Dataset>;
            using RasterView = ldm::variable::RasterView<DatasetPtr>;

            auto input_dataset_ptr =
                std::make_shared<ldm::Dataset>(ldm::open_dataset(dataset_pathname, H5F_ACC_RDONLY));

            lh5::Shape grid_shape{};

            // Open a view on the raster
            RasterView input_raster_view{input_dataset_ptr, phenomenon_name, property_set_name};

            grid_shape = lh5::Shape{input_raster_view.grid_shape()};

            lue_hpx_assert(rank<Shape> == 2);  // TODO(KDJ)

            return Shape{
                {static_cast<typename Shape::value_type>(grid_shape[0]),
                 static_cast<typename Shape::value_type>(grid_shape[1])}};
        }

    }  // namespace detail


    namespace policy::from_lue {

        /*!
            @brief      Default policies
            @tparam     OutputElement Type of output elements
        */
        template<typename OutputElement>
        using DefaultPolicies =
            policy::DefaultPolicies<AllValuesWithinDomain<>, OutputElements<OutputElement>, InputElements<>>;


        /*!
            @brief      Default value policies
            @tparam     OutputElement Type of output elements
        */
        template<typename OutputElement>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<AllValuesWithinDomain<>, OutputElements<OutputElement>, InputElements<>>;

    }  // namespace policy::from_lue


    /*!
        @brief      Read a hyperslab from an array from a LUE dataset
        @tparam     Policies Policies type
        @tparam     Shape Shape type
        @param      policies Policies to use
        @param      array_pathname Pathname of the property to read from, formatted as
                    `<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>`
        @param      hyperslab Hyperslab to read from the array
        @param      partition_shape Shape of the array partitions to use
        @param      object_id ID of object whose property value to read
        @return     New array
        @exception  .
    */
    template<typename Policies, typename Shape>
    auto from_lue(
        Policies const& policies,
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        Shape const& partition_shape,
        data_model::ID const object_id) -> PartitionedArray<policy::OutputElementT<Policies>, rank<Shape>>
    {
        using Element = policy::OutputElementT<Policies>;
        using Array = PartitionedArray<Element, rank<Shape>>;
        using Functor = InstantiateDefaultInitialized<Element, rank<Shape>>;

        Shape const array_shape{detail::hyperslab_to_shape<Shape>(hyperslab)};
        Array array{create_partitioned_array(policies, array_shape, partition_shape, Functor{})};

        return detail::from_lue(policies, array_pathname, hyperslab.start(), object_id, std::move(array));
    }


    /*!
        @overload

        Default policies will be used.
    */
    template<typename Element, typename Shape>
    auto from_lue(
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        Shape const& partition_shape,
        data_model::ID const object_id) -> PartitionedArray<Element, rank<Shape>>
    {
        using Policies = policy::from_lue::DefaultPolicies<Element>;

        return from_lue(Policies{}, array_pathname, hyperslab, partition_shape, object_id);
    }


    /*!
        @overload

        The whole array will be read.
    */
    template<typename Policies, typename Shape>
    auto from_lue(
        Policies const& policies,
        std::string const& array_pathname,
        Shape const& partition_shape,
        data_model::ID const object_id) -> PartitionedArray<policy::OutputElementT<Policies>, rank<Shape>>
    {
        Shape const array_shape{detail::constant_array_shape<Shape>(array_pathname)};

        return from_lue(
            policies, array_pathname, detail::shape_to_hyperslab(array_shape), partition_shape, object_id);
    }


    /*!
        @overload

        Default policies will be used and the whole array will be read.
    */
    template<typename Element, typename Shape>
    auto from_lue(
        std::string const& array_pathname,
        Shape const& partition_shape,
        data_model::ID const object_id) -> PartitionedArray<Element, rank<Shape>>
    {
        using Policies = policy::from_lue::DefaultPolicies<Element>;

        return from_lue(Policies{}, array_pathname, partition_shape, object_id);
    }


    // -------------------------------------------------------------------------


    /*!
        @brief      Read a hyperslab from a temporal array from a LUE dataset
        @tparam     Policies Policies type
        @tparam     Shape Shape type
        @param      policies Policies to use
        @param      array_pathname Pathname of the property to read from, formatted as
                    `<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>`
        @param      hyperslab Hyperslab to read from the array
        @param      partition_shape Shape of the array partitions to use
        @param      object_id ID of object whose property value to read
        @param      time_step_idx Index of time step to read
        @return     New array
        @exception  .
    */
    template<typename Policies, typename Shape>
    auto from_lue(
        Policies const& policies,
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        Shape const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx) -> PartitionedArray<policy::OutputElementT<Policies>, rank<Shape>>
    {
        using Element = policy::OutputElementT<Policies>;
        using Array = PartitionedArray<Element, rank<Shape>>;
        using Functor = InstantiateDefaultInitialized<Element, rank<Shape>>;

        Shape const array_shape{detail::hyperslab_to_shape<Shape>(hyperslab)};
        Array array{create_partitioned_array(policies, array_shape, partition_shape, Functor{})};

        return detail::from_lue(
            policies, array_pathname, hyperslab.start(), object_id, time_step_idx, std::move(array));
    }


    /*!
        @overload

        Default policies will be used.
    */
    template<typename Element, typename Shape>
    auto from_lue(
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        Shape const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx) -> PartitionedArray<Element, rank<Shape>>
    {
        using Policies = policy::from_lue::DefaultPolicies<Element>;

        return from_lue(Policies{}, array_pathname, hyperslab, partition_shape, object_id, time_step_idx);
    }


    /*!
        @overload

        The whole array will be read.
    */
    template<typename Policies, typename Shape>
    auto from_lue(
        Policies const& policies,
        std::string const& array_pathname,
        Shape const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx) -> PartitionedArray<policy::OutputElementT<Policies>, rank<Shape>>
    {
        Shape const array_shape{detail::variable_array_shape<Shape>(array_pathname)};

        return from_lue(
            policies,
            array_pathname,
            detail::shape_to_hyperslab(array_shape),
            partition_shape,
            object_id,
            time_step_idx);
    }


    /*!
        @overload

        Default policies will be used and the whole array will be read.
    */
    template<typename Element, typename Shape>
    auto from_lue(
        std::string const& array_pathname,
        Shape const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx  // ,
        // TODO IOStrategy const strategy = {{IOStrategy::Grouping::Locality}}
        ) -> PartitionedArray<Element, rank<Shape>>
    {
        using Policies = policy::from_lue::DefaultPolicies<Element>;

        return from_lue(Policies{}, array_pathname, partition_shape, object_id, time_step_idx);
    }

}  // namespace lue
