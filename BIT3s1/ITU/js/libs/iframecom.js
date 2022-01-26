/**
* @file iframecom.js [API] (FITeduler)
* 
* @brief Funkce pro komunikaci a vytvoření JavaScriptového API
* @date 2022-01-13 (YYYY-MM-DD)
* @author Karel Jirgl
* @update 2022-01-23 (YYYY-MM-DD)
*/

function serverIframe(origin, callback) {
	
	window.addEventListener("message", (event) => {
		
		if (event.origin === origin)
			event.source.postMessage([event.data[0], callback(event.data[1])], event.origin);
			
	}, false);
}

function fetchIframe(origin, url, message, callback){
	
	var iframe = document.createElement("iframe");
	iframe.src = url;
	iframe.style.display = 'none';
	document.body.appendChild(iframe);
	
	iframe.addEventListener("load", (ievent) => {
		
		iframe.contentWindow.postMessage([ievent.timeStamp+message, message], origin);
		
		window.addEventListener("message", (event) => {
			
			if (ievent.timeStamp+message == event.data[0]) {
				
				iframe.parentNode.removeChild(iframe);
				
				if (event.origin === origin)
					callback(event.data[1])
				
				window.removeEventListener("message", this, false);
			}
			
		}, false);
	});
}
