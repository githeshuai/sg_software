#ifndef DialogManagerX11_h__
#define DialogManagerX11_h__

#include <boost/noncopyable.hpp>
#include <string>

#include "../DialogManager.h"

class DialogManagerX11 : public DialogManager
{
public:
    void OpenFolderDialog(const FB::BrowserHostPtr& host, FB::PluginWindow* win, bool multi, const PathCallback& cb);
    void _showFolderDialog(FB::PluginWindow* win, bool multi, const PathCallback& cb);

protected:
    DialogManagerX11() {};
    ~DialogManagerX11() {};
    friend class DialogManager;
};

#endif // DialogManagerX11_h__