let chip_model = '';
let chip_revision = '';

socket = new WebSocket("ws://192.168.4.1/ws");

socket.onopen = function () {
	socket.send("Hello Server!");
};

// Gérer les messages reçus du serveur
socket.addEventListener('message', (event) => {
	json_data = JSON.parse(event.data);

	// Afficher le tag hexadécimal dans la console
	console.log(`Tag hexadécimal : ${json_data.tag}`);
	console.log(`length : ${json_data.length}`);

	// Récupérer l'élément HTML avec l'identifiant "message-container" de la page actuelle
	messageContainer = document.getElementById('message-container');
		
	// Récupérer l'URL de la page actuelle
	url = window.location.href;

	// Extraire le paramètre d'identification de la page
	pageId = url.split('/').pop(); // Supposons que l'ID de la page est le dernier segment de l'URL

	// Filtrer les données en fonction de la page active
	switch (pageId) {
		case '':
		if (json_data.tag === 0x01) {
				chip_model = `chip_model : unknown<br>`;
				if (json_data.length !== 0) {
					chip_model = `${json_data.value}`;
				}
      		}
		if (json_data.tag === 0x02) {
				chip_revision = `chip_revision : unknown<br>`;
				if (json_data.length !== 0) {
					chip_revision = `${json_data.value}`;
				}
		}
		messageContainer.innerHTML = `chip model : ${chip_model}<br>`;
                messageContainer.innerHTML += `chip revision : ${chip_revision}<br>`;
		break;

		case 'frequencymeter':
		if (json_data.tag === 0x03 || json_data.tag === 0x07) {
			messageContainer.innerHTML += `Valeur champ ${json_data.tag} : ${json_data.value}<br>`;
      		}
		break;

		case 'powermeter':
	        if (json_data.tag === 0x46 || json_data.tag === 0xAB) {
			messageContainer.innerHTML += `Valeur champ ${json_data.tag} : ${json_data.value}<br>`;
		}
		break;
       	}
});

// Gérer les erreurs de connexion
socket.addEventListener('error', (event) => {
	console.error('Erreur de connexion WebSocket : ', event);
});

// Gérer la fermeture de la connexion
socket.addEventListener('close', (event) => {
	console.log('Connexion WebSocket fermée');
});
