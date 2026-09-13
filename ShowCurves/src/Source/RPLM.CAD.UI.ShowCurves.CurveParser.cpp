#include "RPLM.CAD.UI.ShowCurves.CurveParser.h"
#include "RGPSession.h"
#include <fstream>

namespace RPLM::CAD::UI
{
    RGK::Vector<RGK::Math::Vector3D> CurveParser::ReadControlPointsFromFile(const Base::Framework::String& filePath)
    {
        std::ifstream inStream(filePath);
        RGK::Vector<RGK::Math::Vector3D> controlPoints;

        // нужно реализовать

        return controlPoints;
    }

    Math::Geometry2D::Geometry::DoubleArray CurveParser::ReadKnotsFromFile(const Base::Framework::String& filePath)
    {
        Math::Geometry2D::Geometry::DoubleArray knots;
        std::ifstream inStream(filePath);

        // нужно реализовать

        return knots;
    }

    CurveParser::ParseResult CurveParser::ReadCurvesFromFile(const Base::Framework::String& filePath,std::vector<RGK::NURBSCurve>& curves)
    {
        // нужно реализовать

        return CurveParser::ParseResult{ CurveParser::ErrorType::SuccessReadFile };
    }

    void CurveParser::SaveCurveInFile(const RGK::NURBSCurve& curve, const Base::Framework::String& filePath)
    {
        // нужно реализовать
    }
}
