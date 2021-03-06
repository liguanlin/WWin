#include "wwidget.h"
#include "wwin/wapplication.h"
#include "wwin/helpers/winapiwindowbuilder.h"
#include "wwin/wscreen.h"

#include <iostream>

int WWidget::_componentCount = 0;

HWND WWidget::hwnd() const
{
    return _hwnd;
}

void WWidget::hwnd(HWND hwnd)
{
    _hwnd = hwnd;
}

HWND WWidget::parentHwnd() const
{
    HWND p_hwnd = HWND_DESKTOP;
    WWidget *parent = parentWidget();
    if( parent && parent->hwnd() ){
        p_hwnd = parent->hwnd();
    }
    return p_hwnd;
}

WWidget* WWidget::parentWidget() const
{
    WObject* parent = WObject::parent();
    if( parent && parent->type() == WObjectType::Widget ){
        return static_cast<WWidget*>( parent );
    }
    return nullptr;
}

WORD WWidget::cid() const
{
    return _cid;
}

int WWidget::style()
{
    return WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
}

bool WWidget::initWndClass(WString className)
{
    _className = className;
    HWND x = WinApiWindowBuilder()
         .className( _className )
         .title( this->title() )
         .style( this->style() )
         .geometry( _x, _y, _width, _height )
         .parent( this->parentHwnd() )
         .hinstance( wApp->getHinstance() )
         .param( reinterpret_cast<LPVOID>( _windowParams ) )
         .menu( reinterpret_cast<HMENU>( this->cid() ) )
         .build();
    this->hwnd(x);
    wApp->addComponent(this);

    return (x != nullptr);
}

bool WWidget::mouseReleaseEvent(WMouseEvent *e)
{
    return e->isAccepted();
}

bool WWidget::mouseDoubleClickEvent(WMouseEvent *e)
{
    return e->isAccepted();
}

bool WWidget::changeEvent(WEvent *e)
{
    return e->isAccepted();
}

bool WWidget::event(WEvent *e)
{
    if( e->type() == WEvent::Type::MouseReleaseEvent ){
        this->mouseReleaseEvent( static_cast<WMouseEvent*>(e) );
    }
    return WObject::event(e);
}

WWidget::WWidget(WWidget *parent, int params)
    : WObject(parent), _windowParams(params)
{
    _cid = _componentCount++;
    this->setType(WObjectType::Widget);

    if( parent == nullptr ) {
        this->initWndClass(L"WWIDGET");
    }
}

WWidget::~WWidget()
{
    wApp->removeComponent(this);
    DestroyWindow(this->hwnd());
}

void WWidget::show()
{
    if( ! this->hwnd() ) {
       this->initWndClass(L"WWIDGET");
    }
    ShowWindow( this->hwnd(), _windowParams );
    UpdateWindow( this->hwnd() );
}

void WWidget::setGeometry(int x, int y, int width, int height)
{
    _x = x;
    _y = y;

    // Можно проще
    if( width < _minWidth ) {
        width = _minWidth;
    } else if( width > _maxWidth ) {
        width = _maxWidth;
    }
    if( height < _minHeight ) {
        height = _minHeight;
    } else if( height > _maxHeight ) {
        height = _maxHeight;
    }

    _width = width;
    _height = height;

    SetWindowPos(this->hwnd(), nullptr, _x, _y, _width, _height, 0);

}

WString WWidget::title() const
{
    return _title;
}

void WWidget::setTitle(const WString &title)
{
    _title = title;
    SetWindowText(this->hwnd(), _title.c_str());
}

bool WWidget::nativeEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  if( message == WM_DESTROY ){
    PostQuitMessage( EXIT_SUCCESS );
    return true;
  }
  if(WM_SIZE == message || WM_MOVE == message) {
    RECT rect;
    if( ! GetWindowRect(hWnd, &rect) ){
      /// \todo process error
    }

    _x = rect.left;
    _y = rect.top;
    _width = rect.right-rect.left;
    _height = rect.bottom - rect.top;
    return true;
  }
  if (WM_PAINT) {
    /// \todo repaint something
  }

  return WObject::nativeEvent(hWnd, message, wParam, lParam);
}

void WWidget::setFocus()
{
    SetFocus( this->hwnd() );
}

