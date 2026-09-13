#pragma once

#include "Applications/RPLM.Base.Framework.Application.h"
#include "RPLM.Shell.UI.Notify.h"

namespace RPLM::Shell::UI
{
    /// <summary>Создаёт команду</summary>
    /// <param name="iContext">Контекст выполнения элемента пользовательского интерфейса</param>
    RPLM_APP void CreateCommand(ToolContext* context);

    /// <summary>Инициализирует команду</summary>
    /// <param name="context">Контекст инициализации</param>
    /// <returns>true в случае, если приложение успешно инициализировано, иначе false</returns>
    RPLM_APP bool InitApplication(Base::Framework::ApplicationInitContext* context);

    /// <summary>Устанавливает доступность команды в меню</summary>
    /// <param name="iContext">Контекст выполнения элемента пользовательского интерфейса</param>
    RPLM_APP void EnableCommandUpdater(ToolContext* iContext);
}

namespace RPLM::CAD::UI
{
    // <summary>Класс уведомления от пользовательского интерфейса базового приложения</summary>
    class UserInterfaceNotify final : public Shell::UI::IInterfaceNotify
    {
    public:
        /// <summary>Возвращает экземпляр сессии</summary>
        /// <returns>Экземпляр сессии</returns>
        static UserInterfaceNotify* Instance();

        // Перечисление группы панели приложений
        enum class RibbonPanel
        {
            Model = 1
        };

    private:
        /// <summary>Конструктор</summary>
        UserInterfaceNotify();

        /// <summary>Инициализирует команду в приложении</summary>
        /// <param name="context">Контекст интерфейса</param>
        void InitUI(Shell::UI::InitUIContext& context) override;

        /// <summary>Запрашивает разрешение на закрытие главного окна</summary>
        /// <param name="context">Контекст уведомления</param>
        /// <returns>true в случае, если главное окно приложения можно закрыть, false если окно закрыть нельзя</returns>
        bool MainWindowClosing(const Shell::UI::MainWindowContext& context) override;
    };
}
