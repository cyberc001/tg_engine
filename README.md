## TG Engine
is a terminal graphics engine that aims to provide simple but exposed and flexible interface for rendering graphical objects in terminal interfaces. It provides:
* Functions to change each individual cell: set the character it holds (supporting Unicode!), set it's formatting (bold/italic/underlined/foreground color/background color)
* An easy way of handling resizing the terminal (it's just an option you have to enable while initializing terminal screen!)
* Functional UI primitives such as windows and control elements.
* Input handling (setting up input that is not buffered on newline feed, handling Unicode characters and some escape sequences)
* Configuration handling (featuring custom configuration format), which is used for binds in UI elements

Windows in TG Engine have following features:
* Have borders that render their number in linked list of windows, current modes and titles.
* Can be toggled between them. Only the currently active window recieves input; It is also highlighted (if it has a border).
* Can be moved. This is a separate mode (apart from normal and resize modes), binds for entering this mode and moving the window should be defined in a configuration file.
* Can be resized. This is also a separate mode that uses binds. A window can be resized-to-fit with a separate key.

#### Compatibility
For now, some features that are specific to many Linux terminals are used (such as SIGWINCH and ioctl) are used, but there are plans to make it compatible with PowerShell and wider variety of terminals. Don't open a feature request for it, please: it is planned.

Currently tested in:
|Terminal|Status|
|---|---|
|Alacritty|Works|

#### Building
Run the provided makefile:
```
make
```
to get file named `libtg_engine.a`. This is a static library file you can can link against.
Don't forget to add `-fms-extensions` flag when using TG Engine headers.
Building wasn't tested on more than one system, but it uses basically nothing than **cstdlib**, **unistd.h** and **sys/ioctl.h**.

#### Usage
For now, refer to the `example/` directory to get a hang to how use the library (especially for keybind configuration).
