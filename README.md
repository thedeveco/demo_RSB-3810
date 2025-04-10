
### Install Dependencies

```sh
sudo apt -y install libglib2-dev pkg-config libgdk-pixbuf-2.0-dev gst-plugins-{base,good,bad,ugly} mpv && \
	sudo snap install go --classic
```

### Compile & Run

```sh
make && ./main
```

---

## Usage

- visit the server at port 8080 from a web-browser
- click & drag the images around - click the images on the top to change the selection
- shift click & drag to resize individual selections
