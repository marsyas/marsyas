from django.db import models

# Create your models here.
class Recording(models.Model):
    name = models.CharField(max_length = 200)
    length_ms = models.IntegerField(default = 0)
