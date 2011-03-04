from django.conf.urls.defaults import *

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns(
    '',
    (r'^$', 'birdsong.main.views.index'),
    (r'^$', 'birdsong.main.views.index'),

    (r'^recordings$', 'birdsong.recordings.views.index'),
    (r'^recordings/show/(?P<recording_id>\d+)$', 'birdsong.recordings.views.show'),
    (r'^recordings/show_annotations/(?P<recording_id>\d+)/(?P<user_id>\d+)$', 'birdsong.recordings.views.show_annotations'),
    (r'^recordings/annotations/(?P<recording_id>\d+)$', 'birdsong.recordings.views.annotations'),
    (r'^recordings/pitchcontour/(?P<annotation_id>\d+)$', 'birdsong.recordings.views.pitchcontour'),

    (r'^annotations$', 'birdsong.annotations.views.index'),
    (r'^annotations/show/(?P<annotation_id>\d+)$', 'birdsong.annotations.views.show'),
    (r'^annotations/update$', 'birdsong.annotations.views.update'),

    # Pitch detection
    (r'^dtw/$', 'birdsong.dtw.views.index'),
    (r'^dtw/pitchcontour/(?P<median>\d+)$', 'birdsong.dtw.views.pitchcontour'),
    (r'^dtw/pitchcontour_embed$', 'birdsong.dtw.views.pitchcontour_embed'),
    
    # Static assets
    (r'^images/(?P<path>.*)$',
     'django.views.static.serve', {'document_root': '/home/sness/dDEV/birdsong/public_html/images'}),
    (r'^flash/(?P<path>.*)$',
     'django.views.static.serve', {'document_root': '/home/sness/dDEV/birdsong/public_html/flash'}),
    (r'^images/(?P<path>.*)$',
     'django.views.static.serve', {'document_root': '/home/sness/dDEV/birdsong/public_html/images'}),
    (r'^src/(?P<path>.*)$',
     'django.views.static.serve', {'document_root': '/home/sness/dDEV/birdsong/public_html/src'}),
    (r'^stylesheets/(?P<path>.*)$',
     'django.views.static.serve', {'document_root': '/home/sness/dDEV/birdsong/public_html/stylesheets'}),
    (r'^javascript/(?P<path>.*)$',
     'django.views.static.serve', {'document_root': '/home/sness/dDEV/birdsong/public_html/javascript'}),
    (r'^assets/(?P<path>.*)$',
     'django.views.static.serve', {'document_root': '/home/sness/dDEV/birdsong/public_html/assets'}),
    (r'^songs/(?P<path>.*)$',
     'django.views.static.serve', {'document_root': '/home/sness/dDEV/birdsong/public_html/songs'}),

    # Admin site
    (r'^admin/', include(admin.site.urls))
)
