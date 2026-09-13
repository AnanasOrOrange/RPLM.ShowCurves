#pragma once

#include "Common/RPLM.Base.Framework.String.h"
#include "Geometry/RGPGeometryForward.h"

namespace RPLM::CAD::UI
{
    class CurveParser
    {
    public:
        // Тип ошибки парсинга
        enum class ErrorType
        {
            // Успешное чтение файла
            SuccessReadFile, 
            // Ошибка открытия файла
            ErrorOpenFile,
            // Неизвестный блок при чтении файла
            UnexpectedBlock,
            // Неверный формат блока со степенью 
            IncorrectFormatBlockDegree,
            // Неверный формат блока ControlPoints
            IncorrectFormatBlockControlPoints,
            // Некорректное количество ожидаемых контрольных точек
            IncorrectNumberExpectControlPoints,
            // Ошибка чтения координаты контрольной точки
            ErrorCoordinateControlPoint,
            // Количество фактических контрольных точек не совпадает с количеством ожидаемых
            NumberActualControlPointsNotMatchExpect,
            // Неверный формат блока с весами 
            IncorrectFormatBlockWeights,
            // Некорректное количество ожидаемых весовых коэффициентов
            IncorrectNumberExpectWeights,
            // Ошибка чтения весового коэффициента
            ErrorWeight,
            // Количество фактических весовых коэффициентов не совпадает с количеством ожидаемых
            NumberActualWeightsNotMatchExpect,
            // Неверный формат блока Knots 
            IncorrectFormatBlockKnots,
            // Некорректное количество ожидаемых узловых коэффициентов
            IncorrectNumberExpectKnots,
            // Ошибка чтения узлового коэффициента
            ErrorKnot,
            // Количество фактических узловых коэффициентов не совпадает с количеством ожидаемых
            NumberActualKnotsNotMatchExpect,
        };

        /// <summary>Результат парсинга файла</summary>
        struct ParseResult
        {
            // Код ошибки
            ErrorType codeError = ErrorType::SuccessReadFile;
            // Номер строки, где произошла ошибка
            std::size_t lineNumber = 0;
        };

        /// <summary>Считывает контрольные точки кривой из файла</summary>
        /// <param name="filePath">Путь к файлу</param>
        /// <returns>Контрольные точки</returns>
        static RGK::Vector<RGK::Math::Vector3D> ReadControlPointsFromFile(const Base::Framework::String& filePath);

        /// <summary>Считывает узловой вектор из файла</summary>
        /// <param name="filePath">Путь к файлу</param>
        /// <returns>Узловой вектор</returns>
        static Math::Geometry2D::Geometry::DoubleArray ReadKnotsFromFile(const Base::Framework::String& filePath);

        /// <summary>Считывает кривые из файла</summary>
        /// <param name="filePath">Путь к файлу</param>
        /// <param name="curves">Кривые, считанные из файла</param>
        /// <returns>Результат чтения файла</returns>
        static ParseResult ReadCurvesFromFile(const Base::Framework::String& filePath, std::vector<RGK::NURBSCurve>& curves);

        /// <summary>Записывает кривую в файл</summary>
        /// <param name="curve">Кривая</param>
        /// <param name="filePath">Путь к файлу</param>
        static void SaveCurveInFile(const RGK::NURBSCurve& curve, const Base::Framework::String& filePath);
    };
}
