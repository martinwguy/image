/*
 * image1-qt4: GUI toolkit test piece to display an image file.
 *
 * The image file is given as a command-line argument (default: image.jpg).
 * The window should open to exactly fit the image at one-pixel-per-pixel size.
 * The user can then resize the window in which case the image scales to fit
 * the window without keeping its aspect ratio.
 * If they hit Control-Q or poke the [X] icon in the window's titlebar,
 * the application should quit.
 *
 * Bugs:
 *    - The image is not scaled to the window: it is centered or has scrollbars.
 *
 *	Martin Guy <martinwguy@gmail.com>, November 2016.
 */

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsPixmapItem item(QPixmap ((argc > 1) ? argv[1] : "image.jpg"));

    scene.addItem(&item);
    view.show();
    return a.exec();
}
