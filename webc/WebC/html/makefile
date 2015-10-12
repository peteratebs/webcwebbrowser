###############################################################################
# Build rules
###############################################################################

htmlroot=.

modules = welcome
modules += frametest
modules += mock1
modules += mock2
modules += mock3
modules += mock4
modules += mock5
modules += EBS_SOFTKEY_CAPITALS
modules += EBS_SOFTKEY_NUMBERS
modules += EBS_SOFTKEY_SMALLSS
modules += EBS_SOFTKEY_SYMBOLS

outsrc = $(modules:%=source/%.c)
outinc = $(modules:%=include/%.h)

source/%.c $(htmlroot)/include/%.h : %.html
	$(htmlroot)/../bin/bintoc $< $*

source/%.c $(htmlroot)/include/%.h : %.png
	$(htmlroot)/../bin/bintoc $< $*

source/%.c $(htmlroot)/include/%.h : %.bmp
	$(htmlroot)/../bin/bintoc $< $*

all: $(outsrc) $(outinc)
