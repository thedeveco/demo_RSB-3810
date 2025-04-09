package main

import (
	"log"
	"net/http"
	"os/exec"
	"fmt"
	"io"
)

func main() {
	cmd := exec.Command("./stream")
	reader, writer := io.Pipe()

	cmd.Stdin = reader

	go func() {
		err := cmd.Run()
		fmt.Println(err)
	}()

	fs := http.FileServer(http.Dir("."))
	http.Handle("/", fs)
	http.Handle("/set", http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		io.Copy(writer, r.Body)
	}))

	log.Println("Server listening on port 8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}