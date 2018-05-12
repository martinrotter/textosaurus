TEMPLATE = subdirs

CONFIG += ordered
SUBDIRS = libtextosaurus \
          textosaurus

libtextosaurus.subdir  = src/libtextosaurus
textosaurus.subdir  = src/textosaurus

textosaurus.depends = libtextosaurus