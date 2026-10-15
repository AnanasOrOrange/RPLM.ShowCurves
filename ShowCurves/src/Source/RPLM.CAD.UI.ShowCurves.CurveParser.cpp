#include "RPLM.CAD.UI.ShowCurves.CurveParser.h"
#include "RGPSession.h"
#include <fstream>

namespace RPLM::CAD::UI
{
    RGK::Vector<RGK::Math::Vector3D> CurveParser::ReadControlPointsFromFile(const Base::Framework::String& filePath)
    {
        RGK::Vector<RGK::Math::Vector3D> controlPoints;

        std::ifstream inStream(filePath);
        if (!inStream.is_open())
            return controlPoints;

        std::string line;
        double x, y, z;
        int expectedPoints = 0;
        while (std::getline(inStream, line))
        {
            if (line.find("Control Points[") != std::string::npos)
            {
                std::size_t startPos = line.find('[');
                std::size_t endPos = line.find(']');
                if (startPos != std::string::npos && endPos != std::string::npos)
                {
                    std::string numberText = line.substr(startPos + 1, endPos - startPos - 1);
                    try 
                    {
                        expectedPoints = std::stoi(numberText);
                    }
                    catch (...) 
                    {
                        return controlPoints;
                    }
                }
                for (int i = 0; i < expectedPoints; i++)
                {
                    if (!std::getline(inStream, line))
                        return controlPoints; 

                    std::replace(line.begin(), line.end(), ',', ' ');
                    std::stringstream ss(line);

                    if (!(ss >> x >> y >> z))
                        return controlPoints; 

                    RGK::Math::Vector3D point(x, y, z);
                    controlPoints.push_back(point);
                }
                break;
            }
        }
        return controlPoints;
    }

    Math::Geometry2D::Geometry::DoubleArray CurveParser::ReadKnotsFromFile(const Base::Framework::String& filePath)
    {
        Math::Geometry2D::Geometry::DoubleArray knots;
        std::ifstream inStream(filePath);
        std::string line;
        double knot;
        if (!inStream.is_open())
            return knots;

        int expectedKnots = 0;
        while (std::getline(inStream, line))
        {
            if (line.find("Knots[") != std::string::npos)
            {
                std::size_t startPos = line.find('[');
                std::size_t endPos = line.find(']');

                if (startPos != std::string::npos && endPos != std::string::npos)
                {
                    std::string numberText = line.substr(startPos + 1, endPos - startPos - 1);
                    try 
                    {
                        expectedKnots = std::stoi(numberText);
                    }
                    catch (...) 
                    {
                        return knots;
                    }
                }
                if (!std::getline(inStream, line))
                    return knots;

                std::replace(line.begin(), line.end(), ',', ' ');
                std::stringstream ss(line);
                while (ss >> knot)
                    knots.push_back(knot);
                break;
            }
        }
        return knots;
    }

