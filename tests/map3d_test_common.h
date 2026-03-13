#pragma once

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

inline bool file_exists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

inline void require_true(const std::string& label, bool condition) {
    if (!condition) {
        std::cerr << label << " failed\n";
        std::exit(EXIT_FAILURE);
    }
}

inline void require_close(
    const std::string& label,
    double actual,
    double expected,
    double tolerance = 1e-9) {
    if (std::fabs(actual - expected) > tolerance) {
        std::cerr << label << " mismatch: expected " << expected
                  << ", got " << actual << '\n';
        std::exit(EXIT_FAILURE);
    }
}

inline void require_finite(const std::string& label, double value) {
    if (!std::isfinite(value)) {
        std::cerr << label << " is not finite: " << value << '\n';
        std::exit(EXIT_FAILURE);
    }
}

inline std::string resolve_hwr010_basename() {
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

    std::cerr << "cannot locate real fieldmap data/hwr010\n";
    std::exit(EXIT_FAILURE);
}
