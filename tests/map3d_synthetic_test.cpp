#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "map3d.h"
#include "map3d_test_common.h"

namespace {

struct Coefficients {
    double c0;
    double cx;
    double cy;
    double cz;
};

double evaluate_linear(const Coefficients& coeffs, double x, double y, double z) {
    return coeffs.c0 + coeffs.cx * x + coeffs.cy * y + coeffs.cz * z;
}

void write_component_file(
    const std::string& filepath,
    const Coefficients& coeffs,
    double xmin,
    double xmax,
    double ymin,
    double ymax,
    double length) {
    const int nx_segments = 1;
    const int ny_segments = 1;
    const int nz_segments = 1;

    const std::vector<double> xs = {xmin, xmax};
    const std::vector<double> ys = {ymin, ymax};
    const std::vector<double> zs = {0.0, length};

    std::ofstream out(filepath);
    require_true("open synthetic component file", out.is_open());

    out << nz_segments << ' ' << length << '\n';
    out << nx_segments << ' ' << xmin << ' ' << xmax << '\n';
    out << ny_segments << ' ' << ymin << ' ' << ymax << '\n';
    out << 1 << '\n';

    for (double z : zs) {
        for (double y : ys) {
            for (double x : xs) {
                out << evaluate_linear(coeffs, x, y, z) << '\n';
            }
        }
    }
}

std::string synthetic_basename() {
    return "map3d_linear_field";
}

void write_synthetic_fieldmap(const std::string& basename) {
    const double xmin = -2.0;
    const double xmax = 2.0;
    const double ymin = -2.0;
    const double ymax = 2.0;
    const double length = 10.0;

    write_component_file(basename + ".edx", {1.0, 2.0, 3.0, 4.0}, xmin, xmax, ymin, ymax, length);
    write_component_file(basename + ".edy", {-2.0, 5.0, 7.0, 11.0}, xmin, xmax, ymin, ymax, length);
    write_component_file(basename + ".edz", {0.5, -1.0, 6.0, 8.0}, xmin, xmax, ymin, ymax, length);
    write_component_file(basename + ".bdx", {3.0, 13.0, 17.0, 19.0}, xmin, xmax, ymin, ymax, length);
    write_component_file(basename + ".bdy", {-4.0, 23.0, 29.0, 31.0}, xmin, xmax, ymin, ymax, length);
    write_component_file(basename + ".bdz", {2.0, -37.0, 41.0, 43.0}, xmin, xmax, ymin, ymax, length);
}

void remove_synthetic_fieldmap(const std::string& basename) {
    std::remove((basename + ".edx").c_str());
    std::remove((basename + ".edy").c_str());
    std::remove((basename + ".edz").c_str());
    std::remove((basename + ".bdx").c_str());
    std::remove((basename + ".bdy").c_str());
    std::remove((basename + ".bdz").c_str());
}

void verify_interpolation(const map3d& field, double x, double y, double z) {
    require_close("Ex", field.Ex(x, y, z), evaluate_linear({1.0, 2.0, 3.0, 4.0}, x, y, z));
    require_close("Ey", field.Ey(x, y, z), evaluate_linear({-2.0, 5.0, 7.0, 11.0}, x, y, z));
    require_close("Ez", field.Ez(x, y, z), evaluate_linear({0.5, -1.0, 6.0, 8.0}, x, y, z));
    require_close("Bx", field.Bx(x, y, z), evaluate_linear({3.0, 13.0, 17.0, 19.0}, x, y, z));
    require_close("By", field.By(x, y, z), evaluate_linear({-4.0, 23.0, 29.0, 31.0}, x, y, z));
    require_close("Bz", field.Bz(x, y, z), evaluate_linear({2.0, -37.0, 41.0, 43.0}, x, y, z));
}

void verify_derivatives(const map3d& field, double x, double y, double z) {
    require_close("dEx/dx", field.dEx_dx(x, y, z), 2.0);
    require_close("dEy/dy", field.dEy_dy(x, y, z), 7.0);
    require_close("dEz/dz", field.dEz_dz(x, y, z), 8.0);
    require_close("dBy/dx", field.dBy_dx(x, y, z), 23.0);
    require_close("dBx/dy", field.dBx_dy(x, y, z), 17.0);
}

void verify_axis_cache_linear(const map3d& field) {
    require_true("axis Ez cache size", field.Ez_axis_.size() == static_cast<std::size_t>(field.grid_.nz));
    require_true("axis dEx/dx cache size", field.axis_dEx_dx_.size() == static_cast<std::size_t>(field.grid_.nz));
    require_true("axis dEy/dy cache size", field.axis_dEy_dy_.size() == static_cast<std::size_t>(field.grid_.nz));
    require_true("axis dEz/dz cache size", field.axis_dEz_dz_.size() == static_cast<std::size_t>(field.grid_.nz));
    require_true("axis dBy/dx cache size", field.axis_dBy_dx_.size() == static_cast<std::size_t>(field.grid_.nz));
    require_true("axis dBx/dy cache size", field.axis_dBx_dy_.size() == static_cast<std::size_t>(field.grid_.nz));

    const double z0 = field.grid_.zmin;
    const std::size_t mid = field.Ez_axis_.size() / 2;
    const double zmid = field.grid_.zmin + static_cast<double>(mid) * field.grid_.dz;
    const double zend = field.grid_.zmax;

    require_close("axis Ez z0", field.Ez_axis_.front(), evaluate_linear({0.5, -1.0, 6.0, 8.0}, 0.0, 0.0, z0));
    require_close("axis Ez zmid", field.Ez_axis_[mid], evaluate_linear({0.5, -1.0, 6.0, 8.0}, 0.0, 0.0, zmid));
    require_close("axis Ez zend", field.Ez_axis_.back(), evaluate_linear({0.5, -1.0, 6.0, 8.0}, 0.0, 0.0, zend));

    require_close("axis dEx/dx z0", field.axis_dEx_dx_.front(), 2.0);
    require_close("axis dEy/dy z0", field.axis_dEy_dy_.front(), 7.0);
    require_close("axis dEz/dz z0", field.axis_dEz_dz_.front(), 8.0);
    require_close("axis dBy/dx z0", field.axis_dBy_dx_.front(), 23.0);
    require_close("axis dBx/dy z0", field.axis_dBx_dy_.front(), 17.0);
}

}  // namespace

int main() {
    const std::string basename = synthetic_basename();
    write_synthetic_fieldmap(basename);

    map3d field(1, basename, 1e-6, 1.0);
    field.set_axis(0.0, 0.0);

    require_close("grid xmin", field.grid_.xmin, -2.0);
    require_close("grid xmax", field.grid_.xmax, 2.0);
    require_close("grid ymin", field.grid_.ymin, -2.0);
    require_close("grid ymax", field.grid_.ymax, 2.0);
    require_close("grid zmin", field.grid_.zmin, 0.0);
    require_close("grid zmax", field.grid_.zmax, 10.0);
    require_close("grid dx", field.grid_.dx, 4.0);
    require_close("grid dy", field.grid_.dy, 4.0);
    require_close("grid dz", field.grid_.dz, 10.0);

    verify_interpolation(field, 1.25, 1.5, 4.0);
    verify_interpolation(field, -0.5, 0.75, 7.5);
    verify_derivatives(field, 1.25, 1.5, 4.0);
    verify_derivatives(field, -0.5, 0.75, 7.5);
    verify_axis_cache_linear(field);

    remove_synthetic_fieldmap(basename);

    std::cout << "map3d synthetic test passed\n";
    return 0;
}
