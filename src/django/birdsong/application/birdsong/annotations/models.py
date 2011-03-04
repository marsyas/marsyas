from django.db import models

from recordings.models import Recording

# Create your models here.
class Annotation(models.Model):
    start_time_ms = models.IntegerField(default = 0)
    end_time_ms = models.IntegerField(default = 0)
    label = models.CharField(max_length = 200)
    recording = models.ForeignKey(Recording)

    def to_string(self):
        return "%i,%i,%i,%s" % (self.id, self.start_time_ms, self.end_time_ms, self.label)
