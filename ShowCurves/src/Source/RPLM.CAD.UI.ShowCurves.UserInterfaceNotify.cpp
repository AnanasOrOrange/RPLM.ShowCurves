#pragma once

#include "RPLM.CAD.UI.ShowCurves.Command.h"
#include "RPLM.CAD.UI.ShowCurves.UserInterfaceNotify.h"
#include "RPLM.CAD.UI.ShowCurves.SessionNotify.h"
#include "RPLM.CAD.UI.ShowCurves.Session.h"
#include "RPLM.Base.Framework.Session.h"
#include "RPLM.Shell.UI.Session.h"
#include "RGPUIData.h"

namespace RPLM::Shell::UI
{
    RPLM_APP void CreateCommand(ToolContext* context)
    {
        if (auto uiData = EP::UI::RGPUIData::GetUIData())
            uiData->AddCommand(new CAD::UI::ShowCurvesCommand(), context);
    }

    RPLM_APP bool InitApplication(Base::Framework::ApplicationInitContext* context)
    {
        if (auto* applicationSession = &CAD::UI::Session::Instance())
        {
            applicationSession->Init();
            context->GetApplcation()->SetID(applicationSession->GetModuleName());

            // Регистрация обработчика уведомлений от сессии базового приложения
            if (auto* platformSession = Base::Framework::Session::GetSession())
            {
                platformSession->ConnectSessionNotifyInterface(CAD::UI::SessionNotify::Instance());

                // Регистрация обработчика уведомлений пользовательского интерфейса базового модуля
                if (auto* uiSession = Shell::UI::Session::GetSession())
                {
                    uiSession->ConnectInterfaceNotify(CAD::UI::UserInterfaceNotify::Instance());

                    return true;
                }
            }
        }

        return false;
    }

    RPLM_APP void EnableCommandUpdater(ToolContext* context)
    {
        context->GetTool()->SetEnabled(true);
    }
}

namespace RPLM::CAD::UI
{
    UserInterfaceNotify::UserInterfaceNotify()
        : IInterfaceNotify(false)
    {
    }

    UserInterfaceNotify* UserInterfaceNotify::Instance()
    {
        static UserInterfaceNotify _notify;
        return &_notify;
    }

    void UserInterfaceNotify::InitUI(Shell::UI::InitUIContext& context)
    {
        if (auto* session = context.GetUISession())
        {
            auto toolID = _STR("CAD.Modeling.Optional.ShowCurves");

            // Если команда не добавлена в сессию
            if (!session->FindTool(toolID))
            {
                auto tool = new Shell::UI::Tool(toolID, RSCADUIW("CAD.Modeling.Optional.ShowCurves"), toolID, &Shell::UI::CreateCommand, &Shell::UI::EnableCommandUpdater);
                session->AddTool(tool);
            }

            if (auto* category = session->GetToolbarsDefinition().GetRibbon().GetCategory(static_cast<int>(RibbonPanel::Model)))
            {
                auto groupID = _STR("CAD.Modeling.Optional");

                // Если в категории нет группы
                if (!category->GetGroup(groupID))
                {
                    // Добавляем группу в категорию
                    auto* group = category->AddGroup(groupID, RSCADUIW("CAD.Modeling.Optional"));

                    // Если в группе нет команды
                    if (group && !group->GetItem(toolID, false))
                    {
                        // Добавляем команду в группу
                        group->AddItem(Shell::UI::ToolbarItem(toolID));
                    }
                }
            }
        }
    }

    bool UserInterfaceNotify::MainWindowClosing(const Shell::UI::MainWindowContext& context)
    {
        if (auto* uiSession = context.GetUISession())
        {
            uiSession->DisconnectInterfaceNotify(CAD::UI::UserInterfaceNotify::Instance());
            return true;
        }

        return false;
    }
}
