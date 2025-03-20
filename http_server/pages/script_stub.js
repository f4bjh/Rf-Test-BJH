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
let freq_digit=new Array(10).fill(0);
let freq_range = `4MHz`; // Valeur par défaut

function send_freq_range_update(freq_range) {
    const jsonData = {
        t: 0x0C,
        l: freq_range.length,
        v: freq_range
    };
    socket.send(JSON.stringify(jsonData));
    console.log(`range envoyé: ${freq_range}`);
}

document.addEventListener("DOMContentLoaded", () => {
    document.querySelectorAll("input[name='freq_range']").forEach(input => {
        input.addEventListener("change", event => {
            if (event.target.checked) {
                freq_range = event.target.value;
                send_freq_range_update(freq_range);
            }
        });
    });
});

// Stub de WebSocket pour émuler la connexion
function WebSocketStub(url) {
  // Simule l'URL du WebSocket
  console.log(`WebSocket connecté à: ${url}`);

  // Simule l'ouverture de la connexion
  setTimeout(() => {
    if (this.onopen) {
      this.onopen();  // Appeler onopen après 1 seconde
    }
  }, 1000);

  // Simule l'envoi de messages
  this.send = function(message) {
    console.log("Message envoyé : " + message);
    // Simule une réponse après un délai
    //setTimeout(() => {
    if (this.onmessage) {
       this.onmessage({ data : message});
    }
    //}, 500);
  };

  // Simule une fermeture de connexion
  this.close = function() {
    console.log("Connexion WebSocket fermée.");
    if (this.onclose) {
      this.onclose();
    }
  };

  // Simule une erreur
  this.onerror = function() {
    console.log("Erreur WebSocket simulée.");
    if (this.onerror) {
      this.onerror();
    }
  };
}

// Remplacer WebSocket par le stub dans l'environnement de test (en local ou hors ligne)
window.WebSocket = WebSocketStub;

var serverIp = window.location.hostname;
//socket = new WebSocket("ws://" + serverIp + "/ws");
socket = new WebSocket("ws://localhost:8080");

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
        //socket.send(jsonString);
};

// Gérer les erreurs de connexion
//socket.addEventListener('error', (event) => {
socket.onerror = function() {
	console.error('Erreur de connexion WebSocket : ', event);
};

// Gérer la fermeture de la connexion
//socket.addEventListener('close', (event) => {
socket.onclose = function() {	
	console.log('Connexion WebSocket fermée');
};

// Gérer les messages reçus du serveur
//socket.addEventListener('message', (event) => {
socket.onmessage = function(event) {
	console.log("Message reçu : " + event.data); 
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
		
		
		console.log(`freq range sélectionné: ${freq_range}`)
		
		
		let strFreq = String(frequency).padStart(10, '0');
		
		for (let i = 0; i < 10; i++) {
			freq_digit[i] = parseInt(strFreq[9-i]);
		}
		
		//here, it depends on precision user set in page
		//but precision will be set according this : 
		// input freq range   |   precision
		//  0 - 4 MHz         |   mHz       ex : 4294967.155Hz  (ie 4MHz)
		// 4MHz - 250MHz      |  1/10       ex : 42120785.1 (ie 42MHz)
		// 250MHz - 4GHz      |   Hz        ex : 400014004   (ie 4GHz)
		// it will have to be managed in meas/freq/freq
		// 1st case (0-4MHz) : 22 bits of data before coma + 10 bits after coma
		// 2nd case (4MHz-250MHz) : 28 bits of data before coma + 4 bits after coma
		// 3rd case (250MHz-4GHz) : 32 bits of data 
		
		if (freq_range === "4MHz") {
			freq_digit.innerHTML = `
			<div class='s7s'><input value='` + freq_digit[6] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='entre-digit'>&nbsp</div>
			<div class='s7s'><input value='` + freq_digit[5] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[4] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[3] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='entre-digit'>.</div>
			<div class='s7s'><input value='` + freq_digit[2] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[1] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[0] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='unit-Hz'>Hz</div>
			`;
		}
		if (freq_range === "250MHz") {
			freq_digit.innerHTML = `
			<div class='s7s'><input value='` + freq_digit[9] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[8] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[7] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='entre-digit'>&nbsp</div>
			<div class='s7s'><input value='` + freq_digit[6] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[5] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[4] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='entre-digit'>&nbsp</div>
			<div class='s7s'><input value='` + freq_digit[3] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[2] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[1] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='entre-digit'>.</div>
			<div class='s7s'><input value='` + freq_digit[0] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='unit-Hz'>Hz</div>
			`;		
		}
		if (freq_range === "4GHz") {
			freq_digit.innerHTML = `
			<div class='s7s'><input value='` + freq_digit[9] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='entre-digit'>&nbsp</div>
			<div class='s7s'><input value='` + freq_digit[8] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[7] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[6] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='entre-digit'>&nbsp</div>
			<div class='s7s'><input value='` + freq_digit[5] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[4] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[3] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='entre-digit'>&nbsp</div>
			<div class='s7s'><input value='` + freq_digit[2] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[1] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='s7s'><input value='` + freq_digit[0] + `'/><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg><seg></seg></div>
			<div class='unit-Hz'>Hz</div>
			`;
		}
		break;
	case 'powermeter.html':
	       if (json_data.t === 0x46 || json_data.t === 0xAB) {
			messageContainer.innerHTML += `Valeur champ ${json_data.t} : ${json_data.v}<br>`;
		}
		break;
        }
};
