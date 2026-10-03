#include "mpvwidget.h"

#include <QOpenGLContext>
#include <QPainter>
#include <QMetaObject>

static void *get_proc_address(void *ctx, const char *name)
{
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if(!glctx)
        return nullptr;
    return reinterpret_cast<void*>(glctx->getProcAddress(QByteArray(name)));
}

MpvWidget::MpvWidget(QWidget *parent, Qt::WindowFlags f):
    QOpenGLWidget(parent, f),
    albumArtImage(":/img/album-art.png")
{
}

MpvWidget::~MpvWidget()
{
    Detach();
}

void MpvWidget::Attach(mpv_handle *handle)
{
    mpv = handle;
    if(glReady)
        createRenderContext();
}

void MpvWidget::Detach()
{
    freeRenderContext();
    mpv = nullptr;
}

void MpvWidget::freeRenderContext()
{
    if(mpv_gl)
    {
        makeCurrent();
        mpv_render_context_free(mpv_gl);
        mpv_gl = nullptr;
        doneCurrent();
    }
}

void MpvWidget::setAlbumArt(bool show)
{
    if(showAlbumArt == show)
        return;
    showAlbumArt = show;
    update();
}

void MpvWidget::initializeGL()
{
    glReady = true;
    // the GL context is recreated when the widget is reparented or the top-level
    // window is recreated (e.g. changing window flags); mpv must follow it
    connect(context(), &QOpenGLContext::aboutToBeDestroyed,
            this, &MpvWidget::freeRenderContext, Qt::DirectConnection);
    if(mpv && !mpv_gl)
        createRenderContext();
}

void MpvWidget::createRenderContext()
{
    makeCurrent();
    mpv_opengl_init_params gl_init_params{get_proc_address, nullptr};
    mpv_render_param params[]{
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char*>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    if(mpv_render_context_create(&mpv_gl, mpv, params) < 0)
    {
        mpv_gl = nullptr;
        qWarning("failed to initialize mpv GL context");
    }
    else
        mpv_render_context_set_update_callback(mpv_gl, MpvWidget::onUpdate, this);
    doneCurrent();
}

void MpvWidget::paintGL()
{
    if(showAlbumArt || !mpv_gl)
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::black);
        if(showAlbumArt && !albumArtImage.isNull())
        {
            QRect r = albumArtImage.rect();
            r.moveCenter(rect().center());
            painter.drawImage(r, albumArtImage);
        }
        return;
    }
    renderMpv();
}

void MpvWidget::renderMpv()
{
    const qreal ratio = devicePixelRatioF();
    const int w = int(width() * ratio),
              h = int(height() * ratio);
    mpv_opengl_fbo mpfbo{static_cast<int>(defaultFramebufferObject()), w, h, 0};
    int flip_y{1};
    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    mpv_render_context_render(mpv_gl, params);
}

void MpvWidget::onUpdate(void *ctx)
{
    // called from an mpv thread; bounce to the GUI thread
    QMetaObject::invokeMethod(static_cast<MpvWidget*>(ctx), "maybeUpdate", Qt::QueuedConnection);
}

void MpvWidget::maybeUpdate()
{
    if(!mpv_gl)
        return;
    // skip frames when the window is hidden/minimized, but still let mpv
    // know it has been "rendered" so playback isn't blocked
    if(window()->isMinimized() || !isVisible())
    {
        if(!showAlbumArt)
        {
            makeCurrent();
            renderMpv();
            doneCurrent();
        }
    }
    else
        update();
}
