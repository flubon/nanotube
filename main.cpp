/*********************************************************************//**
 * AVAILABLE https://gitee.com/Bovera/nanotube, COPYRIGHT Bovera (2022)
 * 
 * @file main.cpp
 * @brief 主函数
 * @version 0.2.0
 * 
 * 主函数示例，请根据需要自行更改
 **************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#define DYNAMICS 2 // 过阻尼朗之万

#include "molecule.h"
#include "nmath.h"
#include "energy.h"

// #define RESTART

// 如果声明了重启，只导入能量函数；否则导入模型类
#ifndef RESTART
#include "model.h"
#endif

#define PARA_rest_len 0
#define PARA_k 1
#define PARA_tau 2

int main(int argc, char* argv[]) {

std::ofstream fout("data.txt");
double tau[4] = {0.01, 0.1, 0.05, 0.5};
for (int g=0; g<5; g++)
for (int t=0; t<4; t++) {
// 如果声明了重启，调用直接 molecule 初始化，否则调用 model 初始化
#ifdef RESTART
    molecule this_model("restart.bin", energy_func::node_energy_func, energy_func::bond_energy_func, argc, argv);
#else
    // 默认模型参数 default_para
    para p = default_para;
    // 修改模型参数，比如 m,n
    p.m = 14, p.n = 7, p.repeat = 8;
    p.direction = -1, p.glide = g, p.climb = 7;
    p.rest_len = 1, p.k = std::sqrt(3)/2, p.tau = tau[t];
    p.damp = 1, p.tempr = 0;
    // 以 p 参数生成模型
    model this_model(p, argc, argv);
#endif
    
    // 输出总能量，设置 z 方向计入的范围，x、y 设无穷即可
    #define OUT_RANGE nano::vector(-INFINITY, -INFINITY, 29), nano::vector(INFINITY, INFINITY, 48)
    #define OUT_DUMP this_model.dump(std::to_string(t)+"_"+std::to_string(g), nano::DATA_FILE | nano::VELOCITY | nano::P_ENERGY | nano::MEAN_CURVE \
        | nano::GAUSS_CURVE | nano::EMPHASIS);

    // 更改运行时参数
    this_model.set_step(1e-2);

#ifndef RESTART
    // 输出位错距离
    nano::pair<double> x = this_model.dis_pair_distance();
    fout << "位错距离为 x=" << x[0] << ", z=" << x[1] << std::endl;
#endif
OUT_DUMP
    // 测试合适的输出范围，用一次就注释掉
    /*for (int i=-5; i<100; i++) {
        std::cout << i << ": " << this_model.total_energy(nano::vector(-INFINITY, -INFINITY, i-3), \
            nano::vector(INFINITY, INFINITY, i+3)) << std::endl;
    }*/
for (int i=0; i<3; i++) {
    // 粒子同步移动的更新
    for (int k=100000; k>0; k--) {
        if (k%1000==0) {
            OUT_DUMP
            fout << this_model.total_particle(OUT_RANGE) << ": " << this_model.total_energy(OUT_RANGE) << std::endl;
        }
        // this_model.set_tempr((k+5000)*1e19);
        // 模型计算更新
        this_model.update();
    }

    // 储存文件以便继续运行
    this_model.store(std::to_string(t)+"_"+std::to_string(g)+"_"+std::to_string(i)+".bin");
}}
    return 0;
}
