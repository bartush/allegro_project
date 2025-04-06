// vv utils
//#include <dlib/matrix.h>

constexpr double g_vv_tolerance = 1e-3;

inline double vv_abs(double val)
{
    return val > .0 ? val : -val;
}

inline bool vv_is_zero(double val)
{
    return vv_abs(val) < g_vv_tolerance;
}

namespace vv_geom
{
    struct vec3
    {
        double x;
        double y;
        double z;

        vec3 operator+(const vec3& rhs)
        {
            vec3 res;
            res.x = x + rhs.x;
            res.y = y + rhs.y;
            res.z = z + rhs.z;
            return res;
        }

        vec3 operator-(const vec3& rhs)
        {
            vec3 res;
            res.x = x - rhs.x;
            res.y = y - rhs.y;
            res.z = z - rhs.z;
            return res;
        }
    };


    void cross_product(const vec3& v1, const vec3& v2, vec3& res)
    {
        res.x = v1.y * v2.z - v1.z * v2.y;
        res.y = v1.x * v2.z - v1.z * v2.x;
        res.z = v1.x * v2.y - v1.y * v2.x;
    }

    vec3 cross_product(const vec3& v1, const vec3& v2)
    {
        vec3 res;
        res.x = v1.y * v2.z - v1.z * v2.y;
        res.y = v1.x * v2.z - v1.z * v2.x;
        res.z = v1.x * v2.y - v1.y * v2.x;
        return res;
    }
}
