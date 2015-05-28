#!/usr/bin/env python

import unittest
import pexpect
import time
import sys
import psycopg2


class TestConchCommand(unittest.TestCase):
    @property
    def child(self):
        """We lazily launch the child process on first use.

        This lets us test both the case where we've started with some
        data in the database and also the empty case.

        """
        if self.__child is None:
            self.__child = pexpect.spawn(
                './conch --host=localhost --database=bugle_expect')
            self.update_screen()
            time.sleep(1)
            self.read_all_available()
            assert self.__child.isalive()
        return self.__child

    def setUp(self):
        self.__child = None
        self.connection = psycopg2.connect(
            'host=localhost dbname=bugle_expect user=bugle'
        )
        self.__child = None
        cursor = self.connection.cursor()
        cursor.execute('truncate table bugle_blast cascade')
        cursor.execute("select setval('bugle_blast_id_seq', 1);")
        self.connection.commit()

    def tearDown(self):
        self.connection.close()
        assert self.child.isalive()
        self.child.send('q')
        start = time.time()
        while self.child.isalive():
            self.read_all_available()
            assert time.time() <= start + 10
        assert self.child.status == 0

    def update_screen(self):
        """Try to read at least one screen from the buffer, timing out quickly
        if none is available."""
        width, height = self.child.getwinsize()
        to_read = width * height

        try:
            self.child.read_nonblocking(to_read, 0.01)
            sys.stdout.flush()
            return True
        except pexpect.TIMEOUT:
            return False

    def read_all_available(self):
        """Read as much from the terminal buffer as ncurses has given us."""
        try:
            while self.update_screen():
                pass
        except pexpect.EOF:
            pass

    def test_can_resize_to_1x1(self):
        self.child.setwinsize(1, 1)


if __name__ == '__main__':
    unittest.main()
