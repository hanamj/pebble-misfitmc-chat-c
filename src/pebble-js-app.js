function getPlayers() {
  var req = new XMLHttpRequest();
  req.open('GET', 'https://misfitmc-players.firebaseio.com/players.json', true);
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
        var response = JSON.parse(req.responseText);
        
        var text = "";
        for(var index in response) {
          text += response[index].displayName + "\n";
        }
        
        console.log(text);
        //text = "1\n2\n3\n4\n5\n" + text;
        var dict = {"PLAYER_LIST": text};
        Pebble.sendAppMessage(dict);
    } else { console.log('HTTP Error'); }
  };
  req.send(null);
}

Pebble.addEventListener('ready', function(e) {
  //console.log('JavaScript app ready and running!');
  setInterval(getPlayers, 5000);
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('Received message: ' + JSON.stringify(e.payload));
});

