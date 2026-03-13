#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include "map3d.h"
#include "map3d_test_common.h"

namespace {

void verify_real_field_sample(const map3d& field, double x, double y, double z, const std::string& label) {
    require_finite(label + " Ex", field.Ex(x, y, z));
    require_finite(label + " Ey", field.Ey(x, y, z));
    require_finite(label + " Ez", field.Ez(x, y, z));
    require_finite(label + " Bx", field.Bx(x, y, z));
    require_finite(label + " By", field.By(x, y, z));
    require_finite(label + " Bz", field.Bz(x, y, z));
    require_finite(label + " dEx/dx", field.dEx_dx(x, y, z));
    require_finite(label + " dEy/dy", field.dEy_dy(x, y, z));
    require_finite(label + " dEz/dz", field.dEz_dz(x, y, z));
    require_finite(label + " dBy/dx", field.dBy_dx(x, y, z));
    require_finite(label + " dBx/dy", field.dBx_dy(x, y, z));
}

void verify_real_axis_matches_grid_nodes(const map3d& field) {
    const int ix = static_cast<int>((0.0 - field.grid_.xmin) / field.grid_.dx + 0.5);
    const int iy = static_cast<int>((0.0 - field.grid_.ymin) / field.grid_.dy + 0.5);

    require_true("real axis x index in range", ix >= 0 && ix < field.grid_.nx);
    require_true("real axis y index in range", iy >= 0 && iy < field.grid_.ny);

    for (int iz = 0; iz < field.grid_.nz; ++iz) {
        const std::size_t index =
            static_cast<std::size_t>(ix + field.grid_.nx * (iy + field.grid_.ny * iz));
        const double raw_ez = field.Ez3D_[index];
        const double interpolated_ez = field.Ez_axis_[static_cast<std::size_t>(iz)];

        require_close("real axis Ez matches raw grid", interpolated_ez, raw_ez, 1e-12);
    }
}

}  // namespace

int main() {
    const std::string basename = resolve_hwr010_basename();
    map3d field(1, basename);
    field.set_axis(0.0, 0.0);

    require_true("real grid nx", field.grid_.nx == 21);
    require_true("real grid ny", field.grid_.ny == 21);
    require_true("real grid nz", field.grid_.nz == 106);
    require_close("real xmin", field.grid_.xmin, -0.02, 1e-12);
    require_close("real xmax", field.grid_.xmax, 0.02, 1e-12);
    require_close("real ymin", field.grid_.ymin, -0.02, 1e-12);
    require_close("real ymax", field.grid_.ymax, 0.02, 1e-12);
    require_close("real zmin", field.grid_.zmin, 0.0, 1e-12);
    require_close("real zmax", field.grid_.zmax, 0.21, 1e-12);

    require_true("real axis Ez cache size", field.Ez_axis_.size() == static_cast<std::size_t>(field.grid_.nz));
    require_true("real axis dEx/dx cache size", field.axis_dEx_dx_.size() == static_cast<std::size_t>(field.grid_.nz));
    require_true("real axis dEy/dy cache size", field.axis_dEy_dy_.size() == static_cast<std::size_t>(field.grid_.nz));
    require_true("real axis dEz/dz cache size", field.axis_dEz_dz_.size() == static_cast<std::size_t>(field.grid_.nz));
    require_true("real axis dBy/dx cache size", field.axis_dBy_dx_.size() == static_cast<std::size_t>(field.grid_.nz));
    require_true("real axis dBx/dy cache size", field.axis_dBx_dy_.size() == static_cast<std::size_t>(field.grid_.nz));

    verify_real_field_sample(field, 0.0, 0.0, 0.0, "origin");
    verify_real_field_sample(field, 0.0, 0.0, 0.105, "axis-mid");
    verify_real_field_sample(field, -0.01, -0.01, 0.105, "inner-point");
    verify_real_axis_matches_grid_nodes(field);

    require_finite("real axis Ez front", field.Ez_axis_.front());
    require_finite("real axis Ez mid", field.Ez_axis_[field.Ez_axis_.size() / 2]);
    require_finite("real axis Ez back", field.Ez_axis_.back());

    double axis_max_abs = 0.0;
    for (double value : field.Ez_axis_) {
        require_finite("real axis Ez value", value);
        axis_max_abs = std::max(axis_max_abs, std::fabs(value));
    }
    require_true("real axis Ez not all zero", axis_max_abs > 0.0);

    std::cout << "map3d hwr010 smoke test passed\n";
    return 0;
}
