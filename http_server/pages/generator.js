let rf_gen_freq = 100000000;
let rf_gen_on = false;
let power = 3;
let freq_digit=new Array(10).fill(0);

function update_rf_gen_freq_display(rf_gen_freq,rf_gen_freq_out) {
        let strFreq = rf_gen_freq.toString().padStart(10, '0');
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
 
        const rf_gen_freq_out = document.getElementById('frequency-display');
 	update_rf_gen_freq_display(rf_gen_freq,rf_gen_freq_out);

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

function send_rf_gen_freq(rf_gen_freq) {
        const jsonData = {
            t: 0x03,
            l: rf_gen_freq.toString().length,
            v: rf_gen_freq.toString()
        };
        socket.send(JSON.stringify(jsonData));
        console.log("Fréquence envoyée: ", jsonData);
}

function send_rf_gen_status(rf_gen_on) {
        const jsonData = {
            t: 0x02,
            l: rf_gen_on ? 2 : 3,
            v: rf_gen_on ? "ON" : "OFF"
        };
	//if (rf_gen_on) {
        //    send_rf_gen_freq();
        socket.send(JSON.stringify(jsonData));
        console.log("État envoyé: ", jsonData);
        //}
}

function send_rf_gen_power(power) {
        const jsonData = {
            t: 0x04,
            l: power.toString().length,
            v: power.toString()
        };
        socket.send(JSON.stringify(jsonData));
        console.log("Power envoyé: ", jsonData);
}

document.addEventListener("DOMContentLoaded", () => {
    //const rf_gen_freq_out = document.querySelectorAll("#frequency-display .s7s input");
    const rf_gen_freq_out = document.getElementById('frequency-display');
    const increase_freq = document.getElementById("increase");
    const decrease_freq = document.getElementById("decrease");
    const scale_freq = document.querySelectorAll("input[name='scale']");
    const output_pwr = document.querySelectorAll("input[name='output_pwr']");
    const rf_gen_status_toggle_btn = document.getElementById("rf-gen-status-toggle-btn");

    increase_freq.addEventListener("click", () => {
        rf_gen_freq = Math.min(rf_gen_freq + scale, 4294967295);
        update_rf_gen_freq_display(rf_gen_freq,rf_gen_freq_out);
        send_rf_gen_freq(rf_gen_freq);
    });

    decrease_freq.addEventListener("click", () => {
        rf_gen_freq = Math.max(rf_gen_freq - scale, 35000000);
        update_rf_gen_freq_display(rf_gen_freq,rf_gen_freq_out);
        send_rf_gen_freq(rf_gen_freq);
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
		send_rf_gen_power(power);
            }
        });
    });

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
        send_rf_gen_status(rf_gen_on);
    });

});
