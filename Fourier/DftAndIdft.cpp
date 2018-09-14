#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;
int main(int a, char **p)
{

    Mat input = imread(p[1], CV_LOAD_IMAGE_GRAYSCALE); //以灰度图像的方式读入图片
                                                       //如果不知到怎么传入p[1]。可以改为
                                                       //Mat input=imread("image.jpg",CV_LOAD_IMAGE_GRAYSCALE);

    // 图像太大，缩小
    cv::resize(input, input, cv::Size(800, 600));

    imshow("input", input); //显示原图
    int w = getOptimalDFTSize(input.cols);
    int h = getOptimalDFTSize(input.rows); //获取最佳尺寸，快速傅立叶变换要求尺寸为2的n次方

    // // 打印图像调整后的尺寸
    // std::cout<< "原始尺寸：" << input.cols << " * " <<input.rows<<std::endl;
    // std::cout<<"检测的尺寸：" << w << " * " << h<< std::endl;

    Mat padded;
    copyMakeBorder(input, padded, 0, h - input.rows, 0, w - input.cols, BORDER_CONSTANT, Scalar::all(0)); //填充图像保存到padded中
    // cv::imshow("test", padded);
    // cv::waitKey(0);
    Mat plane[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)}; //创建通道
    Mat complexIm;
    merge(plane, 2, complexIm);              //合并通道
    dft(complexIm, complexIm);               //进行傅立叶变换，结果保存在自身
    split(complexIm, plane);                 //分离通道
    magnitude(plane[0], plane[1], plane[0]); //获取幅度图像，0通道为实数通道，1为虚数，因为二维傅立叶变换结果是复数
    int cx = padded.cols / 2;
    int cy = padded.rows / 2; //一下的操作是移动图像，左上与右下交换位置，右上与左下交换位置
    Mat temp;
    Mat part1(plane[0], Rect(0, 0, cx, cy));
    Mat part2(plane[0], Rect(cx, 0, cx, cy));
    Mat part3(plane[0], Rect(0, cy, cx, cy));
    Mat part4(plane[0], Rect(cx, cy, cx, cy));

    part1.copyTo(temp);
    part4.copyTo(part1);
    temp.copyTo(part4);

    part2.copyTo(temp);
    part3.copyTo(part2);
    temp.copyTo(part3);

    // 进行滤波
    cv::Mat ps_mat = cv::imread(p[2], 0);
    if (!ps_mat.data)
    {
        std::cout << p[2] << "图像加载失败" << std::endl;
    }
    if (ps_mat.cols != complexIm.cols || ps_mat.rows != ps_mat.rows)
    {
        std::cout << "图像的尺寸不对！" << std::endl;
    }
    // 将图像对象进行相换
    int cx_ps = ps_mat.cols / 2;
    int cy_ps = ps_mat.rows / 2; //一下的操作是移动图像，左上与右下交换位置，右上与左下交换位置
    Mat temp_ps;
    Mat part1_ps(ps_mat, Rect(0, 0, cx_ps, cy_ps));
    Mat part2_ps(ps_mat, Rect(cx_ps, 0, cx_ps, cy_ps));
    Mat part3_ps(ps_mat, Rect(0, cy_ps, cx_ps, cy_ps));
    Mat part4_ps(ps_mat, Rect(cx_ps, cy_ps, cx_ps, cy_ps));

    part1_ps.copyTo(temp_ps);
    part4_ps.copyTo(part1_ps);
    temp_ps.copyTo(part4_ps);

    part2_ps.copyTo(temp_ps);
    part3_ps.copyTo(part2_ps);
    temp_ps.copyTo(part3_ps);

    cv::imshow("test_ps", ps_mat);
    // 合并
    Mat ps_mats[] = {Mat_<float>(ps_mat), Mat_<float>(ps_mat)}; //创建通道
    Mat ps_tow_ch;
    merge(ps_mats, 2, ps_tow_ch); //合并通道
    complexIm = complexIm.mul(ps_tow_ch);

    //*******************************************************************

    Mat _complexim;
    complexIm.copyTo(_complexim);                                                            //把变换结果复制一份，进行逆变换，也就是恢复原图
    Mat iDft[] = {Mat::zeros(plane[0].size(), CV_32F), Mat::zeros(plane[0].size(), CV_32F)}; //创建两个通道，类型为float，大小为填充后的尺寸
    idft(_complexim, _complexim);                                                            //傅立叶逆变换
    split(_complexim, iDft);                                                                 //结果貌似也是复数
    magnitude(iDft[0], iDft[1], iDft[0]);                                                    //分离通道，主要获取0通道
    normalize(iDft[0], iDft[0], 1, 0, CV_MINMAX);                                            //归一化处理，float类型的显示范围为0-1,大于1为白色，小于0为黑色

    imshow("idft", iDft[0]); //显示逆变换

    plane[0] += Scalar::all(1); //傅立叶变换后的图片不好分析，进行对数处理，结果比较好看
    log(plane[0], plane[0]);
    // normalize(plane[0], plane[0], 1, 0, CV_MINMAX);
    normalize(plane[0], plane[0], 1, 0, CV_MINMAX);

    imshow("dft", plane[0]);
    waitKey(0);

    // 保存图像
    plane[0] = plane[0] * 255;
    cv::imwrite("./dft_0001.jpg", plane[0]);

    return 0;
}
