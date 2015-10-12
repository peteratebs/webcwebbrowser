
function toggleNotice() {
	var notice = document.getElementById('centralNotice');
	if (!wgNoticeToggleState) {
		notice.className = notice.className.replace('collapsed', 'expanded');
		toggleNoticeCookie('0');
	} else {
		notice.className = notice.className.replace('expanded', 'collapsed');
		toggleNoticeCookie('1');
	}
	wgNoticeToggleState = !wgNoticeToggleState;
}
function toggleNoticeStyle(elems, display) {
	if(elems)
		for(var i=0;i<elems.length;i++)
			elems[i].style.display = display;
}
function toggleNoticeCookie(state) {
	var e = new Date();
	e.setTime( e.getTime() + (7*24*60*60*1000) ); // one week
	var work='hidesnmessage='+state+'; expires=' + e.toGMTString() + '; path=/';
	document.cookie = work;
}
function pickTemplate(templates, weights) {
	var weightedTemplates = new Array();
	var currentTemplate = 0;
	var totalWeight = 0;

	if (templates.length == 0)
		return '';

	while (currentTemplate < templates.length) {
		totalWeight += weights[currentTemplate];
		for (i=0; i<weights[currentTemplate]; i++) {
			weightedTemplates[weightedTemplates.length] = templates[currentTemplate];
		}
		currentTemplate++;
	}
	
	if (totalWeight == 0)
		return '';

	var randomnumber=Math.floor(Math.random()*totalWeight);
	return weightedTemplates[randomnumber];
}


var wgNoticeToggleState = (document.cookie.indexOf('hidesnmessage=1')==-1);
document.writeln("\x3cstyle type=\"text/css\"\x3e\n#centralNotice .siteNoticeSmall{display:none;}\n#centralNotice .siteNoticeSmallAnon{display:none;}\n#centralNotice .siteNoticeSmallUser{display:none;}\n#centralNotice.collapsed .siteNoticeBig{display:none;}\n#centralNotice.collapsed .siteNoticeSmall{display:block;}\n#centralNotice.collapsed .siteNoticeSmallUser{display:block;}\n#centralNotice.collapsed .siteNoticeSmallAnon{display:block;}\n#centralNotice.anonnotice .siteNoticeSmallUser{display:none !important;}\n#centralNotice.usernotice .siteNoticeSmallAnon{display:none !important;}\n\x3c/style\x3e");

