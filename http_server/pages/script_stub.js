// Remplacer WebSocket par le stub dans l'environnement de test (en local ou hors ligne)
window.WebSocket = WebSocketStub;

var serverIp = window.location.hostname;
//socket = new WebSocket("ws://" + serverIp + "/ws");
socket = new WebSocket("ws://localhost:8080");

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

document.addEventListener("DOMContentLoaded", () => {
    document.querySelectorAll("input[name='freq_range']").forEach(input => {
        input.addEventListener("change", event => {
            if (event.target.checked) {
                freq_range = event.target.value;
                //send_freq_range_update(freq_range);
            }
        });
    });
});

document.addEventListener("DOMContentLoaded", () => {
    //const rf_gen_freq_out = document.querySelectorAll("#frequency-display .s7s input");
    const rf_gen_freq_out = document.getElementById('frequency-display');
    const increase_freq = document.getElementById("increase");
    const decrease_freq = document.getElementById("decrease");
    const scale_freq = document.querySelectorAll("input[name='scale']");
    const output_pwr = document.querySelectorAll("input[name='output_pwr']");
    const rf_gen_status_toggle_btn = document.getElementById("rf-gen-status-toggle-btn");
    let scale = 1;
    let frequency = 1000000;
    let rf_gen_on = false;
	let power = 3;

    function update_rf_gen_freq_display() {
        let strFreq = frequency.toString().padStart(10, '0');
       	for (let i = 0; i < 10; i++) {
			freq_digit[i] = parseInt(strFreq[9-i]);
		}
        console.log("Affichage initialisé avec :", strFreq);
		
		rf_gen_freq_out.innerHTML = `
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

    function getFrequencyFromInputs() {
        return parseInt(Array.from(rf_gen_freq_out).map(input => input.value).join('')) || 0;
    }

    function send_rf_gen_freq() {
        const jsonData = {
            t: 0x02,
            l: frequency.toString().length,
            v: frequency.toString()
        };
        socket.send(JSON.stringify(jsonData));
        console.log("Fréquence envoyée: ", jsonData);
    }

    function send_rf_gen_status() {
        const jsonData = {
            t: 0x03,
            l: rf_gen_on ? 2 : 3,
            v: rf_gen_on ? "ON" : "OFF"
        };
	if (rf_gen_on) {
            send_rf_gen_freq();
        socket.send(JSON.stringify(jsonData));
        console.log("État envoyé: ", jsonData);
        }
    }

    function send_rf_gen_power() {
        const jsonData = {
            t: 0x04,
            l: power.toString().length,
            v: power.toString()
        };
        socket.send(JSON.stringify(jsonData));
        console.log("Power envoyé: ", jsonData);
    }


    increase_freq.addEventListener("click", () => {
        frequency = Math.min(frequency + scale, 4294967295);
        update_rf_gen_freq_display();
        send_rf_gen_freq();
    });

    decrease_freq.addEventListener("click", () => {
        frequency = Math.max(frequency - scale, 0);
        update_rf_gen_freq_display();
        send_rf_gen_freq();
    });

    scale_freq.forEach(checkbox => {
        checkbox.addEventListener("change", (event) => {
            if (event.target.checked) {
                scale = parseInt(event.target.value);
            }
        });
    });
	
	output_pwr.forEach(checkbox => {
        checkbox.addEventListener("change", (event) => {
            if (event.target.checked) {
                power = parseInt(event.target.value);
				send_rf_gen_power();
            }
        });
    });


    //rf_gen_freq_out.forEach(input => {
    //    input.addEventListener("input", () => {
    //        frequency = getFrequencyFromInputs();
    //        update_rf_gen_freq_display();
    //        send_rf_gen_freq();
    //    });
    //});

    rf_gen_status_toggle_btn.addEventListener("click", () => {
        rf_gen_on = !rf_gen_on;
		let rf_gen_status = document.getElementById("rf-gen-status");
        if (rf_gen_status.classList.contains("power-off")) {
            rf_gen_status.textContent = "ON";
            rf_gen_status.classList.remove("power-off");
            rf_gen_status.classList.add("power-on");
        } else {
            rf_gen_status.textContent = "OFF";
            rf_gen_status.classList.remove("power-on");
            rf_gen_status.classList.add("power-off");
        }
        send_rf_gen_status();
    });

    setTimeout(update_rf_gen_freq_display, 100); // Assurer un rendu après chargement
});




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
		
		if (pageId === "index.html" || pageId === "" ) {
			import("index.js");
		}
		if (pageId === "frequencymeter.html") {
			import("./frequencymeter.js")
  			.then((module) => {
				module.setupSocket(socket);
  			})
  			.catch((err) => console.error("Erreur d'importation de frequencymeter.js", err));
		}
		if (pageId === "upload.html") {
			import("upload.js");
		}
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
	const chip_info = document.getElementById('chip-information');
	const part_info = document.getElementById('partition-info');
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

		chip_info.innerHTML = `CPU0 counter = ${cpu0_counter}`;
		chip_info.innerHTML += ` | CPU1 counter = ${cpu1_counter}<br><br><br><br>`;
		chip_info.innerHTML += `chip model : ${chip_model} - chip revision : ${chip_revision}<br><br><br><br>`;
		
		part_info.innerHTML =  `current partition : ${current_partition} | next partition    : ${next_partition}<br><br><br><br>`;

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
		part_info.innerHTML =  `current partition : ${current_partition}<br>`;
		part_info.innerHTML += `next partition    : ${next_partition}<br>`;
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
	    
		break;
    }
};
