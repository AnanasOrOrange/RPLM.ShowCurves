#include "RPLM.CAD.UI.ShowCurves.Session.h"
#include "Common/RPLM.Base.Framework.ResourceDefs.h"

namespace RPLM::CAD::UI
{
    Session::Session()
    {
    }

    Session::~Session()
    {
    }

    Session& Session::Instance()
    {
        static Session session;
        return session;
    }

    void Session::Init()
    {
        Base::Framework::LoadResourcesForCurrentModule(GetModuleName().c_str());
    }

    const Base::Framework::String& Session::GetModuleName()
    {
        static Base::Framework::String name(_STR("RPLM.CAD.UI.ShowCurves"));
        return name;
    }
}