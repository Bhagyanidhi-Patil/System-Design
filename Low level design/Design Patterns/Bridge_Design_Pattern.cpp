/*
The Bridge Design Pattern is a structural design pattern that separates an abstraction from its implementation,
so the two can vary independently.
“Split a class into two parts: abstraction and implementation — and connect them with a bridge.”

Imagine you have:

Shapes → Circle, Rectangle
Drawing APIs → OpenGL, DirectX

Without Bridge:
CircleOpenGL, CircleDirectX, RectangleOpenGL, RectangleDirectX

With Bridge:
Shape → uses → DrawingAPI

🧱 Structure
1.Abstraction
    High-level control (e.g., Shape)
2.Refined Abstraction
    Extended abstraction (e.g., Circle)
3.Implementor (interface)
    Defines implementation methods (e.g., DrawingAPI)
4.Concrete Implementors
    Actual implementations (OpenGL, DirectX)

                UML-Diagram
                
                <<abstract>>
                +----------------------+
                |        Shape         |
                +----------------------+
                | - api: DrawingAPI*   |
                +----------------------+
                | + draw()             |
                +----------------------+
                          ▲
                          │  (is-a)
                          │
                +----------------------+
                |       Circle         |
                +----------------------+
                | - x: float           |
                | - y: float           |
                | - radius: float      |
                +----------------------+
                | + draw()             |
                +----------------------+


        (has-a / association)
   Shape ◇---------------------------> DrawingAPI       [Abstraction -----> Implementor]
                                                                    (has-a)


                <<interface>>
                +----------------------+
                |     DrawingAPI       |
                +----------------------+
                | + drawCircle()       |
                +----------------------+
                          ▲
             ┌────────────┴────────────┐
             │                         │
      (is-a) │                         │ (is-a)
             │                         │
+----------------------+   +----------------------+
|      OpenGLAPI       |   |     DirectXAPI       |
+----------------------+   +----------------------+
| + drawCircle()       |   | + drawCircle()       |
+----------------------+   +----------------------+
*/

#include<iostream>
using namespace std;

//Implementor Interface
class DrawingAPI {
public:
    virtual void drawCircle(float x, float y, float radius) = 0;
    virtual ~DrawingAPI() {}
};

//Concrete Implementations
class OpenGLAPI : public DrawingAPI {
public:
    void drawCircle(float x, float y, float radius) override {
        std::cout << "Drawing circle using OpenGL\n";
    }
};

class DirectXAPI : public DrawingAPI {
public:
    void drawCircle(float x, float y, float radius) override {
        std::cout << "Drawing circle using DirectX\n";
    }
};

//Abstraction
class Shape {
protected:
    DrawingAPI* api;

public:
    Shape(DrawingAPI* api) : api(api) {}
    virtual void draw() = 0;
    virtual ~Shape() {}
};

//Refined Abstraction
class Circle : public Shape {
private:
    float x, y, radius;

public:
    Circle(float x, float y, float r, DrawingAPI* api)
        : Shape(api), x(x), y(y), radius(r) {}

    void draw() override {
        api->drawCircle(x, y, radius);
    }
};

int main() {
    DrawingAPI* opengl = new OpenGLAPI();
    DrawingAPI* directx = new DirectXAPI();

    Shape* c1 = new Circle(1, 2, 3, opengl);
    Shape* c2 = new Circle(4, 5, 6, directx);

    c1->draw();
    c2->draw();

    delete c1;
    delete c2;
    delete opengl;
    delete directx;

    return 0;
}