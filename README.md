
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

- view the HLS stream with `mpv`: `mpv http://<server IP>/playlist.m3u8 `
- visit the server at port 80 from a web-browser
- click & drag the images around - click the 1-9 buttons up top to change the selection
- shift click & drag to resize individual selections
