#include <X11/X.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef uint8_t  u8;
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

    int pixel_bits = 32;
    int pixel_bytes = pixel_bits/8;
    int window_buffer_size = screen_width * screen_height * pixel_bytes;
    int pitch = screen_width * pixel_bytes;
    char* buffer = malloc(window_buffer_size);
    if (!buffer) {
        fprintf(stderr, "%s:%d: malloc failed\n", __FILE__, __LINE__);
        exit(1);
    }

    XImage* x_window_buffer = XCreateImage(display, visual_info.visual, visual_info.depth, ZPixmap, 0, buffer, screen_width, screen_height, pixel_bits, 0);
    GC default_GC = DefaultGC(display, default_screen);


    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", True);
    if (!XSetWMProtocols(display, window, &wm_delete_window, 1)) {
        fprintf(stderr, "%s:%d: Couldn't register WM_DELETE_WINDOW property\n", __FILE__, __LINE__);
        exit(1);
    }



    int running = 1;
    int size_change = 0;
    XEvent event;
    int x_offset = 0;
    int y_offset = 0;
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
            case ClientMessage:
            {
                XClientMessageEvent* message_event = (XClientMessageEvent*)&event;
                if (message_event->data.l[0] == wm_delete_window) {
                    XDestroyWindow(display, window);
                    running = 0;
                }
            }
            break;
            }
        }

        u8* row = (u8*) buffer;
        for (int y = 0; y < screen_height; y++) {

            u32* pixel = (u32*) row;
            for (int x = 0; x < screen_width; x++) {
                u8 red = x + x_offset;
                u8 green = y + y_offset;
                u8 blue = 0x00;
                u8 alpha = 0xFF;
                *pixel = alpha << 24 | red << 16| green << 8| blue;
                pixel++;
            }
            row += pitch;
        }

        x_offset++;
        y_offset++;

        XPutImage(display, window, default_GC, x_window_buffer, 0, 0, 0, 0, screen_width, screen_height);

    }
}
