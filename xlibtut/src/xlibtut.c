#include <X11/X.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef uint32_t u32;

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

    int default_root_window = DefaultRootWindow(display); // We are making a normal "top level" window,
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
    XMapWindow(display, window);
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
            case ButtonPress:
            {
                XButtonEvent* button_event = &event.xbutton;
                button_event->
            }
            }
        }
    }
}
