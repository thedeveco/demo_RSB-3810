
### Install Dependencies

```sh
sudo apt -y install libglib2-dev pkg-config libgdk-pixbuf-2.0-dev gst-plugins-{base,good,bad,ugly} && \
	sudo snap install go --classic
```

### Compile & Run

```sh
make && ./main
```

---

## Usage

- once the server is started, visit the server at port 80 from a web-browser
