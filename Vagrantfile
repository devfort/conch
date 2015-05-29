# -*- mode: ruby -*-
# vi: set ft=ruby :

def on_a_fort
  fort_response = Net::HTTP.get_response(URI('http://files.fort/'))
  fort_response.is_a?(Net::HTTPSuccess)
rescue
  false
end

Vagrant.configure(2) do |config|

  config.vm.box = "chef/ubuntu-14.04"
  if on_a_fort
    config.vm.box_url = "http://files.fort/boxes/chef-ubuntu-14.04.box"
    config.vm.box_check_update = false
  end

  config.ssh.forward_agent = true

  config.vm.provision "shell", inline: <<-SHELL
    ## when on a fort, use the clonque apt repo
    ## should be removed if building VM off fort
    sudo sed -ibackup s,archive.ubuntu.com/ubuntu,apt.fort,g /etc/apt/sources.list
    ## if using the fort box, it's actually us.archive.ubuntu.com :(
    ## doing both will have no effect if file was as expected
    sudo sed -ibackup s,us.apt,apt,g /etc/apt/sources.list

    sudo apt-get update

    # required for development on the VM
    sudo apt-get install -y git
    sudo apt-get install -y vim
    sudo apt-get install -y virtualbox-guest-additions-iso

    # packages required for the application
    sudo apt-get install -y libmagickwand-dev libcaca-dev
    sudo apt-get install -y libncurses5-dev libncursesw5-dev
    sudo apt-get install -y postgresql libpq-dev
    sudo apt-get install -y check
    sudo apt-get install -y clang-format-3.6
    sudo apt-get install -y python-virtualenv
    sudo apt-get install -y liblua5.2-dev
    sudo apt-get install -y libcurl4-openssl-dev

    # To rename clang-format to what the Makefile expects
    sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-3.6 10

    ## MANUAL STEPS #####################################################
    ## To be automated                                                  #
    ##                                                                  #
    ## 1. To run the tests you will have to make two changes:           #
    ##    Edit `/etc/postgresql/9.3/main/pg_hba.conf`                   #
    ##    Under # Database administrative login by Unix domain socket   #
    ##    Change the final column for postgres from                     #
    ##   `peer` to `trust`                                              #
    ##    Add two lines under #IPv4 local connections ABOVE what's      #
    ##    already there:                                                #
    ##    `host  all  bugle    127.0.0.1/32  trust`                     #
    ##    `host  all  vagrant  127.0.0.1/32  trust`                     #
    ##    and the same under #IPv6 local connections:                   #
    ##    `host  all  bugle     ::1/128      trust`                     #
    ##    `host  all  vagrant   ::1/128      trust`                     #
    ##    If you know whether you are using IPv4 or IPv6 to connect     #
    ##    to Postgres, you do not need to do both.                      #
    ##    Then restart Postgres.                                        #
    ##                                                                  #
    ## 2. Add a vagrant superuser:                                      #
    ##    `createuser -U postgres -s vagrant`                           #
    ##                                                                  #
    ## 3. If your clock keeps losing time, you can set                  #
    ##    `VBoxManage modifyvm [vm_name] --biossystemtimeoffset 10000`  #
    ##    on your host machine. Virtualbox guest additions, which       #
    ##    is installed as part of this runs every 10 mins to correct    #
    ##    the clock, but only if it is 20mins out of date. This         #
    ##                                                                  #
    ## 4. You may find your network is slow and some tests timeout. If  #
    ##    so, add `CK_DEFAULT_TIMEOUT=6` (or higher) when running them  #
    ##                                                                  #
    #####################################################################
  SHELL
end
