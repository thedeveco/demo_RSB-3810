build: stream main

stream: src/main.c
	gcc $$(pkg-config --cflags --libs gdk-pixbuf-2.0) $$(pkg-config --cflags --libs glib-2.0) $$(pkg-config --cflags --libs gstreamer-1.0) $< -o $@

main: main.go
	go build
