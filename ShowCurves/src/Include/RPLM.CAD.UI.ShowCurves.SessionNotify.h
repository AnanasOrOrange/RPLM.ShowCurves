#pragma once

#include "Notifications/RPLM.Base.Framework.SessionNotify.h"

namespace RPLM::CAD::UI
{
    // <summary>Класс уведомления от сессии базового приложения</summary>
    class SessionNotify final : public Base::Framework::ISessionNotify
    {
    public:
        /// <summary>Возвращает экземпляр сессии</summary>
        /// <returns>Экземпляр сессии</returns>
        static SessionNotify* Instance();

    private:
        /// <summary>Конструктор</summary>
        SessionNotify();

        /// <summary>Уведомляет приложение перед завершением платформы</summary>
        /// <param name="context">Контекст уведомления о событии</param>
        void BeforeClosePlatform(const Base::Framework::BeforeClosePlatformContext& context) override;
    };
}
