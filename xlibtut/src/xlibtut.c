#include <X11/X.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef uint32_t u32;

void set_size_hint(Display* display, Window window, int min_width, int min_height, int max_width, int max_height);
Status toggle_maximize_window(Display* display, Window window, Window default_root_window);

void set_size_hint(Display* display, Window window, int min_width, int min_height, int max_width, int max_height)
{
    XSizeHints hints = {};
    if (min_width && min_height) hints.flags |= PMinSize;
    if (max_width && max_height) hints.flags |= PMaxSize;

    hints.min_width = min_width;
    hints.min_height = min_height;
    hints.max_width = max_width;
    hints.max_height = max_height;

    XSetNormalHints(display, window, &hints);
}

Status toggle_maximize_window(Display* display, Window window, Window default_root_window) {
    XClientMessageEvent event = {};
    Atom wm_state = XInternAtom(display, "_NET_WM_STATE", True);
    Atom state_toggle = XInternAtom(display, "_NET_WM_STATE_TOGGLE", True);
    Atom maximized_horizontal = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", True);
    Atom maximized_vertical = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", True);

    if (wm_state == None) {
        return 0;
    }

    event.type = ClientMessage;
    event.format = 32;
    event.window = window;
    event.message_type = wm_state;
    event.data.l[0] = state_toggle;
    event.data.l[1] = maximized_horizontal;
    event.data.l[2] = maximized_vertical;
    event.data.l[3] = 1; // 1 for normal applications

    return XSendEvent(display, default_root_window, False, SubstructureNotifyMask, (XEvent*)&event);
}


int main(int argc, char **args)
{
    int screen_width = 800;
    int screen_height = 600;

    Display *display = XOpenDisplay(0);
    if (!display)
    {
        fprintf(stderr, "%s:%d: XOpenDisplay failed\n", __FILE__, __LINE__);
        exit(1);
    }

    Window default_root_window = DefaultRootWindow(display); // We are making a normal "top level" window,
                                                          // so our parent needs to be the "desktop", this is what the root window is.
    int default_screen = DefaultScreen(display);

    int screen_bit_depth = 24; // RGB (8+8+8)
    XVisualInfo visual_info = {};
    if (!XMatchVisualInfo(display, default_screen, screen_bit_depth, TrueColor, &visual_info))
    {
        fprintf(stderr, "%s:%d: XMatchVisualInfo failed\n", __FILE__, __LINE__);
        exit(1);
    }

    XSetWindowAttributes window_attributes;
    window_attributes.background_pixel = 0;
    window_attributes.colormap = XCreateColormap(display, default_root_window, visual_info.visual, AllocNone);
    window_attributes.event_mask = StructureNotifyMask;
    u32 attribute_mask = CWBackPixel | CWColormap | CWEventMask;

    Window window = XCreateWindow(display, default_root_window, 0, 0, screen_width, screen_height, 0, visual_info.depth,
                                  InputOutput, visual_info.visual, attribute_mask, &window_attributes);
    if (!window)
    {
        fprintf(stderr, "%s:%d: XCreateWindow failed\n", __FILE__, __LINE__);
        exit(1);
    }

    XStoreName(display, window, "Hello World");
    set_size_hint(display, window, 400, 300, 0, 0);
    XMapWindow(display, window);
    toggle_maximize_window(display, window, default_root_window);
    XFlush(display);

    int running = 1;
    XEvent event;
    while (running)
    {
        while (XPending(display))
        {
            XNextEvent(display, &event);
            switch (event.type)
            {
            case DestroyNotify:
            {
                if (event.xdestroywindow.window == window)
                {
                    running = 0;
                }
            }
            break;
            }
        }
    }
}
