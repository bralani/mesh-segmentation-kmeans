/*============================================================================
 *  Kernel.hpp
 *  Description: [Insert file description]
 *  Author: [Insert author if necessary]
 *  Date: [Insert date if necessary]
 *============================================================================*/

 #ifndef KERNEL_HPP
 #define KERNEL_HPP
 
 #include <Eigen/Dense>
 #include <cmath>
 
 using Eigen::VectorXd;
 
 // Define M_PI if not already defined
 #ifndef M_PI
 #define M_PI 3.14159265358979323846
 #endif
 
 /**
  * \class Kernel
  * \brief Provides various kernel functions for density estimation and regression.
  *
  * This class implements several common kernel functions, which are used in
  * statistical applications such as kernel density estimation and non-parametric regression.
  */
 class Kernel {
 public:
     /**
      * \brief Computes the Gaussian kernel.
      *
      * \param u Input vector.
      * \return Computed kernel value.
      */
     static double gaussian(const VectorXd& u);
 
     /**
      * \brief Computes the Epanechnikov kernel.
      *
      * \param u Input vector.
      * \return Computed kernel value.
      */
     static double epanechnikov(const VectorXd& u);
 
     /**
      * \brief Computes the Uniform kernel.
      *
      * \param u Input vector.
      * \return Computed kernel value.
      */
     static double uniform(const VectorXd& u);
 
     /**
      * \brief Computes the Triangular kernel.
      *
      * \param u Input vector.
      * \return Computed kernel value.
      */
     static double triangular(const VectorXd& u);
 
     /**
      * \brief Computes the Biweight kernel.
      *
      * \param u Input vector.
      * \return Computed kernel value.
      */
     static double biweight(const VectorXd& u);
 
     /**
      * \brief Computes the Triweight kernel.
      *
      * \param u Input vector.
      * \return Computed kernel value.
      */
     static double triweight(const VectorXd& u);
 
     /**
      * \brief Computes the Cosine kernel.
      *
      * \param u Input vector.
      * \return Computed kernel value.
      */
     static double cosine(const VectorXd& u);
 };
 
 #endif // KERNEL_HPP
 