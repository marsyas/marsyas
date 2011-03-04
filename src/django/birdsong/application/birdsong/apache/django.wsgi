import os
import sys

sys.path.append('/usr/sness/django')
os.environ['DJANGO_SETTINGS_MODULE'] = 'calls.settings'

import django.core.handlers.wsgi
application = django.core.handlers.wsgi.WSGIHandler()
