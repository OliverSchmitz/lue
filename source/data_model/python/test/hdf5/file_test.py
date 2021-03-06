# -*- coding: utf-8 -*-
import os
import lue.hdf5
import lue_test


class FileTest(lue_test.TestCase):

    def test_create_remove(self):

        file_pathname = "file_create.h5"

        lue_test.remove_file_if_existant(file_pathname)

        self.assertFalse(lue.hdf5.file_exists(file_pathname))

        file = lue.hdf5.create_file(file_pathname)

        self.assertTrue(lue.hdf5.file_exists(file_pathname))
        self.assertTrue(os.path.isfile(file_pathname))
        self.assertTrue(os.access(file_pathname, os.F_OK))
        self.assertTrue(os.access(file_pathname, os.R_OK))
        self.assertTrue(os.access(file_pathname, os.W_OK))
        self.assertFalse(os.access(file_pathname, os.X_OK))

        lue.hdf5.remove_file(file_pathname)
        self.assertFalse(lue.hdf5.file_exists(file_pathname))

    def test_pathname(self):

        # Relative path
        file_pathname = "file_pathname.h5"
        lue_test.remove_file_if_existant(file_pathname)
        file = lue.hdf5.create_file(file_pathname)
        self.assertEqual(file.pathname, file_pathname)

        # Relative path
        file_pathname = "./file_pathname.h5"
        lue_test.remove_file_if_existant(file_pathname)
        file = lue.hdf5.create_file(file_pathname)
        self.assertEqual(file.pathname, file_pathname)

        # Absolute path
        file_pathname = os.path.abspath("file_pathname.h5")
        lue_test.remove_file_if_existant(file_pathname)
        file = lue.hdf5.create_file(file_pathname)
        self.assertEqual(file.pathname, file_pathname)

        # Unicode
        file_pathname = u"ﬁlæ_¶åthnæmæ.h5"
        lue_test.remove_file_if_existant(file_pathname)
        file = lue.hdf5.create_file(file_pathname)
        self.assertEqual(file.pathname, file_pathname)

    def test_open_readonly(self):
        file_pathname = "file_open_readonly.h5"
        lue_test.remove_file_if_existant(file_pathname)
        file = lue.hdf5.create_file(file_pathname)
        file = lue.hdf5.open_file(file_pathname, "r")
        # TODO How to test for readonly-ness
        # - Update and fail

    def test_open_writable(self):
        file_pathname = "file_open_writable.h5"
        lue_test.remove_file_if_existant(file_pathname)
        file = lue.hdf5.create_file(file_pathname)
        file = lue.hdf5.open_file(file_pathname, "w")
        # TODO How to test for writable-ness
        # - Update and succeed
