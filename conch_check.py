#!/usr/bin/env python

import unittest
import time
import psycopg2
import os
import tmux
import binascii


class TestConchCommand(unittest.TestCase):
    def start_conch(self):
        if self.started:
            return
        self.started = True
        sessions = self.tmux.sessions()
        assert len(sessions) == 1
        assert sessions[0] != self.session_name
        sessions = self.tmux.sessions()
        self.tmux.new_session(
            name=self.session_name,
            command=(
                './conch --host=localhost '
                '--database=bugle_expect && echo Hurrah; sleep 5'
            )
        )
        assert len(self.tmux.sessions()) == 2
        self.tmux.kill_session(sessions[0])
        panes = self.tmux.panes()
        assert len(panes) == 1
        self.target_pane = panes[0]

    def setUp(self):
        self.started = False
        self.session_name = binascii.hexlify(os.urandom(8))
        self.tmux = tmux.Tmux("TestConchCommand.%s" % (self._testMethodName,))
        self.connection = psycopg2.connect(
            'host=localhost dbname=bugle_expect user=bugle'
        )
        cursor = self.connection.cursor()
        cursor.execute('truncate table bugle_blast cascade')
        cursor.execute("select setval('bugle_blast_id_seq', 1);")
        self.connection.commit()

    def tearDown(self):
        try:
            self.connection.close()
            assert self.tmux.sessions()
            self.press('q')
            self.await_text("Hurrah")
        finally:
            self.tmux.shutdown()

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

    def current_screen(self):
        return self.tmux.capture_pane(self.target_pane)

    def await(self, screen_filter, timeout, message=''):
        start = time.time()
        while time.time() <= start + timeout:
            screen = self.current_screen()
            if screen_filter(screen):
                return screen
        print(screen)
        if message:
            self.fail(
                "Timed out for waiting for %s" % (message,)
            )
        else:
            self.fail("Timed out while waiting")

    def await_text(self, text, timeout=1):
        return self.await(
            lambda screen: text in screen, timeout=timeout,
            message="text %r to appear" % (text,)
        )

    def test_shows_a_blast_in_db_at_startup(self):
        self.blast("DRMacIver", "Hello world")
        self.wait_for_loading_screen()
        self.await(
            lambda screen: "Hello world" in screen, 1,
            message="Blast appearing"
        )

    def press(self, key):
        return self.tmux.send_keys(self.target_pane, [key])

    def test_shows_a_blast_inserted_in_db(self):
        self.wait_for_loading_screen()
        self.blast("DRMacIver", "Hello world")
        self.press("0")
        self.await_text("Hello world", 0.5)

    def wait_for_loading_screen(self):
        self.start_conch()
        loading_text = 'conch loading:'
        self.await(
            lambda x: loading_text in x, timeout=0.5,
            message="loading to start"
        )
        self.press("@")
        return self.await(
            lambda x: loading_text not in x, timeout=5,
            message="loading to finish"
        )

    def test_can_scroll_to_the_bottom_of_many_blasts(self):
        n = 50
        for i in range(n):
            self.blast("steve", "Message %d for test" % (n - i,))
        self.wait_for_loading_screen()
        self.await_text("Message 1 for test")
        for _ in range(n):
            self.press("j")
        screen = self.await_text("Message %d for test" % (n,))
        assert "Message 1 for test" not in screen

    def test_auto_follow_mode_auto_scrolls_to_top(self):
        self.wait_for_loading_screen()
        self.press("s")
        assert "pkqk" not in self.current_screen()
        self.blast("pkqk", "Some test message")
        self.await_text("pkqk")

    def test_give_me_a_conch(self):
        self.wait_for_loading_screen()
        marker = "%%%%"
        assert marker not in self.current_screen()
        self.press("@")
        self.await_text(marker)
        self.press("@")
        self.await(
            lambda screen: marker not in screen, timeout=1
        )

    def test_does_not_crash_if_you_press_n_without_search(self):
        self.wait_for_loading_screen()
        self.press("n")

    def test_shift_g_scrolls_to_bottom(self):
        n = 200
        for i in range(n):
            self.blast("steve", "Message %d for test" % (n - i,))
        self.wait_for_loading_screen()
        self.press("G")
        self.await_text("Message %d for test" % (n,))
        self.press("g")
        self.await_text("Message 1 for test")

    def test_detail_view_should_not_segfault_when_database_is_empty(self):
        self.wait_for_loading_screen()
        self.press("Enter")


if __name__ == '__main__':
    unittest.main()
