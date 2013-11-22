package goapp

import (
	//"appengine"
	//"encoding/json"
	"fmt"
	//"io"
        //"html/template"
	"net/http"
)


func PebbleConfig(res http.ResponseWriter, req *http.Request) {
	fmt.Fprintf(res, "<h1>Config n shit.</h1>")
}
