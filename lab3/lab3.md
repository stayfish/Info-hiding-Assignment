# 扩展技术

***1953246方若愚***

## 实验要求

要求：在 Lena 图中嵌入一个 64×64 (共4096位) 的 Logo，需要使用 STDM 方法嵌入在 DCT 系数 中

## 实验环境

| 环境                          |
| ----------------------------- |
| vscode                        |
| windows10                     |
| x86_64-w64-mingw32 + makefile |

## 步骤

嵌入包括如下几个步骤：

1. 将图像的位图分为 $8\times8$ 的小块，可以得到 $M=\frac{512\times512}{8\times8}=2^{12}$ 块
2. 对每个小块进行 DCT 变换，按 Zigzag 排序选出 $K$ 个 ***中频系数*** 进行嵌入操作，如果不是中频系数，解码时会出现问题
3. 使用 $N$ 个系数嵌入一位 b，假设需要嵌入 $L=64\times64=2^{12}$ 位数据，则 $N=\frac{M\times K}L=K$，即需要选出 $\frac{N}K=1$ 个块嵌入一个 b，根据输入的嵌入强度，可以得到 $s_i=x_i+(q_b(\overline{x})-\overline{x})\cdot w_i$，然后将 $s_i$ 放回原来 $x_i$ 的位置

水印解码的过程如下：

1. 将图像的位图分为 $8\times8$ 的小块，可以得到 $M$ 块

