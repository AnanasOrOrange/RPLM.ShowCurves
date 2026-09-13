#include "RPLM.CAD.UI.ShowCurves.Command.h"
#include "RPLM.CAD.UI.ShowCurves.CurveParser.h"
#include "Representations/RGPPresentationContexts.h"
#include "Representations/RGPModelScene.h"
#include "Objects/RGPBodyObject.h"
#include "Generators/BodyConstructor.h"
#include "RGPSession.h"

namespace RPLM::CAD::UI
{
    ShowCurvesCommand::ShowCurvesCommand() :
        _sourceCurvesFilePath(_STR("RPLM.CAD.UI.ShowCurvesCommand.SourceCurvesFilePath"), RSCADUIW("RPLM.CAD.UI.ShowCurvesCommand.SourceCurvesFilePath")),
        _numberOfCurvesFromFile(_STR("RPLM.CAD.UI.ShowCurvesCommand.NumberOfCurvesFromFile"), RSCADUIW("RPLM.CAD.UI.ShowCurvesCommand.NumberOfCurvesFromFile"))
    {
        _dialog.SetTitle(RSCADUIW("RPLM.CAD.UI.ShowCurvesCommand.ShowCurves"));

        AddOkToDialog(&_dialog);
        AddCancelToDialog(&_dialog);

        _dialog.AddControl(_sourceCurvesFilePath);
        _dialog.AddControl(_numberOfCurvesFromFile);
        _numberOfCurvesFromFile.SetReadOnly(true);
        _numberOfCurvesFromFile.SetPlaceholderText(_STR("0"));

        _ok.PressEvent = std::bind(&ShowCurvesCommand::OnOK, this);
        _dialog.OnCloseEvent = std::bind(&ShowCurvesCommand::OnCloseDialog, this);
        _sourceCurvesFilePath.LinkChanged = std::bind(&ShowCurvesCommand::OnFilePathChanged, this);
    }

    ShowCurvesCommand::~ShowCurvesCommand()
    {}

    bool ShowCurvesCommand::Start(EP::UI::StartCommandParameters& parameters)
    {
        if (!Command::Start(parameters))
            return false;

        CreateCommandDialog(_dialog, GetMainWindow(), GetDocument());
        _dialog.NeedToAdjust();
        _dialog.Show();

        _ok.SetEnabled(IsOkEnabled());

        return true;
    }

    void ShowCurvesCommand::Finish()
    {
        _dialog.Destroy();
        Command::Finish();
    }

    EP::UI::ControlLayout* ShowCurvesCommand::GetDialog()
    {
        return &_dialog;
    }

    std::string ShowCurvesCommand::GetID()
    {
        return "RPLM.CAD.UI.ShowCurves";
    }

    void ShowCurvesCommand::OnOK()
    {
        if (_sourceCurves.empty())
        {
            EP::UI::Command::Alert(RSCADUIW("RPLM.CAD.UI.ShowCurvesCommand.ErrorEmptyCurveContainer"), AlertType::Error);
            Terminate();
            return;
        }

        for (const auto& curve : _sourceCurves)
        {
            if (RGK::Failed(DrawCurve(curve)))
            {
                EP::UI::Command::Alert(RSCADUIW("RPLM.CAD.UI.ShowCurvesCommand.ErrorDrawCurve"), AlertType::Error);
                Terminate();
                return;
            }
        }

        Terminate();
    }

    bool ShowCurvesCommand::OnCloseDialog()
    {
        Terminate();
        return false;
    }

    void ShowCurvesCommand::OnFilePathChanged()
    {
        _ok.SetEnabled(IsOkEnabled());

        if (!IsOkEnabled())
        {
            _numberOfCurvesFromFile.SetPlaceholderText(_STR("0"));
            return;
        }

        auto sourceCurvesFilePath = _sourceCurvesFilePath.GetFullName();

        if (sourceCurvesFilePath.empty())
        {
            EP::UI::Command::Alert(_STR("Пустой путь к файлу."), AlertType::Error);
            return;
        }

        auto result = CurveParser::ReadCurvesFromFile(sourceCurvesFilePath, _sourceCurves);

        if (result.codeError == CurveParser::ErrorType::SuccessReadFile)
        {
            _numberOfCurvesFromFile.SetPlaceholderText(std::to_wstring(_sourceCurves.size()));
        }
        else
        {
            std::string errorMessage = "";

            switch (result.codeError)
            {
            case CurveParser::ErrorType::ErrorOpenFile:
                errorMessage = "Ошибка открытия файла.";
                break;
            case CurveParser::ErrorType::UnexpectedBlock:
                errorMessage = "Встречен неожиданный/неизвестный блок (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::IncorrectFormatBlockDegree:
                errorMessage = "Некорректный формат блока Degree (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::IncorrectFormatBlockControlPoints:
                errorMessage = "Некорректный формат блока Control Points (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::IncorrectNumberExpectControlPoints:
                errorMessage = "Некорректное количество ожидаемых контрольных точек (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::ErrorCoordinateControlPoint:
                errorMessage = "Ошибка чтения координат контрольной точки (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::NumberActualControlPointsNotMatchExpect:
                errorMessage = "Количество контрольных точек не совпадает с ожидаемым (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::IncorrectFormatBlockWeights:
                errorMessage = "Некорректный формат блока Weights (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::IncorrectNumberExpectWeights:
                errorMessage = "Некорректное количество ожидаемых весовых коэффициентов (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::ErrorWeight:
                errorMessage = "Ошибка чтения весового коэффициента (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::NumberActualWeightsNotMatchExpect:
                errorMessage = "Количество весовых коэффициентов не совпадает с ожидаемым (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::IncorrectFormatBlockKnots:
                errorMessage = "Некорректный формат блока Knots (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::IncorrectNumberExpectKnots:
                errorMessage = "Некорректное количество ожидаемых узловых коэффициентов (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::ErrorKnot:
                errorMessage = "Ошибка чтения узлового коэффициента (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            case CurveParser::ErrorType::NumberActualKnotsNotMatchExpect:
                errorMessage = "Количество узловых коэффициентов не совпадает с ожидаемым (Строка " + std::to_string(result.lineNumber) + ").";
                break;
            }

            EP::UI::Command::Alert(Base::Framework::ConvertStringToWstring(errorMessage), AlertType::Error);
            _numberOfCurvesFromFile.SetPlaceholderText(_STR("0"));
        }
    }

