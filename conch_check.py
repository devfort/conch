#!/usr/bin/env python

import unittest
import pexpect
import time
import sys


class TestConchCommand(unittest.TestCase):
    def setUp(self):
        name = self._testMethodName

        self.child = pexpect.spawn((
            'valgrind --log-file=logs/%(name)s-valgrind.log '
            './conch --host=localhost --database=bugle_expect '
            ) % {'name': name}
        )
        time.sleep(0.5)
        self.read_all_available()
        assert self.child.isalive()

    def tearDown(self):
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
