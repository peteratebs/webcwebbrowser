
Target specific files

rtpdobj.c   - Currently disabled.
rtpfile.c   -
rtpwfile.c  -


rtpmem.c    - Contains only calloc. memalloc.c in parrot contains _rtp_malloc et al.
              note: new is overridden in main.cpp for TI, see webcmem.cpp if memory debug is on.
              WEBC_MEMORY_DEBUG is not enabbled now but need to support.
rtpdate.c   -
   Probably need to move linux code over and use the GetLocalTime(&sysT ); function
   see drivers\rtctime.c

Put a modified copy of rtpnet.c from linux into the kwiknet directory and removed rtpnet from the rtplatform directory
    Note: FD_SETSIZE is out of sync
    Note: Some hacks made to compile


Changed rtplatform.pjt - Removed the following

rtpsignl.c  - DO NOT NEED - remove from the project.
rtpthrd.c   - DO NOT NEED - remove from the project.
rtpenv.c    - DO NOT NEED - remove from the project.
rtpkern.c   - DO NOT NEED - remove from the project. rtp_kern_init() et al. Should remove from project and build

Changed main.cpp. Added call to network init.


These seem to be the only rtpltaform includes
#include "rtpchar.h"
#include "rtpdate.h"
#include "rtpdebug.h"
#include "rtpdobj.h"
#include "rtpdutil.h"
#include "rtpfile.h"
#include "rtpmem.h"
#include "rtpmemdb.h"
#include "rtpnet.h"
#include "rtpprint.h"
#include "rtpscnv.h"
#include "rtpssl.h"
#include "rtpstr.h"
#include "rtpthrd.h"
#include "rtptime.h"
#include "rtptotc.h"
#include "rtpwcs.h"
#include "rtpwscnv.h"




rtpterm.c   - Calls ConioOutput looks okay.
rtptime.c   - uCos implementation OS_TICKS_PER_SEC


C:\dev\ebs\motosymbol\webc_gs\ebs\rtplatform\v0.4\source\motorola_symbol>dir /b

rtp.c
rtpbsearch.c
rtpchar.c
rtpdllist.c
rtpdutil.c
rtpfio.c
rtphelper.c
rtpmemdb.c
rtpmtxdb.c
rtpnetsm.c
rtpprint.c
rtpqsort.c
rtprand.c
rtprot.c
rtpscnv.c
rtpsemdb.c
rtpstdup.c
rtpstr.c
rtptimer.c
rtpwchar.c
rtpwcs.c
rtpwscnv.c

C:\dev\ebs\motosymbol\webc_gs\ebs\rtplatform\v0.4\source\motorola_symbol>

Generic files
