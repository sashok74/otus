#pragma once
#include <string>
#include <memory>
/**
 * @class GraphicPrimitive
 * @brief Базовый класс для графических примитивов.
 * @note Базовый класс для графических примитивов, содержащий имя примитива.
 */
class GraphicPrimitive
{
public:
    std::string m_name;
    virtual ~GraphicPrimitive() = default;
};

/**
 * @class Circle
 * @brief Класс, представляющий круг.
 * @note Класс, представляющий круг, наследуется от GraphicPrimitive.
 */
class Circle : public GraphicPrimitive
{
private:
    double m_radius;

public:
    Circle(double radius) : m_radius(radius) {m_name = "Circle";}
};

/**
 * @class Line
 * @brief Класс, представляющий линию.
 * @note Класс, представляющий линию, наследуется от GraphicPrimitive.
 */
class Line : public GraphicPrimitive
{
private:
    double m_length;

public:
    Line(double length) : m_length(length) {m_name = "Line";}
};

class PrimitiveFactory
{
public:
    std::shared_ptr<GraphicPrimitive> createPrimitive(const std::string &type, double param)
    {
        if (type == "Circle")
            return std::make_shared<Circle>(param);
        else if (type == "Line")
            return std::make_shared<Line>(param);
        return nullptr;    
    }
};