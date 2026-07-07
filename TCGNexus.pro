TEMPLATE = subdirs
SUBDIRS = TCGCore \
          TCGGUI \
          TCGTests

TCGGUI.depends = TCGCore
TCGTests.depends = TCGGUI
