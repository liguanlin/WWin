#ifndef WOBJECT_H
#define WOBJECT_H

#include <windows.h>
#include "wwin/wstring.h"
#include <list>

#include "wwin/wevent.h"

namespace WObjectType {
    enum WObjectType{
        Object,
        Widget
    };
}

class WObject
{
private:
    WObject *_parent;
    WString _objectName;

    std::list<WObject> _childrens;

protected:

public:
    WObject(WObject *parent);
    virtual ~WObject();

    WObjectType::WObjectType _type = WObjectType::Object;

    void setObjectName(WString &name);

    void setType(WObjectType::WObjectType type);
    WObjectType::WObjectType type() const;

    void setParent(WObject* parent);
    WObject* parent() const;

    virtual bool event(WEvent *e);
//    virtual bool eventFilter(WObject *watched, WEvent *event);
    void removeEventFilter(WObject* obj);

    // WINAPI Processing
    virtual bool nativeEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // опции и рекурсивно
    WObject* findChild(const WString &name) const;
    std::list<WObject*> children() const;
    std::list<WObject*> findChildren(const WString &name) const;

//    WString tr(const char *sourceText, constChar *disambiguation = nullptr, int n = -1);
};

#endif // WOBJECT_H
