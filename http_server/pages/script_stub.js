// Remplacer WebSocket par le stub dans l'environnement de test (en local ou hors ligne)
window.WebSocket = WebSocketStub;

var serverIp = window.location.hostname;
//socket = new WebSocket("ws://" + serverIp + "/ws");
window.socket = new WebSocket("ws://localhost:8080");

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

window.socket.onopen = function () {
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

