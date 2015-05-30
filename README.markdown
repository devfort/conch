
     :tX8  tt
     %88888@@@@8@
    ;S@88XS%t%S88888888X
     S88X%ttS888888@@XX@@8
    :S88X%X8@888@XSS%%S@8@XS%%t;::@
    .t@8@8X@888@SS%%S8888888888@XX%t;
    :.%@X%X888@XS%S8888888@@@@@8888@X%;.
     ::::%X88@XSX8@8888@@XSSSSSXX@888@St:
      ;..tX88@@8@@888@@XSSS%%%%SSXX@88@Xt.
      %; tX888SS@888@@XSS%%%%%%%SSX@888@%:;
      Xt :S@@t%X8888@XSS%%%%%%%%%SXX@88@S:X8:; ;X8X8
      8S; t%.:%X888@@XSS%%%%%%%%%SSX@88@S;8X@..   8tt88
      @@%:.:.:tS888@@XSS%%%%%%%%SSXX@88@XSX%@...     8.@8:
        @%t%; ;%@888@XSSS%%%%%%%SSXX@888X8%8         X8@:8@
         8XX%:.;S@888@XSSS%%%%%SSSXX@8888%%:       SX88@S @8
          88Xt:.;S@888@XSSSSSSSSSSXX@888%%; .    S888XSt;.;8@
          888X%: ;%@888@@XXXSSSXXXX@@88S%: .  S@888@%t: .;tX8S
          X@88@%; :tX@888@@@XXXXXX@@88SS;::. X88@XXt: :t%SX@88
          SX@88@St: ;%X@888888@@@@@8X8SX ;. X8@%;:. ;;:%X@8888
           SSX888XS;: :tSX@88888888XS@@XS. X8S;; :;:tXX@888888
            %SX@888XSt: .;t%SX@@888tS@8S% X8X: .tSSS@8888888@8
              %SX@888@S%t:  .:;tttS S@8@: 8S.:%SX@888888@@@@X8
                SSX@8888XS%t;::   .t:8@.. 8t %XSX888@@@XXXXX@X
                  %SXX@8888@@XXS%%tS%@@88S@;.X8888X@@XSSSSXX8t
                    %SSX@@888888888@8 8@8XX %@8XXSSXSS%%%SX@S
                       %SSXXX@@@888888.8@8; S88@S%%%tttt%S8S
                   .       %%SSSSSX@88@t@@X888@X%tttttt%X8%
                      ;            %S@8X8:X8XXXSS%%%%SX8S
                          @;         %S@8X%8;;@888888% 8
                               tX@@888@       S.

# Conch

Conch is an ncurses client for [Bugle](https://github.com/devfort/bugle), written in C.

Be very afraid.

## Installing

To build: `make`.

To install: `sudo make install`. This allows you to run conch from any
directory, and includes a man page.

You can also install to a custom location (which doesn't require
installing as root):

  make install PREFIX=$USER/local

To run the tests: `make check`.

### Mac OS X

You'll need to install various packages. We suggest using homebrew:

  echo Ask Ash what the Homebrew packages are.
  brew install ImageMagick libcaca ncurses libpq lua52 libcurl4 check

### Linux (using vagrant)

We have a Vagrantfile which brings up a machine with all the required
packages installed, for both running conch and for the tests.

If you find your VM clock keeps losing time, you can set
`VBoxManage modifyvm [vm_name] --biossystemtimeoffset 10000` on your host
machine. Virtualbox guest additions, which is installed as part of this,
runs every 10 mins to correct the clock, but only if it is 20mins out of date.
This command sets it to correct if it's 10s out.

### Linux (without vagrant)

You'll need to install the packages that are listed in the
[Vagrantfile](Vagrantfile). Note that you'll have to rename
`clang-format`:

    sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-3.6 10

(The last argument is a priority number; arbitrary but necessary.)

## Testing

To run the tests: `make check`.

You will need postgres installed and running and your current user
will need to be a postgres superuser. Your pg\_hba.conf should be set
up to allow passwordless local connections to the bugle\_test database
as the bugle user; there is a suitable file in
[`vagrant_files/pg_hba.conf`](vagrant_files/pg_hba.conf), although you
should note that it also allows any user on the same machine as the
database to connect as a superuser, which is considered unwise.

## Running conch

Conch connects to a postgres database. Once everything is set up,
you just do:

  conch

This should work out of the box at a fort, but in other places you'll
need to at least override the hostname of the database:

  conch -H your.database.server

You may also need to change the database name from the default ("bugle"):

  conch -d your-dbname

### Setting up your postgres database for development

With postgres on your local machine set up for testing (for which, see
above), you want a database called "bugle" with the correct database
schema:

  createdb -O bugle bugle
  psql -h localhost -U bugle bugle < rsrc/schema.sql
  psql -h localhost -U bugle bugle < rsrc/add-trigger.sql

The first `psql` command is the schema, the second is a custom SQL
trigger which we use to detect when there are new blasts available.

(If the `createdb` command does not work, you need to give it a
postgres user to connect as. One way of doing this is to follow the
commands in the Vagrantfile, which set up a "vagrant" user; if you use
your own Unix/OS X username here then you'll subsequently be able to
run the command above.)

You then want to create a bugle user in that database:

  psql -h localhost -U bugle bugle -c \
  "INSERT INTO auth_user (id, username, first_name, last_name, email, password, is_staff, is_superuser, is_active, last_login, date_joined) VALUES (1, 'myusername', '', '', '', '', false, false, true, NOW(), NOW())"

(The database schema is compatible with the bugle web app, which means
there are lots of fields not used by conch. Sorry about that.)

You can then run conch connecting to this database using this username:

  conch -u myusername -H localhost

### Setting up a postgres database for production

We'd like to take this opportunity to remind you that conch is not
intended to be secure; the word "production" here should be considered
with extreme reservations. In particular, conch does not support
authenticated access to its database; you must allow passwordless
connections as the bugle user.

The best way of having a production database is to be at a fort;
conch's configurations defaults will find the bugle database running
on bugle.fort quite happily. (If you're setting up a fort's bugle
database, start by getting bugle running -- which will set up the
database schema from Django -- and then run in the
`rsrc/add-trigger.sql` file mentioned previously.)

If you really want to set up your own, you broadly want to go through
the same steps as for setting up a development database above. The
main difference is that you don't want to follow the Vagrantfile
commands for preparing the database, because that lowers your security
further than you actually need. Assuming you can run the `psql`
command as a superuser (possibly using the command line option `-U` to
specify the username), you want to create a "bugle" user and a "bugle"
database.

Before you do that, however, you need to make the bugle user (once it
exists) able to connect from anywhere in the world. Don't worry(*), as
it will only be able to modify the bugle database itself(+).

To do that you want the following in your `pg_hba.conf`:

  host all bugle 0.0.0.0/0 trust

Then restart your postgres server.

Then create the user and database, and set up the database schema:

  createuser bugle
  createdb -O bugle bugle
  psql -U bugle bugle < rsrc/schema.sql
  psql -U bugle bugle < rsrc/add-trigger.sql

Finally you need to create users as required using the `INSERT INTO`
SQL command given earlier. Conch doesn't support per-user
authentication to its database; anyone can post as any registered
user. This is considered a feature.

(*) Do worry. This is a terrible idea from a security point of view.

(+) This isn't really true. Again, you probably shouldn't run this in
    production.

## git integration

We provide a sample post-receive hook as `tools/post-receive` which
will blast what changed.
