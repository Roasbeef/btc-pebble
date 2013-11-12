var mt_gox = {'api_path' :'http://data.mtgox.com/api/2/BTCUSD/money/ticker_fast',
              'parse_json': function(){ return; },
             };

function get_btc_info(exchange) {
    console.log('We here!');
    var req = new XMLHttpRequest();
    console.log(exchange.api_path)
    req.open('GET', exchange.api_path, true);
    req.onload = function(e) {
        if (req.readyState == 4 && req.status == 200) {
            if(req.status == 200) {
                var response = JSON.parse(req.responseText);
                console.log(JSON.stringify(response));
            } else {
                console.log("Error");
            }
            //Pebble.sendAppMessage(send);
        }
    };
    req.send(null);
}

Pebble.addEventListener("ready", function(e) {
    console.log('REAADY!');
    var btc_exchange = mt_gox;
    setTimeout(get_btc_info, 30000, btc_exchange);
    console.log('TIMEOUT ACTIVATED');
});

Pebble.addEventListener("appmessage", function(e) {
    console.log('GOT MESSAGE FORM APP');
    console.log(e.type);
});
