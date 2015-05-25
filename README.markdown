# Conch

Conch is a C client for Bugle.

Be very afraid.

## Testing

To run the tests: `make check`. If they are timing out, you can use the environment variable CK_DEFAULT_TIMEOUT with the make target. The default is 4 seconds, I've found that my Vagrant VM requires 6 seconds, i.e. `make check CK_DEFAULT_TIMEOUT=6`.

You will need postgres installed and running and to be able to create users and
databases as your current user.

## Making it work on your system

To rename your copy of a library so it matches what compilation expects:

````

sudo update-alternatives --install /path/to/new_thing new_thingname /path/to/existing_thing 10

````

(The last argument is a priority number; arbitrary but necessary.)
