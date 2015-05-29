"""Use Hypothesis + pexpect to drive our ncurses interface.

This isn't very clever testing. It mostly feeds random characters to the
program and resizes the windows and checks that the program doesn't
crash, then at the end sends a q character and asserts that it exits in
reasonable time.

"""

from __future__ import print_function

from hypothesis.stateful import GenericStateMachine
import hypothesis.strategies as strat
from hypothesis.settings import Verbosity, Settings
import unittest
import time
from collections import namedtuple
import psycopg2
import tmux
import binascii
import os
from leven import levenshtein

Blast = namedtuple('Blast', ('user', 'content', 'extended', 'attachment'))
Sleep = namedtuple('Sleep', ('time',))
Search = namedtuple('Search', ('search',))
Start = namedtuple('Start', ('width', 'height'))

start_strategy = strat.builds(
    Start, strat.integers(20, 200), strat.integers(1, 200),)


search_strategy = strat.text().map(Search)

sleep_strategy = strat.builds(
    Sleep, strat.integers(1, 100)
)


class RunnerMachine(GenericStateMachine):

    def init_app(self, width, height):
        """We lazily launch the child process on first use.

        This lets us test both the case where we've started with some
        data in the database and also the empty case.

        """
        assert self.tmux is None
        self.name = binascii.hexlify(os.urandom(8))
        self.success_terminator = "success-" + binascii.hexlify(
            os.urandom(8)).decode('utf-8')
        self.fail_terminator = "fail-" + binascii.hexlify(
            os.urandom(8)).decode('utf-8')
        self.tmux = tmux.Tmux(self.name)
        sessions = self.tmux.sessions()
        assert len(sessions) == 1
        self.tmux.new_session(
            name=self.name,
            command=(
                './conch --host=localhost '
                '--database=bugle_expect && echo %s || echo %s; sleep 5'
            ) % (self.success_terminator, self.fail_terminator),
            width=width, height=height
        )
        assert len(self.tmux.sessions()) == 2
        self.tmux.kill_session(sessions[0])
        panes = self.tmux.panes()
        assert len(panes) == 1
        self.target_pane = panes[0]
        self.press("@")
        self.await_text("/dev/fort 11", timeout=5)
        self.wait_to_stabilize()

    def fail(self, message):
        raise ValueError(message)

    def current_screen(self):
        screen = self.tmux.capture_pane(self.target_pane)
        if not hasattr(self, 'last_screen'):
            self.last_screen = screen
        elif (
            self.success_terminator not in self.last_screen and
            self.fail_terminator not in self.last_screen
        ):
            self.last_screen = screen
        return screen

    def await(self, screen_filter, timeout, message=''):
        start = time.time()
        while time.time() <= start + timeout:
            screen = self.current_screen()
            if screen_filter(screen):
                return screen
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

    def await_no_text(self, text, timeout=1):
        return self.await(
            lambda screen: text not in screen, timeout=timeout,
            message="text %r to disappear" % (text,)
        )

    def press(self, key):
        return self.tmux.send_keys(self.target_pane, [key])

    def __init__(self):
        """Starting a new state machine run connects to the database and gives
        us the tools we will need later to generate operations."""
        self.connection = psycopg2.connect(
            'host=localhost dbname=bugle_expect user=bugle'
        )
        self.tmux = None
        cursor = self.connection.cursor()
        cursor.execute('truncate table bugle_blast cascade')
        cursor.execute("select setval('bugle_blast_id_seq', 1);")
        self.connection.commit()

        cursor.execute('select username from auth_user order by id')
        self.users = [r[0] for r in cursor]
        cursor.close()
        assert self.users
        self.blast_strategy = strat.builds(
            Blast,
            user=strat.sampled_from(self.users), content=strat.text(),
            extended=strat.text(), attachment=(
                strat.just('') | strat.text(
                    alphabet='abcdefghijklmnopqrstuvwxyz0123456789._-',
                    max_size=50)),
        )

    def wait_to_stabilize(self, timeout=2):
        screen = self.current_screen()
        start = time.time()
        while True:
            time.sleep(0.5)
            new_screen = self.current_screen()
            size_difference = levenshtein(screen, new_screen)
            if size_difference <= 2:
                return
            screen = new_screen
            if time.time() >= start + 2:
                print(new_screen)
                self.fail("Took too long to stabilize")

    def steps(self):
        if self.tmux is None:
            return self.blast_strategy | start_strategy

        return (
            strat.just('\t') |
            strat.text('j') |
            strat.text('k') |
            strat.just('0') |
            strat.just('s') |
            strat.just('g') |
            strat.just('G') |
            strat.just('n') |
            strat.just('PageUp') |
            strat.just('PageDown') |
            strat.just('Enter') |
            self.blast_strategy |
            sleep_strategy |
            search_strategy
        )

    def execute_step(self, step):
        if isinstance(step, Blast):
            cursor = self.connection.cursor()
            cursor.execute("""
                insert into bugle_blast(user_id, message, extended, attachment)
                values(
                    (select id from auth_user where username=%s),
                    %s, %s, %s
                )
            """, tuple(step))
            self.connection.commit()
            cursor.close()
        elif isinstance(step, Start):
            self.init_app(step.width, step.height)
        elif isinstance(step, Sleep):
            time.sleep(step.time / 1000.0)
        elif isinstance(step, Search):
            self.press("/")
            if step.search:
                buf = self.tmux.a_buffer()
                self.tmux.set_buffer(buf, step.search)
                self.tmux.execute_command(
                    "paste-buffer", "-b", buf, "-d", "-t", self.target_pane)
            self.press("Enter")
        else:
            assert isinstance(step, str)
            self.press(step)
        if self.tmux is not None:
            self.wait_to_stabilize()

    def teardown(self):
        """Close down the child program by sending it a q signal.

        We give it quite a lot of time to close down (10 seconds) but if
        it hasn't closed down by then that's definitely a bug.

        """
        self.connection.close()
        if self.tmux is None:
            return
        self.current_screen()
        self.tmux.shutdown()
        print(self.last_screen)
        assert self.fail_terminator not in self.last_screen.replace("\n", "")

Settings.default.verbosity = Verbosity.debug
Settings.default.timeout = -1
Settings.default.stateful_step_count = 200
Settings.default.max_examples = 10000
Settings.default.max_shrinks = 50

TestConch = RunnerMachine.TestCase

if __name__ == '__main__':
    unittest.main()
