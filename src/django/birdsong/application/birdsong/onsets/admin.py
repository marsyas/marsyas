from django.contrib import admin
from calls.onsets.models import Recording

class RecordingAdmin(admin.ModelAdmin):
    list_display = ('audio', 'image', 'length')

admin.site.register(Recording, RecordingAdmin)
