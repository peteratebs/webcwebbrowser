###############################################################################
# Build rules
###############################################################################

htmlroot=..

modules  = ebslogo
modules += browsericon
modules += prompt
modules += alert

outsrc = $(modules:%=$(htmlroot)/source/%.c)
outinc = $(modules:%=$(htmlroot)/include/%.h)

$(htmlroot)/source/%.c $(htmlroot)/include/%.h : %.html
	$(htmlroot)/../bin/bintoc $< $*

$(htmlroot)/source/%.c $(htmlroot)/include/%.h : %.png
	$(htmlroot)/../bin/bintoc $< $*

$(htmlroot)/source/%.c $(htmlroot)/include/%.h : %.gif
	$(htmlroot)/../bin/bintoc $< $*

$(htmlroot)/source/%.c $(htmlroot)/include/%.h : %.jpg
	$(htmlroot)/../bin/bintoc $< $*

all: $(outsrc) $(outinc)
