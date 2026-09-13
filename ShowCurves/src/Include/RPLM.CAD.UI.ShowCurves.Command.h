#pragma once

#include "RGPCommand.h"

#define RSCADUIW(key) Base::Framework::GetModuleResource(L##key, RPLM::Base::Framework::String(_STR("RPLM.CAD.UI.ShowCurves")))

namespace RPLM::CAD::UI
{
    /// <summary>Тестовая команда для вывода кривых</summary>
    class ShowCurvesCommand : public EP::UI::Command
    {
    public:
        /// <summary>Конструктор</summary>
        ShowCurvesCommand();

        /// <summary>Деструктор</summary>
        ~ShowCurvesCommand();

        /// <summary>Запуск (начало выполнения) команды</summary>
        /// <param name="parameters">Параметры инициализации команды</param>
        /// <returns>true в случае успешного запуска команды, иначе false</returns>
        virtual bool Start(EP::UI::StartCommandParameters& parameters) override;

        /// <summary>Завершение команды</summary>
        virtual void Finish() override;

        /// <summary>Возвращает диалог команды</summary>
        /// <returns>Диалог команды</returns>
        virtual EP::UI::ControlLayout* GetDialog() override;

        /// <summary>Получить идентификатор команды</summary>
        /// <returns>Идентификатор текущей команды</returns>
        virtual std::string GetID() override;

    private:
        /// <summary>Обрабатывает событие нажатия кнопки Ок</summary>
        void OnOK();

        /// <summary>Обрабатывает событие закрытия диалогового окна</summary>
        /// <returns>false</returns>
        bool OnCloseDialog();

        /// <summary>Обрабатывает событие изменения пути к файлу в элементах управления</summary>
        void OnFilePathChanged();

        /// <summary>Делает проверку на доступность кнопки Ок</summary>
        /// <returns>true в случае, если кнопка доступна, иначе false</returns>
        bool IsOkEnabled();

        /// <summary>Проверяет корректность пути к файлу</summary>
        /// <param name="filePath">Путь к файлу</param>
        /// <returns>true в случае, если путь корректен, иначе false</returns>
        bool IsFilePathValid(const Base::Framework::String& filePath);

        /// <summary>Отображает кривую на сцене</summary>
        /// <param name="curve">Кривая</param>
        /// <param name="drawByIntervals">True в случае, если кривую необходимо отобразить по частям - интервалам узлового вектора.
        /// Это может быть необходимо, в случае, если у кривой есть несколько кратных узлов. False в случае, если кривую необходимо отобразить за 1 интервал</param>
        /// <returns>Результат операции</returns>
        RGK::Result DrawCurve(const RGK::NURBSCurve& curve, bool drawByIntervals = true) const;

        // Диалоговое окно
        EP::UI::ControlLayout _dialog;
        // Элемент управления для выбора файла с исходными кривыми
        EP::UI::FileNameControl _sourceCurvesFilePath;
        // Элемент управления для отображения кол-ва кривых из файла 
        EP::UI::EditControl _numberOfCurvesFromFile;
        // Кривые, считанные из файла
        RGK::NURBSCurveVector _sourceCurves;
    };
}
