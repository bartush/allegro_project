#ifndef vv_utils
#define vv_utils
#include <cmath>

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

    vec3() : x(.0), y(.0), z(.0) {}
    vec3(double x, double y, double z) : x(x), y(y), z(z) {}

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
	void normalize()
	    {
		double norm = sqrt( x * x + y * y + z * z);
		if (norm > 0)
		{
		    x /= norm;
		    y /= norm;
		    z /= norm;
		}
	    }
    };

    double dot_product(const vec3& v1, const vec3& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

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

    // Quaternion struct
    struct quat
    {
	double w,    //cosine of half the rotation angle
	    x, y, z; //unit vector scaled by sine of half the angle

    quat() : w(1.0), x(.0), y(.0), z(.0) {}
    quat(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) {}

	quat operator*(const quat& q) const
	    {
		return quat(
		    w * q.w - x * q.x - y * q.y - z * q.z,
		    w * q.x + x * q.w + y * q.z - z * q.y,
		    w * q.y - x * q.z + y * q.w + z * q.x,
		    w * q.z + x * q.y - y * q.x + z * q.w
		    );
	    }

	quat operator*(const double& scalar) const
	    {
		return quat(w * scalar,x * scalar, y * scalar, z * scalar);
	    }

	void normalize()
	    {
		double norm = sqrt(w * w + x * x + y * y + z * z);
		if (norm > g_vv_tolerance)
		{
		    w /= norm;
		    x /= norm;
		    y /= norm;
		    z /= norm;
		}
	    }

	quat normal() const
	    {
		double norm = sqrt(w * w + x * x + y * y + z * z);
		if (norm > g_vv_tolerance)
		    return quat(w / norm, x / norm, y /norm, z / norm);
		return quat(w, x, y, z);
	    }

	// Функция для получения инвертированного кватерниона
        quat inverse() const
	    {
		double norm_squared = w * w + x * x + y * y + z * z;
		if (norm_squared < g_vv_tolerance)
		    return quat(1.0, 0.0, 0.0, 0.0); // Handle zero quaternion case

		double inv_norm_squared = 1.0 / norm_squared;
		return quat(w * inv_norm_squared, -x * inv_norm_squared, -y * inv_norm_squared, -z * inv_norm_squared);
	    }

	void to_rotation_matrix(double matrix[16]) const
	    {
		float x2  = x + x;
		float y2  = y + y;
		float z2  = z + z;
		float xx2 = x * x2;
		float xy2 = x * y2;
		float xz2 = x * z2;
		float yy2 = y * y2;
		float yz2 = y * z2;
		float zz2 = z * z2;
		float wx2 = w * x2;
		float wy2 = w * y2;
		float wz2 = w * z2;

		// матрица столбцов (column-major matrix)
		matrix[0] = 1 - (yy2 + zz2);
		matrix[1] = xy2 + wz2;
		matrix[2] = xz2 - wy2;
		matrix[3] = 0;

		matrix[4] = xy2 - wz2;
		matrix[5] = 1 - (xx2 + zz2);
		matrix[6] = yz2 + wx2;
		matrix[7] = 0;

		matrix[8] = xz2 + wy2;
		matrix[9] = yz2 - wx2;
		matrix[10] = 1 - (xx2 + yy2);
		matrix[11] = 0;

		matrix[12] = 0;
		matrix[13] = 0;
		matrix[14] = 0;
		matrix[15] = 1;
	    }

	// Функция для создания кватерниона из оси и угла поворота
	static quat from_axis_angle(const vv_geom::vec3& axis, double angle)
	    {
		double half_angle = angle / 2.0;
		double sin_half_angle = sin(half_angle);
		return quat(
		    cos(half_angle),
		    axis.x * sin_half_angle,
		    axis.y * sin_half_angle,
		    axis.z * sin_half_angle
		    );
	    }

        // Функция для создания кватерниона из двух векторов
	static quat from_vectors(const vv_geom::vec3& v1, const vv_geom::vec3& v2)
	    {
		vv_geom::vec3 cross = vv_geom::cross_product(v1, v2);
		double dot = vv_geom::dot_product(v1, v2);
		double half_cos_angle = sqrt((1 + dot) / 2);
		double half_sin_angle = sqrt((1 - dot) / 2);
		return quat(
		    half_cos_angle,
		    cross.x * half_sin_angle,
		    cross.y * half_sin_angle,
		    cross.z * half_sin_angle
		    );
	    }

	// Функция для создания кватерниона из углов Эйлера
	void from_euler(double xa, double ya, double za)
	    {
		double cy = cos(za * 0.5);
		double sy = sin(za * 0.5);
		double cp = cos(ya * 0.5);
		double sp = sin(ya * 0.5);
		double cr = cos(xa * 0.5);
		double sr = sin(xa * 0.5);

		w = cr * cp * cy + sr * sp * sy;
		x = sr * cp * cy - cr * sp * sy;
		y = cr * sp * cy + sr * cp * sy;
		z = cr * cp * sy - sr * sp * cy;
	    }

	// Функция для получения углов Эйлера по матрице повортоа (матрице столбцов)
	static void get_euler_from_matrix(const double matrix[16], double &xa, double &ya, double &za)
	    {
		double sy = std::sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1]);

		if (sy > 1e-6) // Проверка на сингулярность
		{
		    xa = std::atan2(matrix[9], matrix[10]);
		    ya = std::atan2(-matrix[8], sy);
		    za = std::atan2(matrix[4], matrix[0]);
		}
		else
		{
		    xa = std::atan2(-matrix[5], matrix[4]);
		    ya = std::atan2(-matrix[8], sy);
		    za = 0; // za становится неопределенным, можно установить в 0
		}
	    }

	void convert_to_euler(double &xa, double &ya, double &za) const
	    {
		// Преобразование кватерниона в углы Эйлера
		double sinr_cosp = 2 * (w * x + y * z);
		double cosr_cosp = 1 - 2 * (x * x + y * y);
		xa = std::atan2(sinr_cosp, cosr_cosp);

		double sinp = 2 * (w * y - z * x);
		if (std::abs(sinp) >= 1)
		    ya = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
		else
		    ya = std::asin(sinp);

		double siny_cosp = 2 * (w * z + x * y);
		double cosy_cosp = 1 - 2 * (y * y + z * z);
		za = std::atan2(siny_cosp, cosy_cosp);
	    }

	void convert_to_euler_from_matrix(double &xa, double &ya, double &za) const
	    {
		double matrix[16];
		to_rotation_matrix(matrix);
		get_euler_from_matrix(matrix, xa, ya, za);
	    }
    };

    vec3 rotate_vector(const vec3& v, const quat& q) 
    {
	quat qv(0, v.x, v.y, v.z);
	quat result = q * qv * q.inverse();
	return vv_geom::vec3(result.x, result.y, result.z);
    }
}
#endif
