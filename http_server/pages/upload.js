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

let current_partition =  `unknown`;
let next_partition =  `unknown`;

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
	const PartitionInfo = document.getElementById('partition-info');


	switch (json_data.t) {
    case 0x03:
        if (json_data.l !== 0) {
            current_partition = `${json_data.v}`;
        }
        break;
    case 0x04:
        if (json_data.l !== 0) {
            next_partition = `${json_data.v}`;
        }
        break;
    }
    PartitionInfo.innerHTML =  `current partition : ${current_partition}<br>`;
    PartitionInfo.innerHTML += `next partition    : ${next_partition}<br>`;
});
