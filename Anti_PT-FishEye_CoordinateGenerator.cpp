// Anti_PT-FishEye_CoordinateGenerator.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include "Anti_PT-FishEye_CoordinateGenerator.h"
#include <iostream>
#include <fstream>
#include <math.h>
//#include <conio.h>
#include <iomanip>  

using namespace std;

double ModelCoo[VERTEX_POINT_NUM];        /* 模板坐标,即目标坐标，4代表4个顶点，3代表三维 */
double TextCoo[TEXCORDS_POINT_NUM];		  /* 纹理坐标,即原坐标，4代表4个顶点，2代表二维 */
double TempCoo[TEXCORDS_POINT_NUM];		  /* 逆透视后的缓冲坐标：模板坐标->缓冲坐标->纹理坐标 */
int main(int argc, char* argv[])
{
    long int pos1 = 0;		/* pos1是纹理坐标数组下标的增量 */
	long int pos2 = 0;		/* pos2是模板坐标数组下标的增量 */
	long int pos3 = 0;		/* pos3是TempCoo的下标， */
    double i, j;
    double k, r, x0, y0, x1, y1;
    double mesh_y,mesh_x;    

    //double temp_x,temp_y,temp_w;
    //double x, y;
	double x_anti_PT, y_anti_PT;
	double delta1, delta4, delta5;
	double u, v;
	double temp_u, temp_v;
	double m0,m1,m2,m3,m4,m5,m6,m7,m8;

    k=0.002;
    r = 0;      
    mesh_x = mesh_y = MESH_X;

    double camera1_matrix[9] = 
    {
        (double)-6253/4807,  
        (double)-512/897,   
        (double)227727/331,   
        (double)129/5833,  
        (double)580/1121,  
        (double)20288/41,    
        (double)1/190403,
        (double)-319/249538,
        (double)1.0,
    };
    double camera2_matrix[9] = { 
        (double)113/7496,  
        (double)-2225/1153,  
        (double)5474/11,    
        (double)-3289/2022,  
        (double)-548/1451,  
        (double)42053/31,    
        (double)-1/72923, 
        (double)-8/10649, 
        (double)1.0,       
    };
    double camera3_matrix[9] = { 
        (double)1902/1655,  
        (double)-1807/4055,  
        (double)22989/107,   
        (double)-115/9527,  
        (double)-2018/1331,  
        (double)71765/97,    
        (double)-6/245051,
        (double)-95/96046, 
        (double)1.0,       
    };
    double camera4_matrix[9] = { 
        (double)-38/12325, 
        (double)1241/835,   
        (double)-7291/168,   
        (double)1172/749,   
        (double)-817/4355,  
        (double)-13735/37,    
        (double)5/246324,
        (double)-15/34274, 
        (double)1.0,       
    };
	
    m0=m1=m2=m3=m4=m5=m6=m7=m8=0;
#if 1
    m0 = camera1_matrix[0];
    m1 = camera1_matrix[1];
    m2 = camera1_matrix[2];
    m3 = camera1_matrix[3];
    m4 = camera1_matrix[4];
    m5 = camera1_matrix[5];
    m6 = camera1_matrix[6];
    m7 = camera1_matrix[7];
    m8 = camera1_matrix[8];
	
    //camera1        
    for (v=CAM1_DOWN_LEFT_Y; v < CAM1_UP_RIGHT_Y; v += mesh_y) 
    {
        for (u=CAM1_DOWN_LEFT_X; u<=CAM1_UP_RIGHT_X; u += mesh_x) 
        {
			temp_u = u;
			temp_v = v;
			
			/************************************************************************/
			/* 某采样点：计算Texture坐标                                            */
			/************************************************************************/
			/**
			**逆透视算法：根据透视后的坐标和透视矩阵，反推出透视前的坐标。
			**输入：temp_u（横坐标），temp_v（纵坐标）
			**输出：x_anti_PT（横坐标），y_anti_PT（纵坐标）
			**/
			delta1 = m0*m4 - m1*m3 + m3*m7*temp_u - m4*m6*temp_u - m0*m7*temp_v + m1*m6*temp_v;	
			delta4 = m1*m5 - m2*m4 + m4*temp_u - m1*temp_v - m5*m7*temp_u + m2*m7*temp_v;
			delta5 = -(m0*m5 - m2*m3 + m3*temp_u - m0*temp_v - m5*m6*temp_u + m2*m6*temp_v);
			x_anti_PT = delta4/delta1;
			y_anti_PT = delta5/delta1;
			TempCoo[pos3++] = x_anti_PT;
			TempCoo[pos3++] = y_anti_PT;

			/**
			**逆鱼眼算法：推算出正常图片中的采样点坐标在鱼眼图片中的对应映射点。
			**输入：i（横坐标），j（纵坐标）。
			**输出：x1（横坐标），y1（纵坐标）。
			**/
			i = x_anti_PT;
			j = y_anti_PT;
			x0 = i-XOFS1;
            y0 = j-YOFS1-OFFSET_Y1;
            r = sqrt(x0*x0+y0*y0);
            x1 = x0 - x0*k*r + XOFS1 +OFFSET_X1;
            y1 = y0 - y0*k*r + YOFS1;
            TextCoo[pos1++] = (x1/CAMERA_WIDTH);
            TextCoo[pos1++] = (y1/CAMERA_HEIGHT);  
			ModelCoo[pos2++] = temp_u;
			ModelCoo[pos2++] = temp_v;
			ModelCoo[pos2++] = CAMERA1_ZBUFFER;

			/************************************************************************/
			/* 相邻采样点：计算Texture坐标                                          */
			/************************************************************************/
			// temp_u = temp_u;
			temp_v += mesh_y;
			/**
			**逆透视算法：根据透视后的坐标和透视矩阵，反推出透视前的坐标。
			**输入：temp_u（横坐标），temp_v（纵坐标）
			**输出：x_anti_PT（横坐标），y_anti_PT（纵坐标）
			**/
			delta1 = m0*m4 - m1*m3 + m3*m7*temp_u - m4*m6*temp_u - m0*m7*temp_v + m1*m6*temp_v;	
			delta4 = m1*m5 - m2*m4 + m4*temp_u - m1*temp_v - m5*m7*temp_u + m2*m7*temp_v;
			delta5 = -(m0*m5 - m2*m3 + m3*temp_u - m0*temp_v - m5*m6*temp_u + m2*m6*temp_v);
			x_anti_PT = delta4/delta1;
			y_anti_PT = delta5/delta1;
			TempCoo[pos3++] = x_anti_PT;
			TempCoo[pos3++] = y_anti_PT;

			/**
			**逆鱼眼算法：推算出正常图片中的采样点坐标在鱼眼图片中的对应映射点。
			**输入：i（横坐标），j（纵坐标）。
			**输出：x1（横坐标），y1（纵坐标）。
			**/
			i = x_anti_PT;
			j = y_anti_PT;
			x0 = i-XOFS1;
            y0 = j-YOFS1-OFFSET_Y1;
            r = sqrt(x0*x0+y0*y0);
            x1 = x0 - x0*k*r + XOFS1 +OFFSET_X1;
            y1 = y0 - y0*k*r + YOFS1;
            TextCoo[pos1++] = (x1/CAMERA_WIDTH);
            TextCoo[pos1++] = (y1/CAMERA_HEIGHT);  
			ModelCoo[pos2++] = temp_u;
			ModelCoo[pos2++] = temp_v;
			ModelCoo[pos2++] = CAMERA1_ZBUFFER;
        }
    }
#endif

    m0 = camera2_matrix[0];
    m1 = camera2_matrix[1];
    m2 = camera2_matrix[2];
    m3 = camera2_matrix[3];
    m4 = camera2_matrix[4];
    m5 = camera2_matrix[5];
    m6 = camera2_matrix[6];
    m7 = camera2_matrix[7];
    m8 = camera2_matrix[8];
    //camera2       
    for (v=CAM2_DOWN_LEFT_Y; v < CAM2_UP_RIGHT_Y; v += mesh_y) 
    {
        for (u=CAM2_DOWN_LEFT_X; u<=CAM2_UP_RIGHT_X; u += mesh_x) 
        {
			temp_u = u;
			temp_v = v;
			
			/************************************************************************/
			/* 某采样点：计算Texture坐标                                            */
			/************************************************************************/
			/**
			**逆透视算法：根据透视后的坐标和透视矩阵，反推出透视前的坐标。
			**输入：temp_u（横坐标），temp_v（纵坐标）
			**输出：x_anti_PT（横坐标），y_anti_PT（纵坐标）
			**/
			delta1 = m0*m4 - m1*m3 + m3*m7*temp_u - m4*m6*temp_u - m0*m7*temp_v + m1*m6*temp_v;	
			delta4 = m1*m5 - m2*m4 + m4*temp_u - m1*temp_v - m5*m7*temp_u + m2*m7*temp_v;
			delta5 = -(m0*m5 - m2*m3 + m3*temp_u - m0*temp_v - m5*m6*temp_u + m2*m6*temp_v);
			x_anti_PT = delta4/delta1;
			y_anti_PT = delta5/delta1;
			TempCoo[pos3++] = x_anti_PT;
			TempCoo[pos3++] = y_anti_PT;

			/**
			**逆鱼眼算法：推算出正常图片中的采样点坐标在鱼眼图片中的对应映射点。
			**输入：i（横坐标），j（纵坐标）。
			**输出：x1（横坐标），y1（纵坐标）。
			**/
			i = x_anti_PT;
			j = y_anti_PT;
			x0 = i-XOFS2;
            y0 = j-YOFS2-OFFSET_Y2;
            r = sqrt(x0*x0+y0*y0);
            x1 = x0 - x0*k*r + XOFS2 +OFFSET_X2;
            y1 = y0 - y0*k*r + YOFS2;
            TextCoo[pos1++] = (x1/CAMERA_WIDTH);
            TextCoo[pos1++] = (y1/CAMERA_HEIGHT);  
			ModelCoo[pos2++] = temp_u;
			ModelCoo[pos2++] = temp_v;
			ModelCoo[pos2++] = CAMERA2_ZBUFFER;

			/************************************************************************/
			/* 相邻采样点：计算Texture坐标                                          */
			/************************************************************************/
			// temp_u = temp_u;
			temp_v += mesh_y;
			/**
			**逆透视算法：根据透视后的坐标和透视矩阵，反推出透视前的坐标。
			**输入：temp_u（横坐标），temp_v（纵坐标）
			**输出：x_anti_PT（横坐标），y_anti_PT（纵坐标）
			**/
			delta1 = m0*m4 - m1*m3 + m3*m7*temp_u - m4*m6*temp_u - m0*m7*temp_v + m1*m6*temp_v;	
			delta4 = m1*m5 - m2*m4 + m4*temp_u - m1*temp_v - m5*m7*temp_u + m2*m7*temp_v;
			delta5 = -(m0*m5 - m2*m3 + m3*temp_u - m0*temp_v - m5*m6*temp_u + m2*m6*temp_v);
			x_anti_PT = delta4/delta1;
			y_anti_PT = delta5/delta1;
			TempCoo[pos3++] = x_anti_PT;
			TempCoo[pos3++] = y_anti_PT;

			/**
			**逆鱼眼算法：推算出正常图片中的采样点坐标在鱼眼图片中的对应映射点。
			**输入：i（横坐标），j（纵坐标）。
			**输出：x1（横坐标），y1（纵坐标）。
			**/
			i = x_anti_PT;
			j = y_anti_PT;
			x0 = i-XOFS2;
            y0 = j-YOFS2-OFFSET_Y2;
            r = sqrt(x0*x0+y0*y0);
            x1 = x0 - x0*k*r + XOFS2 +OFFSET_X2;
            y1 = y0 - y0*k*r + YOFS2;
            TextCoo[pos1++] = (x1/CAMERA_WIDTH);
            TextCoo[pos1++] = (y1/CAMERA_HEIGHT);  
			ModelCoo[pos2++] = temp_u;
			ModelCoo[pos2++] = temp_v;
			ModelCoo[pos2++] = CAMERA2_ZBUFFER;
        }
    }

    m0 = camera3_matrix[0];
    m1 = camera3_matrix[1];
    m2 = camera3_matrix[2];
    m3 = camera3_matrix[3];
    m4 = camera3_matrix[4];
    m5 = camera3_matrix[5];
    m6 = camera3_matrix[6];
    m7 = camera3_matrix[7];
    m8 = camera3_matrix[8];
    //camera3      
    for (v=CAM3_DOWN_LEFT_Y; v < CAM3_UP_RIGHT_Y; v += mesh_y) 
    {
        for (u=CAM3_DOWN_LEFT_X; u<=CAM3_UP_RIGHT_X; u += mesh_x) 
        {
			temp_u = u;
			temp_v = v;
			
			/************************************************************************/
			/* 某采样点：计算Texture坐标                                            */
			/************************************************************************/
			/**
			**逆透视算法：根据透视后的坐标和透视矩阵，反推出透视前的坐标。
			**输入：temp_u（横坐标），temp_v（纵坐标）
			**输出：x_anti_PT（横坐标），y_anti_PT（纵坐标）
			**/
			delta1 = m0*m4 - m1*m3 + m3*m7*temp_u - m4*m6*temp_u - m0*m7*temp_v + m1*m6*temp_v;	
			delta4 = m1*m5 - m2*m4 + m4*temp_u - m1*temp_v - m5*m7*temp_u + m2*m7*temp_v;
			delta5 = -(m0*m5 - m2*m3 + m3*temp_u - m0*temp_v - m5*m6*temp_u + m2*m6*temp_v);
			x_anti_PT = delta4/delta1;
			y_anti_PT = delta5/delta1;
			TempCoo[pos3++] = x_anti_PT;
			TempCoo[pos3++] = y_anti_PT;

			/**
			**逆鱼眼算法：推算出正常图片中的采样点坐标在鱼眼图片中的对应映射点。
			**输入：i（横坐标），j（纵坐标）。
			**输出：x1（横坐标），y1（纵坐标）。
			**/
			i = x_anti_PT;
			j = y_anti_PT;
			x0 = i-XOFS3;
            y0 = j-YOFS3-OFFSET_Y3;
            r = sqrt(x0*x0+y0*y0);
            x1 = x0 - x0*k*r + XOFS3 +OFFSET_X3;
            y1 = y0 - y0*k*r + YOFS3;
            TextCoo[pos1++] = (x1/CAMERA_WIDTH);
            TextCoo[pos1++] = (y1/CAMERA_HEIGHT);  
			ModelCoo[pos2++] = temp_u;
			ModelCoo[pos2++] = temp_v;
			ModelCoo[pos2++] = CAMERA3_ZBUFFER;

			/************************************************************************/
			/* 相邻采样点：计算Texture坐标                                          */
			/************************************************************************/
			// temp_u = temp_u;
			temp_v += mesh_y;
			/**
			**逆透视算法：根据透视后的坐标和透视矩阵，反推出透视前的坐标。
			**输入：temp_u（横坐标），temp_v（纵坐标）
			**输出：x_anti_PT（横坐标），y_anti_PT（纵坐标）
			**/
			delta1 = m0*m4 - m1*m3 + m3*m7*temp_u - m4*m6*temp_u - m0*m7*temp_v + m1*m6*temp_v;	
			delta4 = m1*m5 - m2*m4 + m4*temp_u - m1*temp_v - m5*m7*temp_u + m2*m7*temp_v;
			delta5 = -(m0*m5 - m2*m3 + m3*temp_u - m0*temp_v - m5*m6*temp_u + m2*m6*temp_v);
			x_anti_PT = delta4/delta1;
			y_anti_PT = delta5/delta1;
			TempCoo[pos3++] = x_anti_PT;
			TempCoo[pos3++] = y_anti_PT;

			/**
			**逆鱼眼算法：推算出正常图片中的采样点坐标在鱼眼图片中的对应映射点。
			**输入：i（横坐标），j（纵坐标）。
			**输出：x1（横坐标），y1（纵坐标）。
			**/
			i = x_anti_PT;
			j = y_anti_PT;
			x0 = i-XOFS3;
            y0 = j-YOFS3-OFFSET_Y3;
            r = sqrt(x0*x0+y0*y0);
            x1 = x0 - x0*k*r + XOFS3 +OFFSET_X3;
            y1 = y0 - y0*k*r + YOFS3;
            TextCoo[pos1++] = (x1/CAMERA_WIDTH);
            TextCoo[pos1++] = (y1/CAMERA_HEIGHT);  
			ModelCoo[pos2++] = temp_u;
			ModelCoo[pos2++] = temp_v;
			ModelCoo[pos2++] = CAMERA3_ZBUFFER;
        }
    }

    m0 = camera4_matrix[0];
    m1 = camera4_matrix[1];
    m2 = camera4_matrix[2];
    m3 = camera4_matrix[3];
    m4 = camera4_matrix[4];
    m5 = camera4_matrix[5];
    m6 = camera4_matrix[6];
    m7 = camera4_matrix[7];
    m8 = camera4_matrix[8];
    //camera4      
    for (v=CAM4_DOWN_LEFT_Y; v < CAM4_UP_RIGHT_Y; v += mesh_y) 
    {
        for (u=CAM4_DOWN_LEFT_X; u<=CAM4_UP_RIGHT_X; u += mesh_x) 
        {
			temp_u = u;
			temp_v = v;
			
			/************************************************************************/
			/* 某采样点：计算Texture坐标                                            */
			/************************************************************************/
			/**
			**逆透视算法：根据透视后的坐标和透视矩阵，反推出透视前的坐标。
			**输入：temp_u（横坐标），temp_v（纵坐标）
			**输出：x_anti_PT（横坐标），y_anti_PT（纵坐标）
			**/
			delta1 = m0*m4 - m1*m3 + m3*m7*temp_u - m4*m6*temp_u - m0*m7*temp_v + m1*m6*temp_v;	
			delta4 = m1*m5 - m2*m4 + m4*temp_u - m1*temp_v - m5*m7*temp_u + m2*m7*temp_v;
			delta5 = -(m0*m5 - m2*m3 + m3*temp_u - m0*temp_v - m5*m6*temp_u + m2*m6*temp_v);
			x_anti_PT = delta4/delta1;
			y_anti_PT = delta5/delta1;
			TempCoo[pos3++] = x_anti_PT;
			TempCoo[pos3++] = y_anti_PT;

			/**
			**逆鱼眼算法：推算出正常图片中的采样点坐标在鱼眼图片中的对应映射点。
			**输入：i（横坐标），j（纵坐标）。
			**输出：x1（横坐标），y1（纵坐标）。
			**/
			i = x_anti_PT;
			j = y_anti_PT;
			x0 = i-XOFS4;
            y0 = j-YOFS4-OFFSET_Y4;
            r = sqrt(x0*x0+y0*y0);
            x1 = x0 - x0*k*r + XOFS4 +OFFSET_X4;
            y1 = y0 - y0*k*r + YOFS4;
            TextCoo[pos1++] = (x1/CAMERA_WIDTH);
            TextCoo[pos1++] = (y1/CAMERA_HEIGHT);  
			ModelCoo[pos2++] = temp_u;
			ModelCoo[pos2++] = temp_v;
			ModelCoo[pos2++] = CAMERA4_ZBUFFER;

			/************************************************************************/
			/* 相邻采样点：计算Texture坐标                                          */
			/************************************************************************/
			// temp_u = temp_u;
			temp_v += mesh_y;
			/**
			**逆透视算法：根据透视后的坐标和透视矩阵，反推出透视前的坐标。
			**输入：temp_u（横坐标），temp_v（纵坐标）
			**输出：x_anti_PT（横坐标），y_anti_PT（纵坐标）
			**/
			delta1 = m0*m4 - m1*m3 + m3*m7*temp_u - m4*m6*temp_u - m0*m7*temp_v + m1*m6*temp_v;	
			delta4 = m1*m5 - m2*m4 + m4*temp_u - m1*temp_v - m5*m7*temp_u + m2*m7*temp_v;
			delta5 = -(m0*m5 - m2*m3 + m3*temp_u - m0*temp_v - m5*m6*temp_u + m2*m6*temp_v);
			x_anti_PT = delta4/delta1;
			y_anti_PT = delta5/delta1;
			TempCoo[pos3++] = x_anti_PT;
			TempCoo[pos3++] = y_anti_PT;

			/**
			**逆鱼眼算法：推算出正常图片中的采样点坐标在鱼眼图片中的对应映射点。
			**输入：i（横坐标），j（纵坐标）。
			**输出：x1（横坐标），y1（纵坐标）。
			**/
			i = x_anti_PT;
			j = y_anti_PT;
			x0 = i-XOFS4;
            y0 = j-YOFS4-OFFSET_Y4;
            r = sqrt(x0*x0+y0*y0);
            x1 = x0 - x0*k*r + XOFS4 +OFFSET_X4;
            y1 = y0 - y0*k*r + YOFS4;
            TextCoo[pos1++] = (x1/CAMERA_WIDTH);
            TextCoo[pos1++] = (y1/CAMERA_HEIGHT);  
			ModelCoo[pos2++] = temp_u;
			ModelCoo[pos2++] = temp_v;
			ModelCoo[pos2++] = CAMERA4_ZBUFFER;
        }
    }    

	/**
	**分别格式化输出三个数据数据到三个文件中
	**/
	//ofstream  ofstream_modelCoo("D:\\Matlab_Home\\fisheye\\Anti_PT-FishEye_CoordinateGenerator\\modelCoo.txt");
	//ofstream  ofstream_textCoo("D:\\Matlab_Home\\fisheye\\Anti_PT-FishEye_CoordinateGenerator\\textCoo.txt");
	//ofstream  ofstream_tempCoo("D:\\Matlab_Home\\fisheye\\Anti_PT-FishEye_CoordinateGenerator\\tempCoo.txt");
	
	ofstream  ofstream_modelCoo("modelCoo.txt");
	ofstream  ofstream_textCoo("textCoo.txt");
	ofstream  ofstream_tempCoo("tempCoo.txt");

	ofstream_modelCoo.setf(ios::fixed);
	ofstream_modelCoo<<setprecision(6);

	ofstream_textCoo.setf(ios::fixed);
	ofstream_textCoo<<setprecision(6);

	ofstream_tempCoo.setf(ios::fixed);
	ofstream_tempCoo<<setprecision(6);

	long int model_count = 0;
	while (model_count<VERTEX_POINT_NUM)
	{
		ofstream_modelCoo<<ModelCoo[model_count++]<<" ";
		ofstream_modelCoo<<ModelCoo[model_count++]<<"\n";
		model_count++;//三维坐标中的z值被忽略
	}
	long int text_count = 0;
	while (text_count<TEXCORDS_POINT_NUM)
	{
		ofstream_textCoo<<TextCoo[text_count++]<<" ";
		ofstream_textCoo<<TextCoo[text_count++]<<"\n";
	}

	long int temp_count = 0;
	while (temp_count<TEXCORDS_POINT_NUM)
	{
		ofstream_tempCoo<<TempCoo[temp_count++]<<" ";
		ofstream_tempCoo<<TempCoo[temp_count++]<<"\n";
	}

	ofstream_modelCoo.close();
	ofstream_textCoo.close();
	ofstream_tempCoo.close();

	cout<<"Anti_PT-FishEye_CoordinateGenerator: DONE!"<<endl;
	//getch();
	return 0;
}
