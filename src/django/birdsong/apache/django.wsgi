import os
import sys

sys.path.append('/var/www/calls/application')

os.environ['PYTHON_EGG_CACHE'] = '/home/sness/dDEV/calls/.python-egg'
os.environ['DJANGO_SETTINGS_MODULE'] = 'calls.settings'

import django.core.handlers.wsgi
application = django.core.handlers.wsgi.WSGIHandler()