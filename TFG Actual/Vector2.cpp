#include "Vector2.h"

Vector2::Vector2(void){ 
    x = 0; 
    y = 0;
}

Vector2::Vector2(float xValue, float yValue){
    x = xValue; 
    y = yValue;
     
}
Vector2::Vector2(const Vector2 & v){
    x = v.x; 
    y = v.y;
}
Vector2::Vector2(const Vector2 * v) {
    x = v->x;
    y = v->y;
}

Vector2::~Vector2(void){}

float Vector2::Distance(const Vector2 & v) const { 
    return sqrt(((x - v.x) * (x - v.x)) + ((y - v.y) * (y - v.y))); 
}

float Vector2::Dot(const Vector2 & v) const { 
    return (x * v.x) + (y * v.y); 
}
