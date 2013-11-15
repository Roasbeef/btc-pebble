var btc_exchanges = (function(){
    var pebble_frame = {};
    var trim_dollar = function(s) { return s.substr(1, s.length - 1); };
    return {
        'mt_gox': {
            'api_path' :'http://data.mtgox.com/api/2/BTCUSD/money/ticker_fast',
            'parse_json': function(resp) { 
                pebble_frame.value = trim_dollar(resp.last.display);
                pebble_frame.buy = trim_dollar(resp.buy.display);
                pebble_frame.sell = trim_dollar(resp.sell.display);

                return pebble_frame;
            },
        },
    };
})();


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
                console.log(Object.keys(Pebble));
                Pebble.sendAppMessage(exchange.parse_json(resp));
            } else {
                console.log('Error');
            }
        }
    };
    req.send(null);
};

Pebble.addEventListener("ready", function(e) {
    console.log('REAADY!');
    setInterval(get_btc_info, 5000, btc_exchanges['mt_gox']);
    console.log('TIMEOUT OK!');
});

Pebble.addEventListener("appmessage", function(e) {
    console.log('GOT MESSAGE FORM APP');
    console.log(e.type);
});
