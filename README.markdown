# Conch

Conch is a C client for Bugle.

Be very afraid.

To run the tests: `make check`. If they are timing out, you can use the environment variable CK_DEFAULT_TIMEOUT with the make target. The default is 4 seconds, I've found that my Vagrant VM requires 6 seconds, i.e. `make check CK_DEFAULT_TIMEOUT=6`.
