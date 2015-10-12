# GNU Makefile for WebC
incpath = .

include mkgnu.inc

includes = . $(lib_to_hexamples) $(lib_to_web_include) $(lib_to_libhpng) $(lib_to_libhjpg) $(lib_to_libhmng) $(lib_to_libhgif) $(lib_to_libhmozilla) $(lib_to_libhzlib) $(lib_to_libhfdlibm) $(lib_to_libhhtmlcontent) $(lib_to_libhguiporting) $(lib_to_rtptarget_include) $(lib_to_rtpgeneric_include)

########################################################################
# SOURCE PATHS
########################################################################
vpath %.cxx . $(xnet_path)
vpath %.c   . $(lib_to_webc_app) $(lib_to_webc_path) $(lib_to_libpng) $(lib_to_libjpg) $(lib_to_libmng) $(lib_to_libgif) $(lib_to_mozilla) $(lib_to_zlib) $(lib_to_fdlibm) $(lib_to_htmlcontent) $(lib_to_rtptarget) $(lib_to_rtpgeneric)

vpath %.cpp . $(lib_to_webc_path) $(lib_to_guiporting) $(lib_to_examples)
vpath %.hpp . $(lib_to_web_include)
vpath %.h   .. $(lib_to_hexamples) $(lib_to_libhpng) $(lib_to_libhjpg) $(lib_to_libhmng) $(lib_to_libhgif) $(lib_to_libhmozilla) $(lib_to_libhzlib) $(lib_to_libhfdlibm) $(lib_to_libhhtmlcontent) $(lib_to_libhguiporting) $(lib_to_rtptarget_include) $(lib_to_rtpgeneric_include)

vpath %.htm . $(lib_to_webc_path)
vpath %.cla . $(lib_to_webc_path)

########################################################################
# OPERATING SYSTEM/ENVIRONMENT SPECIFIC MODULES
########################################################################
# The only files that should go here are files that are specific to one
# particular target, operating system, compiler, etc. and must go into
# the library.  Make sure your conditional(s) are as general as possible.

ifeq ($(os),LINUX)
#modules +=
endif

########################################################################
# EBS PACKAGES
########################################################################

ifeq ($(dowebc),"Y")
# defines += WEBC_SUPPORT_JSCRIPT=1 WEBC_SUPPORT_UNICODE=1 _WEBC_=1
defines += WEBC_SUPPORT_JSCRIPT=1 _WEBC_=1
endif


#############################################################################
# SETUP
#############################################################################

# The basic required list of modules, defines, includes.

# clean up the modules, includes, defines
# dirs = $(lib_to_webc_path) 

dirs :=  $(lib_to_libjpg) $(lib_to_webc_path) $(lib_to_libpng) $(lib_to_libmng) $(lib_to_libgif) $(lib_to_mozilla) $(lib_to_zlib) $(lib_to_fdlibm) $(lib_to_htmlcontent) $(lib_to_guiporting)  $(lib_to_examples)
cpfiles := $(foreach dir,$(dirs),$(wildcard $(dir)/*.cpp))
cpmodules := $(patsubst %.cpp,%,$(cpfiles))
cfiles := $(foreach dir,$(dirs),$(wildcard $(dir)/*.c))
cfiles += $(lib_to_rtpgeneric)/rtpchar.c
cfiles += $(lib_to_rtpgeneric)/rtpdutil.c
cfiles += $(lib_to_rtpgeneric)/rtpmemdb.c
cfiles += $(lib_to_rtpgeneric)/rtprand.c
cfiles += $(lib_to_rtpgeneric)/rtpscnv.c
cfiles += $(lib_to_rtpgeneric)/rtpstr.c
cfiles += $(lib_to_rtpgeneric)/rtpwchar.c
cfiles += $(lib_to_rtpgeneric)/rtpwcs.c
cfiles += $(lib_to_rtpgeneric)/rtpwscnv.c
cfiles += $(lib_to_rtptarget)/rtpdate.c
cfiles += $(lib_to_rtptarget)/rtpfile.c
cfiles += $(lib_to_rtptarget)/rtpmem.c
cfiles += $(lib_to_rtptarget)/rtpnet.c
cfiles += $(lib_to_rtptarget)/rtptime.c

cmodules := $(patsubst %.c,%,$(cfiles))

modules += $(cpmodules)
modules += $(cmodules)

modules  := $(sort $(modules))
includes := $(sort $(includes))
defines  := $(sort $(defines))

# now set up the object list
blah1 = $(modules:%=%.$(obj))
objects = $(notdir $(blah1))

#############################################################################
# RULESa
#############################################################################
all: webcdemo.out

webcv.a: $(objects)	
main.o: main.cpp
webcdemo.out: main.o webcv.a

vikram:
	echo dirs $(dirs)
	echo cfiles $(cfiles)
	echo cmodules $(cmodules)
	echo modules $(modules)







include rulesgnu.inc



#############################################################################