2. 对每个小块进行 DCT 变换，按 Zigzag 排序选出 $K$ 个 ***中频系数*** 计算投影 $\overline y=\frac1N\sum_{i=1}^Ny_i\cdot w_i$ ，利用投影解码
   $$
   b=\left\{\begin{matrix}
    0,\quad |q_0(\overline y)-\overline y|\lt \frac\Delta4 \\
    1,\quad s\lt0
   \end{matrix}
   \right.
   $$

3. 将 $L$ 个 b 恢复为 $L$ 位的位图，嵌入 bmp 图的框架即可

其中二维 DCT 变换公式为：
$$
F(u,v)=c(u)c(v)\sum_{i=0}^{N-1}\sum_{j=0}^{N-1}f(i,j)\cdot
\cos[\frac{(2i+1)\pi}{2N}u]\cdot
\cos[\frac{(2j+1)\pi}{2N}v]\\
其中\,c(u)=\left\{
\begin{matrix}
 \sqrt{\frac1N},\quad u=0 \\
 \sqrt{\frac2N},\quad u\neq0
\end{matrix}
\right.
$$
逆变换公式为：
$$
f(i,j)=\sum_{u=0}^{N-1}\sum_{v=0}^{N-1}c(u)\cdot c(v)\cdot F(u,v)\cdot
\cos[\frac{(2i+1)\pi}{2N}u]\cdot
\cos[\frac{(2j+1)\pi}{2N}v]\\
其中\,c(u)=\left\{
\begin{matrix}
 \sqrt{\frac1N},\quad u=0 \\
 \sqrt{\frac2N},\quad u\neq0
\end{matrix}
\right.
$$


## 代码实现

- DCT 变换
  ```c++
  double c(int u) {
      if(u == 0)
          return sqrt(1.0 / NUM);
      else 
          return sqrt(2.0 / NUM);
  }
  
  void DCT(const BLOCK& blk1,BLOCK& blk2) {
      
      for(int u = 0; u < NUM; u++) {
          for(int v = 0; v < NUM; v++) {
              double cu = c(u);
              double cv = c(v);
              double sum(0);
              for(int i = 0; i < NUM; i++) {
                  for(int j = 0; j < NUM; j++) {
                      sum += blk1[i][j] * 
                      cos((2 * i + 1) * PI * u / (2 * NUM)) *
                      cos((2 * j + 1) * PI * v / (2 * NUM)); 
                  }
              }
              blk2[u][v] = cu * cv * sum;
          }
      }
  }
  
  void inverse_DCT(const BLOCK& blk1, BLOCK& blk2, bool flag) {
      for(int i = 0; i < NUM; i++) {
          for(int j = 0; j < NUM; j++) {
              double sum(0);
              for(int u = 0; u < NUM; u++) {
                  for(int v = 0; v < NUM; v++) {
                      double cu = c(u);
                      double cv = c(v);
                      sum += cu * cv * blk1[u][v] * 
                      cos((2 * i + 1) * PI * u / (2 * NUM)) *
                      cos((2 * j + 1) * PI * v / (2 * NUM)); 
                  }
              }
              if(flag) {
                  if(sum > 255) sum = 255;
                  else if (sum < 0) sum = 0;
                  sum = nearest(sum);
              }
              blk2[i][j] = sum;
          }
      }
  }
  ```

  

- bitmap 类
  ```c++
  #pragma once
  
  #include<cmath>
  #include<cstdlib>
  #include<cstring>
  
  #include<string>
  using std::string;
  
  #include<fstream>
  using std::ofstream;
  using std::ifstream;
  
  #include<iostream>
  using std::ostream;
  using std::istream;
  using std::cout;
  #include<iomanip>
  using std::ios;
  using std::hex;
  using std::setw;
  using std::setfill;
  
  using std::move;
  
  typedef unsigned char BYTE;
  
  #ifdef DEBUG
  #define TEST_BMP
  void print_BYTE(BYTE*, int);
  #endif
  
  /**
   * 多位 BYTE 存储的时候采用大端，高位放在大地址
   * 读取的字节，低地址存在数组的低索引处
   */
  
  struct HEADER {
      BYTE type[2] ;
      BYTE bmp_size[4];
      BYTE reserved1[2];
      BYTE reserved2[2];
      BYTE offset[4];
  };
  
  struct BITMAPINFOHEADER {
      BYTE info_header_size[4]; // 该 Header 的大小，40
      BYTE width[4];
      BYTE height[4];
      BYTE planes[2]; // 色彩平面数
      BYTE depth[2]; // 像素位数
      BYTE method[4]; // 压缩方法
      BYTE image_size[4]; // 图片大小
      BYTE h_resolution[4]; // 横向分辨率
      BYTE v_resolution[4]; // 纵向分辨率
      BYTE palette_colors[4]; // 调色板颜色数
      BYTE key_colors[4]; // 重要颜色数
  };
  
  struct RGBQUAD {
      BYTE blue;
      BYTE green;
      BYTE red;
      BYTE reserved;
  };
  
  typedef RGBQUAD* PALETTE;
  typedef BYTE* BITMAP;
  
  // depth = 8 的 BMP
  class BMP {
  public:
      BMP(): m_palette(nullptr), m_bitmap(nullptr) {};
      ~BMP();
      BMP(const BMP&);
      void operator=(const BMP&);
      void operator=(BMP&&);
      BMP(BMP&&);
      void set_bitmap(const BITMAP&, int n);
      BITMAP get_bitmap() const ;
      int get_bitmap_size() const ;
      int get_width() const ;
      int get_height() const ;
      int get_depth() const;
      void load_bmp(const string&);
      void write_bmp(const string&);
  #ifdef TEST_BMP
      void output_BYTE();
      void bmp_test();
  #endif
  private:
      void write_BYTE_to(ostream&, BYTE*, int);
      // void read_BYTE_from(const istream&, BYTE*, int);
      int get_row_size() const ;
      int get_padding();
      int get_palette_colors();
  private:
      int m_colors;
      int m_row_size;
      int m_bitmap_size;
      HEADER m_header;
      BITMAPINFOHEADER m_info_header;
      PALETTE m_palette;
      BITMAP m_bitmap;
  };
  void copy_BYTE(BYTE*, const BYTE*, int); // byte1 is a copy of byte2
  int BYTE_to_int(const BYTE*, int);
  void create_platte(int);
  ```

  

- stdm 类
  ```c++
  #pragma once
  
  #include<vector>
  #include<string>
  using std::vector;
  using std::string;
  
  #include"dct.h"
  #include"bitmap.h"
  
  class STDM {
  public:
      STDM();
      void set_delta(double);
      void encode(const string&, const string&, const string&);
      double decode(const string&, const string&, const string&);
  private:
      double m_d[2];
      double m_delta;
      vector<int> m_w;
      vector<int> m_zig_mid;
      int m_k;
      int m_n;
      BMP m_logo;
  private:
      void get_w();
      void get_zig_mid();
      void quantization(BLOCK&, int);
      int quantization_decode(const BLOCK&) const;
  };
  ```



## 测试效果

测试效果如下图：

![](.\images\rate.png)

其中横轴是量化步长，纵轴是错误率