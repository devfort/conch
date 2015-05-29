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
    config.vm.provision "shell", inline: <<-SHELL
      sudo sed -ibackup s,us.archive.ubuntu.com/ubuntu,apt.fort,g  \
        /etc/apt/sources.list
    SHELL
  end

  config.ssh.forward_agent = true

  config.vm.provision "shell", inline: <<-SHELL
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
    sudo apt-get install -y python-virtualenv python-dev tmux
    sudo apt-get install -y liblua5.2-dev
    sudo apt-get install -y libcurl4-openssl-dev

    # To rename clang-format to what the Makefile expects
    sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-3.6 10

    # We need our config file which allows passwordless
    # connections from bugle and vagrant users
    echo "Copying our pg_hba.conf..."
    sudo cp /etc/postgresql/9.3/main/pg_hba.conf /etc/postgresql/9.3/main/pg_hba.conf.backup
    sudo cp /vagrant/vagrant_files/pg_hba.conf /etc/postgresql/9.3/main/pg_hba.conf

    echo "Restarting postgres to take advantage of new config..."
    sudo service postgresql restart

    echo "Adding a vagrant superuser..."
    createuser -U postgres -s vagrant
  SHELL
end
