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
