
#ifndef VECTOR2_H
#define VECTOR2_H

#include <math.h>

class Vector2 {
    public:
        float x;
        float y;

        inline const Vector2 operator + (const Vector2 & v) const { return Vector2(x + v.x, y + v.y); }
        inline const Vector2 operator - (const Vector2 & v) const { return Vector2(x - v.x, y - v.y); }
        inline const Vector2 operator * (const Vector2 & v) const { return Vector2(x * v.x, y * v.y); }
        inline const Vector2 operator * (float v) const { return Vector2(x * v, y * v); }        
        
        Vector2(void); 
        Vector2(float xValue, float yValue);
        Vector2(const Vector2 & v);
        Vector2(const Vector2 * v);  
        ~Vector2(void);    
        
        float Distance(const Vector2 & v) const;
        float Dot(const Vector2 & v) const;
        float Angle(const Vector2 & v) const;
};
#endif