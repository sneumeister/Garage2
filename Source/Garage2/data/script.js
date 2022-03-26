	'use strict';
//	document.addEventListener('DOMContentLoaded', function(event) {			// Asynchrones laden der SCripts...
	window.addEventListener('load', function(event) {						// Erst starten wenn DOM komplett...
		const	ICOBASE='';
//      const	ICOBASE='/fs/';
		const 	GARAGEFILNAM = ['garage0.png', 'garage1.png', 'garage2.png', 'garage3.png', 'garage4.png'];
		const	GARAGETEXT 	 = ['Keine Info!', 'Ganz offen', 'Nicht offen', 'Nicht geschlossen', 'Geschlossen'];
		const	BUTTONFILNAM = ['button_lost.png', 'button.png', 'button_push.png', 'button_blocked.png'];
		const	BUTTONALTTXT = ['Button keien Verbindung!', 'Button bereit', 'Button gedrückt', 'Button blockiert!'];
		const 	REQUESTINTERVAL = 800 ; //*** 800ms until request new doorlevel...
		const	REQUESTCONSECERRMAX =	5	; //*** After reaching this number of consecutive errors, assume connection lost and stop autorequest doorlevel. 
		const	INTERIMBLOCKCNTMAX	=	5  ; //*** After reaching this number of 'readyState' issues temp. block oush_button.
		const	IDGARAGEICON = 'garageicon';
		const	IDBUTTONICON = 'buttonico';
		const	IDGARAGELEVEL = 'garagelevel';
		
		let	garageIconNodes = [];
		let	buttonIconNodes = [];
		
		let interimBlockCnt = 0;	 //*** Count 'readyState' issues...
		let	requestConsecErr = 0;    //*** 0 Consecutive Errors for doorlevel requests...
		let	lastReqWasError	= false; //*** Marker to identify consecutive error ... or not .....
		let	allGaragePreloaded = false;
		let	allButtonPreloaded = false;
		let	doorLevelValid		 = false;
		let query_garage = document.querySelector('#'+IDGARAGEICON).parentNode;
		let query_txt 	 = document.querySelector('#'+IDGARAGELEVEL);
		let query_button = document.querySelector('#'+IDBUTTONICON).parentNode;
		let	intervalID	= false;  //*** Holder for SetInterval-Functiom.....
		
//**** Functions to preload Images: Start **************		
	
		
		function preLoadImgNodes(urls, alts, id, nodeArray) {
			nodeArray.length=0
			for(var i=0; i<urls.length ; i++) {
				nodeArray.length=i+1;
				nodeArray[i]=document.createElement('IMG');
				nodeArray[i].alt=alts[i];
				nodeArray[i].id=id;
				preloadImage(urls[i], nodeArray[i]);
			}
			
			function preloadImage(url, img, anImageLoadedCallback){
				img.onload = anImageLoadedCallback;
				img.src = ICOBASE + url;
			}
		}
		
		preLoadImgNodes(GARAGEFILNAM, GARAGETEXT, IDGARAGEICON, garageIconNodes);
		preLoadImgNodes(BUTTONFILNAM, BUTTONALTTXT, IDBUTTONICON, buttonIconNodes);
		
/*****
		for (var i=0; i<garageIconNodes.length; i++) {
				document.querySelector('#garageicon').appendChild(garageIconNodes[i]);
		}
		for (var i=0; i<buttonIconNodes.length; i++) {
				query_button = document.querySelector("#buttonico").appendChild(buttonIconNodes[i]);
		}
***/
//**** Functions to preload Images: End **************/
		

		var	lastmsgtxt = 99;
		var ajax_req = new XMLHttpRequest();
		var ajax_push =  new XMLHttpRequest();
		
//**** Error-Handlers.......		

		ajax_req.addEventListener('error',  function(event) {	
			console.warn("ajax_req => ERROR ", requestConsecErr);
			relaesePushButton('block');
			++requestConsecErr;
			if (requestConsecErr >= REQUESTCONSECERRMAX) {
				stopAllAction()
			}
		});

		ajax_push.addEventListener('error',  function(event) {	
			console.warn("ajax_push => ERROR ", requestConsecErr);
			relaesePushButton('block');
			++requestConsecErr;
			if (requestConsecErr >= REQUESTCONSECERRMAX) {
				stopAllAction()
			}
		});

	function stopAllAction() {
		if (intervalID) {
			window.clearInterval(intervalID);
		}
		setGarageIcon(0);
		relaesePushButton('lost');
		query_txt.innerText = "Verbindung verloren!";
		document.querySelector('#button').removeEventListener('click', push_button);
	};



		
		ajax_req.addEventListener('load', function(event) {	
			//console.log("Req-DoorLevel was 'load'ed.");
			if (ajax_req.status >= 200 && ajax_req.status < 300) {
				var msgtxt=parseInt(ajax_req.responseText, 10);
				//console.log("EventListener: ajax_req.responseText=", msgtxt);
				if (msgtxt>=0 && msgtxt<=3) {
					//console.log("msgtxt=", msgtxt);
					if (!doorLevelValid) { relaesePushButton('release'); };
						requestConsecErr=0;
					//if (msgtxt!=lastmsgtxt) {
						lastmsgtxt=msgtxt;
						setGarageIcon(msgtxt+1);
						//query_txt.innerText = GARAGETEXT[msgtxt+1] + ' (' + msgtxt +')';
						//query_garage.src = ICOBASE + GARAGEFILNAM[msgtxt+1];
						//console.log(ICOBASE + GARAGEFILNAM[(msgtxt)+1]);
						
					//}
				} 
				else {	// *** Illegal DoorLevel.....
					relaesePushButton('block');  //*** Temp. block button...
					++requestConsecErr;
					setGarageIcon(0);
					//query_txt.innerText = GARAGETEXT[0] + ' ()';
					//query_garage.src = ICOBASE + GARAGEFILNAM[0];
				}
			} 
			else {   // *** Error in HTTP-Request ... mostly not finished....
				relaesePushButton('block');  //*** Temp. block button...
				++requestConsecErr;		//*** if we come here, last request was successfully send (DONE) or it is first time (UNSENT)...
				console.warn("/doorlevel:", ajax_req.statusText, request.responseText);	}
		});
	
		function setGarageIcon(i) {
			let orglevel = i?' ('+(i-1)+')':'';
			query_txt.innerText = GARAGETEXT[i] + orglevel;
			//query_garage.src = ICOBASE + GARAGEFILNAM[i];
			query_garage.replaceChild(garageIconNodes[i], query_garage.firstChild);
		}
	
	
		
	//************	
		intervalID = window.setInterval(function() {
			//if (!(allButtonPreloaded && allGaragePreloaded)) {return ;} // All pictures preloaded in browser cache?
			if (document.readyState != "complete") {	return ;}			// DOM completely loaded and parsed...
			//console.log("setInterval(): ajax_req.readyState=", ajax_req.readyState);
			switch(ajax_req.readyState){
				case XMLHttpRequest.UNSENT: 			// int 0
				case XMLHttpRequest.DONE:				// int 4
					//console.log("SetInterval: ajax_req.readyState=", ajax_req.readyState);
					ajax_req.open("GET", "/doorlevel", true);
					ajax_req.send();
					//console.log("SetInterval_send(): ajax_req.send(): afterajax_req.readyState=", ajax_req.readyState);
					interimBlockCnt=0;	//*** Reset issue counter..
					break;
				//case XMLHttpRequest.OPENED: 			// int 1
				//case XMLHttpRequest.HEADERS_RECEIVED:	// int 2
				case XMLHttpRequest.LOADING:			// int 3
					break;
				default: // setIntervall() repaeted without being finished....
					++interimBlockCnt;
					if (interimBlockCnt>=INTERIMBLOCKCNTMAX) {
						relaesePushButton('block');  //*** Temp. block button...
						setGarageIcon(0);
					};
					console.warn("SetInterval_default: ajax_req.readyState=", ajax_req.readyState, ajax_req.statusText, ajax_req.responseText);
					break;
			}
		}, REQUESTINTERVAL ) ;
//*********************

	document.querySelector('#button').addEventListener('click', push_button);
	
	function setButtonPic(indx) {
		//query_button.src = ICOBASE + BUTTONFILNAM[indx];
		//query_button.alt = ICOBASE + BUTTONALTTXT[indx];
		query_button.replaceChild(buttonIconNodes[indx], query_button.firstChild);
	}

	function relaesePushButton(action) {
		//console.log('relaesePushButton(', action ,')');
		switch (action) {
			case 'release':
				setButtonPic(1);
				doorLevelValid = true;
				break;
			case 'block':
				setButtonPic(3);
				doorLevelValid = false;
				break;
			case 'lost': // connection lost....
				setButtonPic(0);
				doorLevelValid = false;
				break;
			default:
				break;
		}
	}
	
	function push_button() {
	if (doorLevelValid && ((ajax_push.readyState==XMLHttpRequest.DONE || ajax_push.readyState==XMLHttpRequest.UNSENT))) {
			setButtonPic(2);
						
			ajax_push.open("POST", "/push_the_button", false);  // 'false' == synchronuse... Send wait until answer from server!
			ajax_push.setRequestHeader('Access-Control-Allow-Origin' , '*');
            //ajax_push.setRequestHeader('Content-Type', 'text/plain');
            ajax_push.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
            
			ajax_push.setRequestHeader('Charset', 'ISO8859-15');
			ajax_push.send("action=push");   // The magic wirds to allow "Push the button"....
			setTimeout(function() {	
				setButtonPic(1); 
			}, 300);
		}
		else {
			console.log("push_button(): doorLevelValid == ", doorLevelValid, " ajax.push.readyState==DONE or UNSET", ajax_push.readyState==XMLHttpRequest.DONE || ajax_push.readyState==XMLHttpRequest.UNSENT);
		}
	}	

//*********************/

});
		  
