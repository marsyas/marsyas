from django.shortcuts import render_to_response

# Debugging
import logging
from django.http import HttpResponse

def index(request):
    return render_to_response('main/index.html')

def update(request):
    if request.method == "POST":
        logging.info(request.POST)
    return HttpResponse("1,100,200,test\n2,300,400,test2\n")
