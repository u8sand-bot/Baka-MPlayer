#ifndef MPVWIDGET_H
#define MPVWIDGET_H

#include <QOpenGLWidget>
#include <QImage>

#include <mpv/client.h>
#include <mpv/render_gl.h>

// Renders mpv video through libmpv's OpenGL render API.
// This replaces the old "wid" window embedding, which is unsupported on
// macOS and Wayland.
class MpvWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit MpvWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~MpvWidget();

    // attach to an initialized mpv handle (vo must be "libmpv")
    void Attach(mpv_handle *mpv);
    // free the render context; must be called before the mpv handle is destroyed
    void Detach();

    void setAlbumArt(bool show);
    bool albumArt() const { return showAlbumArt; }

protected:
    void initializeGL() override;
    void paintGL() override;

private slots:
    void maybeUpdate();

private:
    void createRenderContext();
    void freeRenderContext();
    void renderMpv();
    static void onUpdate(void *ctx);

    mpv_handle *mpv = nullptr;
    mpv_render_context *mpv_gl = nullptr;
    bool glReady = false;
    bool showAlbumArt = false;
    QImage albumArtImage;
};

#endif // MPVWIDGET_H
