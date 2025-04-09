#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>

#include <gst/gst.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define LENGTH(x)  ((int)(sizeof (x) / sizeof *(x)))
//#define MIN(a, b)  ((a) > (b) ? (b) : (a))
//#define MAX(a, b)  ((a) < (b) ? (b) : (a))


int main(int argc, char *argv[]) {

	int i, n;
	char buf[1024*32], *s, *attr;

	int busfd;

	GstElement *pipeline, *src, *sink, *dec, *f, *filters[10] = {}, *enc;
	GError *err;
	gchar *info;
	GstStateChangeReturn r;
	GstBus *bus;
	GstMessage *m;
	fd_set rfds;

	FILE *file;
	GdkPixbuf *pixbufs[10];

	gst_init(&argc, &argv);


	// "v4l2src device=/dev/video0 ! jpegdec ! gdkpixbufoverlay offset-x=100 offset-y=100 overlay-width=500 location=elmer.jpg ! xvimagesink";

	pipeline = gst_pipeline_new("my-pipeline");
	src = gst_element_factory_make("v4l2src", "src");
	if (src == NULL)
		g_error("Could not create 'videotestsrc' element");

	dec = gst_element_factory_make("jpegdec", "dec");

	for (i = 0; i < LENGTH(filters); i++) {
		sprintf(buf, "filter-%d", i);
		f = gst_element_factory_make("gdkpixbufoverlay", buf);
		fprintf(stderr, "%d\n", i);
		if (f == NULL) {
			g_error("Could not create 'gdkpixbufoverlay' element");
			break;
		}
		filters[i] = f;
	}
	fprintf(stderr, "made %d %d filters\n", LENGTH(filters), i);

	enc = gst_element_factory_make("x264enc", "enc");
	if (enc == NULL) {
		g_error("Could not create neither '265enc'");
	}

	sink = gst_element_factory_make("hlssink2", "sink");
	if (sink == NULL) {
		g_error("Could not create neither 'hlssink2'");
	}

	gst_bin_add_many(
		GST_BIN(pipeline),
		src,
		dec,
		filters[0],
		filters[1],
		filters[2],
		filters[3],
		filters[4],
		filters[5],
		filters[6],
		filters[7],
		filters[8],
		filters[9],
		enc,
		sink,
		NULL
	);

	gst_element_link_many(
		src,
		dec,
		filters[0],
		filters[1],
		filters[2],
		filters[3],
		filters[4],
		filters[5],
		filters[6],
		filters[7],
		filters[8],
		filters[9],
		enc,
		sink,
		NULL
	);

	for (i = 0; i < LENGTH(filters); i++) {
		f = filters[i];
		fprintf(stderr, "%d\n", i);
		if (f == NULL) {
			fprintf(stderr, "breaking\n");
			break;
		}
		g_object_set(G_OBJECT(f), "offset-x", 100, NULL);
		g_object_set(G_OBJECT(f), "offset-y", 100, NULL);
		g_object_set(G_OBJECT(f), "overlay-width", 400, NULL);
		g_object_set(G_OBJECT(f), "overlay-height", 400, NULL);
		fprintf(stderr, "%d\n", pixbufs[i]);
		sprintf(buf, "%d.png", i);
		err = NULL;
		GdkPixbuf *pb = gdk_pixbuf_new_from_file(buf, &err);
		if (!pb || err != NULL) {
			break;
		}
		pixbufs[i] = pb;
		g_object_set(G_OBJECT(f), "pixbuf", pixbufs[i], NULL);
	}

	g_object_set(G_OBJECT(sink), "max-files", 3, NULL);
	g_object_set(G_OBJECT(sink), "playlist-length", 3, NULL);
	g_object_set(G_OBJECT(sink), "playlist-root", ".", NULL);
	g_object_set(G_OBJECT(sink), "target-duration", 1, NULL);

	

	switch ((r = gst_element_set_state(pipeline, GST_STATE_PLAYING))) {
	case GST_STATE_CHANGE_ASYNC:
		fprintf(stderr, "buffering...\n");
		break;
	default:
		fprintf(stderr, "FAILURE\n");
		return 1;
	}

	bus = gst_element_get_bus(pipeline);
	gst_bus_get_pollfd(bus, (GPollFD*)&busfd);
	fprintf(stderr, "busfd: %d\n", busfd);


	for (;;){

		FD_ZERO(&rfds);
		FD_SET(0,     &rfds);
		FD_SET(busfd, &rfds);

		if (select(FD_SETSIZE, &rfds, NULL, NULL, NULL) == -1) {
			if (errno == EINTR)
				fprintf(stderr, "interrupted\n");
		}

		if (FD_ISSET(0, &rfds)) {
			if ((n = read(0, &buf[0], sizeof(buf)-1)) == -1) {
				fprintf(stderr, "stdin read failed %d\n", errno);
				return 1;
			} else if (n == 0) {
				fprintf(stderr, "read eof\n");
				break;
			}
			buf[n] = '\0';
			s = strtok(&buf[0], " ");
			if (s == NULL) {
				fprintf(stderr, "parse failure\n");
				continue;
			}
			i = atoi(s);
			
			s = strtok(NULL, " ");
			if (s == NULL) {
				continue;
			}
			if (strcmp(s, "location") != 0) {
				// ok
			} else if ((s = strtok(NULL, " \n")) == NULL) {
				// ok
				fprintf(stderr, "failed to parse location\n");
				continue;
			} else {
				fprintf(stderr, "set %d location to %s\n", i, s);
				/*
				GdkPixbuf *pb = gdk_pixbuf_new_from_file(s, &err);
				gdk_pixbuf_fill(pixbufs[i], 0xffffffff);
				if (err != NULL && err->message != NULL) {
					fprintf(stderr, "%d %d %s\n", pixbufs[i], pb, err->message);
				}
				gdk_pixbuf_copy_area(pb, 0, 0, gdk_pixbuf_get_width(pb), gdk_pixbuf_get_height(pb), pixbufs[i], 0, 0);
				gst_element_set_state(pipeline, GST_STATE_PAUSED);
				g_object_set(G_OBJECT(filters[i]), "pixbuf", pixbufs[i], NULL);
				gst_element_set_state(pipeline, GST_STATE_PLAYING);

				*/
				continue;
			}

			attr = s;
			s = strtok(NULL, " ");
			if (s == NULL) {
				fprintf(stderr, "failed to parse\n");
				continue;
			}
			n = atoi(s);
			fprintf(stderr, "atoi %d %s\n", n, attr);
			if (n <= 0) {
				continue;
			}
			g_object_set(G_OBJECT(filters[i]), attr, n, NULL);
			fprintf(stderr, "set %d %s to %d\n", i, attr, n);
			fprintf(stderr, "read %.*s\n", n, &buf[0]);
		}

		if (FD_ISSET(busfd, &rfds)) {
			m = gst_bus_pop(bus);
			if (GST_MESSAGE_TYPE(m) == GST_MESSAGE_ERROR) {
				gst_message_parse_error(m, &err, &info);
				fprintf(stderr, "error: %s - %s - %s\n", GST_MESSAGE_TYPE_NAME(m), err->message, info);
			} else {
				//fprintf(stderr, "%s\n", GST_MESSAGE_TYPE_NAME(m));
			}
			gst_message_unref(m);
		}
	}

	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	gst_deinit();

	return 0;
}
