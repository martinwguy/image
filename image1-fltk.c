#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

int main(int argc, char *argv[]) {
   Fl_Window* w = new Fl_Window(330, 190, "image1-fltk");
   new Fl_Button(110, 130, 100, 35, "Okay");
   w->end();
   w->show(argc, argv);
   return Fl::run();
}
