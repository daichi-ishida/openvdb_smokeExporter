#include <cmath>
#include <vector>
#include <openvdb/openvdb.h>
#include <openvdb/tools/Dense.h>

#include "perlin.hpp"

constexpr int N = 64;
constexpr int DIM_X = 2*N;
constexpr int DIM_Y = N;
constexpr int DIM_Z = N;

int main()
{

    double frequency = 3.0 / static_cast<double>(N);
    double center_x = static_cast<double>(DIM_X) / 2.0;
    double center_y = static_cast<double>(DIM_Y) / 2.0;
    double center_z = static_cast<double>(DIM_Z) / 2.0;

    Perlin perlin_generator;
    std::vector<double> density;
    std::vector<double> u((DIM_X+1)*DIM_Y*DIM_Z, 1.0);

    for (int z = 0; z < DIM_Z; ++z)
    {
        for (int y = 0; y < DIM_Y; ++y)
        {
            for (int x = 0; x < DIM_X; ++x)
            {
                double dx = center_x - x;
                double dy = center_y - y;
                double dz = center_z - z;

                double off = std::abs(perlin_generator.octavePerlin(x * frequency, 
                                                                y * frequency,
                                                                z * frequency,
                                                                5,
                                                                0.5));

                double d = std::sqrt(dx * dx + dy * dy + dz * dz) / static_cast<double>(N);
                double value = ((d - off) < 0.0001) ? 1.0 : 0.0;
                density.push_back(value);
            }
        }
    }

    // The grid spacing
    const double dx = 1.0;

    // Initialize the OpenVDB library.
    openvdb::initialize();

    // density
    openvdb::DoubleGrid::Ptr densityGrid = openvdb::DoubleGrid::create(0.0);
    densityGrid->setName("density");
    densityGrid->setTransform(openvdb::math::Transform::createLinearTransform(dx));
    densityGrid->setGridClass(openvdb::GRID_FOG_VOLUME);

    // u
    openvdb::DoubleGrid::Ptr uGrid = openvdb::DoubleGrid::create(0.0);
    uGrid->setName("u");
    uGrid->setTransform(openvdb::math::Transform::createLinearTransform(dx));
    uGrid->setGridClass(openvdb::GRID_STAGGERED);

    auto densityAccessor = densityGrid->getAccessor();
    auto uAccessor = uGrid->getAccessor();
    for (int z = 0; z < DIM_Z; ++z)
    {
        for (int y = 0; y < DIM_Y; ++y)
        {
            for (int x = 0; x <= DIM_X; ++x)
            {
                int offset = x + (y + z * DIM_Y) * DIM_X;
                int offset_u = x + (y + z * DIM_Y) * (DIM_X+1);
                openvdb::Coord xyz(x, y, z);
                if(x < DIM_X)
                {
                    densityAccessor.setValue(xyz, density[offset]);
                }
                uAccessor.setValue(xyz, u[offset_u]);
            }
        }
    }

    // Add the grid pointer to a container.
    openvdb::GridPtrVec grids;
    grids.push_back(densityGrid);
    grids.push_back(uGrid);

    // Create a VDB file object.
    openvdb::io::File file("output/smoke.vdb");

    // Write out the contents of the container.
    file.write(grids);
    file.close();
}