#ifndef MPVWIDGET_H
#define MPVWIDGET_H

#include <QImage>

#include <mpv/client.h>

// Shows mpv's video. Two modes, chosen at build time:
//  - default: renders through libmpv's OpenGL render API in a QOpenGLWidget
//    (needed on macOS and Wayland, where window embedding doesn't work)
//  - BAKA_MPV_WID: embeds mpv's own video window via "wid" (used on Windows,
//    where mpv then renders with Direct3D 11 and no OpenGL is required)
#ifdef BAKA_MPV_WID
#include <QWidget>
using MpvWidgetBase = QWidget;
#else
#include <QOpenGLWidget>
#include <mpv/render_gl.h>
using MpvWidgetBase = QOpenGLWidget;
#endif

class MpvWidget : public MpvWidgetBase
{
    Q_OBJECT
public:
    explicit MpvWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~MpvWidget();

    // set the video output options; must be called before mpv_initialize
    void Configure(mpv_handle *mpv);
    // attach to the initialized mpv handle
    void Attach(mpv_handle *mpv);
    // release mpv resources; must be called before the mpv handle is destroyed
    void Detach();

    void setAlbumArt(bool show);
    bool albumArt() const { return showAlbumArt; }

protected:
#ifdef BAKA_MPV_WID
    void paintEvent(QPaintEvent *event) override;
#else
    void initializeGL() override;
    void paintGL() override;
#endif

private slots:
    void maybeUpdate();

private:
    void paintAlbumArt();

#ifndef BAKA_MPV_WID
    void createRenderContext();
    void freeRenderContext();
    void renderMpv();
    static void onUpdate(void *ctx);

    mpv_render_context *mpv_gl = nullptr;
    bool glReady = false;
#endif
    mpv_handle *mpv = nullptr;
    bool showAlbumArt = false;
    QImage albumArtImage;
};

#endif // MPVWIDGET_H
