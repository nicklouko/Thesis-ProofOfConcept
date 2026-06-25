#pragma once

//
// [SECTION] Matrices
//

struct Matrix4x4
{
    float access(int row_index, int column_index) const
    {
        return m[(row_index * 4) + column_index];
    }

    float operator[](int index)
    {
        return m[index];
    }

    float operator()(int row_index, int column_index) const
    {
        return access(row_index, column_index);
    }

    float m[16];
};

//
// [SECTION] Vectors
//

struct Vector4
{
    Vector4(float _x = 0.f, float _y = 0.f, float _z = 0.f, float _w = 0.f)
        : x(_x), y(_y), z(_z), w(_w)
    {
    }

    Vector4 operator+(Vector4& other)
    {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    void operator+=(Vector4& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
    }

    Vector4 operator-(Vector4& other)
    {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    void operator-=(Vector4& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
    }

    Vector4 operator*(float scalar)
    {
        return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    void operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
    }

    Vector4 operator/(float scalar)
    {
        float inv = 1.0f / scalar;
        return Vector4(x * inv, y * inv, z * inv, w * inv);
    }

    void operator/=(float scalar)
    {
        float inv = 1.0f / scalar;
        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;
    }

    float x, y, z, w;

    // Multiply the vector by a column-major 4x4 matrix
    Vector4 mulColumnMajor4x4Matrix(Matrix4x4* p_matrix)
    {
        Vector4 result;

        // Perform matrix-vector multiplication for each component
        result.x = this->x * p_matrix->access(0, 0) + this->y * p_matrix->access(1, 0) + this->z * p_matrix->access(2, 0) + this->w * p_matrix->access(3, 0);
        result.y = this->x * p_matrix->access(0, 1) + this->y * p_matrix->access(1, 1) + this->z * p_matrix->access(2, 1) + this->w * p_matrix->access(3, 1);
        result.z = this->x * p_matrix->access(0, 2) + this->y * p_matrix->access(1, 2) + this->z * p_matrix->access(2, 2) + this->w * p_matrix->access(3, 2);
        result.w = this->x * p_matrix->access(0, 3) + this->y * p_matrix->access(1, 3) + this->z * p_matrix->access(2, 3) + this->w * p_matrix->access(3, 3);

        return result;
    }
};

struct Vector3
{
    Vector3(float _x = 0.f, float _y = 0.f, float _z = 0.f)
        : x(_x), y(_y), z(_z)
    {
    }

    Vector3 operator+(Vector3& other)
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    void operator+=(Vector3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    Vector3 operator-(Vector3& other)
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    void operator-=(Vector3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    Vector3 operator*(float scalar)
    {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    void operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }

    Vector3 operator/(float scalar)
    {
        float inv = 1.0f / scalar;
        return Vector3(x * inv, y * inv, z * inv);
    }

    void operator/=(float scalar)
    {
        float inv = 1.0f / scalar;
        x *= inv;
        y *= inv;
        z *= inv;
    }

    // Multiply the vector by a column-major 4x4 matrix 
    Vector4 mulColumnMajor4x4Matrix(Matrix4x4* p_matrix)
    {
        Vector4 result;

        result.x = this->x * p_matrix->access(0, 0) + this->y * p_matrix->access(1, 0) + this->z * p_matrix->access(2, 0) + p_matrix->access(3, 0);
        result.y = this->x * p_matrix->access(0, 1) + this->y * p_matrix->access(1, 1) + this->z * p_matrix->access(2, 1) + p_matrix->access(3, 1);
        result.z = this->x * p_matrix->access(0, 2) + this->y * p_matrix->access(1, 2) + this->z * p_matrix->access(2, 2) + p_matrix->access(3, 2);
        result.w = this->x * p_matrix->access(0, 3) + this->y * p_matrix->access(1, 3) + this->z * p_matrix->access(2, 3) + p_matrix->access(3, 3);

        return result;
    }

    // Multiply the vector by a row-major 4x4 matrix
    Vector4 mulRowMajor4x4Matrix(Matrix4x4* p_matrix)
    {
        Vector4 result;

        result.x = this->x * p_matrix->access(0, 0) + this->y * p_matrix->access(0, 1) + this->z * p_matrix->access(0, 2) + p_matrix->access(0, 3);
        result.y = this->x * p_matrix->access(1, 0) + this->y * p_matrix->access(1, 1) + this->z * p_matrix->access(1, 2) + p_matrix->access(1, 3);
        result.z = this->x * p_matrix->access(2, 0) + this->y * p_matrix->access(2, 1) + this->z * p_matrix->access(2, 2) + p_matrix->access(2, 3);
        result.w = this->x * p_matrix->access(3, 0) + this->y * p_matrix->access(3, 1) + this->z * p_matrix->access(3, 2) + p_matrix->access(3, 3);

        return result;
    }

    float x, y, z;
};

struct Vector2
{
    Vector2(float _x = 0.f, float _y = 0.f)
        : x(_x), y(_y)
    {
    }

    Vector2 operator+(Vector2& other)
    {
        return Vector2(x + other.x, y + other.y);
    }

    void operator+=(Vector2& other)
    {
        x += other.x;
        y += other.y;
    }

    Vector2 operator-(Vector2& other)
    {
        return Vector2(x - other.x, y - other.y);
    }

    void operator-=(Vector2& other)
    {
        x -= other.x;
        y -= other.y;
    }

    Vector2 operator*(float scalar)
    {
        return Vector2(x * scalar, y * scalar);
    }

    void operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
    }

    Vector2 operator/(float scalar)
    {
        float inv = 1.0f / scalar;
        return Vector2(x * inv, y * inv);
    }

    void operator/=(float scalar)
    {
        float inv = 1.0f / scalar;
        x *= inv;
        y *= inv;
    }

    float x, y;
};