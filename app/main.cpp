#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "map3d.h"

namespace {

bool file_exists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

std::string resolve_default_basename() {
    const std::vector<std::string> candidates = {
        "../../data/hwr010",
        "../data/hwr010",
        "data/hwr010",
        "../../../data/hwr010"
    };

    for (const std::string& candidate : candidates) {
        if (file_exists(candidate + ".edx")) {
            return candidate;
        }
    }

    return candidates.front();
}

double midpoint(double min_value, double max_value) {
    return 0.5 * (min_value + max_value);
}

void print_grid_info(const GridInfo& grid) {
    std::cout << "[MAIN] Grid size      : "
              << grid.nx << " x " << grid.ny << " x " << grid.nz << '\n';
    std::cout << "[MAIN] X range / step : ["
              << grid.xmin << ", " << grid.xmax << "], dx = " << grid.dx << '\n';
    std::cout << "[MAIN] Y range / step : ["
              << grid.ymin << ", " << grid.ymax << "], dy = " << grid.dy << '\n';
    std::cout << "[MAIN] Z range / step : ["
              << grid.zmin << ", " << grid.zmax << "], dz = " << grid.dz << '\n';
}

void print_field_sample(const map3d& field, double x, double y, double z) {
    std::cout << "[MAIN] Sample point   : (" << x << ", " << y << ", " << z << ")\n";
    std::cout << "[MAIN] E(x,y,z)       : "
              << "Ex=" << field.Ex(x, y, z)
              << ", Ey=" << field.Ey(x, y, z)
              << ", Ez=" << field.Ez(x, y, z) << '\n';
    std::cout << "[MAIN] B(x,y,z)       : "
              << "Bx=" << field.Bx(x, y, z)
              << ", By=" << field.By(x, y, z)
              << ", Bz=" << field.Bz(x, y, z) << '\n';
    std::cout << "[MAIN] Derivatives    : "
              << "dEx/dx=" << field.dEx_dx(x, y, z)
              << ", dEy/dy=" << field.dEy_dy(x, y, z)
              << ", dEz/dz=" << field.dEz_dz(x, y, z)
              << ", dBy/dx=" << field.dBy_dx(x, y, z)
              << ", dBx/dy=" << field.dBx_dy(x, y, z) << '\n';
}

void print_axis_sample(const map3d& field) {
    if (field.Ez_axis_.empty()) {
        std::cout << "[MAIN] Axis cache     : not initialized\n";
        return;
    }

    const std::size_t mid = field.Ez_axis_.size() / 2;
    std::cout << "[MAIN] Axis Ez        : "
              << "z0=" << field.Ez_axis_.front()
              << ", zmid=" << field.Ez_axis_[mid]
              << ", zend=" << field.Ez_axis_.back() << '\n';
    std::cout << "[MAIN] Axis dEz/dz    : "
              << "z0=" << field.axis_dEz_dz_.front()
              << ", zmid=" << field.axis_dEz_dz_[mid]
              << ", zend=" << field.axis_dEz_dz_.back() << '\n';
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string basename =
        (argc > 1) ? argv[1] : resolve_default_basename();

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "[MAIN] Program begin...\n";
    std::cout << "[MAIN] Field basename : " << basename << '\n';

    map3d field(1, basename);
    field.set_axis();

    print_grid_info(field.grid_);

    const double x = midpoint(field.grid_.xmin, field.grid_.xmax);
    const double y = midpoint(field.grid_.ymin, field.grid_.ymax);
    const double z = midpoint(field.grid_.zmin, field.grid_.zmax);

    print_field_sample(field, x, y, z);
    print_axis_sample(field);

    std::cout << "[MAIN] Program finished.\n";
    return 0;
}
