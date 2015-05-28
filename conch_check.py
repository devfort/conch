#!/usr/bin/env python

import unittest
import pexpect
from pexpect import ANSI
import time
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
            self.read_all_available()
            assert self.__child.isalive()
        return self.__child

    def setUp(self):
        self.screen = ANSI.ANSI()
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

    def blast(self, username, content, attachment=None, extended=None):
        cursor = self.connection.cursor()
        cursor.execute("""
            insert into bugle_blast(user_id, message, extended, attachment)
            values(
                (select id from auth_user where username=%s),
                %s, %s, %s
            )
        """, (username, content, attachment or '', extended or ''))
        self.connection.commit()
        cursor.close()

    def update_screen(self):
        """Try to read at least one screen from the buffer, timing out quickly
        if none is available."""
        width, height = self.child.getwinsize()
        to_read = width * height

        try:
            data = self.child.read_nonblocking(to_read, 0.01)
            self.screen.write(data)
            return True
        except pexpect.TIMEOUT:
            return False

    def read_a_screen(self):
        self.read_all_available()
        return self.screen.pretty()

    def read_all_available(self):
        """Read as much from the terminal buffer as ncurses has given us."""
        try:
            while self.update_screen():
                pass
        except pexpect.EOF:
            pass

    def test_can_resize_to_1x1_after_loading(self):
        self.wait_for_loading_screen()
        self.child.setwinsize(1, 1)

    def test_can_resize_to_1x1_before_loading(self):
        self.child.setwinsize(1, 1)

    def test_shows_a_blast_in_db_at_startup(self):
        self.blast("DRMacIver", "Hello world")
        self.wait_for_loading_screen()
        self.child.send("0")
        time.sleep(0.5)
        self.read_all_available()
        screen = self.screen.dump()
        self.assertTrue("Hello world" in screen, screen)

    def test_shows_a_blast_inserted_in_db(self):
        self.wait_for_loading_screen()
        self.blast("DRMacIver", "Hello world")
        time.sleep(0.5)
        self.child.send("0")
        self.read_all_available()
        self.assert_screen_contains("Hello world")

    def assert_screen_contains(self, text):
        screen = self.screen.dump()
        if text not in screen:
            print("Expected %r to be on the screen, but the screen is:" % (
                text,))
            print(self.screen.pretty())
            self.fail()

    def assert_screen_does_not_contain(self, text):
        screen = self.screen.dump()
        if text in screen:
            print("Expected %r to not be on the screen, but the screen is:" % (
                text,))
            print(self.screen.pretty())
            self.fail()

    def wait_for_loading_screen(self):
        start = time.time()
        while True:
            self.read_all_available()
            if 'conch loading:' in self.screen.dump():
                time.sleep(0.05)
            else:
                return
            if time.time() >= start + 5:
                print(self.screen.pretty())
                self.fail("Took too long to complete loading screen")

    def test_can_scroll_to_the_bottom_of_many_blasts(self):
        n = 50
        for i in range(n):
            self.blast("steve", "Message %d for test" % (n - i,))
        self.wait_for_loading_screen()
        self.assert_screen_contains("Message 1 for test")
        for _ in range(n):
            self.child.send("j")
            self.read_all_available()
        self.assert_screen_contains("Message %d for test" % (n,))
        self.assert_screen_does_not_contain("Message 1 for test")


if __name__ == '__main__':
    unittest.main()
