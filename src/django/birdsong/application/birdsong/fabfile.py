from fabric.context_managers import cd
from fabric.operations import sudo
from fabric.api import settings,run
from fabric.api import *

env.hosts = ['django.venus.orchive.net']

def update():
    with cd('/var/www/calls/'):
        run('svn up')
        sudo('/usr/sbin/apache2ctl restart')
