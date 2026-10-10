#include "RPLM.CAD.UI.ShowCurves.CurveParser.h"
#include "RGPSession.h"
#include <fstream>

namespace RPLM::CAD::UI
{
    RGK::Vector<RGK::Math::Vector3D> CurveParser::ReadControlPointsFromFile(const Base::Framework::String& filePath)
    {
        RGK::Vector<RGK::Math::Vector3D> controlPoints;

        std::ifstream inStream(filePath);
        if (!inStream.is_open()) {

            return controlPoints;
        }
        
        std::string line;
        double x, y, z;
        int expectedPoints = 0;
        while (std::getline(inStream, line)) {

            if (line.find("Control Points[") != std::string::npos) {
                size_t startPos = line.find('[');
                size_t endPos = line.find(']');

                if (startPos != std::string::npos && endPos != std::string::npos) {
                    std::string numberText = line.substr(startPos + 1, endPos - startPos - 1);
                    expectedPoints = std::stoi(numberText);
                }
                for(int i=0; i < expectedPoints; i++){
                    std::getline(inStream, line);
                    for (int j = 0; j < line.size(); j++) {
                        if (line[j] == ',') { line[j] = ' '; }
                    
                    }
                    std::stringstream ss(line);
                    ss >> x >> y >> z;
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
        if (!inStream.is_open()) {

            return knots;
        }

        int expectedPoints = 0;
        while (std::getline(inStream, line)) {


            if (line.find("Knots[") != std::string::npos) {
                size_t startPos = line.find('[');
                size_t endPos = line.find(']');


                if (startPos != std::string::npos && endPos != std::string::npos) {
                    std::string numberText = line.substr(startPos + 1, endPos - startPos - 1);
                    expectedPoints = std::stoi(numberText);
                }

                std::getline(inStream, line);
                
                for (int j = 0; j < line.size(); j++) {
                    if (line[j] == ',') { line[j] = ' '; }

                    }
                std::stringstream ss(line);
                while (ss >> knot) {
                    knots.push_back(knot);

                }
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
        std::string line;
        int degree = 0;
        bool isPeriodic = false;
        std::vector<double> weights;
        auto controlPoints = ReadControlPointsFromFile(filePath);
        auto knots = ReadKnotsFromFile(filePath);

        while (std::getline(inStream, line)) {
            if (line.find("Degree:") != std::string::npos) {

                degree = std::stoi(line.substr(line.find(':') + 1));


            }
            else if (line.find("IsPeriodic:") != std::string::npos) {

                isPeriodic = (line.find("true") != std::string::npos);


            }
            else if (line.find("Weights[") != std::string::npos) {
                int expectedPoints = 0;

                size_t startPos = line.find('[');
                size_t endPos = line.find(']');


                if (startPos != std::string::npos && endPos != std::string::npos) {
                    std::string numberText = line.substr(startPos + 1, endPos - startPos - 1);
                    expectedPoints = std::stoi(numberText);
                }

                std::getline(inStream, line);

                for (int j = 0; j < line.size(); j++) {
                    if (line[j] == ',') { line[j] = ' '; }

                }
                std::stringstream ss(line);
                double weight;
                while (ss >> weight) {
                    weights.push_back(weight);

                }


            }




        }

        if (!controlPoints.empty()) {
            RGK::Vector<RGK::Math::Vector4D> points4D;
            for (size_t i = 0; i < controlPoints.size(); i++) {
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

            if (result == RGK::Success) {
                curves.push_back(curve);  
            }
            else {
                
                return ParseResult{ ErrorType::UnexpectedBlock };  
            }
            return CurveParser::ParseResult{ CurveParser::ErrorType::SuccessReadFile };
        }
    }

    void CurveParser::SaveCurveInFile(const RGK::NURBSCurve& curve, const Base::Framework::String& filePath)
    {
        std::ofstream outFile(filePath);
        if (!outFile.is_open()) {
            return; 
        }

        
        int degree = curve.GetDegree();
        bool isPeriodic = curve.IsPeriodic();
        auto controlPoints = curve.GetControlPoints();
        auto weights = curve.GetWeights();
        auto knots = curve.GetKnots();

        
        outFile << std::fixed << std::setprecision(6);

        
        outFile << "{\n";
        outFile << "  \"degree\": " << degree << ",\n";
        outFile << "  \"isPeriodic\": " << (isPeriodic ? "true" : "false") << ",\n";

        // Control Points
        outFile << "  \"controlPoints\": [\n";
        for (size_t i = 0; i < controlPoints.size(); i++) {
            outFile << "    {\"x\": " << controlPoints[i].GetX()
                << ", \"y\": " << controlPoints[i].GetY()
                << ", \"z\": " << controlPoints[i].GetZ() << "}";
            if (i < controlPoints.size() - 1) outFile << ",";
            outFile << "\n";
        }
        outFile << "  ],\n";

        // Weights
        outFile << "  \"weights\": [";
        for (size_t i = 0; i < weights.size(); i++) {
            outFile << weights[i];
            if (i < weights.size() - 1) outFile << ", ";
        }
        outFile << "],\n";

        // Knots
        outFile << "  \"knots\": [";
        for (size_t i = 0; i < knots.size(); i++) {
            outFile << knots[i];
            if (i < knots.size() - 1) outFile << ", ";
        }
        outFile << "]\n";

        outFile << "}\n";
        outFile.close();
    }
}
