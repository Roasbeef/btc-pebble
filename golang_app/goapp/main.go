package goapp

import (
	"net/http"

	"github.com/gorilla/mux"
)

func init() {
	router := mux.NewRouter()

	router.HandleFunc("/", PebbleConfig).Methods("GET")

	http.Handle("/", router)
}
