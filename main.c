#include <gst/gst.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	// Initialize GStreamer
	gst_init(&argc, &argv);

	GstElement *pipeline, *src, *sink, *dec, *filter;
	GstCaps *filtercaps;
	GstPad *pad;

	/* init GStreamer */
	// "v4l2src device=/dev/video0 ! jpegdec ! gdkpixbufoverlay offset-x=100 offset-y=100 overlay-width=500 location=back.jpg ! xvimagesink";

	/* build */
	pipeline = gst_pipeline_new("my-pipeline");
	src = gst_element_factory_make("v4l2src", "device=/dev/video0");
	if (src == NULL)
		g_error("Could not create 'videotestsrc' element");

	dec = gst_element_factory_make("jpegdec", "dec");

	filter = gst_element_factory_make("gdkpixbufoverlay", "filter");
	if (filter == NULL)
		g_error("Could not create 'videoconvert' element");

	g_assert(filter != NULL); /* should always exist */

	sink = gst_element_factory_make("xvimagesink", "sink");
	if (sink == NULL) {
		sink = gst_element_factory_make("ximagesink", "sink");
		if (sink == NULL)
			g_error("Could not create neither 'xvimagesink' nor 'ximagesink' element");
	}

	gst_bin_add_many(GST_BIN(pipeline), src, dec, filter, sink, NULL);
	gst_element_link_many(src, dec, filter, sink, NULL);
	g_object_set (G_OBJECT(filter), "offset-x", 100, NULL);
	g_object_set (G_OBJECT(filter), "offset-y", 100, NULL);
	g_object_set (G_OBJECT(filter), "location", "back.jpg", NULL);
	/*
	filtercaps = gst_caps_new_simple("video/x-raw",
							 "format", G_TYPE_STRING, "RGB16",
							 "width", G_TYPE_INT, 384,
							 "height", G_TYPE_INT, 288,
							 "framerate", GST_TYPE_FRACTION, 25, 1,
							 NULL);
	gst_caps_unref (filtercaps);
	*/

	/*
	pad = gst_element_get_static_pad (src, "src");
	gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER,
			(GstPadProbeCallback) cb_have_data, NULL, NULL);
	gst_object_unref (pad);
	*/

	fprintf(stderr, "%d\n", pipeline);

	// Set the pipeline to the PLAYING state
	GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Check if the pipeline state change was successful
	if (ret != GST_STATE_CHANGE_SUCCESS) {
		g_printerr("Failed to set pipeline state: %s\n", ret == GST_STATE_CHANGE_FAILURE ? "FAILURE" : "ERROR");
		fprintf(stderr, "%d\n", gst_error_get_message(0, ret));
		//gst_object_unref(pipeline);
		//return -1;
	}

	// Run the pipeline (wait for a signal)
	GstBus *bus = gst_element_get_bus(pipeline);
	GstMessage *message;
	GstStateChangeReturn state_change_ret;

	int i;
	for (i = 0;; i++) {
		message = gst_bus_timed_pop_filtered(bus, /*GST_CLOCK_TIME_NONE*/ 1000000, GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_ASYNC_DONE);
		if (message) {
			switch (GST_MESSAGE_TYPE(message)) {
				case GST_MESSAGE_ERROR:
					g_printerr("Error: %s\n", gst_error_get_message(GstCoreError, GST_MESSAGE_TYPE(message)));
					break;
				case GST_MESSAGE_EOS:
					g_printerr("End of Stream\n");
					break;
				case GST_MESSAGE_ASYNC_DONE:
					g_printerr("Async Done\n");
					break;
				default:
					g_printerr("Unknown message type\n");
					break;
			}
			gst_message_unref(message);
		}
		fprintf(stderr, "%d\n", i);
		if (i % 1000 == 0) {
			if ((i / 1000) % 2) { 
				g_object_set(G_OBJECT(filter), "offset-y", 200, NULL);
			} else {
				g_object_set(G_OBJECT(filter), "offset-y", 100, NULL);
			}
		}
	}

	// Set the pipeline to NULL state
	gst_element_set_state(pipeline, GST_STATE_NULL);

	// Free the pipeline
	gst_object_unref(pipeline);

	// Shut down GStreamer
	gst_deinit();

	return 0;
}