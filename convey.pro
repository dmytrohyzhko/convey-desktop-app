CONFIG += ordered

TEMPLATE = subdirs

SUBDIRS += httpserver \
           convey

convey_windows_app.depends = httpserver
