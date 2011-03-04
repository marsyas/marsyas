from django.db import models

class Call(models.Model):
    name = models.CharField(max_length=200)
    matriline = models.CharField(max_length=200)
    notes = models.CharField(max_length=200)
    audio = models.CharField(max_length=200)
    image = models.CharField(max_length=200)
    duration = models.FloatField()
    
    
