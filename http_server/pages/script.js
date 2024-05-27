	var socket = new WebSocket("ws://192.168.4.1/ws");
	var chip_model = document.getElementById('chip_model');

	socket.onopen = function () {
	  console.log("WebSocket connected."); 
	  socket.send("Hello Server!");
	};

	socket.onmessage = function (message) {
	  console.log("Message from server ", message.data);
	  chip_model.innerHTML = message.data +'<br />';
	};

	socket.onerror = function (error) {
	  console.log('WebSocket error: ' + error);
	};
