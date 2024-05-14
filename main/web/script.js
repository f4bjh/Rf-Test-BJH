function NombreBis(val){
var Compteur=val.toString().split('');
var topborder = new Array([1,1,0,1],[0,1,0,0],[1,1,1,0],[1,1,1,0],[0,1,1,1],[1,0,1,1],[1,0,1,1],[1,1,0,0],[1,1,1,1],[1,1,1,1]);
var botborder = new Array([0,1,1,1],[0,1,0,0],[1,0,1,1],[1,1,1,0],[1,1,0,0],[1,1,1,0],[1,1,1,1],[0,1,0,0],[1,1,1,1],[1,1,1,0]);
var i=0;
 
var bcontainer = document.createElement('div');
bcontainer.className="fig"
var btopfig = document.createElement('div');
btopfig.className="figt"
var bbotfig = document.createElement('div');
bbotfig.className="figb"
 
while(Compteur[i++]){
var container=bcontainer.cloneNode(true)
var bc=""
 
var topfig=btopfig.cloneNode(true)
for(bar in topborder[Compteur[i-1]]){bc+=topborder[Compteur[i-1]][bar]?"white ":"black "}
topfig.style.borderColor=bc
bc="";
var botfig=bbotfig.cloneNode(true)
for(bar in botborder[Compteur[i-1]]){ bc+=botborder[Compteur[i-1]][bar]?"white ":"black "}
botfig.style.borderColor=bc
 
container.appendChild(topfig);
container.appendChild(botfig);
document.body.appendChild(container)
 }
 
}