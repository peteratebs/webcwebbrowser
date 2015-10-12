###############################################################################
# Build rules
###############################################################################

htmlroot=..

modules  = alert_small
modules += archive
modules += archive_large
modules += archive_selected
modules += archives
modules += archives_pda
modules += back_default
modules += back_disable
modules += back_over
modules += back_halo
modules += back_over_halo
modules += bookmark_default
modules += bookmark_selected
modules += bookmarks
modules += bookmarks_large
modules += bookmarks_pda
modules += bottom_border
modules += brushedmetal
modules += brushedmetal_large
modules += brushedmetal_side
modules += button
modules += button_over
modules += chrome
modules += chrome_pda
modules += chrome_pda_small
modules += close_default
modules += close_over
modules += disabled_gradiant
modules += divider
modules += document
modules += forward_default
modules += forward_disable
modules += forward_over
modules += forward_halo
modules += forward_over_halo
modules += home_default
modules += home_over
modules += home_halo
modules += home_over_halo
modules += in_progress
modules += in_progress_small
modules += load_background
modules += loadChrome
modules += loadChrome_pda
modules += loadChrome_pda_small
modules += new_page
modules += new_page_over
modules += offline
modules += offline_disabled
modules += offline_toggle
modules += online
modules += online_disabled
modules += online_toggle
modules += prompt_small
modules += refresh_default
modules += refresh_halo
modules += refresh_over
modules += refresh_over_halo
modules += savetoarchive
modules += savetoarchive_disabled
modules += search_bottom
modules += search_left
modules += search_left_menu
modules += search_right
modules += search_top
modules += select_gradiant
modules += small_back
modules += small_forward
modules += small_refresh
modules += smallGlobe
modules += smallGlobe_gray
modules += stop_default
modules += stop_disable
modules += stop_over
modules += stop_halo
modules += stop_over_halo
modules += stripes
modules += top_border
modules += url_left
modules += url_right

outsrc = $(modules:%=$(htmlroot)/source/%.c)
outinc = $(modules:%=$(htmlroot)/include/%.h)

$(htmlroot)/source/%.c $(htmlroot)/include/%.h : %.html
	$(htmlroot)/../bin/bintoc $< $*

$(htmlroot)/source/%.c $(htmlroot)/include/%.h : %.png
	$(htmlroot)/../bin/bintoc $< $*

$(htmlroot)/source/%.c $(htmlroot)/include/%.h : %.gif
	$(htmlroot)/../bin/bintoc $< $*

all: $(outsrc) $(outinc)
