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
