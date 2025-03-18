let chip_model = `unknown`;
let chip_revision = `unknown`;
let cpu0_counter =  `unknown`;
let cpu1_counter =  `unknown`;
let current_partition =  `unknown`;
let next_partition =  `unknown`;
let current_part_version = `unknown`;
let current_part_build_date = `unknown`;
let next_part_version = `unknown`;
let next_part_build_date = `unknown`;
let frequency = `unknown`;
let freq_digit3 = 0;
let freq_digit2 = 0;
let freq_digit1 = 0;
let freq_digit0 = 0;



var serverIp = window.location.hostname;
socket = new WebSocket("ws://" + serverIp + "/ws");

socket.onopen = function () {
        // Extraire le paramètre d'identification de la page
	const url = window.location.href;
	const pageId = url.split('/').pop();

        // Conversion en chaîne binaire pour estimer la longueur en octets (UTF-8)
        const encoder = new TextEncoder();
        const vEncoded = encoder.encode(pageId);
        const l = vEncoded.length; // Longueur en octets

        // Création de l'objet JSON
        const jsonData = {
        t: 0x01,
        l: l,
        v: pageId
        };

        // Conversion en JSON
        const jsonString = JSON.stringify(jsonData);
        console.log(jsonString); // {"t":0,"l":3,"v":"123"}

        // Envoi via WebSocket
        socket.send(jsonString);
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
	if (!event.data || event.data.length === 0) {
          console.warn("Message vide reçu sur le WebSocket.");
          return;
        }

	const json_data = JSON.parse(event.data);

	// Afficher le tag hexadécimal dans la console du navigateur
	console.log(`t : ${json_data.t}`);
	console.log(`l : ${json_data.l}`);
	console.log(`v : ${json_data.v}`);

	// Récupérer l'élément HTML avec l'identifiant "message-container" de la page actuelle
	const ChipInfo = document.getElementById('chip-information');
	const PartitionInfo = document.getElementById('partition-info');
	const current_part_info = document.getElementById('current-part-info');
	const next_part_info = document.getElementById('next-part-info');
	const freq_digit = document.getElementById('freq-digit'); //MSB
				
	// Récupérer l'URL de la page actuelle
	const url = window.location.href;

	// Extraire le paramètre d'identification de la page
	const pageId = url.split('/').pop(); // Supposons que l'ID de la page est le dernier segment de l'URL

	// Filtrer les données en fonction de la page active
	switch (pageId) {
	case 'index.html':
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
				cpu0_counter = `${json_data.v}`;
			}
		}
		if (json_data.t === 0x06) {
			if (json_data.l !== 0) {
				cpu1_counter = `${json_data.v}`;
			}
		}
		if (json_data.t === 0x07) {
			if (json_data.l !== 0) {
				current_part_version = `${json_data.v}`;
			}
	      	}
		if (json_data.t === 0x08) {
			if (json_data.l !== 0) {
				current_part_build_date = `${json_data.v}`;
			}
	      	}
		if (json_data.t === 0x09) {
			if (json_data.l !== 0) {
				next_part_version = `${json_data.v}`;
			}
	      	}
		if (json_data.t === 0x0A) {
			if (json_data.l !== 0) {
				next_part_build_date = `${json_data.v}`;
			}
	      	}

		ChipInfo.innerHTML = `CPU0 counter = ${cpu0_counter}`;
		ChipInfo.innerHTML += ` | CPU1 counter = ${cpu1_counter}<br><br><br><br>`;
		ChipInfo.innerHTML += `chip model : ${chip_model} - chip revision : ${chip_revision}<br><br><br><br>`;
		
		PartitionInfo.innerHTML =  `current partition : ${current_partition} | next partition    : ${next_partition}<br><br><br><br>`;

		current_part_info.innerHTML = `${current_partition} : ${current_part_version} / ${current_part_build_date}<br>`;
		next_part_info.innerHTML = `${next_partition}  : ${next_part_version} / ${next_part_build_date}<br>`;
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
		if (json_data.t === 0xB) {
	    		if (json_data.l !== 0) {
				frequency = `${json_data.v}`;
			}
		}
		let strFreq = String(frequency).padStart(4, '0');
		//for an unknown reason, MSB and LSB seems to be switched
		freq_digit3 = parseInt(strFreq[0]);
		freq_digit2 = parseInt(strFreq[1]);
		freq_digit1 = parseInt(strFreq[2]);
		freq_digit0 = parseInt(strFreq[3]);
		freq_digit.innerHTML = `
		<div class='s7s'><input value='` + freq_digit3 + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
		<div class='s7s'><input value='` + freq_digit2 + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
		<div class='s7s'><input value='` + freq_digit1 + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
		<div class='s7s'><input value='` + freq_digit0 + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
		<div class='unit-Hz'>MHz</div>
		`;
		break;
	case 'powermeter.html':
	       if (json_data.t === 0x46 || json_data.t === 0xAB) {
			messageContainer.innerHTML += `Valeur champ ${json_data.t} : ${json_data.v}<br>`;
		}
		break;
        }
});
