#include <Python.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <Imlib2.h>

/* X code adapted from hsetroot */

enum image_placement {
    image_placement_fill,
#if XROOT_EXTENDED_IMAGE_PLACEMENT
    image_placement_full,
    image_placement_center,
    image_placement_tile
#endif
};

// Adapted from fluxbox' bsetroot
static int
xroot_set_atoms(Display *display,
                int screen,
                Pixmap pixmap)
{
    Atom atom_root, atom_eroot, type;
    unsigned char *data_root, *data_eroot;
    int format;
    unsigned long length, after;

    atom_root = XInternAtom(display, "_XROOTMAP_ID", True);
    atom_eroot = XInternAtom(display, "ESETROOT_PMAP_ID", True);

    // doing this to clean up after old background
    if (atom_root != None && atom_eroot != None) {
        XGetWindowProperty(display, RootWindow(display, screen),
                           atom_root, 0L, 1L, False,
                           AnyPropertyType,
                           &type, &format, &length,
                           &after, &data_root);
        if (type == XA_PIXMAP) {
            XGetWindowProperty(display, RootWindow(display, screen),
                               atom_eroot, 0L, 1L, False, AnyPropertyType,
                               &type, &format, &length, &after, &data_eroot);
            if (data_root && data_eroot && type == XA_PIXMAP &&
                *((Pixmap *)data_root) == *((Pixmap *)data_eroot)) {
                XKillClient(display, *((Pixmap *)data_root));
            }
        }
    }

    atom_root = XInternAtom (display, "_XROOTPMAP_ID", False);
    atom_eroot = XInternAtom (display, "ESETROOT_PMAP_ID", False);

    if (atom_root == None || atom_eroot == None)
        return -1;

    // setting new background atoms
    XChangeProperty(display, RootWindow(display, screen),
                    atom_root, XA_PIXMAP, 32, PropModeReplace,
                    (unsigned char *)&pixmap, 1);
    XChangeProperty(display, RootWindow(display, screen),
                    atom_eroot, XA_PIXMAP, 32, PropModeReplace,
                    (unsigned char *)&pixmap, 1);

    return 0;
}


static int
xroot_load_image(enum image_placement placement,
                 const char *path,
                 int rootW,
                 int rootH,
                 int alpha,
                 Imlib_Image rootimg)
{
    int imgW, imgH;
    Imlib_Image buffer;

    if (!(buffer = imlib_load_image(path))) {
        return -1;
    }

    imlib_context_set_image(buffer);

    imgW = imlib_image_get_width(),
        imgH = imlib_image_get_height();

#if XROOT_EXTENDED_ALPHA
    if (alpha < 255) {
        int i;
        // Create alpha-override mask
        imlib_image_set_has_alpha(1);
        Imlib_Color_Modifier modifier = imlib_create_color_modifier();
        imlib_context_set_color_modifier(modifier);

        DATA8 r[256], g[256], b[256], a[256];
        imlib_get_color_modifier_tables(r, g, b, a);
        for (i = 0; i<256; ++i)
            a[i] = (DATA8) alpha;
        imlib_set_color_modifier_tables(red, green, blue, a);

        imlib_apply_color_modifier();
        imlib_free_color_modifier();
    }
#endif

    imlib_context_set_image(rootimg);
    if (placement == image_placement_fill) {
        imlib_blend_image_onto_image(buffer, 0, 0, 0, imgW, imgH,
                                     0, 0, rootW, rootH);
    }
#if XROOT_EXTENDED_IMAGE_PLACEMENT
    else if (placement == image_placement_full) {
        double aspect = ((double) rootW) / imgW;
        int top, left;
        if ((int) (imgH * aspect) > rootH)
            aspect = (double) rootH / (double) imgH;
        top = (rootH - (int) (imgH * aspect)) / 2;
        left = (rootW - (int) (imgW * aspect)) / 2;
        imlib_blend_image_onto_image(buffer, 0, 0, 0, imgW, imgH,
                                      left, top, (int) (imgW * aspect),
                                      (int) (imgH * aspect));
    } else {
        int left = (rootW - imgW) / 2, top = (rootH - imgH) / 2;

        if (placement == image_placement_tile) {
            int x, y;
            for (; left > 0; left -= imgW);
            for (; top > 0; top -= imgH);

            for (x = left; x < rootW; x += imgW)
                for (y = top; y < rootH; y += imgH)
                    imlib_blend_image_onto_image(buffer, 0, 0, 0, imgW, imgH,
                                                  x, y, imgW, imgH);
        } else {
            imlib_blend_image_onto_image(buffer, 0, 0, 0, imgW, imgH,
                                         left, top, imgW, imgH);
        }
    }
#endif

    imlib_context_set_image(buffer);
    imlib_free_image();

    imlib_context_set_image(rootimg);

    return 0;
}

static PyObject *
xroot_set(PyObject *self, PyObject *args)
{
    Display *display;
    Visual *visual;
    Colormap colormap;
    Imlib_Context *context;
    Imlib_Image image;
    int width, height, depth, i;
    Pixmap pixmap;

    /* args */
    int alpha = 255;
    enum image_placement placement = image_placement_fill;
    const char *path;

    int screen, screen_count = ScreenCount(XOpenDisplay(NULL));

    if (!PyArg_ParseTuple(args, "s", &path))
        return NULL;

    display = XOpenDisplay(NULL);

    for (screen=0; screen<screen_count; ++screen) {
        context = imlib_context_new();
        imlib_context_push(context);

        imlib_context_set_display(display);

        visual = DefaultVisual(display, screen);
        colormap = DefaultColormap(display, screen);
        width = DisplayWidth(display, screen);
        height = DisplayHeight(display, screen);
        depth = DefaultDepth(display, screen);

        pixmap = XCreatePixmap(display,
                               RootWindow(display, screen),
                               width, height,
                               depth);

        imlib_context_set_visual(visual);
        imlib_context_set_colormap(colormap);
        imlib_context_set_drawable(pixmap);
        imlib_context_set_color_range(imlib_create_color_range());

        image = imlib_create_image(width, height);
        imlib_context_set_image(image);

        imlib_context_set_dither(1);
        imlib_context_set_blend(1);

        if (xroot_load_image(placement, path, width, height, alpha, image) != 0)
            ;                   /* FIXME: error handling */

        imlib_render_image_on_drawable(0, 0);
        imlib_free_image();
        imlib_free_color_range();

        if (xroot_set_atoms(display, screen, pixmap) != 0)
            ;                   /* FIXME: error handling */

        XKillClient(display, AllTemporary);
        XSetCloseDownMode(display, RetainTemporary);

        XSetWindowBackgroundPixmap(display,
                                   RootWindow(display, screen),
                                   pixmap);
        XClearWindow(display, RootWindow(display, screen));

        XFlush(display);
        XSync(display, False);

        imlib_context_pop();
        imlib_context_free(context);
    }

    return Py_BuildValue("i", 0);
}

static PyMethodDef xroot_methods[] = {
    {"set",  xroot_set, METH_VARARGS, "Set X root."},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef xroot_module = {
	PyModuleDef_HEAD_INIT,
	"xroot",   /* name of module */
	NULL, /* module documentation, may be NULL */
	-1,       /* size of per-interpreter state of the module,
				or -1 if the module keeps state in global variables. */

	xroot_methods
};

PyMODINIT_FUNC
PyInit_xroot(void)
{
    return PyModule_Create(&xroot_module);
}
