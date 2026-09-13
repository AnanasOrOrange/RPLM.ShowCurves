#pragma once

#include "RPLM.CAD.UI.ShowCurves.SessionNotify.h"
#include "RPLM.Base.Framework.Session.h"

namespace RPLM::CAD::UI
{
    SessionNotify* SessionNotify::Instance()
    {
        static SessionNotify _notify;
        return &_notify;
    }

    SessionNotify::SessionNotify()
        : ISessionNotify(false)
    {
    }

    void SessionNotify::BeforeClosePlatform(const Base::Framework::BeforeClosePlatformContext& context)
    {
        // Отписываемся от уведомлений базового модуля
        if (auto* session = Base::Framework::Session::GetSession())
            session->DisconnectSessionNotifyInterface(SessionNotify::Instance());
    }
}
