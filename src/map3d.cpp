#include "map3d.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>


// ═════════════════════════════════════════════════════════════════
// 构造函数
// ═════════════════════════════════════════════════════════════════
map3d::map3d(int type, const string& basename , double ke, double kb)
    : type_(type),
      filename_(basename)
{
    switch (type_) {
        case 1:
            Load_RF(filename_, ke, kb);
            break;
        case 2:
            Load_Electro(filename_);
            break;
        case 3:
            Load_Magnetic(filename_);
            break;
        default:
            std::cerr << "[map3d][ERROR]: Unknown field type." << std::endl;
            std::exit(EXIT_FAILURE);
    }
}


// ═════════════════════════════════════════════════════════════════
// RF场：读取六分量
// ═════════════════════════════════════════════════════════════════
void map3d::Load_RF(const string& basename, double ke, double kb)
{
    Read_one_component(basename + ".edx", Ex3D_, /*fill_grid=*/true);
    Read_one_component(basename + ".edy", Ey3D_, false);
    Read_one_component(basename + ".edz", Ez3D_, false);
    Read_one_component(basename + ".bdx", Bx3D_, false);
    Read_one_component(basename + ".bdy", By3D_, false);
    Read_one_component(basename + ".bdz", Bz3D_, false);
   
    const double escale = ke * 1e6;
    // const double bscale = kb * 1e6;
    for (double& v : Ex3D_) v *= escale;
    for (double& v : Ey3D_) v *= escale;
    for (double& v : Ez3D_) v *= escale;
    for (double& v : Bx3D_) v *= kb;
    for (double& v : By3D_) v *= kb;
    for (double& v : Bz3D_) v *= kb;

}


// ═════════════════════════════════════════════════════════════════
// 静电场：读取三分量 Ex/Ey/Ez（待实现）
// ═════════════════════════════════════════════════════════════════
void map3d::Load_Electro(const string& basename)
{
    // TODO: 确认静电场文件后缀格式后实现
    (void)basename;
    std::cerr << "[map3d][ERROR]: ELECTRO field type not yet implemented." << std::endl;
    std::exit(EXIT_FAILURE);
}


// ═════════════════════════════════════════════════════════════════
// 磁场：读取三分量 Bx/By/Bz（待实现）
// ═════════════════════════════════════════════════════════════════
void map3d::Load_Magnetic(const string& basename)
{
    // TODO: 确认磁场文件后缀格式后实现
    (void)basename;
    std::cerr << "[map3d][ERROR]: MAGNETIC field type not yet implemented." << std::endl;
    std::exit(EXIT_FAILURE);
}


