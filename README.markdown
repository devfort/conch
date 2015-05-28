
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

Conch is a C client for Bugle.

Be very afraid.

## Installing

On Linux you'll need to install

  sudo apt-get install libimlib2-dev libcaca-dev libncurses5 liblualib50-dev

On OS X you'll need to install

  echo Ask Ash what the Homebrew packages are.

## Testing

To run the tests: `make check`. If they are timing out, you can use the
environment variable CK_DEFAULT_TIMEOUT with the make target. The default is 4
seconds, I've found that my Vagrant VM requires 6 seconds, i.e. `make check
CK_DEFAULT_TIMEOUT=6`.

You will need postgres installed and running and your current user will need to be a
postgres superuser. Your pg\_hba.conf should be set up to allow passwordless local
connections to the bugle\_test database as the bugle user.

## Making it work on your system

To rename your copy of a library so it matches what compilation expects:

    sudo update-alternatives --install /path/to/new_thing new_thingname /path/to/existing_thing 10

For example:

    sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-3.6 10

(The last argument is a priority number; arbitrary but necessary.)
