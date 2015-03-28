var page_control = 0;

var previous = "";

function getPlayers() {
  if (page_control != 1) return; //We've since returned to the main menu.
  
  var req = new XMLHttpRequest();
  req.open('GET', 'https://misfitmc-players.firebaseio.com/players.json', true);
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
        var response = JSON.parse(req.responseText);
        
        var text = "";
        for(var index in response) {
          text += response[index].displayName + "\n";
        }
        
      console.log("Got Player Data");
        //text = "1\n2\n3\n4\n5\n" + text;
      if (text == previous) {
        console.log("Same data, not sending to phone");
      } else {
        previous = text;
        var dict = {"PLAYER_LIST": text};
        console.log("JS SENT:" + text);
        Pebble.sendAppMessage(dict);
      }
    } else { console.log('HTTP Error'); }
  };
  req.send(null);
  
  //If still on this page, plan to check again in 5 mins, otherwise just stop
  if (page_control === 1) {
    setTimeout(getPlayers, 5000);
  }
}
//

function getChat() {
  if (page_control != 2) return; //We've since returned to the main menu.
  
  var req = new XMLHttpRequest();
  req.open('GET', 'https://misfitmc-players.firebaseio.com/newchat.json?orderBy=%22$priority%22&limitToLast=15', true);
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
        var d = JSON.parse(req.responseText);
        console.log("Got Chat Data");
      
        var text = "";
        var name = "";
        var msg = "";
      
        for(var index in d) {
          name = removeFormatting("" + d[index].dname);
          if (name.length > 18) name = name.substring(0, 16) + "..";
          
          msg = d[index].chat;
          msg = removeFormatting(msg);
          
          if (d[index].type === "chat") {
            text = "-- " + name + ":\n" + msg + "\n" + text;
          } else if (d[index].type === "join") {
            text = "> " + name + " joined.\n" + text;
          } else if (d[index].type === "quit") {
            text = "< " + name + " quit.\n" + text;
          } else {
            text = "* " + msg + "\n" + text;
          }
        }
      
      //Only send an update if the text is changed
      if (text == previous) {
        console.log("Same data, not sending to phone");
      } else {
        previous = text;
        var dict = {"CHAT_LIST": text};
        console.log("JS SENT:" + text);
        Pebble.sendAppMessage(dict);
      }
    } else { console.log('HTTP Error'); }
  };
  req.send(null);
  
  //If still on this page, plan to check again in 5 mins, otherwise just stop
  if (page_control === 2) {
    setTimeout(getChat, 5000);
  }
}

Pebble.addEventListener('ready', function(e) {
  //console.log('JavaScript app ready and running!');
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('JS Message: ' + JSON.stringify(e.payload));
  
  if (page_control == e.payload.PAGE_CONTROL) return;
  
  page_control = e.payload.PAGE_CONTROL;
  
  if (page_control === 0) {
    return;
  } else if (page_control === 1) {
    getPlayers();
  } else if (page_control === 2) {
    getChat();
    return;
  }
});

function removeFormatting(str) {
  var i = str.indexOf("§");
  while (i != -1) {
    str = str.substring(0, i) + str.substring(i+2);
    i = str.indexOf("§");
  }
  
  while ((str.indexOf("[") != -1) && (str.indexOf("]") != -1)) {
    str = str.substring(0, str.indexOf("[")) + str.substring(str.indexOf("]")+2);
  }
  
  return str;
}

