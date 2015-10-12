#include $(targ).inc

# cflags  += -mpentium -fno-builtin -fno-defer-pop -I. -DCPU=SIMNT -DTOOL_FAMILY=gnu -DTOOL=gnu -D__vxworks__ -D__STDC__ -DWNDML_DEMO
# ccflags += -mpentium -ansi -fno-builtin -fno-defer-pop -I.  -DCPU=SIMNT -DTOOL_FAMILY=gnu -DTOOL=gnu -D__vxworks__ -D__STDC__ -DWNDML_DEMO
#cflags  += -mpentium -fno-builtin -fno-defer-pop -I. -DCPU=SIMNT -DTOOL_FAMILY=gnu -DTOOL=gnu -D__vxworks__ -D__STDC__
#ccflags += -mpentium -ansi -fno-builtin -fno-defer-pop -I.  -DCPU=SIMNT -DTOOL_FAMILY=gnu -DTOOL=gnu -D__vxworks__ -D__STDC__
cflags  += -mpentium -w -O -Wall -D_WRS_KERNEL -fno-builtin -fno-defer-pop -I. -DCPU=SIMNT -DTOOL_FAMILY=gnu -DTOOL=gnu -D__vxworks__ -D__STDC__
ccflags += -mpentium -w -ansi -O -Wall -D_WRS_KERNEL -fno-builtin -fno-defer-pop -I.  -DCPU=SIMNT -DTOOL_FAMILY=gnu -DTOOL=gnu -D__vxworks__ -D__STDC__
ldflags = -r

# -M -w -ansi -O -Wall -D_WRS_KERNEL


ifeq ($(compiler),GNUCOMPILER)
%.$(lib):
	echo ARCHIVING $@
	arsimpc.exe ru $@ $(notdir $^)

%.$(obj): %.c
	echo COMPILING $<
	ccsimpc.exe $(cflags) $(defines:%=-D%) $(includes:%=-I%) -c $< -o $(notdir $@)

#%.$(obj): %.cxxa
#	echo COMPILING $<
#	$(toolprefix)gcc $(ccflags)	$(defines:%=-D%) $(includes:%=-I%) -c $< -o $(notdir $@)

%.$(obj): %.cpp
	echo xxx %.cpp COMPILING $<
	ccsimpc.exe $(ccflags)	 $(defines:%=-D%) $(includes:%=-I%) -c $< -o $(notdir $@)

%.$(exe):
	echo LINKING $@
	ldsimpc.exe $(ldflags) -o $(notdir $@)	$^  C:\PentiumTornado2.2\target\lib\simpc\SIMNT\gnu\libcplus.a C:\PentiumTornado2.2\target\lib\simpc\SIMNT\gnu\libgnucplus.a

#%.$(exe):
#	echo LINKING $@
#	rm ctdt.o
#	nmsimpc.exe *.o > blah
#	nmsimpc.exe *.o | wtxtcl C:\PentiumTornado2.2\host\src\hutils\munch.tcl > ctdt.c
#	ccsimpc.exe $(cflags) -fdollars-in-identifiers -c ctdt.c -o ctdt.o
#	ldsimpc.exe $(ldflags) -o $(notdir $@)	$^  ctdt.o C:\PentiumTornado2.2\target\lib\simpc\SIMNT\gnu\libcplus.a C:\PentiumTornado2.2\target\lib\simpc\SIMNT\gnu\libgnucplus.a


endif #	GNUCOMPILER



#### Generic Stuff


# Make the web page	files
#%.c %.h: %.htm	%.cla
#	echo BINTOC	$<
#	../webpage/bintoc $(subst /,\\,$<) $(subst /,\\,$(basename $(notdir	$<)))

# Make dependency files
%.d: %.c
	echo UPDATING DEPENDENCIES FOR $<
	g++simpc.exe	-undef -M $< $(defines:%=-D%) $(builtins:%=-D%)	$(includes:%=-I%) |	depend $(obj) >	$@

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
