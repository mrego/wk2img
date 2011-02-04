#include <gtk/gtk.h>
#include <webkit/webkit.h>

static cairo_surface_t *
gdk_pixbuf_to_cairo_surface (GdkPixbuf *pixbuf)
{
  static const cairo_user_data_key_t key;
  gint width;
  gint height;
  guchar *gdk_pixels;
  int gdk_rowstride;
  int n_channels;
  int cairo_stride;
  guchar *cairo_pixels;
  cairo_format_t format;
  cairo_surface_t *surface;
  int j;

  g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), NULL);

  width = gdk_pixbuf_get_width (pixbuf);
  height = gdk_pixbuf_get_height (pixbuf);
  gdk_pixels = gdk_pixbuf_get_pixels (pixbuf);
  gdk_rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  n_channels = gdk_pixbuf_get_n_channels (pixbuf);

  if (n_channels == 3)
    format = CAIRO_FORMAT_RGB24;
  else
    format = CAIRO_FORMAT_ARGB32;

  cairo_stride = cairo_format_stride_for_width (format, width);
  cairo_pixels = g_malloc (height * cairo_stride);
  surface = cairo_image_surface_create_for_data ((unsigned char *)cairo_pixels,
                                                 format,
                                                 width, height, cairo_stride);

  cairo_surface_set_user_data (surface, &key,
			       cairo_pixels, (cairo_destroy_func_t)g_free);

  for (j = height; j; j--)
    {
      guchar *p = gdk_pixels;
      guchar *q = cairo_pixels;

      if (n_channels == 3)
	{
	  guchar *end = p + 3 * width;
	  
	  while (p < end)
	    {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
	      q[0] = p[2];
	      q[1] = p[1];
	      q[2] = p[0];
#else	  
	      q[1] = p[0];
	      q[2] = p[1];
	      q[3] = p[2];
#endif
	      p += 3;
	      q += 4;
	    }
	}
      else
	{
	  guchar *end = p + 4 * width;
	  guint t1,t2,t3;
	    
#define MULT(d,c,a,t) G_STMT_START { t = c * a + 0x7f; d = ((t >> 8) + t) >> 8; } G_STMT_END

	  while (p < end)
	    {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
	      MULT(q[0], p[2], p[3], t1);
	      MULT(q[1], p[1], p[3], t2);
	      MULT(q[2], p[0], p[3], t3);
	      q[3] = p[3];
#else	  
	      q[0] = p[3];
	      MULT(q[1], p[0], p[3], t1);
	      MULT(q[2], p[1], p[3], t2);
	      MULT(q[3], p[2], p[3], t3);
#endif
	      
	      p += 4;
	      q += 4;
	    }
	  
#undef MULT
	}

      gdk_pixels += gdk_rowstride;
      cairo_pixels += cairo_stride;
    }

  return surface;
}

static void
status_cb (WebKitWebView *view, GParamSpec *spec, GtkWidget *window)
{
    GdkPixbuf *p;
    cairo_surface_t *s;
    WebKitLoadStatus status;

    status = webkit_web_view_get_load_status (view);
    if (status == WEBKIT_LOAD_FINISHED) {
        GtkRequisition req;
        GtkAllocation alloc;
        gtk_widget_size_request (GTK_WIDGET (view), &req);
        alloc.x = alloc.y = 0;
        alloc.width = req.width;
        alloc.height = req.height;
        g_debug ("ALLOCATE %d x %d", alloc.width, alloc.height);
        gtk_widget_size_allocate (GTK_WIDGET (view), &alloc);

        while (gtk_events_pending ())
            gtk_main_iteration_do (FALSE);

        p = gtk_offscreen_window_get_pixbuf (GTK_OFFSCREEN_WINDOW (window));
        s = gdk_pixbuf_to_cairo_surface (p);
        cairo_surface_write_to_png (s, "foo.png");

        gtk_main_quit ();
    }
}

int main(int argc, char* argv[])
{
    GtkWidget *window, *view;

    if (argc < 2)
        return 1;

    gtk_init(&argc, &argv);

    view = webkit_web_view_new ();
    window = gtk_offscreen_window_new ();
    gtk_container_add (GTK_CONTAINER (window), view);
    g_debug ("Loading ... %s", argv[1]);
    webkit_web_view_load_uri (WEBKIT_WEB_VIEW (view), argv[1]);
    g_signal_connect (view, "notify::load-status", G_CALLBACK (status_cb), window);

    gtk_widget_show_all (window);

    gtk_main();

    return 0;
}
