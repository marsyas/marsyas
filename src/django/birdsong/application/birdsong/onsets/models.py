from django.db import models

class Recording(models.Model):
    audio = models.CharField(max_length=200)
    image = models.CharField(max_length=200)
    length = models.FloatField()

