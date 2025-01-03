import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class DownstreamDistanceTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)

        direction = 1
        flow_direction = lfr.create_array(
            array_shape, lfr.flow_direction_element_type, direction
        )
        cell_size = 10

        for element_type in lfr.floating_point_element_types:
            _ = lfr.downstream_distance(flow_direction, cell_size, element_type)
