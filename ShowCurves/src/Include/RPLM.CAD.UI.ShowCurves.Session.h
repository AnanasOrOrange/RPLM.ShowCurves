#pragma once

#include "Common/RPLM.Base.Framework.String.h"

namespace RPLM::CAD::UI
{
    /// <summary>Сессия текущей команды</summary>
    class Session
    {
    public:
        Session(const Session&) = delete;
        Session(Session&&) = delete;
        Session& operator=(Session&&) = delete;
        Session& operator=(const Session&) = delete;

        /// <summary>Возвращает экземпляр сессии текущей команды</summary>
        /// <returns>Экземпляр сессии текущей команды</returns>
        static Session& Instance();

        /// <summary>Инициализирует сессию</summary>
        void Init();

        /// <summary>Возвращает название модуля</summary>
        /// <returns>Название модуля</returns>
        const Base::Framework::String& GetModuleName();

    private:
        /// <summary>Конструктор</summary>
        Session();

        /// <summary>Деструктор</summary>
        ~Session();
    };
}
