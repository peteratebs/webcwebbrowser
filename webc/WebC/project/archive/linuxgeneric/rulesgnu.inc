#include $(targ).inc

#cflags  +=
ccflags += -fpermissive

ifeq ($(compiler),GNUCOMPILER)
%.$(lib):
	echo ARCHIVING $@
	ar ru $@ $(notdir $^)

%.$(obj): %.c
	echo COMPILING $<
	gcc	$(cflags) $(defines:%=-D%) $(includes:%=-I%) -c $<	-o $(notdir $@)

#%.$(obj): %.cxxa
#	echo COMPILING $<
#	$(toolprefix)gcc $(ccflags)	$(defines:%=-D%) $(includes:%=-I%) -c $< -o $(notdir $@)

%.$(obj): %.cpp
	echo xxx %.cpp COMPILING $<
	$(toolprefix)gcc $(ccflags)	$(defines:%=-D%) $(includes:%=-I%) -c $< -o $(notdir $@)

%.$(exe):
	echo LINKING $@
	g++ $(ldflags) -o $(notdir $@)	$^ -lm -lc -lgcc

endif #	GNUCOMPILER



#### Generic Stuff


# Make the web page	files
#%.c %.h: %.htm	%.cla
#	echo BINTOC	$<
#	../webpage/bintoc $(subst /,\\,$<) $(subst /,\\,$(basename $(notdir	$<)))

# Make dependency files
%.d: %.c
	echo UPDATING DEPENDENCIES FOR $<
	gcc	-undef -M $< $(defines:%=-D%) $(builtins:%=-D%)	$(includes:%=-I%) |	depend $(obj) >	$@

.INTERMEDIATE: $(modules:%=%.d)
$(targ).inc: $(modules:%=%.d)
	echo MAKING	$@
	cat	*.d	> $@

.PHONY:	CLEAN
CLEAN:
	echo CLEANING
	rm -f	*.o
	rm -f	*.a
	rm -f   *.out

.PHONY:	clean
clean:
	echo CLEANING
	rm -f	*.o
	rm -f	*.a
	rm -f   *.out


.PHONY:	dep
dep:
	echo DEPENDENCIES UPDATED