    CurveParser::ParseResult CurveParser::ReadCurvesFromFile(const Base::Framework::String& filePath, std::vector<RGK::NURBSCurve>& curves)
    {
        RGK::Context rgkContext;
        EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(rgkContext);

        std::ifstream inStream(filePath);
        if (!inStream.is_open())
            return ParseResult{ ErrorType::ErrorOpenFile, 0 };

        std::string line;
        std::size_t lineNumber = 0;

        int degree = 0;
        bool isPeriodic = false;
        std::vector<double> weights;

        auto controlPoints = ReadControlPointsFromFile(filePath);
        auto knots = ReadKnotsFromFile(filePath);

        if (controlPoints.empty())
            return ParseResult{ ErrorType::IncorrectFormatBlockControlPoints, 0 };
        if (knots.empty())
            return ParseResult{ ErrorType::IncorrectFormatBlockKnots, 0 };

        while (std::getline(inStream, line))
        {
            lineNumber++;

            if (line.find("Degree:") != std::string::npos)
            {
                try 
                {
                    degree = std::stoi(line.substr(line.find(':') + 1));
                }
                catch (...)
                {
                    return ParseResult{ ErrorType::IncorrectFormatBlockDegree, lineNumber };
                }
                    
            }
            else if (line.find("IsPeriodic:") != std::string::npos)
                isPeriodic = (line.find("true") != std::string::npos);
            else if (line.find("Weights[") != std::string::npos)
            {
                int expectedWeights = 0;
                std::size_t startPos = line.find('[');
                std::size_t endPos = line.find(']');

                if (startPos != std::string::npos && endPos != std::string::npos)
                {
                    std::string numberText = line.substr(startPos + 1, endPos - startPos - 1);
                    try 
                    {
                        expectedWeights = std::stoi(numberText);
                    }
                    catch (...) 
                    {
                        return ParseResult{ ErrorType::IncorrectNumberExpectWeights, lineNumber };
                    }
                }
                else
                {
                    return ParseResult{ ErrorType::IncorrectFormatBlockWeights, lineNumber };
                }

                if (!std::getline(inStream, line))
                    return ParseResult{ ErrorType::NumberActualWeightsNotMatchExpect, lineNumber };

                lineNumber++;
                std::replace(line.begin(), line.end(), ',', ' ');
                std::stringstream ss(line);
                double weight;
                while (ss >> weight)
                    weights.push_back(weight);

                if (weights.size() != static_cast<std::size_t>(expectedWeights))
                    return ParseResult{ ErrorType::NumberActualWeightsNotMatchExpect, lineNumber };
            }
        }

        if (weights.size() != controlPoints.size())
            return ParseResult{ ErrorType::NumberActualWeightsNotMatchExpect, 0 };

        if (knots.size() != controlPoints.size() + degree + 1)
            return ParseResult{ ErrorType::NumberActualKnotsNotMatchExpect, 0 };

        RGK::Vector<RGK::Math::Vector4D> points4D;
        for (std::size_t i = 0; i < controlPoints.size(); i++)
        {
            points4D.push_back(RGK::Math::Vector4D(
                controlPoints[i].GetX(),
                controlPoints[i].GetY(),
                controlPoints[i].GetZ(),
                weights[i]
            ));
        }

        RGK::NURBSCurve curve;
        auto result = RGK::NURBSCurve::Create(
            rgkContext,
            points4D,
            degree,
            knots,
            isPeriodic,
            curve
        );
        if (result == RGK::Success)
        {
            curves.push_back(curve);
            return ParseResult{ ErrorType::SuccessReadFile, 0 };
        }
        else
            return ParseResult{ ErrorType::UnexpectedBlock, 0 };
    }

    void CurveParser::SaveCurveInFile(const RGK::NURBSCurve& curve, const Base::Framework::String& filePath)
    {
        std::ofstream outFile(filePath);
        if (!outFile.is_open())
            return;

        int degree = curve.GetDegree();
        bool isPeriodic = curve.IsPeriodic();
        auto controlPoints = curve.GetControlPoints();
        auto weights = curve.GetWeights();
        auto knots = curve.GetKnots();

        outFile << std::fixed << std::setprecision(6);

        outFile << "{\n";
        outFile << "  \"degree\": " << degree << ",\n";
        outFile << "  \"isPeriodic\": " << (isPeriodic ? "true" : "false") << ",\n";

        outFile << "  \"controlPoints\": [\n";
        for (std::size_t i = 0; i < controlPoints.size(); i++) 
        {
            outFile << "    {\"x\": " << controlPoints[i].GetX()
                << ", \"y\": " << controlPoints[i].GetY()
                << ", \"z\": " << controlPoints[i].GetZ() << "}";
            if (i < controlPoints.size() - 1) 
                outFile << ",";
            outFile << "\n";
        }
        outFile << "  ],\n";

        outFile << "  \"weights\": [";
        for (std::size_t i = 0; i < weights.size(); i++) 
        {
            outFile << weights[i];
            if (i < weights.size() - 1) 
                outFile << ", ";
        }
        outFile << "],\n";

        outFile << "  \"knots\": [";
        for (std::size_t i = 0; i < knots.size(); i++) 
        {
            outFile << knots[i];
            if (i < knots.size() - 1) 
                outFile << ", ";
        }
        outFile << "]\n";
        outFile << "}\n";
        outFile.close();
    }
}