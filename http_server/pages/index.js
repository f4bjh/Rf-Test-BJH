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

// Gérer les messages reçus du serveur
window.socket.addEventListener('message', (event) => {
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
					
	switch (json_data.t) {
	case 0x00:
		chip_model = `unable to read chip info`;
		chip_revision = `unable to read chip info`;
		break;
	case 0x01:
		if (json_data.l !== 0) {
			chip_model = `${json_data.v}`;
		}
		break;
	case 0x02:
		if (json_data.l !== 0) {
			chip_revision = `${json_data.v}`;
		}
		break;
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
	case 0x05:
		if (json_data.l !== 0) {
			cpu0_counter = `${json_data.v}`;
		}
		break;
	case 0x06:
		if (json_data.l !== 0) {
			cpu1_counter = `${json_data.v}`;
		}
		break;
	case 0x07:
		if (json_data.l !== 0) {
			current_part_version = `${json_data.v}`;
		}
	    break;
	case 0x08:
		if (json_data.l !== 0) {
			current_part_build_date = `${json_data.v}`;
		}
	    break;
	case 0x09:
		if (json_data.l !== 0) {
			next_part_version = `${json_data.v}`;
		}
	    break;
	case 0x0A:
		if (json_data.l !== 0) {
			next_part_build_date = `${json_data.v}`;
		}
	    break;
	}

	ChipInfo.innerHTML = `CPU0 counter = ${cpu0_counter}`;
	ChipInfo.innerHTML += ` | CPU1 counter = ${cpu1_counter}<br><br><br><br>`;
	ChipInfo.innerHTML += `chip model : ${chip_model} - chip revision : ${chip_revision}<br><br><br><br>`;
	
	PartitionInfo.innerHTML =  `current partition : ${current_partition} | next partition    : ${next_partition}<br><br><br><br>`;

	current_part_info.innerHTML = `${current_partition} : ${current_part_version} / ${current_part_build_date}<br>`;
	next_part_info.innerHTML = `${next_partition}  : ${next_part_version} / ${next_part_build_date}<br>`;
});
