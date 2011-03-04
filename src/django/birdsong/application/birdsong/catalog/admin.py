from django.contrib import admin
from calls.catalog.models import Call

class CallAdmin(admin.ModelAdmin):
    list_display = ('name', 'matriline', 'notes', 'audio', 'image')

admin.site.register(Call, CallAdmin)
