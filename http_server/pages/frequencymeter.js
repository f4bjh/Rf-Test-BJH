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

let freq = `unknown`;
let freq_digit=new Array(10).fill(0);
let freq_range = `4MHz`; // Valeur par défaut

function send_freq_range_update(freq_range) {
    const jsonData = {
        t: 0x0C,
        l: freq_range.length,
        v: freq_range
    };
    window.socket.send(JSON.stringify(jsonData));
    console.log(`range envoyé: ${freq_range}`);
}

window.addEventListener("DOMContentLoaded", () => {
	const radios = document.querySelectorAll('input[name="freq_range"]');
	radios.forEach(radio => {
		radio.addEventListener("change", () => {
			if (radio.checked) {
				send_freq_range_update(radio.value);
			}
		});
	});
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

    const freq_digit = document.getElementById('freq-digit'); //MSB
                    

    switch (json_data.t) {
    case 0xB: 
        if (json_data.l !== 0) {
        freq = `${json_data.v}`;
        }
        break;
    }

    let strFreq = String(freq).padStart(10, '0');

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
});

