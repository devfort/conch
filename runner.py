"""Use Hypothesis + pexpect to drive our ncurses interface.

This isn't very clever testing. It mostly feeds random characters to the
program and resizes the windows and checks that the program doesn't
crash, then at the end sends a q character and asserts that it exits in
reasonable time.

"""

from hypothesis.stateful import GenericStateMachine
import pexpect.ANSI
import hypothesis.strategies as strat
from hypothesis.settings import Verbosity, Settings
import unittest
import time
from collections import namedtuple
import psycopg2


Window = namedtuple('Window', ('width', 'height'))
Blast = namedtuple('Blast', ('user', 'content', 'extended', 'attachment'))
Sleep = namedtuple('Sleep', ('time',))

sleep_strategy = strat.builds(
    Sleep, strat.integers(1, 100)
)

window_strategy = strat.builds(
    Window,
    strat.integers(1, 100),
    strat.integers(1, 100),
)


class RunnerMachine(GenericStateMachine):

    @property
    def child(self):
        """We lazily launch the child process on first use.

        This lets us test both the case where we've started with some
        data in the database and also the empty case.

        """
        if self.__child is None:
            self.__child = pexpect.spawn(
                './conch --host=localhost --database=bugle_expect')
            self.screen = pexpect.ANSI.ANSI()
            self.update_screen()
            time.sleep(1)
            self.read_all_available()
        return self.__child

    def __init__(self):
        """Starting a new state machine run connects to the database and gives
        us the tools we will need later to generate operations."""
        self.connection = psycopg2.connect(
            'host=localhost dbname=bugle_expect user=bugle'
        )
        self.__child = None
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

    def update_screen(self):
        """Try to read at least one screen from the buffer, timing out quickly
        if none is available."""
        width, height = self.child.getwinsize()
        to_read = width * height

        try:
            data = self.child.read_nonblocking(to_read, 0.01)
            self.last_screen = data
            self.screen.process(data)
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

    def steps(self):
        return (
            strat.just('\t') |
            strat.text('j', average_size=5) |
            strat.text('k', average_size=5) |
            strat.just('0') |
            strat.just('s') |
            window_strategy |
            self.blast_strategy |
            sleep_strategy
        )

    def execute_step(self, step):
        if isinstance(step, Window):
            self.child.setwinsize(step.width, step.height)
        elif isinstance(step, Blast):
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
        elif isinstance(step, Sleep):
            time.sleep(step.time / 1000.0)
        else:
            for c in step:
                self.child.send(c)
                self.update_screen()
        self.read_all_available()
        assert self.child.isalive()

    def teardown(self):
        """Close down the child program by sending it a q signal.

        We give it quite a lot of time to close down (10 seconds) but if
        it hasn't closed down by then that's definitely a bug.

        """
        assert self.child.isalive()
        self.child.send('q')
        start = time.time()
        while self.child.isalive():
            self.read_all_available()
            assert time.time() <= start + 10
        assert self.child.status == 0

Settings.default.verbosity = Verbosity.debug
Settings.default.timeout = -1
Settings.default.stateful_step_count = 200
Settings.default.max_examples = 10000
Settings.default.max_shrinks = 50

TestConch = RunnerMachine.TestCase

if __name__ == '__main__':
    unittest.main()
