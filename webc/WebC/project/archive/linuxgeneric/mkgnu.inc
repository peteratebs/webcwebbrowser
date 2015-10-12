# This file evaluates the variables set on the command line and acts on them.
# targ is set to something such as POLG68K, RTPXMSP3, etc.
# RELENV should be N for ebsenv, Y otherwise.
# v=v for debug, v=o for optimized.

########################################################################
#targ = LINUXGENERIC
#targ = LINUXPEGLINUXXLIB
targ = LINUXXLIB

# set the following to "Y" for webC, "N" otherwise.
dowebc = "Y"
# set the following to "Y" for webC, "N" otherwise.
dogif = "Y"



########################################################################
# Select the version (o)ptimzed or (dbv)debug here or pass it in on the command line
dbv = v

root    = ..
kernel  = c:
app_to_webc_path             =
lib_to_webc_path             = ../../source/browser ../../source/htmldisplay ../../source/htmldom ../../source/imgconvert ../../source/imgdecode ../../source/jscript ../../source/minigui ../../source/protocol ../../source/util
lib_to_libpng                = ../../lib/libpng
lib_to_libjpg                = ../../lib/libjpeg
lib_to_libmng                = ../../lib/libmng
lib_to_libgif                = ../../lib/libgif
lib_to_mozilla               = ../../lib/mozilla
lib_to_zlib                  = ../../lib/zlib
lib_to_fdlibm                = ../../lib/fdlibm
lib_to_htmlcontent           = ../../html/source
lib_to_examples              = ../../source/webc_example_controls/basic
lib_to_rtptarget             = ../../../../rtplatform/v0.4/source/linux
lib_to_rtpgeneric            = ../../../../rtplatform/v0.4/source/generic

lib_to_web_include           = ../../include/browser ../../include/htmldisplay ../../include/htmldom ../../include/imgconvert ../../include/imgdecode ../../include/jscript ../../include/minigui ../../include/protocol ../../include/util ../../source/htmldisplay ../../source/minigui
lib_to_libhpng               = ../../lib/libpng
lib_to_libhjpg               = ../../lib/libjpeg
lib_to_libhmng               = ../../lib/libmng
lib_to_libhgif               = ../../lib/libgif
lib_to_libhmozilla           = ../../lib/mozilla
lib_to_libhzlib              = ../../lib/zlib
lib_to_libhfdlibm            = ../../lib/fdlibm
lib_to_libhhtmlcontent       = ../../html/include
lib_to_hexamples             = ../../source/webc_example_controls/include
lib_to_rtpgeneric_include    = ../../../../rtplatform/v0.4/include
lib_to_rtptarget_include     = ../../../../rtplatform/v0.4/include/linux



########################################################################

known = "N"

ifeq ($(targ),LINUXGENERIC)
known = "Y"
# Set the compiler type for later conditionals
compiler = GNUCOMPILER
lib_to_libhguiporting        = ../../gui/template/include
lib_to_guiporting            = ../../gui/template/source
endif

ifeq ($(targ),LINUXPEG)
known = "Y"
# Set the compiler type for later conditionals
compiler = GNUCOMPILER
# Include Path 
lib_to_libhguiporting        = ../../gui/peg/include
# PEG Library Include Path
# This assumes that PEG is installed in the root directory. Please change !!!
lib_to_libhguiporting       += /peg/include
#Source Path
lib_to_guiporting            = ../../gui/peg/source
endif

ifeq ($(targ),LINUXXLIB)
known = "Y"
# Set the compiler type for later conditionals
compiler = GNUCOMPILER
# Include Path 
lib_to_libhguiporting        = ../../gui/xlib/include
# PEG Library Include Path
#Source Path
lib_to_guiporting            = ../../gui/xlib/source

ldflags += -lX11
ldflags += -lpthread
endif

ifneq ($(known),"Y")
.PHONY: error
error:
ifneq ($(targ),)
	@echo Incorrect target: $(targ)
else
	@echo Please set the targ and v variables.
endif
	@echo Please edit
endif

########################################################################

ifeq ($(compiler),GNUCOMPILER)
# Set the file extensions
obj = o
lib = a
exe = out

ifeq ($(dbv),v)
ccflags += -g
ldflags += -g
endif
endif

