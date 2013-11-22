var btc_exchanges = (function(){
    // 'exchange_func' is an exchange specific parsing function.
    // Ex: converting Chinese Yuan to US Dollar or handling exchange specific
    // json parsing
    var json_parser = function(last, buy, sell, exchange_func) {
        return function(resp) {
            var pebble_frame = {};
            exchange_func = exchange_func || function(s) {return s;};
            function getattr(obj, attrs) {
                var attrs = attrs.split(".");
                for(var i=0;i<attrs.length;i++) {
                    obj = obj[attrs[i]];
                };
                return obj;
            };
            pebble_frame.value = exchange_func(getattr(resp, last));
            pebble_frame.buy = exchange_func(getattr(resp, buy));
            pebble_frame.sell = exchange_func(getattr(resp, sell));

            return pebble_frame;
        };
    };
    console.log('returning');
    return {
        'mt_gox': {
            'api_path' :'http://data.mtgox.com/api/2/BTCUSD/money/ticker_fast',
            'parse_json': json_parser('data.last.display', 'data.buy.display', 
                                      'data.sell.display', function(s) {
                                          // Trim off the "$" sign.
                                          return s.substr(1, s.length - 1);
                                      }),
        },
        'bitstamp': {
            'api_path' :'http://www.bitstamp.net/api/ticker/',
            'parse_json': json_parser('last', 'bid', 'ask'),
        },
        'btc-e': {
            'api_path': 'https://btc-e.com/api/2/btc_usd/ticker',
            'parse_json': json_parser('ticker.last', 'ticker.buy', 'ticker.sell',
                                      function(i) { 
                                          // Trim off extra decimal place.
                                          var str_dollar = String(i);
                                          return str_dollar.substr(0, str_dollar.length - 1);
                                      }),
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
                var resp = JSON.parse(req.responseText);
                console.log(JSON.stringify(resp));
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
    Pebble.openURL('http://btc-pebble.appspot.com/');
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
