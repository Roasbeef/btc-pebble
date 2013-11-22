var btc_exchanges = (function(){
    var trim_dollar = function(s) { return s.substr(1, s.length - 1); };
    var json_parser = function(last, buy, sell) {
        return function(resp) {
            var pebble_frame = {};
            function getattr(obj, attrs) {
                var attrs = attrs.split(".");
                for(var i=0;i<attrs.length;i++) {
                    obj = obj[attrs[i]];
                };
                return obj;
            };
            pebble_frame.value = trim_dollar(getattr(resp, last));
            pebble_frame.buy = trim_dollar(getattr(resp, buy));
            pebble_frame.sell = trim_dollar(getattr(resp, sell));

            return pebble_frame;
        };
    };
    return {
        'mt_gox': {
            'api_path' :'http://data.mtgox.com/api/2/BTCUSD/money/ticker_fast',
            'parse_json': json_parser('last.display', 'buy.display', 
                                      'sell.display'),
        },
        'bitstamp': {
            'api_path' :'https://www.bitstamp.net/api/ticker/',
            'parse_json': function(resp) { 
                pebble_frame.value = trim_dollar(resp.last);
                pebble_frame.buy = trim_dollar(resp.bid);
                pebble_frame.sell = trim_dollar(resp.ask);

                return pebble_frame;
            },
        },
        'btc-e': {
            'api_path' :'',
            'parse_json': ''
        },
        'btcchina': {
            'api_path' :'',
            'parse_json': ''
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

Pebble.addEventListener("webviewclosed", function(e) {
    console.log('Config Page Closed');
});

Pebble.addEventListener("showConfiguration", function(e) {
    console.log('Opening Config Page');
    Pebble.openURL('http://www.google.com/');
});

Pebble.addEventListener("ready", function(e) {
    console.log('REAADY!');
    setInterval(get_btc_info, 5000, btc_exchanges['mt_gox']);
    console.log('TIMEOUT OK!');
});

Pebble.addEventListener("appmessage", function(e) {
    console.log('GOT MESSAGE FORM APP');
    // Grab config values previously saved on watch storage.
    //  * exchange
    //  * poll interval in ms
    //  * high alert threshold (default to ATH)
    //  * low alert threshold 
    //  * keep track of ATH
    //  What do do about graph?
    //  Tap to cycle between exchanges?
    console.log(e.type);
});
