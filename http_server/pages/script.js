let chip_model = `unknown`;
let chip_revision = `unknown`;
let counter =  `unknown`;
let current_partition =  `unknown`;
let next_partition =  `unknown`;

socket = new WebSocket("ws://192.168.4.1/ws");

socket.onopen = function () {
	socket.send("Hello Server!");
};

// Gérer les erreurs de connexion
socket.addEventListener('error', (event) => {
	console.error('Erreur de connexion WebSocket : ', event);
});

// Gérer la fermeture de la connexion
socket.addEventListener('close', (event) => {
	console.log('Connexion WebSocket fermée');
});

// Gérer les messages reçus du serveur
socket.addEventListener('message', (event) => {
	const json_data = JSON.parse(event.data);

	// Afficher le tag hexadécimal dans la console du navigateur
	console.log(`t : ${json_data.t}`);
	console.log(`l : ${json_data.l}`);
	console.log(`v : ${json_data.v}`);

	// Récupérer l'élément HTML avec l'identifiant "message-container" de la page actuelle
	const ChipInfo = document.getElementById('chip-information');
	const PartitionInfo = document.getElementById('partition-info');
		
	// Récupérer l'URL de la page actuelle
	const url = window.location.href;

	// Extraire le paramètre d'identification de la page
	const pageId = url.split('/').pop(); // Supposons que l'ID de la page est le dernier segment de l'URL

	// Filtrer les données en fonction de la page active
	switch (pageId) {
	case '':
		if (json_data.t === 0x00) {
			chip_model = `unable to read chip info`;
			chip_revision = `unable to read chip info`;
		}	
		if (json_data.t === 0x01) {
			if (json_data.l !== 0) {
				chip_model = `${json_data.v}`;
			}
		}
		if (json_data.t === 0x02) {
			if (json_data.l !== 0) {
				chip_revision = `${json_data.v}`;
			}
		}
		if (json_data.t === 0x03) {
			if (json_data.l !== 0) {
				current_partition = `${json_data.v}`;
			}
	      	}
		if (json_data.t === 0x04) {
			if (json_data.l !== 0) {
				next_partition = `${json_data.v}`;
			}
	      	}
		if (json_data.t === 0x05) {
			if (json_data.l !== 0) {
				counter = `${json_data.v}`;
			}
		}
		if (json_data.t === 0x03) {
			if (json_data.l !== 0) {
				current_partition = `${json_data.v}`;
			}
	      	}
		if (json_data.t === 0x04) {
			if (json_data.l !== 0) {
				next_partition = `${json_data.v}`;
			}
	      	}

		ChipInfo.innerHTML = `chip model : ${chip_model}<br>`;
                ChipInfo.innerHTML += `chip revision : ${chip_revision}<br>`;
		ChipInfo.innerHTML += `counter = ${counter}<br>`;

		PartitionInfo.innerHTML =  `current partition : ${current_partition}<br>`;
		PartitionInfo.innerHTML += `next partition    : ${next_partition}<br>`;


		break;
	case 'upload.html':
		if (json_data.t === 0x03) {
			if (json_data.l !== 0) {
				current_partition = `${json_data.v}`;
			}
	      	}
		if (json_data.t === 0x04) {
			if (json_data.l !== 0) {
				next_partition = `${json_data.v}`;
			}
	      	}
		PartitionInfo.innerHTML =  `current partition : ${current_partition}<br>`;
		PartitionInfo.innerHTML += `next partition    : ${next_partition}<br>`;
		break;
	case 'frequencymeter.html':
		if (json_data.t === 0x03 || json_data.t === 0x07) {
			messageContainer.innerHTML += `Valeur champ ${json_data.t} : ${json_data.v}<br>`;
	    		}
		break;
	case 'powermeter.html':
	       if (json_data.t === 0x46 || json_data.t === 0xAB) {
			messageContainer.innerHTML += `Valeur champ ${json_data.t} : ${json_data.v}<br>`;
		}
		break;
        }
});