    bool ShowCurvesCommand::IsOkEnabled()
    {
        return IsFilePathValid(_sourceCurvesFilePath.GetFullName());
    }

    bool ShowCurvesCommand::IsFilePathValid(const Base::Framework::String& filePath)
    {
        if (filePath.empty())
            return false;

        std::ifstream inStream(filePath.c_str());

        if (!inStream.is_open())
            return false;

        // Проверка расширения файла 
        std::size_t dotPos = filePath.rfind(_STR('.'));

        if (dotPos != std::wstring::npos)
        {
            std::wstring extension = filePath.substr(dotPos);
            std::vector<std::wstring> allowedExtensions = { _STR(".txt") };
            bool validExtension = false;

            for (const auto& ext : allowedExtensions)
            {
                if (_wcsicmp(extension.c_str(), ext.c_str()) == 0)
                {
                    validExtension = true;
                    break;
                }
            }

            if (!validExtension)
            {
                inStream.close();
                return false;
            }
        }
        else
        {
            inStream.close();
            return false;
        }

        inStream.close();

        return true;
    }

    RGK::Result ShowCurvesCommand::DrawCurve(const RGK::NURBSCurve& curve, bool drawByIntervals) const
    {
        if (!curve)
            return RGK::Result::NullPointer;

        RGK::Context rgkContext;
        EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(rgkContext);

        RGK::BodyConstructor::Data data(0, RGK::Body::Type::Wire);
        data.CreateCoEdgeParametricCurvesAutomatically(true);

        auto controlPoints = curve.GetControlPoints();
        auto knots = curve.GetKnots();

        auto tolerance = rgkContext.GetAngularPrecision();
        data.AddVertex(controlPoints.front(), tolerance);
        data.AddVertex(controlPoints.back(), tolerance);

        int ends[2] = { 0, 1 };

        if (drawByIntervals)
        {
            int degree = curve.GetDegree();

            // Реальный интервал определения кривой
            double uMin = knots[degree];
            double uMax = knots[knots.size() - degree - 1];

            // Собираем все уникальные узлы в интервале [uMin, uMax]
            std::vector<double> breakpoints;
            breakpoints.push_back(uMin);

            for (size_t i = degree + 1; i != knots.size() - degree; ++i)
            {
                // Добавляем только уникальные значения (избегаем повторов)
                if (std::abs(knots[i] - breakpoints.back()) > tolerance)
                    breakpoints.push_back(knots[i]);
            }

            // Добавляем последний узел, если его нет
            if (std::abs(breakpoints.back() - uMax) > tolerance)
                breakpoints.push_back(uMax);

            // Создаём рёбра для каждого сегмента между узлами
            for (size_t i = 0; i != breakpoints.size() - 1; ++i)
            {
                double interval[2] = { breakpoints[i], breakpoints[i + 1] };
                data.AddEdge(ends, curve, true, interval, tolerance);
            }
        }
        else
        {
            double interval[2] = { knots.front(), knots.back() };
            data.AddEdge(ends, curve, true, interval, tolerance);
        }

        RGK::BodyConstructor::Report report;
        auto resultCreationBody = RGK::BodyConstructor::Create(rgkContext, data, report);

        if (resultCreationBody != RGK::Success)
            return resultCreationBody;

        EP::Model::ModelScenePtr modelScene = nullptr;

        for (std::size_t i = 0; i != GetDocument()->Representations().Size(); ++i)
        {
            auto presentation = GetDocument()->Representations()[i];

            if (!presentation)
                continue;

            if (presentation->IsTypeOf(EP::Model::ModelScene::ClassID()))
            {
                modelScene = EP::Model::Cast<EP::Model::ModelScene>(presentation);
                break;
            }
        }

        if (!modelScene)
            return RGK::Result::NullPointer;

        EP::Model::EditDocument edit(GetDocument(), RSCADUIW("RPLM.CAD.UI.ShowCurvesCommand"));
        {
            auto bodyObject = std::make_shared<EP::Model::BodyObject>((report.GetBody()));
            GetDocument()->Objects().AddObject(bodyObject);
            modelScene->EditReferences()->AddObject(bodyObject);
            EP::Model::Regeneration::RegenerationContext regenerationContext(GetDocument(), &rgkContext);
            modelScene->Update(EP::Model::PresentationUpdateContext(&regenerationContext));
        }

        edit.End(false);

        return RGK::Success;
    }
}