wgNotice=pickTemplate(["\x3cstyle type=\"text/css\"\x3e \n/*\nStyles for  Notices\n*/\n\n.notice-wrapper-stew2010-vote, .notice-collapsed-wrapper-stew2010-vote {\n    width: 100%;\n    margin: 2px auto 0;\n    padding: 0;\n    background: #DDD;\n    font-family: \'Arial\', \'Helvetica\', \'Tahoma\', sans-serif;\n    font-size: 1.0em;\n    font-weight: 200;\n    color: #333;\n}\n\n.notice-wrapper-stew2010-vote {\n    border: 1px solid #BBB;\n    background: #FCFCFC;\n    text-align: left;\n    font-size: 1.0em;\n}\n\n.notice-wrapper-stew2010-vote a {\n    color: #006699;\n}\n\n.trans-box {\n    padding: 0;\n    text-align: right;\n    font-size: 0.8em;\n    white-space: nowrap;\n}\n\n.toggle-box {\n    text-align: right;\n    font-size: 0.8em;\n    padding: 0;\n}\n\n.notice-text-stew2010-vote {\n    margin: 0 auto 5px;\n    padding: 2px 1px 4px;\n    font-size: 1.0em;\n}\n\n.line-ht-fix {\n    line-height: 1em;\n}\n\n#centralNotice.anonnotice .siteNoticeUser,\n#centralNotice.collapsed .siteNoticeUser {\n    display:none !important;\n}\n\x3c/style\x3e\n\n\x3ctable class=\"siteNoticeUser notice-wrapper-stew2010-vote\" id=\"notice-wrapper-stew2010-vote\"\x3e\n   \x3ctr\x3e\n      \x3ctd\x3e\n         \x3cdiv class=\"notice-text-stew2010-vote\"\x3e\n            The Wikimedia Foundation\'s 2010 steward election has started. \x3ca href=\"http://meta.wikimedia.org/wiki/Stewards/elections_2010\"\x3ePlease vote.\x3c/a\x3e\n         \x3c/div\x3e\n      \x3c/td\x3e\n      \x3ctd class=\"line-ht-fix\"\x3e\n         \x3cspan class=\"toggle-box\"\x3e\n            [\x3ca href=\"#\" onclick=\"toggleNotice();return false\"\x3eHide\x3c/a\x3e]\n         \x3c/span\x3e\x3cbr /\x3e\n         \x3cspan class=\"trans-box\"\x3e\n            [\x3ca href=\"http://meta.wikimedia.org/wiki/Stewards/elections_2010/Guidelines#Translators\"\x3eHelp us with translations!\x3c/a\x3e]\n         \x3c/span\x3e\n      \x3c/td\x3e\n   \x3c/tr\x3e\n\x3c/table\x3e\n\x3cdiv class=\"siteNoticeSmallAnon notice-collapsed-wrapper-stew2010-vote\"\x3e\n\x3c/div\x3e\n\n\x3cscript type=\"text/javascript\"\x3e\nvar wgHideStewardVoteNotice = {\n	\'notice\':null,\n	\'min_edit_count\':600, // minimum crosswiki edit count for eligibility\n	\'min_registration\':\'2009-11-01T00:00:00Z\', // minimum local registration timestamp for eligibility\n	\'cookie_key\':\'StewardVoteEligible_r7\',\n	\n	/*********\n	** Log error\n	*********/\n	\'log\':function( text ) {\n		//if( !window.console || !console.log )\n		//	return;\n		//console.log( \'wgHideStewardVoteNotice: \' + text );\n	},\n	\n	/*********\n	** Initialize script\n	*********/\n	\'initialize\':function() {\n		/* not logged in */\n		if( !wgUserName )\n			return;\n\n		/* get elements */\n		this.notice = document.getElementById( \'notice-wrapper-stew2010-vote\' );\n		if( !this.notice )\n			return this.log( \'could not locate #notice-wrapper-stew2010-vote\' );\n		\n		/* already checked */\n		var pos = document.cookie.indexOf( this.cookie_key + \'=\' );\n		if( pos \x3e -1 ) {\n			if( document.cookie.charAt(pos + this.cookie_key.length + 1) == 0 )\n				this.notice.style.display = \'none\';\n			return;\n		}\n				\n		/* initialize AJAX request */\n		this.sendAjaxRequest();\n	},\n	\n	/*********\n	** Send AJAX request\n	*********/	\n	\'sendAjaxRequest\':function() {\n		var uri = wgServer + wgScriptPath + \'/api.php?action=query\x26meta=globaluserinfo\x26guiprop=merged|unattached\x26format=json\x26guiuser=\' + encodeURIComponent( wgUserName );\n		\n		var request = sajax_init_object();\n		request.onreadystatechange = this.onAjaxStateChange;\n		request.open( \'GET\', uri, true/*asynchronous*/ );\n		request.send( \'\' );\n	},\n	\n	/*********\n	** Handle AJAX request state change\n	*********/\n	\'onAjaxStateChange\':function() {\n		/* request in progress */\n		if( this.readyState != 4 )\n			return;\n			\n		/* request failed */\n		if( this.status != 200 )\n			return this.log( \'AJAX error \' + this.status + \': \' + this.statusText );\n		\n		/* parse */\n		result = eval( \'(\' + this.responseText + \')\' );\n		wgHideStewardVoteNotice.checkEligibility( result );\n	},\n	\n	/*********\n	** Check eligibility\n	*********/\n	\'checkEligibility\':function( data ) {\n		if( data.error )\n			return this.log( \'API error \' + data.error.code + \': \' + data.error.info );\n		\n		/* merge lists */\n		var wikis = data.query.globaluserinfo.merged;\n		wikis.concat( data.query.globaluserinfo.unattached );\n		if( wikis.length == 0 )\n			return this.log( \'API returned no wikis (how can this happen?)\' );\n		\n		/* get edit count \x26 oldest registration date */\n		var edit_count = 0;\n		var oldest_account = wikis[0].timestamp;\n		var eligible;\n\n		for( var i = 0, len = wikis.length; i \x3c len; i++ ) {\n			edit_count += parseInt(wikis[i].editcount);\n			// this.log( wikis[i].wiki + \': edits=\' + wikis[i].editcount + \' (total=\' + edit_count + \'), timestamp=\' + wikis[i].timestamp );\n			if( wikis[i].timestamp \x3c oldest_account )\n				oldest_account = wikis[i].timestamp;\n			\n			eligible = (edit_count \x3e= this.min_edit_count \x26\x26 oldest_account \x3c= this.min_registration);\n			if( eligible )\n				break;\n		}\n\n		/* hide notice and store result */\n		this.log( \'checked likely eligibility: \' + eligible + \' (\' + edit_count + \' edits, oldest account \' + oldest_account + \')\' );\n		if( !eligible ) {\n			this.setCookie( this.cookie_key, \'0\' );\n			this.notice.style.display = \'none\';\n		}\n		else\n			this.setCookie( this.cookie_key, \'1\' );\n	},\n	\n	/*********\n	** Set cookie\n	*********/\n	\'setCookie\':function( key, value ) {\n		// set expiry to end of election\n		var expiryDate = new Date();\n		expiryDate.setDate( 29 );\n		\n		// set cookie\n		document.cookie = key + \'=\' + value + \';expires=\' + expiryDate.toGMTString();\n	}\n}\nwgHideStewardVoteNotice.initialize();\n\x3c/script\x3e"],[100]);
if (wgNotice != '')
wgNotice='<div id="centralNotice" class="' + (wgNoticeToggleState ? 'expanded' : 'collapsed') + ' ' + (wgUserName ? 'usernotice' : 'anonnotice' ) + '">' + wgNotice+'</div>';
