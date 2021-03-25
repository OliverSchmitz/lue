import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    pass
    # lue_test.stop_hpx_runtime()


class AccuTest(lue_test.TestCase):

    def test_overloads(self):

        array_shape = (600, 400)
        partition_shape = (10, 10)

        west = 4
        flow_direction = lfr.create_array(array_shape, partition_shape, np.dtype(np.uint8), west)
        material = lfr.create_array(array_shape, partition_shape, np.dtype(np.uint64), 1)

        lfr.accu(flow_direction, material)
