# QT_VERSION = 5.12.12

CONFIG += ordered

TEMPLATE = subdirs

SUBDIRS += httpserver \
           convey

convey_windows_app.depends = httpserver