// ═════════════════════════════════════════════════════════════════
// 私有：读取单个分量文件
// 文件数据排列约定：x变化最快，y居中，z最慢
// 即循环顺序为 for z { for y { for x { read } } }
// ═════════════════════════════════════════════════════════════════
void map3d::Read_one_component(const string& filepath,
                                    vector<double>& target,
                                    bool fill_grid)
{
    std::ifstream fin(filepath);
    if (!fin.is_open()) {
        std::cerr << "[map3d][ERROR]: cannot open file: "
                  << filepath << std::endl;
        std::exit(EXIT_FAILURE);
    }

    int Nz = 0, Ny = 0, Nx = 0, Norm = 0;
    double length = 0.0, ymin = 0.0, ymax = 0.0, xmin = 0.0, xmax = 0.0;

    fin >> Nz >> length;
    fin >> Nx >> xmin >> xmax;
    fin >> Ny >> ymin >> ymax;
    fin >> Norm;

    const int nx = Nx + 1, ny = Ny + 1, nz = Nz + 1;

    if (nx <= 0 || ny <= 0 || nz <= 0) {
        std::cerr << "[map3d][ERROR]: Invalid grid dimensions in: "
                  << filepath << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (fill_grid) {
        grid_.nx = nx; grid_.ny = ny; grid_.nz = nz;
        grid_.xmin = xmin; grid_.xmax = xmax;
        grid_.ymin = ymin; grid_.ymax = ymax;
        grid_.zmin = 0.0;  grid_.zmax = length;
        grid_.dx = (nx > 1) ? (xmax - xmin) / (nx - 1) : 0.0;
        grid_.dy = (ny > 1) ? (ymax - ymin) / (ny - 1) : 0.0;
        grid_.dz = (nz > 1) ?  length       / (nz - 1) : 0.0;
    } else {
        if (nx != grid_.nx || ny != grid_.ny || nz != grid_.nz) {
            std::cerr << "[map3d][ERROR]: Grid mismatch in: "
                      << filepath << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    target.resize(nx * ny * nz);

    // for (auto& v : target) fin >> v;
    // fin.close();

     // 一次性读入剩余内容，再用 strtod 解析，比 fin >> v 快很多
    std::istreambuf_iterator<char> beg(fin), end_it;
    string buf(beg, end_it);
    fin.close();

    const char* p = buf.data();
    for (auto& v : target) {
        char* q;
        v = strtod(p, &q);
        if (q == p) {
            std::cerr << "[Field_Map][ERROR]: Unexpected end of data in: "
                      << filepath << std::endl;
            std::exit(EXIT_FAILURE);
        }
        p = q;
    }
}


// ═════════════════════════════════════════════════════════════════
// 预计算：轴线 Ez 和五个偏导数数组
// ═════════════════════════════════════════════════════════════════
void map3d::set_axis(double x_axis, double y_axis)
{
    const int nz = grid_.nz;
    const double hz = grid_.dz;
    if (type_ == 1) // RF field
    {
        Ez_axis_.resize(nz);
        axis_dEx_dx_.resize(nz);
        axis_dEy_dy_.resize(nz);
        axis_dEz_dz_.resize(nz);
        axis_dBy_dx_.resize(nz);
        axis_dBx_dy_.resize(nz);

        double gx, gy, gz;
        for (int iz = 0; iz < nz; ++iz)
        {
            const double z0 = grid_.zmin + iz * hz;

            Ez_axis_[iz] = Trilinear_grad(Ez3D_, x_axis, y_axis, z0, gx, gy, gz);
            axis_dEz_dz_[iz] = gz;

            Trilinear_grad(Ex3D_, x_axis, y_axis, z0, gx, gy, gz);
            axis_dEx_dx_[iz] = gx;

            Trilinear_grad(Ey3D_, x_axis, y_axis, z0, gx, gy, gz);
            axis_dEy_dy_[iz] = gy;

            Trilinear_grad(By3D_, x_axis, y_axis, z0, gx, gy, gz);
            axis_dBy_dx_[iz] = gx;

            Trilinear_grad(Bx3D_, x_axis, y_axis, z0, gx, gy, gz);
            axis_dBx_dy_[iz] = gy;
        }
    }
}


// ═════════════════════════════════════════════════════════════════
// 轴线 Ez 查询（给定 z 坐标，线性插值）
// ═════════════════════════════════════════════════════════════════
// double map3d::Ez_axis(double z) const
// {
//     double fz = (z - grid_.zmin) / grid_.dz;
//     fz = std::max(0.0, std::min(fz, (double)(grid_.nz - 2)));
//     const int iz = (int)fz;
//     const double t = fz - iz;
//     return Ez_axis_[iz] * (1.0 - t) + Ez_axis_[iz + 1] * t;
// }


// ═════════════════════════════════════════════════════════════════
// 三线性插值（返回插值值，同时输出三方向解析梯度，私有）
// 索引约定：ix 变化最快，iz 最慢
//   index = ix + nx * (iy + ny * iz)
// ═════════════════════════════════════════════════════════════════
double map3d::Trilinear_grad(const vector<double>& F,
                                  double x, double y, double z,
                                  double& dFdx, double& dFdy, double& dFdz) const
{
    double fx = (x - grid_.xmin) / grid_.dx;
    double fy = (y - grid_.ymin) / grid_.dy;
    double fz = (z - grid_.zmin) / grid_.dz;

    fx = std::max(0.0, std::min(fx, (double)(grid_.nx - 1)));
    fy = std::max(0.0, std::min(fy, (double)(grid_.ny - 1)));
    fz = std::max(0.0, std::min(fz, (double)(grid_.nz - 1)));

    const int ix = (int)std::min(fx, (double)(grid_.nx - 2));
    const int iy = (int)std::min(fy, (double)(grid_.ny - 2));
    const int iz = (int)std::min(fz, (double)(grid_.nz - 2));
    const double tx = fx - ix, ty = fy - iy, tz = fz - iz;

    auto idx = [&](int i, int j, int k) {
        return i + grid_.nx * (j + grid_.ny * k);
    };

    const double c000 = F[idx(ix,   iy,   iz  )];
    const double c100 = F[idx(ix+1, iy,   iz  )];
    const double c010 = F[idx(ix,   iy+1, iz  )];
    const double c110 = F[idx(ix+1, iy+1, iz  )];
    const double c001 = F[idx(ix,   iy,   iz+1)];
    const double c101 = F[idx(ix+1, iy,   iz+1)];
    const double c011 = F[idx(ix,   iy+1, iz+1)];
    const double c111 = F[idx(ix+1, iy+1, iz+1)];

    // 插值值
    const double c00 = c000*(1-tx) + c100*tx;
    const double c10 = c010*(1-tx) + c110*tx;
    const double c01 = c001*(1-tx) + c101*tx;
    const double c11 = c011*(1-tx) + c111*tx;
    const double c0  = c00*(1-ty)  + c10*ty;
    const double c1  = c01*(1-ty)  + c11*ty;
    const double val = c0*(1-tz) + c1*tz;

    // 解析梯度：对分段线性插值直接求导
    // dF/dx：tx 方向线性系数之差，除以格子宽度
    const double d00 = (c100 - c000) * (1-ty)*(1-tz);
    const double d10 = (c110 - c010) *    ty *(1-tz);
    const double d01 = (c101 - c001) * (1-ty)*   tz ;
    const double d11 = (c111 - c011) *    ty *   tz ;
    dFdx = (d00 + d10 + d01 + d11) / grid_.dx;

    // dF/dy
    const double e00 = (c010 - c000) * (1-tx)*(1-tz);
    const double e10 = (c110 - c100) *    tx *(1-tz);
    const double e01 = (c011 - c001) * (1-tx)*   tz ;
    const double e11 = (c111 - c101) *    tx *   tz ;
    dFdy = (e00 + e10 + e01 + e11) / grid_.dy;

    // dF/dz
    const double f00 = (c001 - c000) * (1-tx)*(1-ty);
    const double f10 = (c101 - c100) *    tx *(1-ty);
    const double f01 = (c011 - c010) * (1-tx)*   ty ;
    const double f11 = (c111 - c110) *    tx *   ty ;
    dFdz = (f00 + f10 + f01 + f11) / grid_.dz;

    return val;
}

// 无梯度版本，供 Ex/Ey/Ez/Bx/By/Bz 调用
double map3d::Trilinear(const vector<double>& F,
                             double x, double y, double z) const
{
    double fx = (x - grid_.xmin) / grid_.dx;
    double fy = (y - grid_.ymin) / grid_.dy;
    double fz = (z - grid_.zmin) / grid_.dz;

    fx = std::max(0.0, std::min(fx, (double)(grid_.nx - 1)));
    fy = std::max(0.0, std::min(fy, (double)(grid_.ny - 1)));
    fz = std::max(0.0, std::min(fz, (double)(grid_.nz - 1)));

    const int ix = (int)std::min(fx, (double)(grid_.nx - 2));
    const int iy = (int)std::min(fy, (double)(grid_.ny - 2));
    const int iz = (int)std::min(fz, (double)(grid_.nz - 2));
    const double tx = fx - ix, ty = fy - iy, tz = fz - iz;

    auto idx = [&](int i, int j, int k) {
        return i + grid_.nx * (j + grid_.ny * k);
    };

    const double c000 = F[idx(ix,   iy,   iz  )];
    const double c100 = F[idx(ix+1, iy,   iz  )];
    const double c010 = F[idx(ix,   iy+1, iz  )];
    const double c110 = F[idx(ix+1, iy+1, iz  )];
    const double c001 = F[idx(ix,   iy,   iz+1)];
    const double c101 = F[idx(ix+1, iy,   iz+1)];
    const double c011 = F[idx(ix,   iy+1, iz+1)];
    const double c111 = F[idx(ix+1, iy+1, iz+1)];

    const double c00 = c000*(1-tx) + c100*tx;
    const double c10 = c010*(1-tx) + c110*tx;
    const double c01 = c001*(1-tx) + c101*tx;
    const double c11 = c011*(1-tx) + c111*tx;
    const double c0  = c00*(1-ty)  + c10*ty;
    const double c1  = c01*(1-ty)  + c11*ty;
    return c0*(1-tz) + c1*tz;
}


// ═════════════════════════════════════════════════════════════════
// 任意点偏导数（解析梯度）
// ═════════════════════════════════════════════════════════════════
double map3d::dEx_dx(double x, double y, double z) const
{
    double gx, gy, gz;
    Trilinear_grad(Ex3D_, x, y, z, gx, gy, gz);
    return gx;
}
double map3d::dEy_dy(double x, double y, double z) const
{
    double gx, gy, gz;
    Trilinear_grad(Ey3D_, x, y, z, gx, gy, gz);
    return gy;
}
double map3d::dEz_dz(double x, double y, double z) const
{
    double gx, gy, gz;
    Trilinear_grad(Ez3D_, x, y, z, gx, gy, gz);
    return gz;
}
double map3d::dBy_dx(double x, double y, double z) const
{
    double gx, gy, gz;
    Trilinear_grad(By3D_, x, y, z, gx, gy, gz);
    return gx;
}
double map3d::dBx_dy(double x, double y, double z) const
{
    double gx, gy, gz;
    Trilinear_grad(Bx3D_, x, y, z, gx, gy, gz);
    return gy;
}
