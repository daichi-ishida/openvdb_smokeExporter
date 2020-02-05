#pragma once

#include <array>

class Perlin
{
public:
  Perlin();
  Perlin(int repeat);
  ~Perlin();
  double perlin(double x, double y, double z);
  double octavePerlin(double x, double y, double z, int octaves, double persistence);

private:
  void initialize();
  double fade(double t);
  int inc(int num);
  double grad(int hash, double x, double y, double z);
  double linearInterpolation(double a, double b, double x);

  static std::array<int, 256> permutation;

  int m_repeat = -1;
  std::array<int, 512> p;
};