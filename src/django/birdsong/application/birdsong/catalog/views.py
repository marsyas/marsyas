from django.template import Context, loader
from django.http import HttpResponse
from django.http import Http404
from django.shortcuts import render_to_response
from calls.catalog.models import Call
import subprocess
import re

def index(request):
    calls = Call.objects.all().order_by('name')
    return render_to_response('catalog/index.html', {'calls' : calls})

