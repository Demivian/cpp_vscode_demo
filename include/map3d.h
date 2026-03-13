#pragma once
#include <string>
#include <vector>

using std::string;
using std::vector;

// ─────────────────────────────────────────────────────────────────
// 场类型（type 参数）
//   1 = 射频场：六分量 Ex/Ey/Ez/Bx/By/Bz
//   2 = 静电场：三分量 Ex/Ey/Ez（待实现）
//   3 = 磁场：  三分量 Bx/By/Bz（待实现）
// ─────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────
// 网格信息
// ─────────────────────────────────────────────────────────────────
struct GridInfo {
    int nx, ny, nz;        // 网格点数（= 分段数 + 1）
    double xmin, xmax;
    double ymin, ymax;
    double zmin, zmax;     // zmin 固定为 0，zmax = length
    double dx, dy, dz;
};

// ─────────────────────────────────────────────────────────────────
// map3d 类
// ─────────────────────────────────────────────────────────────────
class map3d {
public:
    // ── 构造函数：指定场类型和文件basename，自动读取对应分量 ──────
    //   RF(1)：读 .edx/.edy/.edz/.bdx/.bdy/.bdz，预计算轴线数据
    //   其他类型接口预留，暂未实现
    map3d(int type, const string& basename, double ke = 1.0, double kb = 1.0);

    // ── 网格信息 ──────────────────────────────────────────────────
    GridInfo grid_;

    // ── 三维场数据（index = ix + nx*(iy + ny*iz)）────────────────
    // RF场：六分量均有效
    // 静电场：仅 Ex/Ey/Ez 有效（待实现）
    // 磁场：  仅 Bx/By/Bz 有效（待实现）
    vector<double> Ex3D_, Ey3D_, Ez3D_;
    vector<double> Bx3D_, By3D_, Bz3D_;

    // ── 射频场专用：轴线场与偏导数缓存（RF类型构造后可用）────────
    vector<double> Ez_axis_;       // Ez 轴线值，长度 nz
    vector<double> axis_dEx_dx_;
    vector<double> axis_dEy_dy_;
    vector<double> axis_dEz_dz_;
    vector<double> axis_dBy_dx_;
    vector<double> axis_dBx_dy_;

    // ── 轴线 Ez 查询（给定 z 坐标，线性插值）────────────────────
    // double Ez_axis(double z) const;

    // ── 重新指定轴线坐标并重新预计算（偏心时使用）────────────────
    void set_axis(double x_axis = 0.0, double y_axis = 0.0);

    // ── 任意点三线性插值（按分量）────────────────────────────────
    double Ex(double x, double y, double z) const { return Trilinear(Ex3D_, x, y, z); }
    double Ey(double x, double y, double z) const { return Trilinear(Ey3D_, x, y, z); }
    double Ez(double x, double y, double z) const { return Trilinear(Ez3D_, x, y, z); }
    double Bx(double x, double y, double z) const { return Trilinear(Bx3D_, x, y, z); }
    double By(double x, double y, double z) const { return Trilinear(By3D_, x, y, z); }
    double Bz(double x, double y, double z) const { return Trilinear(Bz3D_, x, y, z); }

    // ── 任意点偏导数（中心差分）──────────────────────────────────
    double dEx_dx(double x, double y, double z) const;
    double dEy_dy(double x, double y, double z) const;
    double dEz_dz(double x, double y, double z) const;
    double dBy_dx(double x, double y, double z) const;
    double dBx_dy(double x, double y, double z) const;

private:
    int type_;
    string filename_;

    void Load_RF(const string& basename, double ke, double kb);
    void Load_Electro(const string& basename);   // 待实现
    void Load_Magnetic(const string& basename);  // 待实现

    void   Read_one_component(const string& filepath,
                               vector<double>& target, bool fill_grid);
    double Trilinear(const vector<double>& F,
                     double x, double y, double z) const;
    double Trilinear_grad(const vector<double>& F,
                          double x, double y, double z,
                          double& dFdx, double& dFdy, double& dFdz) const;
};