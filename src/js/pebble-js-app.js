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
                var resp = JSON.parse(req.responseText).data;
                console.log(JSON.stringify(resp));
                var value = resp.last.display;
                var buy = resp.buy.display;
                var sell = resp.sell.display;
                Pebble.sendAppMessage({
                    // Trim off the dollar signs.
                    'value': value.substr(1, value.length - 1),
                    'buy': buy.substr(1, buy.length - 1),
                    'sell': sell.substr(1, sell.length - 1)
                });
            } else {
                console.log('Error');
            }
        }
    };
    req.send(null);
};

Pebble.addEventListener("ready", function(e) {
    console.log('REAADY!');
    var btc_exchange = mt_gox;
    setInterval(get_btc_info, 5000, btc_exchange);
    console.log('TIMEOUT OK!');
});

Pebble.addEventListener("appmessage", function(e) {
    console.log('GOT MESSAGE FORM APP');
    console.log(e.type);
});
