import numpy
import lue
import lue_test


class ConstantTest(lue_test.TestCase):

    def test_case_study(self):

        dataset = lue.create_dataset("planets.lue")
        planets = dataset.add_phenomenon("planets")

        nr_planets = 3
        planets.object_id.expand(nr_planets)[:] = \
            numpy.arange(nr_planets)

        constants = planets.property_sets.add("constants")
        gravity = constants.add_property(
            "gravity", dtype=numpy.dtype(numpy.float32))
        gravity.value.expand(nr_planets)[:] = \
            numpy.array([1.5, 2.5, 3.5], dtype=numpy.float32)

        lue.assert_is_valid(dataset)