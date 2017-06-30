#include <stdio.h>
#include <iostream>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;
using namespace cv::detail;

void readme();
Mat mosaicPar (Mat image1, Mat image2);

/** @function main */
int main( int argc, char** argv )
{
  Mat image1= imread( "./img_inputs/DJI_0027.JPG" );  // Left image
  Mat image2= imread( "./img_inputs/DJI_0026.JPG" );  // Right image
  Mat image3= imread( "./img_inputs/DJI_0025.JPG" );  // Left image
  Mat image4= imread( "./img_inputs/DJI_0024.JPG" );  // Right image

  Mat result1 = mosaicPar(image1, image2);
  imwrite("./img_outputs/output_image01.jpg", result1);

  Mat result2 = mosaicPar(result1, image3);
  imwrite("./img_outputs/output_image02.jpg", result2);

  Mat result3 = mosaicPar(result2, image4);
  imwrite("./img_outputs/output_image03.jpg", result3);





  return 0;
}

/** @function readme */
 void readme()
{
  std::cout << " Usage: Panorama < img1 > < img2 >" << std::endl;
}

/** @function mosaic 2 imagens */
Mat mosaicPar (Mat image1, Mat image2)
{

  Mat gray_image1;
  Mat gray_image2;

  // Convert to Grayscale
  cvtColor( image1, gray_image1, CV_RGB2GRAY );
  cvtColor( image2, gray_image2, CV_RGB2GRAY );

  //imshow("first image",image2);
  //imshow("second image",image1);

  if( !gray_image1.data || !gray_image2.data )
  {
    std::cout<< " --(!) Error reading images " << std::endl;
  }

  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 400;

  // NEW CODE FOR OPENCV 3.0
  Ptr<xfeatures2d::SURF> surf = xfeatures2d::SURF::create(minHessian);
  std::vector< KeyPoint > keypoints_object, keypoints_scene;
  Mat descriptors_object, descriptors_scene;

  surf->detectAndCompute(gray_image1, noArray(), keypoints_object, descriptors_object);
  surf->detectAndCompute(gray_image2, noArray(), keypoints_scene, descriptors_scene);

//-- Step 3: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
  std::vector<DMatch> matches;
  matcher.match( descriptors_object, descriptors_scene, matches );

  double max_dist = 0; double min_dist = 100;

  //-- Quick calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_object.rows; i++ )
  {
    double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  //printf("-- Max dist : %f \n", max_dist );
  //printf("-- Min dist : %f \n", min_dist );

  //-- Use only "good" matches (i.e. whose distance is less than 3*min_dist )
  std::vector< DMatch > good_matches;

  for( int i = 0; i < descriptors_object.rows; i++ )
    if( matches[i].distance < 3*min_dist )
      good_matches.push_back( matches[i]);

  std::vector< Point2f > obj;
  std::vector< Point2f > scene;

  for( int i = 0; i < (int)good_matches.size(); i++ )
  {
    //-- Get the keypoints from the good matches
    obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
    scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
  }

  // Find the Homography Matrix
  Mat H = findHomography( obj, scene, CV_RANSAC );
  // Use the Homography Matrix to warp the images
  cv::Mat result;
  warpPerspective(image1,result,H,cv::Size(image1.cols+image2.cols,image1.rows+image2.rows));
  cv::Mat half(result,cv::Rect(0,0,image2.cols,image2.rows));
  image2.copyTo(half);

  return result;
}
