#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

const double camera_factor = 1000;
const double camera_cx = 319.500000;
const double camera_cy = 239.500000;
const double camera_fx = 571.623718;
const double camera_fy = 571.623718;

int main(){
	Mat color0 = imread("color0.jpg");
	Mat depth0 = imread("depth0.pgm", IMREAD_UNCHANGED);
	Mat depth1 = imread("depth1.pgm", IMREAD_UNCHANGED);
	Mat_<double> T0(4, 4), T1(4, 4);
	double a[16] = { -0.955421,0.119616,-0.269932,2.65583,
					 0.295248,0.388339,-0.872939,2.9816,
					 0.000407581,-0.91372,-0.406343,1.36865,0,0,0,1 };
	int k = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++){
			T0(i, j) = a[k];
			k++;
		}
	double b[16] = { -0.951595,0.120375,-0.282803,2.65552,
				   0.307283,0.392547,-0.866882,2.98135,
				   0.00666285,-0.91182,-0.410535,1.36186,0,0,0,1 };
	k = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++){
			T1(i, j) = b[k];
			k++;
		}
	resize(color0, color0, depth0.size());
	Mat_<double> c0(4, 1), c1(4, 1), w(4, 1);
	Mat_<Vec3b> img1(color0.rows, color0.cols);
	//someplace in hera may have problem but i do not  know how to deal with this  
	for (int m = 0; m < depth0.rows; m++)
		for (int n = 0; n < depth0.cols; n++){
			int u, v;
			ushort d0 = depth0.ptr<ushort>(m)[n];
			ushort d1 = depth1.ptr<ushort>(m)[n];
			// deepth(z)
			double z0 = double(d0) / camera_factor;
			double z1 = double(d1) / camera_factor;
			//cam1
			c0(0, 0) = (n - camera_cx) * z0 / camera_fx;
			c0(1, 0) = (m - camera_cy) * z0 / camera_fy;
			c0(2, 0) = z0;
			c0(3, 0) = 1.0;
			w = T0 * c0;
			c1 = T1.inv() * w; 
			//cam2
			u = int(c1(0, 0) * camera_fx / c1(2, 0) + camera_cx + 0.5);
			v = int(c1(1, 0) * camera_fy / c1(2, 0) + camera_cy + 0.5);
			if (u >= img1.cols)
				u = img1.cols - 1;
			if (u < 0)
				u = 0;
			if (v >= img1.rows)
				v = img1.rows - 1;
			if (v < 0)
				v = 0;
			img1(v, u) = color0.ptr<Vec3b>(m)[n];
		}
	imshow("img1", img1);
	waitKey(0);
}